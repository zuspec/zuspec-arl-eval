
file(GLOB zsp_arl_eval_SRC
  "*.h"
  "*.cpp"
  )

add_library(zsp-arl-eval SHARED ${zsp_arl_eval_SRC})

target_include_directories(zsp-arl-eval PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/include
    ${DEBUG_MGR_DIR}/src/include
    ${LIBVSC_DM_DIR}/src/include
    ${JSON_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    )

target_link_directories(zsp-arl-eval PRIVATE
    ${CMAKE_BINARY_DIR}/lib
    ${CMAKE_BINARY_DIR}/lib64
    )
target_link_libraries(zsp-arl-eval
    vsc-dm
    debug-mgr)

add_dependencies(zsp-arl-eval 
    LibVSC-DM 
    ZSP-ARL-DM
    LibVSC-Solvers
    DebugMgr)

install(TARGETS zsp-arl-eval
    DESTINATION lib
    EXPORT zsp-arl-eval-targets)

