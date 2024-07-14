#include "Jax/Lexer/Lexer.h"
#include "Jax/Util/Logger.h"

namespace JAX
{
	Lexer::Lexer(const std::shared_ptr<CompilerInstance>& instance)
		: m_CompilerInstance(instance), m_Position({ 1, 1, instance->InFilePath }), m_CurrentExpressionLevel(0), m_CurrentParenthesisData({})
	{
	}

	LexerResult Lexer::Lex()
	{
		Token token = NextToken();
		while (token)
		{
			m_Tokens.push_back(token);
			token = NextToken();
		}

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

	char Lexer::PeekChar() const
	{
		return m_CompilerInstance->InFileStream.peek();
	}

	void Lexer::PutBack(char c)
	{
		m_CompilerInstance->InFileStream.putback(c);
	}

	Token Lexer::NextToken()
	{
		Token token;

		char c = PeekChar();
		switch (c)
		{
			// Whitespace
		case ' ':
		case '\t':
			token = ProcessWhitespace();
			break;

			// Numbers
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			token = MakeNumberToken();
			break;

			// String
		case '"':
			token = MakeStringToken('"', '"');
			break;

			// Operators
		case '+':
		case '-':
		case '*':
		case '%':
		case '=':
		case '<':
		case '>':
		case '!':
		case '&':
		case '|':
		case '^':
		case '~':
		case '[':
		case '(':
		case '?':
		case ',':
		case '.':
			token = MakeOperatorToken(); // Doesnt Handle #include <X>
			break;

			// Symbols
		case '{':
		case '}':
		case ':':
		case ';':
		case '#':
		case '\\':
		case ')':
		case ']':
			token = MakeSymbolToken();
			break;

		case EOF:
			break;
		default:
			JAX_LOG_CRITICAL("Lexer Unexpected Token : {}\n on line {}, col {} in file {}", c, m_Position.Line, m_Position.Col, m_CompilerInstance->InFilePath);
		}

		return token;
	}

	Token Lexer::ProcessWhitespace()
	{
		if (m_Tokens[m_Tokens.size() - 1])
			m_Tokens[m_Tokens.size() - 1].Whitespace = true;
		NextChar();
		return NextToken();
	}

	Token Lexer::MakeNumberToken()
	{
		std::string numbers;
		for (char c = PeekChar(); c >= '0' && c <= '9'; c = PeekChar())
		{
			numbers.push_back(c);
			NextChar();
		}

		Token token;
		token.Type = TokenType::Number;
		token.Position = m_Position;
		token.LLNum = std::atoi(numbers.c_str());
		return token;
	}

	Token Lexer::MakeStringToken(char sdelim, char edelim)
	{
		if (NextChar() != sdelim)
			JAX_LOG_CRITICAL("Lexer: Bad String Delimiter!");

		std::string str;
		char c = NextChar();
		for (; c != edelim && c != EOF; c = NextChar())
		{
			// Escape Characters
			if (c == '\\')
				continue;
			str.push_back(c);
		}

		Token token;
		token.Type = TokenType::String;
		token.Position = m_Position;
		token.SVal = new char[str.size() + 1];
		memcpy((char*)token.SVal, str.c_str(), str.size() + 1);
		return token;
	}

	Token Lexer::MakeOperatorToken()
	{
		char op = PeekChar();
		if (op == '<')
			if (!m_Tokens.empty())
				if (m_Tokens.back().Type == TokenType::Keyword && std::strcmp(m_Tokens.back().SVal, "include"))
					return MakeIncludePreprocessorToken();
		return MakeRegularOperatorToken();
	}

	Token Lexer::MakeRegularOperatorToken()
	{
		bool singleOperator = true;

		char op = NextChar();
		std::string opStr{ op };

		if (!IsSinglyOperator(op))
		{
			// Stackable Operator: i.e ++
			op = PeekChar();
			if (IsOperator(op))
			{
				opStr += op;
				singleOperator = false;
				NextChar();
			}
		}

		// PutBack All Except First One
		if (!singleOperator && !IsValidOperator(opStr))
			for (size_t i = opStr.size() - 1; i >= 1; i--)
				PutBack(opStr[i]);

		if (singleOperator && !IsValidOperator(opStr))
			JAX_LOG_CRITICAL("Lexer Invalid Operator : {}\n on line {}, col {} in file {}", opStr, m_Position.Line, m_Position.Col, m_CompilerInstance->InFilePath);

		Token token;
		token.Type = TokenType::Operator;
		token.Position = m_Position;
		token.SVal = new char[opStr.size() + 1];
		memcpy((char*)token.SVal, opStr.c_str(), opStr.size() + 1);
		return token;
	}

	Token Lexer::MakeIncludePreprocessorToken()
	{
		return MakeStringToken('<', '>');
	}

	Token Lexer::MakeSymbolToken()
	{
		char c = NextChar();
		Token token;
		token.Type = TokenType::Symbol;
		token.Position = m_Position;
		token.CVal = c;
		return token;
	}

	bool Lexer::IsOperator(char op) const
	{
		return (op == '+' || op == '-' || op == '*' || 
			op == '%' || op == '=' || op == '<' || 
			op == '>' || op == '!' || op == '&' || 
			op == '|' || op == '^' || op == '~' || 
			op == '[' || op == '(' || op == '?' || 
			op == ',' || op == '.');
	}

	bool Lexer::IsSinglyOperator(char op) const
	{
		return (op == ',' || op == '.' || op == '(' || op == '[' || op == '?' || op == '*');
	}

	bool Lexer::IsValidOperator(const std::string& op) const
	{
		return (op == "+" || op == "-" || op == "*" ||
			op == "/" || op == "%" || op == "!" ||
			op == "^" || op == "~" || op == "?" ||
			op == "(" || op == "[" || op == "," ||
			op == "." || op == "<" || op == ">" ||
			op == "|" || op == "&" || op == "+=" ||
			op == "-=" || op == "*=" || op == "/=" ||
			op == ">>" || op == "<<" ||	op == ">=" ||
			op == "<=" || op == "||" ||	op == "&&" ||
			op == "++" || op == "--" ||	op == "=" ||
			op == "!=" || op == "==" ||	op == "->" ||
			op == ">>=" || op == "<<=" || op == "...");
	}
}
