###############################################################################
#################################### begin google test ########################
###############################################################################

# Be sure to clear these each time
SET(GENERATED_TEST_LIST "" CACHE INTERNAL "")
SET(GENERATED_FILTER_LIST "" CACHE INTERNAL "")
SET(GENERATED_TEST_SOURCE_LIST "" CACHE INTERNAL "")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

SET(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject" FORCE)
SET(INSTALL_GTEST OFF CACHE BOOL "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" FORCE)
FetchContent_MakeAvailable(googletest)
INCLUDE_DIRECTORIES(${googletest_INCLUDE_DIRECTORIES})
INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}")

###############################################################################
##################################### end google test #########################
###############################################################################

###############################################################################
################################### begin cyclus testing ######################
###############################################################################

INCLUDE_DIRECTORIES(BEFORE ${CYCLUS_CORE_INCLUDE_DIRS})

set(CYCLUS_TEST_COIN_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/solver_factory_tests.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/prog_translator_tests.cc"
    )

FILE(GLOB cc_files "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

# Remove test source files that rely on COIN if we don't have it
if(NOT COIN_FOUND)
  foreach(ccfile ${CYCLUS_TEST_COIN_SRC})
    list(REMOVE_ITEM cc_files ${ccfile})
  endforeach()
endif()

SET(CYCLUS_CORE_TEST_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

FILE(GLOB header_files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB test_agents "${CMAKE_CURRENT_SOURCE_DIR}/test_agents/*.h")
INSTALL(FILES ${header_files}
    DESTINATION include/cyclus
    COMPONENT testing
    )

ADD_SUBDIRECTORY(input)
ADD_SUBDIRECTORY(toolkit)
ADD_SUBDIRECTORY(agent_tests)
SET(
    CYCLUS_CORE_TEST_SOURCE "${CYCLUS_CORE_TEST_SOURCE}"
    "${cc_files}"
    PARENT_SCOPE
    )
SET(
    CYCLUS_CORE_INCLUDE_DIRS "${CYCLUS_CORE_INCLUDE_DIRS}"
    "${CYCLUS_CORE_TEST_INCLUDE_DIRS}"
    "${GTEST_INCLUDE_DIRS}"
    PARENT_SCOPE
    )

SET(CYCLUS_TEST_LIBRARIES
    "${CYCLUS_TEST_LIBRARIES}" GTest::gtest GTest::gtest_main PARENT_SCOPE) # for stubs/agents

ADD_SUBDIRECTORY(test_agents)
INSTALL(FILES ${test_agents}
    DESTINATION include/cyclus/test_agents
    COMPONENT testing
    )

# read tests after building the driver, and add them to ctest
set(tgt "cyclus_unit_tests")

add_custom_target(
    ${tgt}-post-build
    DEPENDS ${tgt}
    COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/generate_gtest_macros.py"
    "--executable=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${tgt}"
    "--output=${CYCLUS_BINARY_DIR}/CTestTestfile.cmake"
    COMMENT "adding tests from ${tgt}"
    VERBATIM
    )

SET(CYCLUS_TEST_LIBRARIES "${CYCLUS_TEST_LIBRARIES}" PARENT_SCOPE)
SET(CYCLUS_AGENT_TEST_LIBRARIES "${CYCLUS_AGENT_TEST_LIBRARIES}" PARENT_SCOPE)

###############################################################################
#################################### end cyclus testing #######################
###############################################################################
