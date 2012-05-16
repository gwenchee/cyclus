// Transaction.cpp

#include "Transaction.h"

#include "Model.h"
#include "MarketModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction::Transaction(Model* creator, TransType type) {
  minfrac_ = 0;
  price_ = 0;

  supplier_ = NULL;
  requester_ = NULL;
  resource_ = NULL;

  type_ = type;
  if (type == OFFER) {
    supplier_ = creator;
  } else {
    requester_ = creator
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Transaction::~Transaction() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel* Transaction::market() {
  MarketModel* market = MarketModel::marketForCommod(commod_);
  return market;
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Transaction::supplier() const {
  if (supplier_ == NULL) {
    string err_msg = "Uninitilized message supplier.";
    throw CycNullMsgParamException(err_msg);
  }
  return supplier_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setSupplier(Model* supplier) {
  supplier_ = supplier;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Transaction::requester() const {
  if (requester_ == NULL) {
    string err_msg = "Uninitilized message requester.";
    throw CycNullMsgParamException(err_msg);
  }
  return requester_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setRequester(Model* requester) {
  requester_ = requester;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Transaction::commod() const {
  return commod_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setCommod(std::string newCommod) {
  commod_ = newCommod;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Transaction::isOffer() const {
  return type_ == OFFER;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Transaction::price() const {
  return price_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setPrice(double newPrice) {
  price_ = newPrice;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rsrc_ptr Transaction::resource() const {
  return resource_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Transaction::setResource(rsrc_ptr newResource) {
  if (newResource.get()) {
    resource_ = newResource->clone();
  }
}

