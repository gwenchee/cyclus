#include "packagedmaterial.h"

#include "error.h"
#include "logger.h"

namespace cyclus {

const ResourceType PackagedMaterial::kType = "PackagedMaterial";

std::map<PackagedMaterial::package, int> PackagedMaterial::qualids_;
int PackagedMaterial::next_qualid_ = 1;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PackagedMaterial::Ptr PackagedMaterial::Create(Agent* creator, double quantity,
                             PackagedMaterial::package quality) {
  if (qualids_.count(quality) == 0) {
    qualids_[quality] = next_qualid_++;
    std::map<std::string, std::map<std::string, double>> mapmap = quality.second;
    std::map<std::string, double> insidemap; 
    std::vector<double> insideval;
    std::vector<std::string> columnname;
    for (std::map<std::string, std::map<std::string, double>>::iterator it = mapmap.begin();it!=mapmap.end(); ++it){
      insidemap = it->second; 
      for (std::map<std::string, double>::iterator itt = insidemap.begin();itt!=insidemap.end();++itt){
        insideval.push_back(itt->second);
        columnname.push_back(it->first + itt->first); 
      }
    }
    std::string valname = mapmap.begin()->first + mapmap.begin()->first;
      creator->context()->NewDatum("PackagedMaterials")
          ->AddVal("QualId", qualids_[quality])
          ->AddVal("Quantity", quantity)
          ->AddVal(columnname[0],insideval[0])
          ->AddVal(columnname[1],insideval[1])
          ->AddVal(columnname[2],insideval[2])
          ->AddVal(columnname[3],insideval[3])
          ->Record();
  }

  // the next lines must come after qual id setting
  PackagedMaterial::Ptr r(new PackagedMaterial(creator->context(), quantity, quality));
  r->tracker_.Create(creator);
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PackagedMaterial::Ptr PackagedMaterial::CreateUntracked(double quantity,
                                      PackagedMaterial::package quality) {
  PackagedMaterial::Ptr r(new PackagedMaterial(NULL, quantity, quality));
  r->tracker_.DontTrack();
  return r;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr PackagedMaterial::Clone() const {
  PackagedMaterial* g = new PackagedMaterial(*this);
  Resource::Ptr c = Resource::Ptr(g);
  g->tracker_.DontTrack();
  return c;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void PackagedMaterial::Absorb(PackagedMaterial::Ptr other) {
  if (other->quality() != quality()) {
    throw ValueError("incompatible resource types.");
  }
  quantity_ += other->quantity();
  other->quantity_ = 0;

  tracker_.Absorb(&other->tracker_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PackagedMaterial::Ptr PackagedMaterial::Extract(double quantity) {
  if (quantity > quantity_) {
    throw ValueError("Attempted to extract more quantity than exists.");
  }

  quantity_ -= quantity;

  PackagedMaterial::Ptr other(new PackagedMaterial(ctx_, quantity, quality_));
  tracker_.Extract(&other->tracker_);
  return other;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr PackagedMaterial::ExtractRes(double qty) {
  return boost::static_pointer_cast<Resource>(Extract(qty));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PackagedMaterial::PackagedMaterial(Context* ctx, double quantity, PackagedMaterial::package quality)
    : quality_(quality),
      quantity_(quantity),
      tracker_(ctx, this),
      ctx_(ctx) {}

}  // namespace cyclus
