cmake_minimum_required(VERSION 3.1)
project(SofaPreconditioner VERSION 1.0)

# Dependencies
find_package(SofaCommon REQUIRED) # SofaImplicitOdeSolver SofaSimpleFem
find_package(SofaGeneral REQUIRED) # SofaGeneralLinearSolver
sofa_find_package(SofaSparseSolver QUIET) # Soft dependency of PrecomputedWarpPreconditioner

# Config
set(HEADER_FILES
    src/SofaPreconditioner/config.h
    )
set(SOURCE_FILES
    src/SofaPreconditioner/initPreconditioner.cpp
    )

# Sources
list(APPEND HEADER_FILES
    src/SofaPreconditioner/BlockJacobiPreconditioner.h
    src/SofaPreconditioner/BlockJacobiPreconditioner.inl
    src/SofaPreconditioner/JacobiPreconditioner.h
    src/SofaPreconditioner/JacobiPreconditioner.inl
    src/SofaPreconditioner/PrecomputedWarpPreconditioner.h
    src/SofaPreconditioner/PrecomputedWarpPreconditioner.inl
    src/SofaPreconditioner/SSORPreconditioner.h
    src/SofaPreconditioner/SSORPreconditioner.inl
    src/SofaPreconditioner/ShewchukPCGLinearSolver.h
    src/SofaPreconditioner/WarpPreconditioner.h
    src/SofaPreconditioner/WarpPreconditioner.inl
    )
list(APPEND SOURCE_FILES
    src/SofaPreconditioner/BlockJacobiPreconditioner.cpp
    src/SofaPreconditioner/JacobiPreconditioner.cpp
    src/SofaPreconditioner/PrecomputedWarpPreconditioner.cpp
    src/SofaPreconditioner/SSORPreconditioner.cpp
    src/SofaPreconditioner/ShewchukPCGLinearSolver.cpp
    src/SofaPreconditioner/WarpPreconditioner.cpp
    )

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC SofaImplicitOdeSolver SofaGeneralLinearSolver SofaSimpleFem)
if(SofaSparseSolver_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC SofaSparseSolver)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE "-DSOFA_BUILD_SOFAPRECONDITIONER")
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<INSTALL_INTERFACE:include>")

sofa_generate_package(
    NAME SofaPreconditioner
    VERSION ${PROJECT_VERSION}
    TARGETS SofaPreconditioner
    INCLUDE_SOURCE_DIR "src/SofaPreconditioner"
    INCLUDE_INSTALL_DIR "SofaPreconditioner"
    RELOCATABLE "plugins"
    )
