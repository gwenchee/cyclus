#ifndef CYCLUS_SRC_PACKAGED_MATERIAL_H_
#define CYCLUS_SRC_PACKAGED_MATERIAL_H_

#include <list>
#include <boost/shared_ptr.hpp>

#include "package.h"
#include "cyc_limits.h"
#include "resource.h"
#include "res_tracker.h"
#include "material.h"

namespace cyclus {

class Context;


class PackagedMaterial: public Resource {
  friend class SimInit;

 public:
  typedef boost::shared_ptr<PackagedMaterial> Ptr;
  typedef std::vector<Material::Ptr> MaterialMap;
  static const ResourceType kType;

  virtual ~PackagedMaterial();

  /// Creates a new packagedmaterial resource that is "live" and tracked. creator is a
  /// pointer to the agent creating the resource (usually will be the caller's
  /// "this" pointer). All future output data recorded will be done using the
  /// creator's context.
  static Ptr Create(Agent* creator, MaterialMap matmap, Package::Ptr p);

  /// Creates a new packagedmaterial resource that does not actually exist as part of
  /// the simulation and is untracked.
  static Ptr CreateUntracked(MaterialMap matmap, Package::Ptr p);

  /// Returns the id of the packagedmaterial's internal materials.
 
 /// virtual int qual_id() const;

  /// Returns PackagedMaterial::kType.
  virtual const ResourceType type() const;

  /// Creates an untracked copy of this packagedmaterial object.
  virtual Resource::Ptr Clone() const;

  /// Records the internal material objects of this resource.
  virtual void Record(Context* ctx) const;


  ///virtual Resource::Ptr ExtractRes(double qty);

  /// Extracts the specified mass from this resource and returns it as a
  /// new material object with the same quality/type.
  ///Ptr ExtractQty(double qty);

  /// Creates a new packagedmaterial by extracting from this one.
  ///
  /// @param qty the mass quantity to extract
  /// @param c the composition the extracted/returned packagedmaterial
  /// @param threshold an absolute mass cutoff below which constituent nuclide
  /// quantities of the remaining unextracted packagedmaterial are set to zero.
  /// @return a new packagedmaterial with quantity qty and composition c
  ///Ptr ExtractComp(double qty, Composition::Ptr c,
  ///                double threshold = eps_rsrc());

  /// Combines packagedmaterial mat with this one.  mat's quantity becomes zero.
  void Absorb(Material::Ptr mat);


 protected:
  PackagedMaterial(Context* ctx, MaterialMap matmap, Package::Ptr p);

 private:
  Context* ctx_;
  double qty_;
  Package::Ptr pack_;
  ResTracker tracker_;
};

/// Creates and returns a new packagedmaterial with the specified quantity and a
/// default, meaningless composition.  This is intended only for testing
/// purposes.
///PackagedMaterial::Ptr NewBlankPackagedMaterial(double qty);

}  // namespace cyclus

#endif  // CYCLUS_SRC_PACKAGED_MATERIAL_H_
