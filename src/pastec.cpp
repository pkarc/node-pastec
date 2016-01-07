#include "pastec.h"

#include <messages.h>
#include <featureextractor.h>
#include <searcher.h>
#include <index.h>

#include <imageloader.h>
#include <opencv2/highgui/highgui.hpp>

namespace pastec {

using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Uint32;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Array;

Persistent<Function> Pastec::constructor;

Pastec::Pastec() {
  index = new ORBIndex("backwardIndex.dat");
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
