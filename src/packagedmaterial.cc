#include "packagedmaterial.h"

#include <math.h>

#include "comp_math.h"
#include "context.h"
#include "decayer.h"
#include "error.h"
#include "logger.h"

namespace cyclus {

const ResourceType PackagedMaterial::kType = "PackagedMaterial";

PackagedMaterial::~PackagedMaterial() {}

PackagedMaterial::Ptr PackagedMaterial::Create(Agent* creator, MaterialMap matmap,
                               Package::Ptr p) {
  PackagedMaterial::Ptr pm(new PackagedMaterial(creator->context(), matmap, p));
  pm->tracker_.Create(creator);
  return pm;
}

PackagedMaterial::Ptr PackagedMaterial::CreateUntracked(MaterialMap matmap,
                                        Package::Ptr p) {
  PackagedMaterial::Ptr pm(new PackagedMaterial(NULL, matmap, p));
  return pm;
}

///int PackagedMaterial::qual_id() const {
///  return comp_->id();
///}

const ResourceType PackagedMaterial::type() const {
  return PackagedMaterial::kType;
}

Resource::Ptr PackagedMaterial::Clone() const {
  PackagedMaterial* pm = new PackagedMaterial(*this);
  Resource::Ptr p(pm);
  pm->tracker_.DontTrack();
  return p;
}

void PackagedMaterial::Record(Context* ctx) const {
  // Note that no time field is needed because the resource ID changes
  // every time the resource changes - state_id by itself is already unique.
  ctx_->NewDatum("PackagedMaterialInfo")
      ->AddVal("ResourceId", state_id())
      ->Record();

  pack_->Record(ctx);
}


///Resource::Ptr PackagedMaterial::ExtractRes(double qty) {
///  return boost::static_pointer_cast<Resource>(ExtractQty(qty));
///}

///PackagedMaterial::Ptr PackagedMaterial::ExtractQty(double qty) {
///  return ExtractComp(qty, comp_);
///}

///PackagedMaterial::Ptr PackagedMaterial::ExtractComp(double qty, Composition::Ptr c,
///                                    double threshold) {
///  if (qty_ < qty) {
///    throw ValueError("mass extraction causes negative quantity");
///  }

  // TODO: decide if ExtractComp should force lazy-decay by calling comp()


  PackagedMaterial::Ptr other(new PackagedMaterial(ctx_, matmap, p));

  // Decay called on the extracted packagedmaterial should have the same dt as for
  // this packagedmaterial regardless of composition.

  tracker_.Extract(&other->tracker_);

  return other;
}

void PackagedMaterial::Absorb(Material::Ptr mat) {
  
  try {
    MaterialMap.push_back(mat);
  } catch (cyclus::Error& e) {
    e.msg(Agent::InformErrorMsg(e.msg()));
    throw e;
}
  }

}

PackagedMaterial::PackagedMaterial(Context* ctx, MaterialMap matmap, Package::Ptr p)
    : qty_(quantity),
      pack_(p),
      tracker_(ctx, this),
      ctx_(ctx),


///PackagedMaterial::Ptr NewBlankPackagedMaterial(double quantity) {
///  Composition::Ptr comp = Composition::CreateFromMass(CompMap());
///  return PackagedMaterial::CreateUntracked(quantity, comp);
///}

}  // namespace cyclus
