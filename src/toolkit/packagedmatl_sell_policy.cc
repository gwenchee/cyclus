#include "packagedmatl_sell_policy.h"

#include "error.h"

#define LG(X) LOG(LEV_##X, "selpol")
#define LGH(X)                                                    \
  LOG(LEV_##X, "selpol") << "policy " << name_ << " (agent "      \
                         << Trader::manager()->prototype() << "-" \
                         << Trader::manager()->id() << "): "

namespace cyclus {
namespace toolkit {

PackagedMatlSellPolicy::PackagedMatlSellPolicy() :
    Trader(NULL),
    name_(""),
    quantize_(-1),
    throughput_(std::numeric_limits<double>::max()),
    ignore_comp_(false) {
  Warn<EXPERIMENTAL_WARNING>(
      "PackagedMatlSellPolicy is experimental and its API may be subject to change");
}

PackagedMatlSellPolicy::~PackagedMatlSellPolicy() {
  if (manager() != NULL)
    manager()->context()->UnregisterTrader(this);
}

void PackagedMatlSellPolicy::set_quantize(double x) {
  assert(x != 0);
  quantize_ = x;
}

void PackagedMatlSellPolicy::set_throughput(double x) {
  assert(x >= 0);
  throughput_ = x;
}

void PackagedMatlSellPolicy::set_ignore_comp(bool x) {
  ignore_comp_ = x;
}

PackagedMatlSellPolicy& PackagedMatlSellPolicy::Init(Agent* manager, ResBuf<PackagedMaterial>* buf,
                                     std::string name) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  return *this;
}

PackagedMatlSellPolicy& PackagedMatlSellPolicy::Init(Agent* manager, ResBuf<PackagedMaterial>* buf,
                                     std::string name, double throughput) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_throughput(throughput);
  return *this;
}

PackagedMatlSellPolicy& PackagedMatlSellPolicy::Init(Agent* manager, ResBuf<PackagedMaterial>* buf,
                                     std::string name, bool ignore_comp) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_ignore_comp(ignore_comp);
  return *this;
}

PackagedMatlSellPolicy& PackagedMatlSellPolicy::Init(Agent* manager, ResBuf<PackagedMaterial>* buf,
                                     std::string name, double throughput,
                                     bool ignore_comp) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_throughput(throughput);
  set_ignore_comp(ignore_comp);
  return *this;
}

PackagedMatlSellPolicy& PackagedMatlSellPolicy::Init(Agent* manager, ResBuf<PackagedMaterial>* buf,
                                     std::string name, double throughput,
                                     bool ignore_comp, double quantize) {
  Trader::manager_ = manager;
  buf_ = buf;
  name_ = name;
  set_quantize(quantize);
  set_throughput(throughput);
  set_ignore_comp(ignore_comp);
  return *this;
}

PackagedMatlSellPolicy& PackagedMatlSellPolicy::Set(std::string commod) {
  commods_.insert(commod);
  return *this;
}

void PackagedMatlSellPolicy::Start() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Sell Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->RegisterTrader(this);
}

void PackagedMatlSellPolicy::Stop() {
  if (manager() == NULL) {
    std::stringstream ss;
    ss << "No manager set on Sell Policy " << name_;
    throw ValueError(ss.str());
  }
  manager()->context()->UnregisterTrader(this);
}


double PackagedMatlSellPolicy::Limit() const {
  double bcap = buf_->quantity();
  double limit = Excl() ?                                               \
                 quantize_ * static_cast<int>(std::floor(bcap / quantize_)) : bcap;
  return std::min(throughput_, limit);
}

std::set<BidPortfolio<PackagedMaterial>::Ptr> PackagedMatlSellPolicy::GetPackagedMatlBids(
    CommodMap<PackagedMaterial>::type& commod_requests) {
  std::set<BidPortfolio<PackagedMaterial>::Ptr> ports;
  if (buf_->empty() || buf_->quantity() < eps())
    return ports;

  std::cout << "in packaged material sell" << std::endl;

  BidPortfolio<PackagedMaterial>::Ptr port(new BidPortfolio<PackagedMaterial>());

  double limit = Limit();
  CapacityConstraint<PackagedMaterial> cc(limit);
  port->AddConstraint(cc);
  ports.insert(port);
  LGH(INFO3) << "bidding out " << limit << " kg";
  
  bool excl = Excl();
  std::string commod;
  Request<PackagedMaterial>* req;
  PackagedMaterial::Ptr pm, offer;
  double qty;
  int nbids;
  std::set<std::string>::iterator sit;
  std::vector<Request<PackagedMaterial>*>::const_iterator rit;
  for (sit = commods_.begin(); sit != commods_.end(); ++sit) {
    commod = *sit;
    if (commod_requests.count(commod) < 1)
      continue;

    const std::vector<Request<PackagedMaterial>*>& requests =
        commod_requests.at(commod);
    for (rit = requests.begin(); rit != requests.end(); ++rit) {
      req = *rit;
      qty = std::min(req->target()->quantity(), limit);
      nbids = excl ? static_cast<int>(std::floor(qty / quantize_)) : 1;
      qty = excl ? quantize_ : qty;
      for (int i = 0; i < nbids; i++) {
        pm = buf_->Pop();
        buf_->Push(pm);
        offer = PackagedMaterial::CreateUntracked(qty, pm->quality());
        port->AddBid(req, offer, this, excl);
        LG(INFO3) << "  - bid " << qty << " kg on a request for " << commod;
      }
    }
  }
  return ports;
}

void PackagedMatlSellPolicy::GetPackagedMatlTrades(
    const std::vector<Trade<PackagedMaterial> >& trades,
    std::vector<std::pair<Trade<PackagedMaterial>, PackagedMaterial::Ptr> >& responses) {
  std::vector<Trade<PackagedMaterial> >::const_iterator it;
  for (it = trades.begin(); it != trades.end(); ++it) {
    double qty = it->amt;
    LGH(INFO3) << " sending " << qty << " kg of " << it->request->commodity();
    PackagedMaterial::Ptr pacmat = buf_->Pop(qty, cyclus::eps_rsrc());
    responses.push_back(std::make_pair(*it, pacmat));
  }
}

}  // namespace toolkit
}  // namespace cyclus
