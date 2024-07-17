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
		char PeekChar() const;
		void PutBack(char c);

		Token NextToken();
		Token ProcessWhitespace();
		Token ProcessNewLine();
		Token ProcessSlashOperator();
		Token ProcessSingleLineComment();
		Token ProcessMultiLineComment();
		Token MakeNumberToken();
		Token MakeStringToken(char sdelim, char edelim);
		Token MakeOperatorToken();
		Token MakeRegularOperatorToken();
		Token MakeIncludePreprocessorToken();
		Token MakeSymbolToken();
		Token MakeWordToken();
		Token MakeIdentifierToken();
		Token MakeQuoteToken();
		Token ProcessNumberBases();
		Token MakeHexNumberToken();
		Token MakeBinaryNumberToken();

		void CreateExpression();
		void EndExpression();
		bool IsInsideExpression() const;

		bool IsOperator(char op) const;
		bool IsSinglyOperator(char op) const;
		bool IsValidOperator(const std::string& op) const;
		bool IsKeyword(const std::string& str) const;
		bool IsHexNumber(char c) const;
		bool IsBinaryNumber(const std::string& str) const;
		char CharToEscapedChar(char c) const;

	private:
		std::shared_ptr<CompilerInstance> m_CompilerInstance;
		Position m_Position;
		std::vector<Token> m_Tokens;
		int32_t m_CurrentExpressionLevel;
		std::string m_CurrentExpressionData;
	};
}
