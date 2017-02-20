#include "platex/core/params.h"

namespace platex {
  CParams* CParams::instance_ = nullptr;

  CParams* CParams::instance() {
    if (!instance_) {
      instance_ = new CParams;
    }
    return instance_;
  }
}/*! \namespace platex*/
