#include "Jax/Lexer/Lexer.h"

namespace JAX
{
	Lexer::Lexer(const std::shared_ptr<CompilerInstance>& instance)
		: m_CompilerInstance(instance), m_Position({ 1, 1, "" }), m_CurrentExpressionLevel(0), m_CurrentParenthesisData({})
	{
	}

	LexerResult Lexer::Lex()
	{
		return LexerResult::Successful;
	}

	char Lexer::NextChar()
	{
		char c = m_CompilerInstance->InFileStream.get();
		m_Position.Col++;
		if (c == '\n')
		{
			m_Position.Line++;
			m_Position.Col = 1;
		}
		return c;
	}

	char Lexer::PeekChar()
	{
		return m_CompilerInstance->InFileStream.peek();
	}

	void Lexer::PutBack(char c)
	{
		m_CompilerInstance->InFileStream.putback(c);
	}
}
