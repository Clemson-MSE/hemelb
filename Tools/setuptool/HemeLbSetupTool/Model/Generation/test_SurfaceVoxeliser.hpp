// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.
#ifndef HEMELBSETUPTOOL_TEST_SURFACEVOXELISER_HPP
#define HEMELBSETUPTOOL_TEST_SURFACEVOXELISER_HPP

#include <cppunit/extensions/HelperMacros.h>
#include "TestResources/Meshes.hpp"
#include "TriangleSorter.h"
#include "SurfaceVoxeliser.h"
#include "Neighbours.h"

#include "range.hpp"
#include "enumerate.hpp"

class SurfaceVoxeliserTests: public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE (SurfaceVoxeliserTests);

	CPPUNIT_TEST (NeighbourInverses);
	CPPUNIT_TEST (Trivial);
	CPPUNIT_TEST (SphereIntersections);
	CPPUNIT_TEST (Sphere);

	CPPUNIT_TEST_SUITE_END();

public:
	void NeighbourInverses() {
		auto& vectors = Neighbours::GetDisplacements();
		auto& opps = Neighbours::GetOpposites();
		CPPUNIT_ASSERT(vectors.size() == 26);
		CPPUNIT_ASSERT(opps.size() == 26);
		for (auto i : range(26)) {
			CPPUNIT_ASSERT(vectors[i] + vectors[opps[i]] == Index::Zero());
		}
	}

	void Trivial() {
	  // 16 cube
	  auto levels = 4;
	  auto tri_level = 2;
	  auto triv = SimpleMeshFactory::MkTrivial();
	  auto tree = TrianglesToTreeSerial(levels, tri_level, triv->points,
					    triv->triangles);
	  
	  SurfaceVoxeliser voxer(1 << tri_level, triv->points, triv->triangles,
				 triv->normals, triv->labels, triv->iolets);
	  auto tri_node = tree.Get(0, 0, 0, tri_level);
	  
	  auto dirs = Neighbours::GetDisplacements();
	  
	  for (auto x: range(1, 3))
	    for (auto y: range(4))
	      for (auto z: range(4)) {
		EdgeSite outleaf;
		voxer.ComputeIntersectionsForSite(x,y,z, outleaf);
		
		// the 2 triangles are at x = 1.2
		// with y = {1.2, 2.2}
		//  and z = {1.2, 2.2}
		auto& cuts = outleaf.closest_cut;
		for (auto i: range(26)) {
		  if (cuts[i].id >= 0) {
		    double expected = dirs[i].x > 0 ? 0.2 : 0.8;
		    CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, cuts[i].dist, 1e-9);
		  }
		}
	      }
	  
	}

	void SphereIntersections() {
		TriTree::Int levels = 5;
		TriTree::Int tri_level = 3;

		Vector sphere_centre(10.5);
		double sphere_radius = 10.0;

		auto sphere = SimpleMeshFactory::MkSphere();
		auto tree = TrianglesToTreeSerial(levels, tri_level, sphere->points,
				sphere->triangles);

		SurfaceVoxeliser voxer(1 << tri_level, sphere->points,
				       sphere->triangles, sphere->normals, sphere->labels, sphere->iolets);

		// 24, 17, 20 is an arbitrary exterior point near the surface
		// Do this block containing this point
		// Coords in binary are (11000, 10001, 10100)
		// So tri_leve (=3) node is (24,16,16)
		auto in_box = tree.Get(24, 16, 16, tri_level);

		// Process it
		EdgeSite vox;
		voxer.ComputeIntersectionsForSite(24,17,20, vox);
		
		// Check it
		Index coord(24, 17, 20);

		const auto& cuts = vox.closest_cut;
		auto directions = Neighbours::GetDisplacements();
		for (auto i : range(26)) {
			const Index& dir = directions[i];
			switch (dir.x) {
			case 1:
				// All +x vectors must have no cut
				CPPUNIT_ASSERT(cuts[i].dist > 1.0);
				break;
			case 0:
				// x = our point, O = outside, I = inside
				// OOO
				// OxO
				// IIO
				if (dir.z == -1 && (dir.y != 1)) {
					// only these two inside
					CPPUNIT_ASSERT(cuts[i].dist < 1.0);
					CPPUNIT_ASSERT(cuts[i].id == 18);
				} else {
					// other 6 outside
					CPPUNIT_ASSERT(cuts[i].dist > 1.0);
				}
				break;
			case -1:
				// x = our point, O = outside, I = inside
				// I*O
				// IxI
				// III
				// * == inside but crossing tri 19
				if (dir.y == 1 && dir.z == 1) {
					// outside
					CPPUNIT_ASSERT(cuts[i].dist > 1.0);
				} else {
					// inside
					CPPUNIT_ASSERT(cuts[i].dist < 1.0);
					if (dir == Index(-1, 0, 1)) {
						CPPUNIT_ASSERT(cuts[i].id == 19);
					} else {
						CPPUNIT_ASSERT(cuts[i].id == 18);
					}
				}
				break;
			default:
				CPPUNIT_FAIL("Invalid direction");
			}
		}

	}

	void Sphere() {
		TriTree::Int levels = 5;
		TriTree::Int tri_level = 3;

		Vector sphere_centre(15.5);
		double sphere_radius = 10.0;

		auto sphere = SimpleMeshFactory::MkSphere();
		auto tree = TrianglesToTreeSerial(levels, tri_level, sphere->points,
				sphere->triangles);

		SurfaceVoxeliser voxer(1 << tri_level, sphere->points,
				       sphere->triangles, sphere->normals, sphere->labels, sphere->iolets);
		auto fluid_tree = voxer(tree, tri_level);

		// Assert things...
		// (24, 17, 20) is known to be outside so shouldn't exist
		CPPUNIT_ASSERT(!fluid_tree.Get(24, 17, 20, 0));

		fluid_tree.IterDepthFirst([](FluidTree::ConstNodePtr node) {
			// Has a valid data pointer iff a leaf node
			if (node->Level() == 0) {
				CPPUNIT_ASSERT(node->Data().leaf);
				CPPUNIT_ASSERT_EQUAL(1U, node->Data().count);
			} else {
				CPPUNIT_ASSERT(!node->Data().leaf);
				// No further tests for non-leaf
				return;
			}

			// Get the node's coordinate
			Vector posn(node->X(), node->Y(), node->Z());
			// Make relative to sphere centre
			posn -= 15.5;
			// All fluid points must be inside the sphere
			auto r2 = posn.GetMagnitudeSquared();
			CPPUNIT_ASSERT(r2 < 100.);
		});
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION (SurfaceVoxeliserTests);

#endif
