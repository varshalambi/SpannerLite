
#include "marshallable.h"
#include <memory>
#include <mutex>
#include <unordered_map>

namespace janus {

std::mutex md_mutex_g;
std::mutex mdi_mutex_g;
shared_ptr<MarshallDeputy::MarContainer> mc_{nullptr};
thread_local shared_ptr<MarshallDeputy::MarContainer> mc_th_{nullptr};

int MarshallDeputy::RegInitializer(int32_t cmd_type,
                                   function<Marshallable * ()> init) {
  md_mutex_g.lock();
  auto pair = Initializers().insert(std::make_pair(cmd_type, init));
  verify(pair.second);
  md_mutex_g.unlock();
  return 0;
}

function<Marshallable * ()>
MarshallDeputy::GetInitializer(int32_t type) {
  if (!mc_th_) {
    mc_th_ = std::make_shared<MarshallDeputy::MarContainer>();
    md_mutex_g.lock();
    *mc_th_ = *mc_;
    md_mutex_g.unlock();
  }
  auto it = mc_th_->find(type);
  verify(it != mc_th_->end());
  auto f = it->second;
  return f;
}

MarshallDeputy::MarContainer&
MarshallDeputy::Initializers() {
  mdi_mutex_g.lock();
  if (!mc_)
    mc_ = std::make_shared<MarshallDeputy::MarContainer>();
  mdi_mutex_g.unlock();
  return *mc_;
};

Marshal &Marshallable::FromMarshal(Marshal &m) {
  verify(0);
  return m;
}

Marshal& MarshallDeputy::CreateActualObjectFrom(Marshal& m) {
  verify(sp_data_ == nullptr);
  switch (kind_) {
//    case CMD_TPC_PREPARE_CAROUSEL:
//      sp_data_.reset(new TpcPrepareCarouselCommand());
//      break;
    case UNKNOWN:
      verify(0);
      break;
    default:
      auto func = GetInitializer(kind_);
      verify(func);
      sp_data_.reset(func());
      break;
  }
  verify(sp_data_);
  sp_data_->FromMarshal(m);
  verify(sp_data_->kind_);
  verify(kind_);
  verify(sp_data_->kind_ == kind_);
  return m;
}

Marshal &Marshallable::ToMarshal(Marshal &m) const {
  verify(0);
  return m;
}

} // namespace janus
