#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "prometheus/exposer.h"

#include "CivetServer.h"
#include "handler.h"
#include "metrics.pb.h"

namespace prometheus {

static const auto uri = std::string{"/metrics"};

Exposer::Exposer(const std::string& bind_address)
    : exposer_registry_(std::make_shared<Registry>()),
      metrics_handler_(
          new detail::MetricsHandler{collectables_, *exposer_registry_}) {
  RegisterCollectable(exposer_registry_);
  rebind(bind_address);
}

Exposer::~Exposer() { server_->removeHandler(uri); }

void
Exposer::rebind(const std::string& bind_address) {
  server_.reset(new CivetServer{{
      "listening_ports", bind_address.c_str(),
      "num_threads", "1",
  }});
  server_->addHandler(uri, metrics_handler_.get());
}

void Exposer::RegisterCollectable(
    const std::weak_ptr<Collectable>& collectable) {
  collectables_.push_back(collectable);
}
}
