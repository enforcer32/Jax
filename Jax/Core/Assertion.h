#pragma once

#include "Jax/Core/Platform.h"
#include "Jax/Core/Logger.h"

#if defined JAX_DEBUG
	#ifdef JAX_PLATFORM_WINDOWS
		#define JAX_DEBUGBREAK() __debugbreak()	
	#else
		#error "Platform doesnt support debugbreak"
	#endif
	#define JAX_ENABLE_ASSERTS
#else
	#define JAX_DEBUGBREAK()
#endif

#ifdef JAX_ENABLE_ASSERTS
	#define JAX_ASSERT(x, msg) { if(!(x)) { JAX_LOG_ERROR("Assertion Error: {}, File: {}, Line: {}", msg, __FILE__, __LINE__); JAX_DEBUGBREAK(); } }
#else
	#define JAX_ASSERT(...)
#endif
