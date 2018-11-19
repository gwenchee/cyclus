#ifndef CYCLUS_TESTS_TEST_TRADER_H_
#define CYCLUS_TESTS_TEST_TRADER_H_

#include "agent.h"
#include "bid_portfolio.h"
#include "context.h"
#include "exchange_context.h"
#include "material.h"
#include "request_portfolio.h"
#include "resource_helpers.h"
#include "test_agents/test_facility.h"
#include "trade.h"
#include "packagedmaterial.h"

namespace cyclus {

class TestObjFactory {
 public:
  TestObjFactory() {
    mat = test_helpers::get_mat();
    commod = "commod";
  }

  Material::Ptr mat;
  PackagedMaterial::Ptr pacmat;
  std::string commod;
};

class TestTrader : public TestFacility {
 public:
  TestTrader(Context* ctx, TestObjFactory* fac = NULL, bool is_requester = true)
      : obj_fac(fac),
        is_requester(is_requester),
        TestFacility(ctx),
        offer(0),
        adjusts(0),
        requests(0),
        bids(0),
        accept(0) {}

  virtual Agent* Clone() {
    TestTrader* m = new TestTrader(context());
    m->InitFrom(this);
    return m;
  }

  void InitFrom(TestTrader* m) {
    TestFacility::InitFrom(this);
    adjusts = m->adjusts;
    requests = m->requests;
    bids = m->bids;
    accept = m->accept;
    offer = m->offer;
    obj_fac = m->obj_fac;
    is_requester = m->is_requester;
    context()->RegisterTimeListener(this);
  }

  virtual std::set<RequestPortfolio<Material>::Ptr>
      GetMatlRequests() {
    requests++;
    if (obj_fac == NULL || !is_requester) {
      return std::set<RequestPortfolio<Material>::Ptr>();
    } else {
      std::set<RequestPortfolio<Material>::Ptr> ports;
      RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
      req = port->AddRequest(obj_fac->mat, this, obj_fac->commod);  // exp request
      reqport = port;
      ports.insert(port);
      return ports;
    }
  }
/*
  virtual std::set<RequestPortfolio<PackagedMaterial>::Ptr>
      GetPackagedMatlRequests() {
    requests++;
    if (obj_fac == NULL || !is_requester) {
      return std::set<RequestPortfolio<PackagedMaterial>::Ptr>();
    } else {
      std::set<RequestPortfolio<PackagedMaterial>::Ptr> ports;
      RequestPortfolio<PackagedMaterial>::Ptr port(new RequestPortfolio<PackagedMaterial>());
      req = port->AddRequest(obj_fac->pacmat, this, obj_fac->commod);  // exp request
      reqport = port;
      ports.insert(port);
      return ports;
    }
  }  */

  virtual std::set<BidPortfolio<Material>::Ptr>
      GetMatlBids(CommodMap<Material>::type& commod_requests) {
    bids++;
    if (obj_fac == NULL || is_requester) {
      return std::set<BidPortfolio<Material>::Ptr>();
    } else {
      req = commod_requests.at(obj_fac->commod)[0];  // obs request

      std::set<BidPortfolio<Material>::Ptr> ports;
      BidPortfolio<Material>::Ptr port(new BidPortfolio<Material>());
      bid = port->AddBid(req, obj_fac->mat, this);  // exp bid
      bidport = port;
      ports.insert(port);
      return ports;
    }
  }

  virtual void AdjustMatlPrefs(PrefMap<Material>::type& prefs) {
    bid = (*prefs[req].begin()).first;  // obs bid
    adjusts++;
  }

  virtual void GetMatlTrades(
    const std::vector< Trade<Material> >& trades,
    std::vector<std::pair<Trade<Material>, Material::Ptr> >& responses) {
    std::vector< Trade<Material> >::const_iterator it;
    for (it = trades.begin(); it != trades.end(); ++it) {
      obs_trade = Trade<Material>(*it);
      responses.push_back(std::make_pair(*it, obj_fac->mat));
      offer++;
    }
  }

  virtual void AcceptMatlTrades(
      const std::vector<std::pair<Trade<Material>,
      Material::Ptr> >& responses) {
    if (responses.size() > 0) {
      obs_trade = Trade<Material>(responses.at(0).first);
      mat = responses.at(0).second;
    }
    accept += responses.size();
  }

  TestObjFactory* obj_fac;
  Request<Material>* req;  // obs or exp
  Bid<Material>* bid;  // obs or exp
  BidPortfolio<Material>::Ptr bidport;
  RequestPortfolio<Material>::Ptr reqport;
  Trade<Material> obs_trade;  // obs trade
  Material::Ptr mat;  // obs mat
  /*
  Request<PackagedMaterial>* pmreq;  // obs or exp
  Bid<PackagedMaterial>* pmbid;  // obs or exp
  BidPortfolio<PackagedMaterial>::Ptr pmbidport;
  RequestPortfolio<PackagedMaterial>::Ptr pmreqport;
  Trade<PackagedMaterial> pmobs_trade;  // obs trade
  Material::Ptr pacmat;  // obs mat
  */
  bool is_requester;
  int accept, offer, requests, bids, adjusts;
};

}  // namespace cyclus

#endif  // CYCLUS_TESTS_TEST_TRADER_H_
