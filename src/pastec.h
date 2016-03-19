// pastec.h
#ifndef PASTEC_H
#define PASTEC_H

#include <node.h>
#include <node_buffer.h>
#include <nan.h>

#include <orb/orbfeatureextractor.h>
#include <orb/orbsearcher.h>
#include <orb/orbwordindex.h>

class Pastec : public Nan::ObjectWrap {
 public:
  static NAN_MODULE_INIT(Init);

 private:
  Index *index;
  ORBWordIndex *wordIndex;
  FeatureExtractor *featureExtractor;
  Searcher *imageSearcher;

  explicit Pastec(string bi = "backwardIndex.dat", string vw = "visualWordsORB.dat");
  ~Pastec();


  static NAN_METHOD(New);
  static NAN_METHOD(addImage);
  static NAN_METHOD(searchImage);
  static Nan::Persistent<v8::Function> constructor;
  string bi_;
  string vw_;

};

#endif
