#ifndef HEMELBSETUPTOOL_TEST_SECTIONTREE_HPP
#define HEMELBSETUPTOOL_TEST_SECTIONTREE_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "TestResources/Meshes.hpp"
#include "SectionTree.h"
#include "SectionTreeBuilder.h"

class SectionTreeTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SectionTreeTests);
  
  CPPUNIT_TEST(LocalOffsets);
  CPPUNIT_TEST(TinyFullTree);
  CPPUNIT_TEST(TinyCentreTree);
  CPPUNIT_TEST(Sphere);
  
  CPPUNIT_TEST_SUITE_END();

  // Tree with ALL leaf nodes
  std::shared_ptr<MaskTree> FullTree(MaskTree::Int lvls) {
    auto mt = std::make_shared<MaskTree>(lvls);
    MaskTree::Int n = 1 << lvls;
    for (auto i: range(n)) {
      for (auto j: range(n)) {
	for (auto k: range(n)) {
	  mt->GetCreate(i,j,k, 0)->Data() = true;
	}
      }
    }
    return mt;
  }
  // tree with central eighth of nodes
  std::shared_ptr<MaskTree> CentreTree(MaskTree::Int lvls) {
    CPPUNIT_ASSERT(lvls >= 2);
    
    auto mt = std::make_shared<MaskTree>(lvls);
    MaskTree::Int n = 1 << lvls;
    MaskTree::Int rmin = n / 4;
    MaskTree::Int rmax = 3 * rmin;
    for (auto i: range(rmin, rmax)) {
      for (auto j: range(rmin, rmax)) {
	for (auto k: range(rmin, rmax)) {
	  mt->GetCreate(i,j,k, 0)->Data() = true;
	}
      }
    }
    return mt;
  }
public:

  
  void LocalOffsets() {
    auto mt = FullTree(2);

    MaskTree::ConstNodePtr node;
    
    node = mt->Get(0,0,0, 0);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 0);
    node = mt->Get(0,0,2, 0);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 0);
    node = mt->Get(0,2,0, 0);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 0);
    node = mt->Get(0,2,2, 0);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 0);
    
    node = mt->Get(2,0,0, 0);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 0);
    node = mt->Get(2,0,2, 0);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 0);
    node = mt->Get(2,2,0, 0);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 0);
    node = mt->Get(2,2,2, 0);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 0);

    node = mt->Get(0,0,0, 1);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 0);
    node = mt->Get(0,0,2, 1);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 1);
    node = mt->Get(0,2,0, 1);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 2);
    node = mt->Get(0,2,2, 1);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 3);

    node = mt->Get(2,0,0, 1);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 4);
    node = mt->Get(2,0,2, 1);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 5);
    node = mt->Get(2,2,0, 1);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 6);
    node = mt->Get(2,2,2, 1);
    CPPUNIT_ASSERT(SectionTreeBuilder::LocalOffset(*node) == 7);
    
  }
  void TinyFullTree() {
    auto mt = FullTree(2);
    SectionTreeBuilder b(*mt);
    
    SectionTree::Ptr st = b();
    auto inds = st->GetTree();

    CPPUNIT_ASSERT_EQUAL(size_t(64), inds[0].size());
    for(auto i: range<uint64_t>(64))
      CPPUNIT_ASSERT_EQUAL(i, inds[0][i]);
    
  
    CPPUNIT_ASSERT_EQUAL(size_t(8), inds[1].size());
    for(auto i: range<uint64_t>(8))
      CPPUNIT_ASSERT_EQUAL(i*8, inds[1][i]);
  }

  void TinyCentreTree() {
    auto mt = CentreTree(3);
    
    SectionTreeBuilder b(*mt);
    SectionTree::Ptr st = b();
    
    auto inds = st->GetTree();
    
    CPPUNIT_ASSERT_EQUAL(size_t(64), inds[0].size());
    for(auto i: range<uint64_t>(64))
      CPPUNIT_ASSERT_EQUAL(i, inds[0][i]);

    CPPUNIT_ASSERT_EQUAL(size_t(64), inds[1].size());

    std::vector<uint64_t> specialdirs{7,6,5,4,3,2,1,0};
    
    for (auto i: range<uint64_t>(8)) {
      for (auto ii:range<uint64_t>(8)) {
	auto sd = specialdirs[i];
	CPPUNIT_ASSERT_EQUAL(ii==sd ? i*8 :SectionTree::NA(), inds[1][8*i + ii]);
      }
    }
    st->Write("tiny.oct");
  }

  void Sphere() {
    TriTree::Int levels = 5;
    TriTree::Int tri_level = 3;
    
    Vector sphere_centre(15.5);
    double sphere_radius = 10.0;
    auto r2 = sphere_radius*sphere_radius;
    
    auto sphere = SimpleMeshFactory::MkSphere();
    auto tree = TrianglesToTreeSerial(levels, tri_level, sphere->points,
				      sphere->triangles);
    
    SurfaceVoxeliser voxer(1 << tri_level, sphere->points,
			   sphere->triangles, sphere->normals, sphere->labels);
    auto fluid_tree = voxer(tree, tri_level);

    // Fill the thing
    FloodFill ff(fluid_tree);
    auto mask_tree = ff();

    SectionTreeBuilder builder(mask_tree);
    auto section_tree = builder();
  }
};
CPPUNIT_TEST_SUITE_REGISTRATION(SectionTreeTests);
#endif
