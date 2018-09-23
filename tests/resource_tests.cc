#include <gtest/gtest.h>

#include "context.h"
#include "recorder.h"
#include "timer.h"
#include "material.h"
#include "product.h"
#include "composition.h"
#include "region.h"
#include "packagedmaterial.h"

using cyclus::Material;
using cyclus::Product;
using cyclus::PackagedMaterial;

class Dummy : public cyclus::Region {
 public:
  Dummy(cyclus::Context* ctx) : cyclus::Region(ctx) {}
  Dummy* Clone() { return NULL; }
};

class ResourceTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    ctx = new cyclus::Context(&ti, &rec);

    cyclus::CompMap v; v[922350000] = 1;
    cyclus::Composition::Ptr c = cyclus::Composition::CreateFromMass(v);
    cyclus::Agent* dummy = new Dummy(ctx);

    m1 = Material::Create(dummy, 3, c);
    m2 = Material::Create(dummy, 7, c);
    p1 = Product::Create(dummy, 3, "bananas");
    p2 = Product::Create(dummy, 7, "bananas");

    cyclus::PackagedMaterial::matstream ms;
    cyclus::PackagedMaterial::package pk;
    ms.push_back(m1);
    ms.push_back(m2);

    pm1 = PackagedMaterial::Create(dummy, 3, pk);
    pm2 = PackagedMaterial::Create(dummy, 7, pk);
  }

  virtual void TearDown() {
    delete ctx;
  }

  cyclus::Timer ti;
  cyclus::Recorder rec;
  cyclus::Context* ctx;
  Material::Ptr m1;
  Material::Ptr m2;
  PackagedMaterial::Ptr pm1;
  PackagedMaterial::Ptr pm2;
  Product::Ptr p1;
  Product::Ptr p2;
};

TEST_F(ResourceTest, MaterialAbsorbTrackid) {
  int obj_id = m1->obj_id();
  m1->Absorb(m2);
  EXPECT_EQ(obj_id, m1->obj_id());
}

TEST_F(ResourceTest, MaterialAbsorbGraphid) {
  int state_id = m1->state_id();
  m1->Absorb(m2);
  EXPECT_LT(state_id, m1->state_id());
}

TEST_F(ResourceTest, MaterialExtractTrackid) {
  int obj_id = m1->obj_id();
  Material::Ptr m3 = m1->ExtractQty(2);
  EXPECT_EQ(obj_id, m1->obj_id());
  EXPECT_LT(obj_id, m3->obj_id());
}

TEST_F(ResourceTest, MaterialExtractGraphid) {
  int state_id = m1->state_id();
  Material::Ptr m3 = m1->ExtractQty(2);
  EXPECT_LT(state_id, m1->state_id());
  EXPECT_LT(state_id, m3->state_id());
  EXPECT_NE(m1->state_id(), m3->state_id());
}


TEST_F(ResourceTest, PackagedMaterialAbsorbTrackid) {
  int obj_id = pm1->obj_id();
  pm1->Absorb(pm2);
  EXPECT_EQ(obj_id, pm1->obj_id());
}

TEST_F(ResourceTest, PackagedMaterialAbsorbGraphid) {
  int state_id = pm1->state_id();
  pm1->Absorb(pm2);
  EXPECT_LT(state_id, pm1->state_id());
}

TEST_F(ResourceTest, PackagedMaterialExtractTrackid) {
  int obj_id = pm1->obj_id();
  PackagedMaterial::Ptr pm3 = pm1->Extract(2);
  EXPECT_EQ(obj_id, pm1->obj_id());
  EXPECT_LT(obj_id, pm3->obj_id());
}

TEST_F(ResourceTest, PackagedMaterialExtractGraphid) {
  int state_id = pm1->state_id();
  PackagedMaterial::Ptr pm3 = pm1->Extract(2);
  EXPECT_LT(state_id, pm1->state_id());
  EXPECT_LT(state_id, pm3->state_id());
  EXPECT_NE(pm1->state_id(), pm3->state_id());
}


TEST_F(ResourceTest, ProductAbsorbTrackid) {
  int obj_id = p1->obj_id();
  p1->Absorb(p2);
  EXPECT_EQ(obj_id, p1->obj_id());
}

TEST_F(ResourceTest, ProductAbsorbGraphid) {
  int state_id = p1->state_id();
  p1->Absorb(p2);
  EXPECT_LT(state_id, p1->state_id());
}

TEST_F(ResourceTest, ProductExtractTrackid) {
  int obj_id = p1->obj_id();
  Product::Ptr p3 = p1->Extract(2);
  EXPECT_EQ(obj_id, p1->obj_id());
  EXPECT_LT(obj_id, p3->obj_id());
}

TEST_F(ResourceTest, ProductExtractGraphid) {
  int state_id = p1->state_id();
  Product::Ptr p3 = p1->Extract(2);
  EXPECT_LT(state_id, p1->state_id());
  EXPECT_LT(state_id, p3->state_id());
  EXPECT_NE(p1->state_id(), p3->state_id());
}

