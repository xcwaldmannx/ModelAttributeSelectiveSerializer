cmake_minimum_required(VERSION 3.12)
project(ModelAndAnimationLoader VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_DEBUG_POSTFIX d)

# ---- Vendors ----
set(VENDORS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ModelAndAnimationLoader/vendor")

include(FetchContent)

# Build everything static by default (affects assimp)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# ---- glm ----
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        8d1fd52e5ab5590e2c81768ace50c72bae28f2ed # 1.0.3
)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm)

# ---- assimp ----
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

find_package(assimp 6.0.5 REQUIRED)

# ---- nlohmann_json (header-only, vendored) ----
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json
        INTERFACE
        "${VENDORS_DIR}/json/include"
)
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)

# ---- Library ----
set(LIB_TARGET ModelAndAnimationLoader)

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/main/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/main/*.h"
)

add_library(${LIB_TARGET} STATIC ${LIB_SOURCES})

target_include_directories(${LIB_TARGET}
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/main"
)

# Link dependencies to the library.
# PUBLIC if your library headers include them (common for glm/json).
target_link_libraries(${LIB_TARGET}
        PUBLIC
        glm::glm
        nlohmann_json::nlohmann_json
        assimp::assimp
)

# ---- Executable ----
set(EXEC_TARGET ${PROJECT_NAME}_exec)

file(GLOB EXEC_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/exec/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/exec/*.h"
)

add_executable(${EXEC_TARGET} ${EXEC_SOURCES})
target_link_libraries(${EXEC_TARGET} PRIVATE ${LIB_TARGET})

# ---- Test ----
set(TEST_TARGET ${PROJECT_NAME}_test)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/test/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/test/*.h"
)

add_executable(${TEST_TARGET} ${TEST_SOURCES})
target_link_libraries(${TEST_TARGET} PRIVATE ${LIB_TARGET})

# ---- Install Process ----
include(GNUInstallDirs)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Install path" FORCE)

# --- Clean ----
install(CODE "
    message(STATUS \"Cleaning old install directory: \${CMAKE_INSTALL_PREFIX}\")

    # Safely prevent deleting your entire hard drive if the prefix is empty or root
    if(\"\${CMAKE_INSTALL_PREFIX}\" STREQUAL \"\" OR \"\${CMAKE_INSTALL_PREFIX}\" STREQUAL \"/\")
        message(FATAL_ERROR \"Safety check failed: CMAKE_INSTALL_PREFIX is empty or root!\")
    endif()

    # Delete the directory and recreate it empty
    file(REMOVE_RECURSE \"\${CMAKE_INSTALL_PREFIX}\")
    file(MAKE_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}\")
")

# ---- Install ----
install(TARGETS
        ${LIB_TARGET}
        ${EXEC_TARGET}
        ${TEST_TARGET}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/main/"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
)
