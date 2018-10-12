#ifndef CYCLUS_SRC_PACKAGED_MATERIAL_H_
#define CYCLUS_SRC_PACKAGED_MATERIAL_H_

#include <boost/shared_ptr.hpp>

#include "context.h"
#include "resource.h"
#include "res_tracker.h"
#include "material.h"

class SimInitTest;

namespace cyclus {

/// A PackagedMaterial is a general type of resource in the Cyclus simulation,
/// and is a catch-all for non-standard resources.  It implements the Resource
/// class interface in a simple way usable for things such as: bananas,
/// man-hours, water, buying power, etc.
class PackagedMaterial : public Resource {
  friend class SimInit;
  friend class ::SimInitTest;

 public:
  typedef boost::shared_ptr<PackagedMaterial> Ptr;
  typedef std::vector<Material::Ptr> matstream;
  typedef std::pair<matstream, std::map<std::string, std::map<std::string, double>>> package;
  static const ResourceType kType;

  /// Creates a new product that is "live" and tracked. creator is a
  /// pointer to the agent creating the resource (usually will be the caller's
  /// "this" pointer). All future output data recorded will be done using the
  /// creator's context.
  static Ptr Create(Agent* creator, double quantity, package quality);

  /// Creates a new product that does not actually exist as part of
  /// the simulation and is untracked.
  static Ptr CreateUntracked(double quantity, package quality);

  /// Returns 0 (for now).
  virtual int qual_id() const {
    return qualids_[quality_];
  }

  /// Returns PackagedMaterial::kType.
  virtual const ResourceType type() const {
    return kType;
  }

  virtual Resource::Ptr Clone() const;

  virtual void Record(Context* ctx) const {}

  virtual std::string units() const { return "NONE"; }

  virtual double quantity() const {
    return quantity_;
  }

  /// Returns the quality of this resource (e.g. bananas, human labor, water, etc.).
  virtual const package& quality() const {
    return quality_;
  }

  virtual Resource::Ptr ExtractRes(double quantity);

  /// Extracts the specified mass from this resource and returns it as a
  /// new product object with the same quality/type.
  ///
  /// @throws ValueError tried to extract more than exists.
  PackagedMaterial::Ptr Extract(double quantity);

  /// Absorbs the contents of the given 'other' resource into this resource.
  /// @throws ValueError 'other' resource is of different quality
  void Absorb(PackagedMaterial::Ptr other);

 private:
  /// @param ctx the simulation context
  /// @param quantity is a double indicating the quantity
  /// @param quality the resource quality
  PackagedMaterial(Context* ctx, double quantity, package quality);

  // map<quality, quality_id>
  static std::map<package, int> qualids_;
  static int next_qualid_;

  Context* ctx_;
  package quality_;
  double quantity_;
  ResTracker tracker_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_PACKAGED_MATERIAL_H_
