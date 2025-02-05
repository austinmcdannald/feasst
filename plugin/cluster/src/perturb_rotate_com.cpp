#include "utils/include/serialize.h"
#include "cluster/include/perturb_rotate_com.h"

namespace feasst {

class MapPerturbRotateCOM {
 public:
  MapPerturbRotateCOM() {
    auto obj = MakePerturbRotateCOM();
    obj->deserialize_map()["PerturbRotateCOM"] = obj;
  }
};

static MapPerturbRotateCOM mapper_ = MapPerturbRotateCOM();

PerturbRotateCOM::PerturbRotateCOM(argtype args) : PerturbRotateCOM(&args) {
  check_all_used(args);
}
PerturbRotateCOM::PerturbRotateCOM(argtype * args) : PerturbRotate(args) {
  class_name_ = "PerturbRotateCOM";
}

std::shared_ptr<Perturb> PerturbRotateCOM::create(std::istream& istr) const {
  return std::make_shared<PerturbRotateCOM>(istr);
}

PerturbRotateCOM::PerturbRotateCOM(std::istream& istr)
  : PerturbRotate(istr) {
  ASSERT(class_name_ == "PerturbRotateCOM", "name: " << class_name_);
  const int version = feasst_deserialize_version(istr);
  ASSERT(409 == version, "mismatch version: " << version);
}

void PerturbRotateCOM::serialize_perturb_rotate_com_(std::ostream& ostr) const {
  serialize_perturb_rotate_(ostr);
  feasst_serialize_version(409, ostr);
}

void PerturbRotateCOM::serialize(std::ostream& ostr) const {
  ostr << class_name_ << " ";
  serialize_perturb_rotate_com_(ostr);
}

void PerturbRotateCOM::move(const bool is_position_held,
    System * system,
    TrialSelect * select,
    Random * random) {
  if (is_position_held) return;
  const Position pivot = select->mobile().geometric_center();
  DEBUG("piv " << pivot.str());
  PerturbRotate::move(system, select, random, pivot);
}

}  // namespace feasst
