#ifndef CYCLUS_SRC_PACKAGED_MATERIAL_H_
#define CYCLUS_SRC_PACKAGED_MATERIAL_H_

#include <list>
#include <boost/shared_ptr.hpp>

#include "composition.h"
#include "cyc_limits.h"
#include "resource.h"
#include "res_tracker.h"
#include "material.h"

namespace cyclus {

class Context;

/// The packagedmaterial class is primarily responsible for enabling basic packagedmaterial
/// manipulation while helping enforce mass conservation.  It also provides the
/// ability to easily decay a packagedmaterial up to the current simulation time; it
/// does not perform any decay related logic itself.
///
/// There are four basic operations that can be performed on packagedmaterials: create,
/// transmute (change packagedmaterial composition - e.g. fission by reactor), absorb
/// (combine packagedmaterials), extract (split a packagedmaterial). All packagedmaterial
/// handling/manipulation will be performed using these operations - and all
/// operations performed will be tracked and recorded. Usage examples:
///
/// * A mining facility that "creates" new packagedmaterial
///
///   @code
///   Composition::Ptr nat_u = ...
///   double qty = 10.0;
///
///   PackagedMaterial::Ptr m = PackagedMaterial::Create(qty, nat_u, ctx);
///   @endcode
///
/// * A conversion facility mixing uranium and flourine:
///
///   @code
///   PackagedMaterial::Ptr uf6 = uranium_buf.Pop();
///   PackagedMaterial::Ptr f = flourine_buf.Pop();
///
///   uf6.Absorb(f);
///   @endcode
///
/// * A reactor transmuting fuel:
///
///   @code
///   Composition::Ptr burned_comp = ... // fancy code to calculate burned nuclides
///   PackagedMaterial::Ptr assembly = core_fuel.Pop();
///
///   assembly.Transmute(burned_comp);
///   @endcode
///
/// * A separations plant extracting stuff from spent fuel:
///
///   @code
///   Composition::Ptr comp = ... // fancy code to calculate extracted nuclides
///   Material::Ptr bucket = spent_fuel.Pop();
///   double qty = 3.0;
///
///   PackagedMaterial::Ptr mox = bucket.ExtractComp(qty, comp);
///   @endcode
///
class PackagedMaterial: public Resource {
  friend class SimInit;

 public:
  typedef boost::shared_ptr<PackagedMaterial> Ptr;
  typedef std::vector<Material::Ptr> matstream;
  static const ResourceType kType;

  virtual  ~PackagedMaterial();

  /// Creates a new packagedmaterial resource that is "live" and tracked. creator is a
  /// pointer to the agent creating the resource (usually will be the caller's
  /// "this" pointer). All future output data recorded will be done using the
  /// creator's context.
  static Ptr Create(Agent* creator, matstream quantity, Composition::Ptr c);

  /// Creates a new packagedmaterial resource that does not actually exist as part of
  /// the simulation and is untracked.
  /// static Ptr CreateUntracked(matstream quantity, Composition::Ptr c);

  /// Returns the id of the packagedmaterial's internal nuclide composition.
  /// virtual int qual_id() const;

  /// Returns PackagedMaterial::kType.
  ///virtual const ResourceType type() const;

  /// Creates an untracked copy of this packagedmaterial object.
  ///virtual Resource::Ptr Clone() const;

  /// Records the internal nuclide composition of this resource.
  ///virtual void Record(Context* ctx) const;

  /// Returns "kg"
  ///virtual std::string units() const;

  /// Returns the mass of this packagedmaterial in kg.
  ///virtual double quantity() const;

  ///virtual Resource::Ptr ExtractRes(matstream qty);

  /// Same as ExtractComp with c = this->comp().
  ///Ptr ExtractQty(matstream qty);

  /// Creates a new packagedmaterial by extracting from this one.
  ///
  /// @param qty the mass quantity to extract
  /// @param c the composition the extracted/returned packagedmaterial
  /// @param threshold an absolute mass cutoff below which constituent nuclide
  /// quantities of the remaining unextracted packagedmaterial are set to zero.
  /// @return a new packagedmaterial with quantity qty and composition c
  /*Ptr ExtractComp(matstream qty, Composition::Ptr c,
                  double threshold = eps_rsrc());
  */
  /// Combines packagedmaterial mat with this one.  mat's quantity becomes zero.
  ///void Absorb(Ptr mat);

  /// Changes the packagedmaterial's composition to c without changing its mass.  Use
  /// this method for things like converting fresh to spent fuel via burning in
  /// a reactor.
  ///void Transmute(Composition::Ptr c);

  /// Updates the packagedmaterial's composition by performing a decay calculation.
  /// This is a special case of Transmute where the new composition is
  /// calculated automatically.  The time delta is calculated as the difference
  /// between curr_time and the last time the packagedmaterial's composition was
  /// updated with a decay calculation (i.e. prev_decay_time).  This may or may
  /// not result in an updated packagedmaterial composition.  Does nothing if the
  /// simulation decay mode is set to "never" or none of the nuclides' decay
  /// constants are significant with respect to the time delta.
  ///void Decay(int curr_time);

  /// Returns the last time step on which a decay calculation was performed
  /// for the packagedmaterial.  This is not necessarily synonymous with the last time
  /// step the packagedmaterial's Decay function was called.
  ///int prev_decay_time() { return prev_decay_time_; }

  /// Returns a double with the decay heat of the packagedmaterial in units of
  /// W/kg.
  ///double DecayHeat();

  /// Returns the nuclide composition of this packagedmaterial.
  ///Composition::Ptr comp();

  /// DEPRECATED - use non-const comp() function.
  ///Composition::Ptr comp() const;

 protected:
  PackagedMaterial(Context* ctx, matstream quantity, Composition::Ptr c);

 private:
  Context* ctx_;
  matstream qty_;
  Composition::Ptr comp_;
  int prev_decay_time_;
  ResTracker tracker_;
};

/// Creates and returns a new packagedmaterial with the specified quantity and a
/// default, meaningless composition.  This is intended only for testing
/// purposes.
/// PackagedMaterial::Ptr NewBlankPackagedMaterial(matstream qty);

}  // namespace cyclus

#endif  // CYCLUS_SRC_PACKAGED_MATERIAL_H_
