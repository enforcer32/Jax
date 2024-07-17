#include "Jax/Parser/Parser.h"

namespace JAX
{
	Parser::Parser(const std::shared_ptr<CompilerInstance>& instance)
		: m_CompilerInstance(instance)
	{
	}

	ParserResult Parser::Parse()
	{
		return ParserResult::Successful;
	}
}
