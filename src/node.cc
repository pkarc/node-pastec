#include "pastec.h"

// node.cc represents the top level of the module.
// C++ constructs that are exposed to javascript are exported here

NAN_MODULE_INIT(InitAll) {
  // Passing target down to the next NAN_MODULE_INIT
  Pastec::Init(target);
}

NODE_MODULE(pastec, InitAll)