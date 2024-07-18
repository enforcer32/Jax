#include "Jax/Parser/Parser.h"
#include "Jax/Util/Logger.h"

namespace JAX
{
	Parser::Parser(const std::shared_ptr<CompilerInstance>& instance)
		: m_CompilerInstance(instance), m_CurrentTokenIndex(0)
	{
	}

	ParserResult Parser::Parse()
	{
		Node node = NextNode();
		while (node)
		{
			m_Root.push_back(node);
			node = NextNode();
		}

		return ParserResult::Successful;
	}

	Token Parser::NextToken()
	{
		if (!HasToken())
			return {};

		Token token = m_CompilerInstance->LexerTokens[m_CurrentTokenIndex++];
		// Ignore Newline & Comments
		while (token && (token.Type == TokenType::Newline || token.Type == TokenType::Comment || (token.Type == TokenType::Symbol && token.CVal == '\\')))
		{
			if (IsLastToken())
				break;
			token = m_CompilerInstance->LexerTokens[m_CurrentTokenIndex++];
		}

		return m_LastToken = token;
	}

	Token Parser::PeekToken()
	{
		if (!HasToken())
			return {};
		
		Token token = m_CompilerInstance->LexerTokens[m_CurrentTokenIndex];
		// Ignore Newline & Comments
		while (token && (token.Type == TokenType::Newline || token.Type == TokenType::Comment || (token.Type == TokenType::Symbol && token.CVal == '\\')))
		{
			if (IsLastToken())
				break;
			token = m_CompilerInstance->LexerTokens[++m_CurrentTokenIndex];
		}
		return token;
	}

	bool Parser::HasToken() const
	{
		return m_CurrentTokenIndex < m_CompilerInstance->LexerTokens.size();
	}

	bool Parser::IsLastToken() const
	{
		return m_CurrentTokenIndex == m_CompilerInstance->LexerTokens.size() - 1;
	}

	Node Parser::NextNode()
	{
		Token token = PeekToken();
		if (!token)
			return {};

		Node node;
		switch (token.Type)
		{
		case TokenType::Number:
		case TokenType::String:
		case TokenType::Identifier:
			node = ParseBasicTypes();
			break;
		default:
			break;
		}

		return node;
	}

	Node Parser::ParseBasicTypes()
	{
		Node node;
		
		Token token = NextToken();
		if (token.Type == TokenType::Number)
		{
			node.Type = NodeType::Number;
			node.Position = token.Position;
			node.LLNum = token.LLNum;
		}
		else if (token.Type == TokenType::String)
		{
			node.Type = NodeType::String;
			node.Position = token.Position;
			node.SVal = token.SVal;
		}
		else if (token.Type == TokenType::Identifier)
		{
			node.Type = NodeType::Identifier;
			node.Position = token.Position;
			node.SVal = token.SVal;
		}
		else
		{
			JAX_LOG_CRITICAL("Parse Invalid Basic Type : {}\n on line {}, col {} in file {}", (int)token.Type, token.Position.Line, token.Position.Col, m_CompilerInstance->InFilePath);
		}

		return node;
	}
}
