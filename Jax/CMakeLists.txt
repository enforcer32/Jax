file(GLOB_RECURSE JaxSources
	"${CMAKE_CURRENT_SOURCE_DIR}/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.inl"
	"${CMAKE_CURRENT_SOURCE_DIR}/**/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/**/*.cxx"
	"${CMAKE_CURRENT_SOURCE_DIR}/**/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/**/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/**/*.inl"
)

add_definitions(-DJAX_DEBUG)

add_executable(${JAX_TARGET_NAME} ${JaxSources})
target_include_directories(${JAX_TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../")
target_link_libraries(${JAX_TARGET_NAME} PRIVATE spdlog)
