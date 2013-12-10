
#include <algorithm> 

#include "cyc_std.h"
#include "error.h"
#include "logger.h"
#include "timer.h"

#include "context.h"

namespace cyclus {

Context::Context(Timer* ti, EventManager* em)
    : ti_(ti), em_(em), trans_id_(0) {};

boost::uuids::uuid Context::sim_id() {
  return em_->sim_id();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Context::GetModelByName(std::string name) {
  Model* found_model = NULL;

  const std::vector<Model*>& models = model_list();
  
  for (int i = 0; i < models.size(); i++) {
    if (name == models.at(i)->name()) {
      found_model = models.at(i);
      break;
    }
  }

  if (found_model == NULL) {
    std::string err_msg = "Model '" + name + "' doesn't exist.";
    throw KeyError(err_msg);
  }
  return found_model;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Context::ProcessCommodities() {
  double max = std::max_element(
      commodity_order_.begin(),
      commodity_order_.end(),
      SecondLT< std::pair<std::string, double> >())->second;
  if (max < 1) max = 0; // in case no orders are specified
  
  std::map<std::string, double>::iterator it;
  for (it = commodity_order_.begin();
       it != commodity_order_.end();
       ++it) {
    if (it->second < 1) it->second = max + 1;
    CLOG(LEV_INFO1) << "Commodity ordering for " << it->first
                    << " is " << it->second;
  }
}

void Context::AddPrototype(std::string name, Model* p) {
  protos_[name] = p;
}

void Context::AddRecipe(std::string name, Composition::Ptr c) {
  recipes_[name] = c;
};

Composition::Ptr Context::GetRecipe(std::string name) {
  if (recipes_.count(name) == 0) {
    throw KeyError("Invalid recipe name " + name);
  }
  return recipes_[name];
};

void Context::InitTime(int start, int duration, int decay, int m0, int y0,
                       std::string handle) {
  ti_->Initialize(this, duration, m0, y0, start, decay, handle);
};

int Context::time() {
  return ti_->time();
};

int Context::start_time() {
  return ti_->start_time();
};

int Context::sim_dur() {
  return ti_->dur();
};

void Context::RegisterTicker(TimeAgent* ta) {
  ti_->RegisterTickListener(ta);
};

Event* Context::NewEvent(std::string title) {
  return em_->NewEvent(title);
};

} // namespace cyclus
