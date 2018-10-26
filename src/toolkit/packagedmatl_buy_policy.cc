#include "packagedmatl_buy_policy.h"

#include <sstream>

#include "error.h"

#define LG(X) LOG(LEV_##X, "buypol")
#define LGH(X)                                                    \
  LOG(LEV_##X, "buypol") << "policy " << name_ << " (agent "      \
                         << Trader::manager()->prototype() << "-" \
                         << Trader::manager()->id() << "): "

namespace cyclus {
namespace toolkit {

PackagedMatlBuyPolicy::PackagedMatlBuyPolicy() :
    Trader(NULL),
    name_(""),
    throughput_(std::numeric_limits<double>::max()),
    quantize_(-1),
    fill_to_(1),
    req_when_under_(1) {
  Warn<EXPERIMENTAL_WARNING>(
      "PackagedMatlBuyPolicy is experimental and its API may be subject to change");
}

PackagedMatlBuyPolicy::~PackagedMatlBuyPolicy() {
  if (manager() != NULL) 
    manager()->context()->UnregisterTrader(this);
}

void PackagedMatlBuyPolicy::set_fill_to(double x) {
  if (x > 1)
    x /= buf_->capacity();
  assert(x > 0 && x <= 1.);
  fill_to_ = x;
}

void PackagedMatlBuyPolicy::set_req_when_under(double x) {
  if (x > 1)
    x /= buf_->capacity();
  assert(x > 0 && x <= 1.);
  req_when_under_ = x;
}

void PackagedMatlBuyPolicy::set_quantize(double x) {
  assert(x != 0);
  quantize_ = x;
}

void PackagedMatlBuyPolicy::set_throughput(double x) {
  assert(x >= 0);
  throughput_ = x;
}

PackagedMatlBuyPolicy& PackagedMatlBuyPolicy::Init(Agent* manager, ResBuf<PackagedMaterial>* buf,
                                   std::string name) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  return *this;
}

PackagedMatlBuyPolicy& PackagedMatlBuyPolicy::Init(Agent* manager, ResBuf<PackagedMaterial>* buf,
                                   std::string name, double throughput) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_throughput(throughput);
  return *this;
}

PackagedMatlBuyPolicy& PackagedMatlBuyPolicy::Init(Agent* manager, ResBuf<PackagedMaterial>* buf,
                                   std::string name,
                                   double fill_to, double req_when_under) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_fill_to(fill_to);
  set_req_when_under(req_when_under);
  return *this;
}

PackagedMatlBuyPolicy& PackagedMatlBuyPolicy::Init(Agent* manager, ResBuf<PackagedMaterial>* buf,
                                   std::string name, double throughput,
                                   double fill_to, double req_when_under,
                                   double quantize) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_fill_to(fill_to);
  set_req_when_under(req_when_under);
  set_quantize(quantize);
  set_throughput(throughput);
  return *this;
}

PackagedMatlBuyPolicy& PackagedMatlBuyPolicy::Set(std::string commod) {
  //CompMap c;
  //c[10010000] = 1e-100;
  // do i need to put stuff inside pack?
  PackagedMaterial::package pack;
  return Set(commod, pack, 1.0);
}

PackagedMatlBuyPolicy& PackagedMatlBuyPolicy::Set(std::string commod, PackagedMaterial::package pack) {
  return Set(commod, pack, 1.0);
}

PackagedMatlBuyPolicy& PackagedMatlBuyPolicy::Set(std::string commod, PackagedMaterial::package pack,
                                  double pref) {
  CommodDetail d;
  d.pack = pack;
  d.pref = pref;
  commod_details_[commod] = d;
  return *this;
}

void PackagedMatlBuyPolicy::Start() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Buy Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->RegisterTrader(this);
}

void PackagedMatlBuyPolicy::Stop() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Buy Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->UnregisterTrader(this);
}

std::set<RequestPortfolio<PackagedMaterial>::Ptr> PackagedMatlBuyPolicy::GetPackagedMatlRequests() {
  rsrc_commods_.clear();
  std::set<RequestPortfolio<PackagedMaterial>::Ptr> ports;
  bool make_req = buf_->quantity() < req_when_under_ * buf_->capacity();
  double amt = TotalQty();
  if (!make_req || amt < eps())
    return ports;

  bool excl = Excl();
  double req_amt = ReqQty();
  int n_req = NReq();
  LGH(INFO3) << "requesting " << amt << " kg via " << n_req << " request(s)";

  // one portfolio for each request
  for (int i = 0; i != n_req; i++) {
    RequestPortfolio<PackagedMaterial>::Ptr port(new RequestPortfolio<PackagedMaterial>());
    std::map<int, std::vector<Request<PackagedMaterial>*> > grps;
    // one request for each commodity
    std::map<std::string, CommodDetail>::iterator it;
    for (it = commod_details_.begin(); it != commod_details_.end(); ++it) {
      std::string commod = it->first;
      CommodDetail d = it->second;
      LG(INFO3) << "  - one " << amt << " kg request of " << commod;
      PackagedMaterial::Ptr m = PackagedMaterial::CreateUntracked(req_amt, d.pack);
      grps[i].push_back(port->AddRequest(m, this, commod, d.pref, excl));
    }

    // if there's more than one commodity, then make them mutual
    if (grps.size() > 1) {
      std::map<int, std::vector<Request<PackagedMaterial>*> >::iterator grpit;
      for (grpit = grps.begin(); grpit != grps.end(); ++grpit) {
        port->AddMutualReqs(grpit->second);
      }
    }
    ports.insert(port);
  }
  
  return ports;
}

void PackagedMatlBuyPolicy::AcceptPackagedMatlTrades(
    const std::vector<std::pair<Trade<PackagedMaterial>, PackagedMaterial::Ptr> >& resps) {
  std::vector<std::pair<Trade<PackagedMaterial>, PackagedMaterial::Ptr> >::const_iterator it;
  rsrc_commods_.clear();
  for (it = resps.begin(); it != resps.end(); ++it) {
    rsrc_commods_[it->second] = it->first.request->commodity();
    LGH(INFO3) << "got " << it->second->quantity() << " kg of "
               << it->first.request->commodity();
    buf_->Push(it->second);
  }
}

}  // namespace toolkit
}  // namespace cyclus
