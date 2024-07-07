#pragma once

#include "Jax/Compiler/Compiler.h"
#include "Jax/Lexer/Token.h"

#include <vector>

namespace JAX
{
	enum class LexerResult
	{
		Successful,
		Failed
	};

	class Lexer
	{
	public:
		Lexer(const std::shared_ptr<CompilerInstance>& instance);
		LexerResult Lex();

	private:
		char NextChar();
		char PeekChar();
		void PutBack(char c);

	private:
		std::shared_ptr<CompilerInstance> m_CompilerInstance;
		Position m_Position;
		std::vector<Token> m_Tokens;
		uint32_t m_CurrentExpressionLevel;
		std::string m_CurrentParenthesisData;
	};
}
