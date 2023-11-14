
file(GLOB zsp_arl_eval_SRC
  "*.h"
  "*.cpp"
  )

add_library(zsp-arl-eval SHARED ${zsp_arl_eval_SRC})

target_include_directories(zsp-arl-eval PUBLIC
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${debug_mgr_INCDIR}
    ${pyapi_compat_if_INCDIR}
    ${zsp_arl_dm_INCDIR}
    ${vsc_dm_INCDIR}
    ${vsc_solvers_INCDIR}
    )

target_link_directories(zsp-arl-eval PRIVATE
    ${CMAKE_BINARY_DIR}/lib
    ${CMAKE_BINARY_DIR}/lib64
    ${zsp_arl_dm_LIBDIR}
    ${vsc_dm_LIBDIR}
    ${vsc_solvers_LIBDIR}
    ${pyapi_compat_if_LIBDIR}
    ${debug_mgr_LIBDIR}
    )
target_link_libraries(zsp-arl-eval
    vsc-dm
    debug-mgr)

#add_dependencies(zsp-arl-eval 
#    LibVSC-DM 
#    LibVSC-Solvers)

install(TARGETS zsp-arl-eval
    DESTINATION lib
    EXPORT zsp-arl-eval-targets)

