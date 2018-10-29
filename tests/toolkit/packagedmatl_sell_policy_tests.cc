#include <gtest/gtest.h>

#include "toolkit/packagedmatl_sell_policy.h"

#include "composition.h"
#include "material.h"
#include "packagedmaterial.h"
#include "bid.h"
#include "request.h"
#include "error.h"
#include "pyne.h"
#include "toolkit/resource_buff.h"

#include "test_context.h"
#include "test_agents/test_facility.h"

namespace cyclus {
namespace toolkit {


class PackagedMatlSellPolicyTests: public ::testing::Test {
 protected:
  TestContext tc;
  TestFacility* fac1;
  double cap, qty;
  ResBuf<PackagedMaterial> buff;
  PackagedMaterial::package pack, pack1;
  PackagedMaterial::Ptr pacmat, pacmat1;
  
  virtual void SetUp() {
    fac1 = new TestFacility(tc.get());
    cap = 5;
    qty = 3;
    buff.capacity(cap);
    pacmat = PackagedMaterial::CreateUntracked(qty, pack);
    buff.Push(pacmat);
    pacmat1 = PackagedMaterial::CreateUntracked(qty, pack1);
  }

  virtual void TearDown() {
    delete fac1;
  }
};

/*
TEST_F(PackagedMatlSellPolicyTests, Init) {
  PackagedMatlSellPolicy p;

  // defaults
  p.Init(fac1, &buff, "");
  ASSERT_FALSE(p.Excl());
  ASSERT_FLOAT_EQ(p.Limit(), qty);

  // limit & excl
  p.Init(fac1, &buff, "", 1.5, false, 1);
  ASSERT_TRUE(p.Excl());
  ASSERT_FLOAT_EQ(p.Limit(), 1.5);

  // throughput = 2.5, quantize = 2, limit = 2
  p.Init(fac1, &buff, "", qty - 0.5, false, qty - 1);
  ASSERT_FLOAT_EQ(p.Limit(), qty - 1);
}

TEST_F(PackagedMatlSellPolicyTests, StartStop) {
  PackagedMatlSellPolicy p;
  p.Init(NULL, &buff, "");
  ASSERT_THROW(p.Start(), ValueError);
  ASSERT_THROW(p.Stop(), ValueError);
}

TEST_F(PackagedMatlSellPolicyTests, Bids) {
  PackagedMatlSellPolicy p;
  std::string commod("commod");  
  CommodMap<PackagedMaterial>::type reqs;
  reqs[commod] = std::vector<Request<PackagedMaterial>*>();
  Request<PackagedMaterial>* req = Request<PackagedMaterial>::Create(pacmat1, fac1, commod);
  reqs[commod].push_back(req);
  std::set<BidPortfolio<PackagedMaterial>::Ptr> obs;

  // basic
  p.Init(NULL, &buff, "").Set(commod);
  obs = p.GetPackagedMatlBids(reqs);
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ((*obs.begin())->bids().size(), 1);
  ASSERT_FLOAT_EQ((*(*obs.begin())->bids().begin())->offer()->quantity(),
                  pacmat->quantity());
  ASSERT_EQ((*(*obs.begin())->bids().begin())->offer()->quality(), pack);

  // excl and ignore_comp
  p.Init(NULL, &buff, "", qty, true, qty / 2).Set(commod);
  obs = p.GetPackagedMatlBids(reqs);
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ((*obs.begin())->bids().size(), 2);
  ASSERT_FLOAT_EQ((*(*obs.begin())->bids().begin())->offer()->quantity(),
                  pacmat->quantity() / 2);
  ASSERT_EQ((*(*obs.begin())->bids().begin())->offer()->quality(), pack1);
  
  delete req;
}

TEST_F(PackagedMatlSellPolicyTests, Trades) {
  PackagedMatlSellPolicy p;
  std::string commod("commod");  
  std::vector<Trade<PackagedMaterial> > trades;
  std::vector<std::pair<Trade<PackagedMaterial>, PackagedMaterial::Ptr> > obs;
  
  Request<PackagedMaterial>* req = Request<PackagedMaterial>::Create(pacmat1, fac1, commod);
  Bid<PackagedMaterial>* bid = Bid<PackagedMaterial>::Create(req, pacmat, fac1);
  Trade<PackagedMaterial> trade(req, bid, 1);
  trades.push_back(trade);
  
  // basic
  p.Init(NULL, &buff, "").Set(commod);
  p.GetPackagedMatlTrades(trades, obs);
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ(obs.begin()->second->quality(), pack);
  
  // ignore comp
  obs.clear();
  p.Init(NULL, &buff, "", qty, true).Set(commod);
  p.GetPackagedMatlTrades(trades, obs);
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ(obs.begin()->second->quality(), pack1);

  delete bid;
  delete req;
}
*/
}
}
