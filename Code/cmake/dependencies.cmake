# This file is part of HemeLB and is Copyright (C)
# the HemeLB team and/or their institutions, as detailed in the
# file AUTHORS. This software is provided under the terms of the
# license in the file LICENSE.

#------Parmetis  ------------
find_package(Parmetis REQUIRED)
include_directories(${PARMETIS_INCLUDE_DIRS})

#------TinyXML ----------------
find_package(TinyXML REQUIRED)
OPTION(TIXML_USE_STL "Use STL with TIXML" ON)
if(TIXML_USE_STL)
  add_definitions(-DTIXML_USE_STL)
endif()
include_directories(${TINYXML_INCLUDE_DIR})
#------BOOST ------------------
find_package(Boost 1.54 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
#------CTemplate ----------------
find_package(CTemplate REQUIRED)
include_directories(${CTEMPLATE_INCLUDE_DIR})
if(HEMELB_BUILD_MULTISCALE)
  #------MPWide ----------------
  find_package(MPWide REQUIRED)
  include_directories(${MPWide_INCLUDE_DIR})
  add_definitions(-DHEMELB_BUILD_MULTISCALE)
endif()

#------zlib ----------------
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})
