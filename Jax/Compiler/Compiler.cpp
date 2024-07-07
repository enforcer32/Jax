#include "Jax/Compiler/Compiler.h"
#include "Jax/Lexer/Lexer.h"
#include "Jax/Util/FileIO.h"
#include "Jax/Util/Logger.h"

#include <iostream>

namespace JAX
{
	CompilerResult Compiler::CompileFile(const JaxProperties& jaxProperties)
	{
		// Setup
		m_CompilerInstance = std::make_shared<CompilerInstance>();
		m_CompilerInstance->InFilePath = jaxProperties.FilePath;
		m_CompilerInstance->InFileStream = std::istringstream(FileIO::ReadFileContent(jaxProperties.FilePath));
		m_CompilerInstance->OutFilePath = jaxProperties.OutPath;
		m_CompilerInstance->OutFileStream = std::ostringstream(jaxProperties.OutPath);
		m_CompilerInstance->Flags = jaxProperties.Flags;

		// Lexer
		Lexer lexer(m_CompilerInstance);
		if (lexer.Lex() != LexerResult::Successful)
		{
			JAX_LOG_ERROR("JAXLexer Failed!");
			return CompilerResult::Failed;
		}

		return CompilerResult::Successful;
	}
}
