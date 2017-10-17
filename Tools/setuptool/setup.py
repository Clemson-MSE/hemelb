# This file is part of HemeLB and is Copyright (C)
# the HemeLB team and/or their institutions, as detailed in the
# file AUTHORS. This software is provided under the terms of the
# license in the file LICENSE.

import sys
import os.path
from setuptools import setup
from setuptools.extension import Extension

lib_src = '''
../../Code/util/Vector3D.cc
HemeLbSetupTool/Model/Generation/FloodFill.cpp
HemeLbSetupTool/Model/Generation/H5.cpp
HemeLbSetupTool/Model/Generation/MkCgalMesh.cpp
HemeLbSetupTool/Model/Generation/Neighbours.cpp
HemeLbSetupTool/Model/Generation/PolyDataGenerator.cpp
HemeLbSetupTool/Model/Generation/SectionTree.cpp
HemeLbSetupTool/Model/Generation/SectionTreeBuilder.cpp
HemeLbSetupTool/Model/Generation/SegmentFactory.cpp
HemeLbSetupTool/Model/Generation/SurfaceVoxeliser.cpp
HemeLbSetupTool/Model/Generation/TriTree.cpp
HemeLbSetupTool/Model/Generation/TriangleSorter.cpp
HemeLbSetupTool/Model/Generation/Vector.cpp
'''.strip().split('\n')


generation_src = lib_src + ['HemeLbSetupTool/Model/Generation.i']
test_src = lib_src + ['HemeLbSetupTool/Model/Generation/test.cpp',
                          'HemeLbSetupTool/Model/Test.i']

if sys.platform == 'darwin':
    # Python thinks it's so smart and sets the
    # MACOSX_DEPLOYMENT_TARGET environment variable that messes around
    # with what features of the compiler and C++ std lib are
    # available. Set this to use your current one.
    import platform
    release, versioninfo, machine = platform.mac_ver()
    os.environ['MACOSX_DEPLOYMENT_TARGET'] = release

main_libs = ['boost_system', 'boost_filesystem', 'hdf5', 'CGAL', 'gmp', 'mpfr']

generation_ext = Extension(
    'HemeLbSetupTool.Model._Generation',
    sources=generation_src,
    libraries=main_libs,
    extra_compile_args=['--std=c++11']
    )
test_ext = Extension(
    'HemeLbSetupTool.Model._Test',
    sources=test_src,
    libraries=main_libs+['cppunit'],
    extra_compile_args=['--std=c++11']
    )

setup(
    name='HemeLbSetupTool',
    version='2.0',
    author='Rupert Nash',
    author_email='r.nash@epcc.ed.ac.uk',
    packages=[
        'HemeLbSetupTool',
        'HemeLbSetupTool.Bindings',
        'HemeLbSetupTool.Util',
        'HemeLbSetupTool.Model',
        'HemeLbSetupTool.View',
        'HemeLbSetupTool.Controller'
        ],
    scripts=[
        'scripts/hemelb-setup',
        'scripts/hemelb-setup-nogui',
        'scripts/hemelb-countsites',
        'scripts/upgrade-profile'],
    ext_modules=[generation_ext, test_ext]
    )
