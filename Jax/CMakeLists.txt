file(GLOB_RECURSE JaxSources
	"${CMAKE_CURRENT_SOURCE_DIR}/Jax.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/Util/Platform.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Util/Assertion.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Util/Logger.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Util/Logger.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Util/FileIO.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Util/FileIO.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/Compiler/Compiler.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Compiler/Compiler.cpp"
)

add_definitions(-DJAX_DEBUG)

add_executable(${JAX_TARGET_NAME} ${JaxSources})
target_include_directories(${JAX_TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../")
target_link_libraries(${JAX_TARGET_NAME} PRIVATE spdlog)
