# CMakeList.txt : CMake project for SelectiveModelAttributeExporter, include source and define
# project specific logic here.
#

include(FetchContent)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.h")

# Add source to this project's executable.
add_executable (SelectiveModelAttributeExporter ${SRC_FILES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SelectiveModelAttributeExporter PROPERTY CXX_STANDARD 20)
endif()

# include all vendor software below

set(VENDORS_DIR $ENV{VENDORS_DIR})

# disable dlls
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Link glm

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	    1.0.1
)

FetchContent_MakeAvailable(glm)

target_link_libraries(SelectiveModelAttributeExporter PRIVATE glm::glm)

# link assimp

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v6.0.2
)

FetchContent_MakeAvailable(assimp)

target_link_libraries(SelectiveModelAttributeExporter PRIVATE assimp::assimp)

# nlohmann json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.12.0
)

FetchContent_MakeAvailable(nlohmann_json)

target_link_libraries(SelectiveModelAttributeExporter PRIVATE nlohmann_json::nlohmann_json)
