cmake_minimum_required(VERSION 3.1)
project(SofaComponentGeneral)

set(HEADER_FILES
    config.h
    initComponentGeneral.h
    )

set(SOURCE_FILES
    initComponentGeneral.cpp
    )

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
foreach(target ${SOFAGENERAL_TARGETS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${target})
endforeach()
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-DSOFA_BUILD_COMPONENT_GENERAL")
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

sofa_install_targets(SofaGeneral ${PROJECT_NAME} "SofaGeneral/${PROJECT_NAME}")
