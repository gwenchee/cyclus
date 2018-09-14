#ifndef CYCLUS_SRC_PACKAGE_H_
#define CYCLUS_SRC_PACKAGE_H_

#include <map>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

class SimInitTest;

namespace cyclus {

class Context;

/// a raw definition of properties of package
typedef std::pair<std::string,std::map<std::string,double> > PackageMap;


class Package {
  friend class SimInit;
  friend class ::SimInitTest;

 public:
  typedef boost::shared_ptr<Package> Ptr;

  /// Creates a new package from v 
  static Ptr Create(PackageMap v);


  /// Returns a unique id associated with this package.  Note that multiple
  /// material objects can share the same composition. Also Note that the id is
  /// not the same for two compositions that were separately created from the
  /// same CompMap.
  int id();


  /// Records the composition in output database Compositions table (if
  /// not done previously).
  ///void Record(Context* ctx);

 protected:

  Package();

 private:

  static int next_id_;
  int id_;
  bool recorded_;
  PackageMap package_;

};

}  // namespace cyclus

#endif  // CYCLUS_SRC_COMPOSITION_H_
