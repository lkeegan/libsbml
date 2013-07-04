####################################################################
#
# SBML comp package - include files to build comp
#
# $Author$
# $Id$
# $HeadURL$
#

if(ENABLE_COMP)

include(${CMAKE_SOURCE_DIR}/comp-package.cmake)

#build up sources
set(COMP_SOURCES)

# go through all directtories: common, extension, sbml and util
foreach(dir common extension sbml util validator validator/constraints)

	# add to include directory
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sbml/packages/comp/${dir})
	
	# file sources
	file(GLOB current ${CMAKE_CURRENT_SOURCE_DIR}/sbml/packages/comp/${dir}/*.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/sbml/packages/comp/${dir}/*.h)
	
	# add sources 
	set(COMP_SOURCES ${COMP_SOURCES} ${current})
	
	# mark header files for installation 
	file(GLOB comp_headers ${CMAKE_CURRENT_SOURCE_DIR}/sbml/packages/comp/${dir}/*.h)
    install(FILES ${comp_headers} DESTINATION include/sbml/packages/comp/${dir})	
	
endforeach()

# create source group for IDEs
source_group(comp_package FILES ${COMP_SOURCES})

# add comp sources to SBML sources
SET(LIBSBML_SOURCES ${LIBSBML_SOURCES} ${COMP_SOURCES})

####################################################################
#
# add test scripts
#
if(WITH_CHECK)

		add_subdirectory(sbml/packages/comp/extension/test)
		add_subdirectory(sbml/packages/comp/sbml/test)
		add_subdirectory(sbml/packages/comp/util/test)
		add_subdirectory(sbml/packages/comp/validator/test)

endif()

endif()