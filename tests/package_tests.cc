#include <map>

#include <gtest/gtest.h>

#include "context.h"
#include "package.h"
#include "comp_math.h"
#include "env.h"
#include "pyne.h"

using cyclus::Package;
using cyclus::CompMap;
using pyne::nucname::id;


TEST(PackageTests, create) {
  cyclus::Env::SetNucDataPath();

  PackageMap v;
  v['layer1']['radius1'] = 1;
  v['layer1']['thermalcond1'] = 1000;
  v['layer2']['radius2'] = 2;
  v['layer2']['thermalcond2'] = 2000;
  Package::Ptr p = Package::Create(v);
  cout <<"hi"<<endl; 

  v = c->atom();
  EXPECT_DOUBLE_EQ(v[922350000] / v[922330000], 2 / 1);
  v = c->mass();
  EXPECT_DOUBLE_EQ(v[922350000] / v[922330000],
                   2 * pyne::atomic_mass(922350000) / pyne::atomic_mass(922330000));
}


TEST(PackageTests, lineage) {
  cyclus::Env::SetNucDataPath();

  TestComp c;

  int dt = 5;
  Package::Ptr dec1 = c.Decay(dt);
  Package::Ptr dec2 = c.Decay(2 * dt);
  Package::Ptr dec3 = c.Decay(2 * dt);
  Package::Ptr dec4 = dec1->Decay(2 * dt);
  Package::Ptr dec5 = dec2->Decay(dt);

  std::map<int, Package::Ptr> chain = c.DecayLine();

  EXPECT_EQ(chain.size(), 3);
  EXPECT_EQ(chain[dt], dec1);
  EXPECT_EQ(chain[2 * dt], dec2);
  EXPECT_EQ(dec2, dec3);
  EXPECT_EQ(chain[3 * dt], dec4);
  EXPECT_EQ(dec4, dec5);
}

TEST(PackageTests, decay) {
  cyclus::Env::SetNucDataPath();

  CompMap v;
  v[id("Cs137")] = 1;
  v[id("U238")] = 10;
  cyclus::compmath::Normalize(&v);
  Package::Ptr c = Package::CreateFromAtom(v);

  double secs_per_timestep = kDefaultTimeStepDur;
  Package::Ptr newc = \
    c->Decay(int(pyne::half_life(std::string("Cs137")) / secs_per_timestep));

  CompMap newv = newc->atom();
  cyclus::compmath::Normalize(&newv);

  ASSERT_TRUE(newv.size() > 0) << "decayed package has zero nuclides - what?";
  ASSERT_TRUE(newv.size() > v.size()) << "decayed package should have more nuclides than original";
  EXPECT_NEAR(v[id("Cs137")] / 2, newv[id("Cs137")], 1e-4);
  EXPECT_NEAR(v[id("U238")], newv[id("U238")], 1e-4);
}

