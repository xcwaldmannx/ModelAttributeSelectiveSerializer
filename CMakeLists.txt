cmake_minimum_required(VERSION 3.12)
project(ModelAttributeSelectiveSerializer VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# ---- Vendors ----
set(VENDORS_DIR $ENV{VENDORS_DIR})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# glm (header only)
add_subdirectory("${VENDORS_DIR}/glm" "${CMAKE_BINARY_DIR}/vendors/glm-build")

# assimp (compiled lib)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "")
set(ASSIMP_INSTALL OFF CACHE INTERNAL "")
add_subdirectory("${VENDORS_DIR}/assimp" "${CMAKE_BINARY_DIR}/vendors/assimp-build")

# nlohmann_json (header only)
add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
set_target_properties(nlohmann_json::nlohmann_json PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${VENDORS_DIR}/json/include"
)

# ---- Library ----
set(LIB_TARGET ModelAttributeSelectiveSerializer)
file(GLOB_RECURSE LIB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/main/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/main/*.h"
)

add_library(${LIB_TARGET} STATIC ${LIB_SOURCES})

target_include_directories(${LIB_TARGET}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/main"
        "${VENDORS_DIR}/json/include"
        "${VENDORS_DIR}/assimp/include"
        "${CMAKE_BINARY_DIR}/vendors/assimp-build/include"
        "${VENDORS_DIR}/glm"
)


target_link_libraries(${LIB_TARGET}
    PRIVATE
        assimp
        glm
        nlohmann_json::nlohmann_json
)

# ---- Executable ----
set(EXEC_TARGET ${PROJECT_NAME}_exec)
file(GLOB EXEC_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/exec/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/exec/*.h"
)
add_executable(${EXEC_TARGET} ${EXEC_SOURCES})
target_link_libraries(${EXEC_TARGET} PRIVATE ${LIB_TARGET})

# ---- Test ----
set(TEST_TARGET ${PROJECT_NAME}_test)
file(GLOB TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/test/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/test/*.h"
)
add_executable(${TEST_TARGET} ${TEST_SOURCES})
target_link_libraries(${TEST_TARGET} PRIVATE ${LIB_TARGET})

# ---- Install ----
include(GNUInstallDirs)

# Set install output root (relative to project dir)
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Install path")

# ---- Install targets ----
install(TARGETS
    ${LIB_TARGET}
    ${EXEC_TARGET}
    ${TEST_TARGET}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}     # bin/
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}     # lib/
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}     # lib/ (for static libs)
)

# ---- Install headers ----
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/main/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
        PATTERN "*.h"
)
