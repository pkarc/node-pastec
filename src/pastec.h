// pastec.h
#ifndef PASTEC_H
#define PASTEC_H

#include <node.h>
#include <node_object_wrap.h>
#include <node_buffer.h>

#include <orb/orbfeatureextractor.h>
#include <orb/orbsearcher.h>
#include <orb/orbwordindex.h>

namespace pastec {

class Pastec : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  Index *index;
  ORBWordIndex *wordIndex;
  FeatureExtractor *featureExtractor;
  Searcher *imageSearcher;
  Pastec();
  ~Pastec();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;
  static void AddImage(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void SearchImage(const v8::FunctionCallbackInfo<v8::Value>& args);

};

}  // namespace demo

#endif
