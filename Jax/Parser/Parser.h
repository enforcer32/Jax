#pragma once

#include "Jax/Compiler/Compiler.h"
#include "Jax/Parser/Node.h"

#include <vector>

namespace JAX
{
	enum class ParserResult
	{
		Successful,
		Failed
	};

	class Parser
	{
	public:
		Parser(const std::shared_ptr<CompilerInstance>& instance);
		ParserResult Parse();

	private:
		std::shared_ptr<CompilerInstance> m_CompilerInstance;
		std::vector<Node> m_Nodes;
		std::vector<Node> m_Root;
	};
}
