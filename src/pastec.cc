#include "pastec.h"

#include <messages.h>
#include <featureextractor.h>
#include <searcher.h>
#include <index.h>

#include <imageloader.h>
#include <opencv2/highgui/highgui.hpp>

using namespace v8;

Nan::Persistent<v8::Function> Pastec::constructor;

NAN_MODULE_INIT(Pastec::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Pastec").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "addImage", addImage);
  Nan::SetPrototypeMethod(tpl, "removeImage", removeImage);
  Nan::SetPrototypeMethod(tpl, "searchImage", searchImage);
  Nan::SetPrototypeMethod(tpl, "searchSimilar", searchSimilar);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("Pastec").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
}

Pastec::Pastec(string bi, string vw) : bi_(bi), vw_(vw) {
  index = new ORBIndex(bi, false);
  wordIndex = new ORBWordIndex(vw);
  featureExtractor = new ORBFeatureExtractor((ORBIndex *)index, wordIndex);
  imageSearcher = new ORBSearcher((ORBIndex *)index, wordIndex);
}

Pastec::~Pastec() {
}

NAN_METHOD(Pastec::New) {
  if (info.IsConstructCall()) {
    string bi = info[0]->IsUndefined() ? "backwardIndex.dat" : string(*Nan::Utf8String(info[0]));
    string vw = info[1]->IsUndefined() ? "visualWordsORB.dat" : string(*Nan::Utf8String(info[1]));
    Pastec *pastec = new Pastec(bi, vw);
    pastec->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 1;
    v8::Local<v8::Value> argv[argc] = {info[0]};
    v8::Local<v8::Function> cons = Nan::New(constructor);
    info.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

NAN_METHOD(Pastec::addImage) {

  Pastec* pastec = Nan::ObjectWrap::Unwrap<Pastec>(info.This());

  if (info.Length() < 3) {
    Nan::ThrowError("Wrong number of arguments");
    return;
  }

  if (!info[0]->IsNumber() || !info[1]->IsObject() || !info[2]->IsString()) {
    Nan::ThrowError("Wrong arguments");
    return;
  }

  u_int32_t i_imageId = info[0]->IntegerValue();

  Local<v8::Object> bufObj = info[1]->ToObject();

  unsigned i_nbFeaturesExtracted;
  char *p_imgData = (char *) node::Buffer::Data(bufObj);
  unsigned i_imgSize = node::Buffer::Length(bufObj);

  u_int32_t i_ret = pastec->featureExtractor->processNewImage(
      i_imageId, i_imgSize, p_imgData,
      i_nbFeaturesExtracted);

  Local<v8::Object> resObj = Nan::New<v8::Object>();
  Nan::Set(resObj, Nan::New("type").ToLocalChecked(), Nan::New(Converter::codeToString(i_ret).c_str()).ToLocalChecked());
  Nan::Set(resObj, Nan::New("imageId").ToLocalChecked(), Nan::New(i_imageId));
  Nan::Set(resObj, Nan::New("featuresExtracted").ToLocalChecked(), Nan::New(i_nbFeaturesExtracted));

  if(i_ret == IMAGE_ADDED){
      string dataStr = string(*Nan::Utf8String(info[2]));
      u_int32_t t_ret = pastec->index->addTag(i_imageId, dataStr);

      if(t_ret == IMAGE_TAG_ADDED)
          Nan::Set(resObj, Nan::New("tag").ToLocalChecked(), Nan::New(dataStr).ToLocalChecked());
  }

  info.GetReturnValue().Set(resObj);

}

NAN_METHOD(Pastec::removeImage) {

  Pastec* pastec = Nan::ObjectWrap::Unwrap<Pastec>(info.This());

  if (!info[0]->IsNumber()) {
    Nan::ThrowError("Wrong arguments");
    return;
  }

  u_int32_t i_imageId = info[0]->IntegerValue();

  u_int32_t i_ret = pastec->index->removeImage(i_imageId);

  Local<v8::Object> resObj = Nan::New<v8::Object>();
  Nan::Set(resObj, Nan::New("type").ToLocalChecked(), Nan::New(Converter::codeToString(i_ret).c_str()).ToLocalChecked());
  Nan::Set(resObj, Nan::New("imageId").ToLocalChecked(), Nan::New(i_imageId));

  info.GetReturnValue().Set(resObj);

}

NAN_METHOD(Pastec::searchImage) {

  Pastec* pastec = Nan::ObjectWrap::Unwrap<Pastec>(info.This());

  if (!info[0]->IsObject()) {
    Nan::ThrowError("Wrong arguments");
    return;
  }

  Local<v8::Object> bufObj = info[0]->ToObject();

  char *p_imgData = (char *) node::Buffer::Data(bufObj);
  unsigned i_imgSize = node::Buffer::Length(bufObj);

  vector<char> imgData(i_imgSize);
  memcpy(imgData.data(), p_imgData, i_imgSize);

  SearchRequest req;
  req.imageData = imgData;
  req.client = NULL;

  u_int32_t i_ret = pastec->imageSearcher->searchImage(req);

  // Create a new empty array.
  Local<v8::Array> results = Nan::New<v8::Array>(req.results.size());

  for (unsigned i = 0; i < req.results.size(); ++i){

      Local<v8::Object> resObj = Nan::New<v8::Object>();
      Nan::Set(resObj, Nan::New("imageId").ToLocalChecked(), Nan::New(req.results[i]));
      Nan::Set(resObj, Nan::New("score").ToLocalChecked(), Nan::New(req.scores[i]));
      Nan::Set(resObj, Nan::New("tag").ToLocalChecked(), Nan::New(req.tags[i]).ToLocalChecked());

      Nan::Set(results, i, resObj);

  }

  Local<v8::Object> resObj = Nan::New<v8::Object>();
  Nan::Set(resObj, Nan::New("type").ToLocalChecked(), Nan::New(Converter::codeToString(i_ret).c_str()).ToLocalChecked());
  Nan::Set(resObj, Nan::New("results").ToLocalChecked(), results);

  info.GetReturnValue().Set(resObj);

}

NAN_METHOD(Pastec::searchSimilar) {

  Pastec* pastec = Nan::ObjectWrap::Unwrap<Pastec>(info.This());

  if (!info[0]->IsNumber()) {
    Nan::ThrowError("Wrong arguments");
    return;
  }

  u_int32_t i_imageId = info[0]->IntegerValue();

  SearchRequest req;
  req.imageId = i_imageId;
  req.client = NULL;

  u_int32_t i_ret = pastec->imageSearcher->searchSimilar(req);

  // Create a new empty array.
  Local<v8::Array> results = Nan::New<v8::Array>(req.results.size());

  for (unsigned i = 0; i < req.results.size(); ++i){

      Local<v8::Object> resObj = Nan::New<v8::Object>();
      Nan::Set(resObj, Nan::New("imageId").ToLocalChecked(), Nan::New(req.results[i]));
      Nan::Set(resObj, Nan::New("score").ToLocalChecked(), Nan::New(req.scores[i]));
      Nan::Set(resObj, Nan::New("tag").ToLocalChecked(), Nan::New(req.tags[i]).ToLocalChecked());

      Nan::Set(results, i, resObj);

  }

  Local<v8::Object> resObj = Nan::New<v8::Object>();
  Nan::Set(resObj, Nan::New("type").ToLocalChecked(), Nan::New(Converter::codeToString(i_ret).c_str()).ToLocalChecked());
  Nan::Set(resObj, Nan::New("results").ToLocalChecked(), results);

  info.GetReturnValue().Set(resObj);

}

NODE_MODULE(pastec, Pastec::Init)