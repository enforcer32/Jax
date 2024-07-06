#include "Jax/Util/Logger.h"
#include "Jax/Compiler/Compiler.h"

int main()
{
	JAX::Logger::Init();
	JAX_LOG_INFO("Jax C Compiler");

	JAX::JaxProperties jaxProperties;
	JAX::Compiler compiler;
	if (compiler.CompileFile(jaxProperties) != JAX::CompilerResult::Successful)
		JAX_LOG_CRITICAL("Compiler failed to Compile");

	return 0;
}
