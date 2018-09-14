#include "package.h"

#include "comp_math.h"
#include "context.h"
#include "decayer.h"
#include "error.h"
#include "recorder.h"

extern "C" {
#include "cram.hpp"
}

namespace cyclus {

int Package::next_id_ = 1;

Package::Ptr Package::Create(PackageMap v) {

  Package::Ptr p(new Package());
  p->package_ = v;
  return p;
}

int Package::id() {
  return id_;
}

/*
void Package::Record(Context* ctx) {
  if (recorded_) {
    return;
  }
  recorded_ = true;

  CompMap::const_iterator it;
  CompMap cm = mass();  // force lazy evaluation now
  compmath::Normalize(&cm, 1);
  for (it = cm.begin(); it != cm.end(); ++it) {
    ctx->NewDatum("Packages")
        ->AddVal("QualId", id())
        ->AddVal("NucId", it->first)
        ->AddVal("MassFrac", it->second)
        ->Record();
  }
}
*/



}  // namespace cyclus
