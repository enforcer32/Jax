#include "Jax/Compiler/Compiler.h"
#include "Jax/Util/FileIO.h"

#include <iostream>

namespace JAX
{
	CompilerResult Compiler::CompileFile(const JaxProperties& jaxProperties)
	{
		// Setup
		m_CompilerProperties.InFilePath = jaxProperties.FilePath;
		m_CompilerProperties.InFileStream = std::istringstream(FileIO::ReadFileContent(jaxProperties.FilePath));
		m_CompilerProperties.OutFilePath = jaxProperties.OutPath;
		m_CompilerProperties.OutFileStream = std::ostringstream(jaxProperties.OutPath);
		m_CompilerProperties.Flags = jaxProperties.Flags;

		return CompilerResult::Successful;
	}
}
