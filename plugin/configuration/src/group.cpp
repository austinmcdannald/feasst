
#include "configuration/include/group.h"
#include "utils/include/utils.h"
#include "utils/include/serialize.h"

namespace feasst {

Group::Group(argtype args) {
  std::string start;
  // if only one site type, allow drop the subscript
  start.assign("site_type");
  if (used(start, args)) {
    site_types_.push_back(integer(start, &args));
  } else {
    int type = site_types_.size();
    std::stringstream key;
    key << start << type;
    while (used(key.str(), args)) {
      site_types_.push_back(integer(key.str(), &args));
      ++type;
      ASSERT(type < 1e8, "type(" << type << ") is very high. Infinite loop?");
      key.str("");
      key << start << type;
    }
  }

  // if only one particle type, allow drop the subscript
  start.assign("particle_type");
  if (used(start, args)) {
    particle_types_.push_back(integer(start, &args));
  } else {
    int type = particle_types_.size();
    std::stringstream key;
    key << start << type;
    while (used(key.str(), args)) {
      particle_types_.push_back(integer(key.str(), &args));
      ++type;
      ASSERT(type < 1e8, "type(" << type << ") is very high. Infinite loop?");
      key.str("");
      key << start << type;
    }
  }

  dynamic_ = boolean("dynamic", &args, true);
  spatial_ = boolean("spatial", &args, false);
  ASSERT(!spatial_, "spatial groups are not implemented");
  check_all_used(args);
}

bool Group::is_empty() const {
  if ( (particle_types_.size() == 0) and
       (site_types_.size() == 0) ) {
    return true;
  }
  return false;
}

bool Group::is_in(const Site& site) const {
  if (site_types_.size() == 0) {
    return true;
  }
  if (find_in_list(site.type(), site_types_)) {
    return true;
  }
  return false;
}

bool Group::is_in(const Particle& particle) const {
  if (particle_types_.size() == 0) {
    return true;
  }
  if (find_in_list(particle.type(), particle_types_)) {
    return true;
  }
  return false;
}

void Group::remove_sites(Particle * particle) const {
//                         std::vector<int> * full_to_partial,
//                         std::vector<int> * partial_to_full) const {
//  // compute site mappings (if requested) before removal
//  if (full_to_partial != NULL and partial_to_full != NULL) {
//    int partial_site = 0;
//    for (int index = 0; index < particle->num_sites(); ++index) {
//      if (is_in(particle->site(index))) {
//        (*full_to_partial).push_back(partial_site);
//        ++partial_site;
//        (*partial_to_full).push_back(index);
//      } else {
//        (*full_to_partial).push_back(-1);
//      }
//    }
//  }

  // loop backward to remove sites
  for (int index = particle->num_sites() - 1; index >= 0; --index) {
    if (!is_in(particle->site(index))) {
      particle->remove_site(index);
    }
  }
}

//Particle Group::remove_sites(const Particle& particle) const {
////                             std::vector<int> * full_to_partial,
////                             std::vector<int> * partial_to_full) const {
//  Particle filtered(particle);
//  remove_sites(&filtered);//, full_to_partial, partial_to_full);
//  return filtered;
//}

std::vector<int> Group::site_indices(const Particle& particle) const {
  std::vector<int> indices;
  for (int index = 0; index < particle.num_sites(); ++index) {
    if (is_in(particle.site(index))) {
      indices.push_back(index);
    }
  }
  return indices;
}

void Group::serialize(std::ostream& ostr) const {
  ostr << "1 "; // version
  feasst_serialize(site_types_, ostr);
  feasst_serialize(particle_types_, ostr);
  ostr << dynamic_ << " " << spatial_ << " ";
}

Group::Group(std::istream& istr) {
  int version;
  istr >> version;
  feasst_deserialize(&site_types_, istr);
  feasst_deserialize(&particle_types_, istr);
  istr >> dynamic_ >> spatial_;
}

}  // namespace feasst
