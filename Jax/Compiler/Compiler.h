#pragma once

#include <string>
#include <sstream>
#include <memory>

namespace JAX
{
	struct JaxProperties
	{
		std::string FilePath;
		std::string OutPath;
		uint32_t Flags;
	};

	struct CompilerInstance
	{
		std::string InFilePath;
		std::istringstream InFileStream;
		std::string OutFilePath;
		std::ostringstream OutFileStream;
		uint32_t Flags;
	};

	enum class CompilerResult
	{
		Successful,
		Failed
	};

	class Compiler
	{
	public:
		CompilerResult CompileFile(const JaxProperties& jaxProperties);

	private:
		JaxProperties m_JaxProperties;
		std::shared_ptr<CompilerInstance> m_CompilerInstance;
	};
}
