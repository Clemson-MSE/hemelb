#include <cppunit/extensions/HelperMacros.h>
#include "Oct.h"
#include <iostream>

class OctreeTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(OctreeTests);
  CPPUNIT_TEST(CreateEmpty);
  CPPUNIT_TEST(CreateSimple);
  CPPUNIT_TEST(SimpleIter);
  CPPUNIT_TEST_SUITE_END();
  
public:
  typedef Octree<char> Tree;
  typedef std::shared_ptr<Tree> TreePtr;

  TreePtr mk_8cube_234() {
    auto tree = TreePtr(new Tree(3));
    auto node = tree->GetCreate(2,3,4, 0);
    node->Data() = 1;
    return tree;
  }

  void CreateEmpty() {
    auto tree = Tree(3);
  }

  void CreateSimple() {
    auto tree = mk_8cube_234();
    for (auto i: {0, 4})
      for (auto j: {0, 4})
	for (auto k: {0, 4}) {
	  // Most of these should be nullptr
	  auto tmp = tree->Get(i, j, k, 2);
	  if (i==0 && j==0 && k==4)
	    // But not this one
	    CPPUNIT_ASSERT(tmp);
	  else
	    CPPUNIT_ASSERT(!tmp);
	}
    
    auto n2 = tree->Get(2, 2, 4, 1);
    CPPUNIT_ASSERT(n2);
    auto n3 = tree->Get(2, 3, 4, 0);
    CPPUNIT_ASSERT(n3);
    CPPUNIT_ASSERT(n3->Data() == 1);
  }
  
  class Counter : public Tree::Visitor {
  public:
    int i;
    
    Counter(): i(0) {}
    
    virtual void Do(Tree::Node& node) {
      CPPUNIT_ASSERT(node.Level() == i);
      ++i;
    }
    
  };
  
  void SimpleIter() {
    auto tree = mk_8cube_234();
    Tree::Int expected_nodes[4][3] = {{2,3,4},
				      {2,2,4},
				      {0,0,4},
				      {0,0,0}};
    //     i = 0
    // for node in tree.IterDepthFirst():
    //     assert node.levels == i
    //     assert np.all(node.offset == expected_nodes[i])
    //     i += 1
    
    // iterate down through all nodes
    int i = 0;
    tree->IterDepthFirst([&i, &expected_nodes](Tree::Node& node) mutable {
	
	CPPUNIT_ASSERT(node.Level() == i);
	CPPUNIT_ASSERT(node.X() == expected_nodes[i][0]);
	CPPUNIT_ASSERT(node.Y() == expected_nodes[i][1]);
	CPPUNIT_ASSERT(node.Z() == expected_nodes[i][2]);
    	++i;
	
      });
    CPPUNIT_ASSERT(i == 4);

    i = 1;
    tree->IterDepthFirst(1, [&i](Tree::Node& node) mutable {
	
	CPPUNIT_ASSERT(node.Level() == i);
    	++i;
	
      });
    CPPUNIT_ASSERT(i == 4);
    
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(OctreeTests);
