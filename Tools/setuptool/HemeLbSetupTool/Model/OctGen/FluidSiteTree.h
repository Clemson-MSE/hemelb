// -*- mode: c++; -*-
#ifndef HEMELBSETUPTOOL_FLUIDSITETREE_H
#define HEMELBSETUPTOOL_FLUIDSITETREE_H

#include <memory>
#include <array>

#include "Oct.h"
#include "util/Vector3D.h"

enum class Intersection {
	None = 0,
	Wall = 1,
	Inlet = 2,
	Outlet = 3
};

// This holds the data for a single link from a fluid site
struct Link {
	inline Link() : type(Intersection::None),
			dist(std::numeric_limits<float>::infinity()),
			id(-1) {
	}
	Intersection type;
	float dist;
	int id;
};

inline std::ostream& operator<<(std::ostream& os, const Link& lnk) {
  os << '(' << static_cast<int>(lnk.type) << ',' << lnk.dist << ',' << lnk.id << ')';
  return os;
}

template<size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<Link,N>& lnk_ar) {
  auto iter = lnk_ar.begin();
  os << *iter;
  
  for (;iter != lnk_ar.end(); ++iter)
    os << "," << *iter;
  return os;
}

// Single precision vector
typedef hemelb::util::Vector3D<float> SVector;

// This will hold the data for a single fluid site ready to be written
struct FluidSite {
	inline FluidSite() : links(), has_normal(false), normal() {
	}

	std::array<Link, 26> links;
	SVector normal;
	bool has_normal;
};

typedef std::shared_ptr<FluidSite> FluidSitePtr;
struct FluidData {
	inline FluidData() : count(0), leaf() {
	}

	unsigned count;
	FluidSitePtr leaf;
};
typedef Octree<FluidData> FluidTree;

#endif // HEMELBSETUPTOOL_FLUIDSITETREE_H
