#include "Jax/Util/Logger.h"
#include "Jax/Compiler/Compiler.h"

int main()
{
	JAX::Logger::Init();
	JAX_LOG_INFO("Jax C Compiler");

	JAX::JaxProperties jaxProperties;
	jaxProperties.FilePath = "test.c";
	jaxProperties.OutPath = "test.out";

	JAX::Compiler compiler;
	if (compiler.CompileFile(jaxProperties) != JAX::CompilerResult::Successful)
		JAX_LOG_CRITICAL("Compiler failed to Compile");

	JAX_LOG_INFO("ALL COMPILATION SUCCESSFUL!");
	return 0;
}
