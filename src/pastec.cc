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
  Nan::SetPrototypeMethod(tpl, "searchImage", searchImage);

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
  Nan::Set(resObj, Nan::New("image_id").ToLocalChecked(), Nan::New(i_imageId));
  Nan::Set(resObj, Nan::New("features_extracted").ToLocalChecked(), Nan::New(i_nbFeaturesExtracted));

  if(i_ret == IMAGE_ADDED){
      string dataStr = string(*Nan::Utf8String(info[2]));
      u_int32_t t_ret = pastec->index->addTag(i_imageId, dataStr);

      if(t_ret == IMAGE_TAG_ADDED)
          Nan::Set(resObj, Nan::New("tag").ToLocalChecked(), Nan::New(dataStr).ToLocalChecked());
  }

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
      Nan::Set(resObj, Nan::New("image_id").ToLocalChecked(), Nan::New(req.results[i]));
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

/*
// node native

Persistent<Function> Pastec::constructor;

Pastec::Pastec() {
  index = new ORBIndex("backwardIndex.dat", false);
  wordIndex = new ORBWordIndex("visualWordsORB.dat");
  featureExtractor = new ORBFeatureExtractor((ORBIndex *)index, wordIndex);
  imageSearcher = new ORBSearcher((ORBIndex *)index, wordIndex);
}

Pastec::~Pastec() {
}

void Pastec::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "Pastec"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  NODE_SET_PROTOTYPE_METHOD(tpl, "addImage", AddImage);
  NODE_SET_PROTOTYPE_METHOD(tpl, "searchImage", SearchImage);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "Pastec"),
               tpl->GetFunction());
}

void Pastec::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new Pastec(...)`
    //double value = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
    Pastec* obj = new Pastec();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `Pastec(...)`, turn into construct call.
    //const int argc = 1;
    //Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(cons->NewInstance());
  }
}

void Pastec::AddImage(const FunctionCallbackInfo<Value>& args) {

  Isolate* isolate = args.GetIsolate();

  Pastec* pastec = ObjectWrap::Unwrap<Pastec>(args.Holder());

  if (args.Length() < 2) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsNumber() || !args[1]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }

  u_int32_t i_imageId = args[0]->IntegerValue();

  Local<Object> bufObj = args[1]->ToObject();

  unsigned i_nbFeaturesExtracted;
  char *p_imgData = (char *) node::Buffer::Data(bufObj);
  unsigned i_imgSize = node::Buffer::Length(bufObj);

  u_int32_t i_ret = pastec->featureExtractor->processNewImage(
      i_imageId, i_imgSize, p_imgData,
      i_nbFeaturesExtracted);

  Local<Object> resObj = Object::New(isolate);
  resObj->Set(String::NewFromUtf8(isolate, "type"), String::NewFromUtf8(isolate, Converter::codeToString(i_ret).c_str()));
  resObj->Set(String::NewFromUtf8(isolate, "image_id"), Uint32::New(isolate, i_imageId));
  resObj->Set(String::NewFromUtf8(isolate, "features_extracted"), Number::New(isolate, i_nbFeaturesExtracted));

  args.GetReturnValue().Set(resObj);
}

void Pastec::SearchImage(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  Pastec* pastec = ObjectWrap::Unwrap<Pastec>(args.Holder());

  char *p_imgData = (char *) node::Buffer::Data(args[0]->ToObject());
  unsigned i_imgSize = node::Buffer::Length(args[0]->ToObject());

  SearchRequest req;

  vector<char> imgData(i_imgSize);
  memcpy(imgData.data(), p_imgData, i_imgSize);

  req.imageData = imgData;
  req.client = NULL;
  u_int32_t i_ret = pastec->imageSearcher->searchImage(req);

  // Create a new empty array.
  Local<Array> results = Array::New(isolate, req.results.size());

  for (unsigned i = 0; i < req.results.size(); ++i)
      results->Set(i, Uint32::New(isolate, req.results[i]));

  Local<Object> resObj = Object::New(isolate);
  resObj->Set(String::NewFromUtf8(isolate, "type"), String::NewFromUtf8(isolate, Converter::codeToString(i_ret).c_str()));
  resObj->Set(String::NewFromUtf8(isolate, "image_ids"), results);

  args.GetReturnValue().Set(resObj);
}

}  // namespace demo
*/
