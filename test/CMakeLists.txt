
include(GoogleTest)

find_package(Threads REQUIRED)

include (ExternalProject)

ExternalProject_Add(
  GTEST
  PREFIX gtest
  SOURCE_DIR ${PACKAGES_DIR}/gtest
  CMAKE_CACHE_ARGS
    -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}
    -DCMAKE_BUILD_TYPE:STRING=Release
  )
  
ExternalProject_Get_Property(GTEST source_dir binary_dir)

# find_package(Gtest)

file(GLOB test_CPP_SRC
  "src/*.cpp"
  )

add_custom_target(
    GEN_CODE_SNIPPETS
    COMMAND ${PYTHON} -m zsp_dataclasses.util.gen_cpp_dt_defs ${test_CPP_SRC} -o ${CMAKE_CURRENT_BINARY_DIR}/zspdefs -d ${CMAKE_CURRENT_BINARY_DIR}/zspdefs.d
)

add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest GTEST)

set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/lib/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
    )

add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)

set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/lib/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
    )
    
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/include
	"${source_dir}/googletest/include"
	"${source_dir}/googlemock/include"
    ${JSON_DIR}/include
    "${zsp_arl_dm_INCDIR}"
    "${vsc_dm_INCDIR}"
    "${vsc_solvers_INCDIR}"
    "${pyapi_compat_if_INCDIR}"
    "${debug_mgr_INCDIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/zspdefs"
	)

file(GLOB test_zsp_arl_eval_SRC
  "src/*.h"
  "src/*.cpp"
  )


add_executable(test-zsp-arl-eval ${test_zsp_arl_eval_SRC})
target_include_directories(test-zsp-arl-eval PUBLIC
    "${source_dir}/googletest/include"
    "${zsp_arl_dm_INCDIR}"
    "${vsc_dm_INCDIR}"
    "${vsc_solvers_INCDIR}"
    "${pyapi_compat_if_INCDIR}"
    "${debug_mgr_INCDIR}"
    )
   
target_link_directories(test-zsp-arl-eval PRIVATE
    ${CMAKE_BINARY_DIR}/lib
    ${CMAKE_BINARY_DIR}/lib64
    ${CMAKE_CURRENT_BINARY_DIR}/lib
    ${CMAKE_CURRENT_BINARY_DIR}/lib64
    "${zsp_arl_dm_LIBDIR}"
    "${vsc_dm_LIBDIR}"
    "${vsc_solvers_LIBDIR}"
    "${pyapi_compat_if_LIBDIR}"
    "${debug_mgr_LIBDIR}"
    )

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(libglog glogd)
else()
	set(libglog glog)
endif()

target_link_libraries(test-zsp-arl-eval 
    zsp-arl-eval
	zsp-arl-dm
    vsc-solvers
    vsc-dm
    pyapi-compat-if
    debug-mgr
	libgtest 
	libgmock
	gtest_main 
    dl
	)
add_dependencies(test-zsp-arl-eval GTEST GEN_CODE_SNIPPETS)
#set_property(TARGET test_libvsc PROPERTY INTERPROCEDURAL_OPTIMIZATION OFF)

gtest_discover_tests(test-zsp-arl-eval)
