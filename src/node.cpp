#include <node.h>

#include "pastec.h"

namespace pastec {

  using v8::Local;
  using v8::Object;

  void InitAll(Local<Object> exports) {
    Pastec::Init(exports);
  }

  NODE_MODULE(pastec, InitAll)

}
