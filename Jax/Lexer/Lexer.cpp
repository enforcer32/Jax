#include "Jax/Lexer/Lexer.h"
#include "Jax/Util/Logger.h"

#include <vector>

namespace JAX
{
	Lexer::Lexer(const std::shared_ptr<CompilerInstance>& instance)
		: m_CompilerInstance(instance), m_Position({ 1, 1, instance->InFilePath }), m_CurrentExpressionLevel(0), m_CurrentExpressionData({})
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

		if (IsInsideExpression())
			m_CurrentExpressionData += c;

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

		case '\n':
			token = ProcessNewLine();
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

			// Other Base Numbers
		case 'x':
		case 'b':
			token = ProcessNumberBases();
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

			// Slash Operator
		case '/':
			token = ProcessSlashOperator();
			break;

		case '\'':
			token = MakeQuoteToken();
			break;

			// others
		case EOF:
			break;

		default:
			// Identifiers & Keywords
			token = MakeWordToken();
			if(!token)
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

	Token Lexer::ProcessNewLine()
	{
		NextChar();
		Token token;
		token.Type = TokenType::Newline;
		token.Position = m_Position;
		if(IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
		return token;
	}

	Token Lexer::ProcessSlashOperator()
	{
		char c = PeekChar();
		if (c == '/')
		{
			NextChar();

			if (PeekChar() == '/')
			{
				NextChar();
				return ProcessSingleLineComment();
			}
			else if (PeekChar() == '*')
			{
				NextChar();
				return ProcessMultiLineComment();
			}

			PutBack('/');
			return MakeOperatorToken();
		}

		return {};
	}

	Token Lexer::ProcessSingleLineComment()
	{
		std::string comment;

		for (char c = PeekChar(); (c != '\n' && c != EOF); c = PeekChar())
		{
			comment += c;
			c = NextChar();
		}

		Token token;
		token.Type = TokenType::Comment;
		token.Position = m_Position;
		token.SVal = new char[comment.size() + 1];
		memcpy((char*)token.SVal, comment.c_str(), comment.size() + 1);
		if (IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
		return token;
	}

	Token Lexer::ProcessMultiLineComment()
	{
		std::string comment;

		while (true)
		{
			char c;
			for (c = PeekChar(); (c != '*' && c != EOF); c = PeekChar())
			{
				comment += c;
				c = NextChar();
			}

			if (c == EOF)
			{
				JAX_LOG_CRITICAL("Lexer Bad MultiLine Comment Format : {}\n on line {}, col {} in file {}", comment, m_Position.Line, m_Position.Col, m_CompilerInstance->InFilePath);
			}
			else if (c == '*')
			{
				NextChar();
				if (PeekChar() == '/')
				{
					NextChar();
					break;
				}
			}
		}

		Token token;
		token.Type = TokenType::Comment;
		token.Position = m_Position;
		token.SVal = new char[comment.size() + 1];
		memcpy((char*)token.SVal, comment.c_str(), comment.size() + 1);
		if (IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
		return token;
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
		if (IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
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
		if (IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
		return token;
	}

	Token Lexer::MakeOperatorToken()
	{
		char op = PeekChar();
		if (op == '<')
			if (!m_Tokens.empty())
				if (m_Tokens.back().Type == TokenType::Keyword && std::strcmp(m_Tokens.back().SVal, "include"))
					return MakeIncludePreprocessorToken();

		if (op == '(')
			CreateExpression();

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
		if (IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
		return token;
	}

	Token Lexer::MakeIncludePreprocessorToken()
	{
		return MakeStringToken('<', '>');
	}

	Token Lexer::MakeSymbolToken()
	{
		char c = NextChar();
		if (c == ')')
			EndExpression();

		Token token;
		token.Type = TokenType::Symbol;
		token.Position = m_Position;
		token.CVal = c;
		if (IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
		return token;
	}

	Token Lexer::MakeWordToken()
	{
		char c = PeekChar();
		if (std::isalpha(c) || c == '_')
			return MakeIdentifierToken();
		return {};
	}

	Token Lexer::MakeIdentifierToken()
	{
		std::string identifier;
		
		for (char c = PeekChar(); ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'); c = PeekChar())
		{
			identifier += c;
			c = NextChar();
		}

		Token token;
		token.Type = (IsKeyword(identifier) ? TokenType::Keyword : TokenType::Identifier);
		token.Position = m_Position;
		token.SVal = new char[identifier.size() + 1];
		memcpy((char*)token.SVal, identifier.c_str(), identifier.size() + 1);
		if (IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
		return token;
	}

	Token Lexer::MakeQuoteToken()
	{
		NextChar();

		char c = NextChar();
		if (c == '\\')
		{
			c = NextChar();
			c = CharToEscapedChar(c);
		}

		if (NextChar() != '\'')
			JAX_LOG_CRITICAL("Lexer Bad Quote Format : {}\n on line {}, col {} in file {}", c, m_Position.Line, m_Position.Col, m_CompilerInstance->InFilePath);
	
		Token token;
		token.Type = TokenType::Number;
		token.Position = m_Position;
		token.CVal = c;
		if (IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
		return token;
	}

	Token Lexer::ProcessNumberBases()
	{
		if (m_Tokens.empty() || !(m_Tokens.back().Type == TokenType::Number && m_Tokens.back().LLNum == 0))
			return MakeIdentifierToken();

		Token lastToken = m_Tokens.back();
		m_Tokens.pop_back();

		char c = PeekChar();
		if (c == 'x')
			return MakeHexNumberToken();
		else if (c == 'b')
			return MakeBinaryNumberToken();
	}

	Token Lexer::MakeHexNumberToken()
	{
		NextChar();

		std::string hexStr{};
		for (char c = PeekChar(); IsHexNumber(c); c = PeekChar())
		{
			hexStr += c;
			NextChar();
		}

		Token token;
		token.Type = TokenType::Number;
		token.Position = m_Position;
		token.LNum = std::strtol(hexStr.c_str(), 0, 16);
		if (IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
		return token;
	}

	Token Lexer::MakeBinaryNumberToken()
	{
		NextChar();

		std::string binaryStr;
		for (char c = PeekChar(); c >= '0' && c <= '9'; c = PeekChar())
		{
			binaryStr.push_back(c);
			NextChar();
		}

		if (!IsBinaryNumber(binaryStr))
			JAX_LOG_CRITICAL("Lexer Bad Binary Number Format : {}\n on line {}, col {} in file {}", binaryStr, m_Position.Line, m_Position.Col, m_CompilerInstance->InFilePath);

		Token token;
		token.Type = TokenType::Number;
		token.Position = m_Position;
		token.LNum = std::strtol(binaryStr.c_str(), 0, 2);
		if (IsInsideExpression())
			token.BracketValue = m_CurrentExpressionData.c_str();
		return token;
	}

	void Lexer::CreateExpression()
	{
		m_CurrentExpressionLevel++;
		if (m_CurrentExpressionLevel == 1)
		{
			m_CurrentExpressionData = "";
		}
	}

	void Lexer::EndExpression()
	{
		m_CurrentExpressionLevel--;
		if (m_CurrentExpressionLevel < 0)
			JAX_LOG_CRITICAL("Lexer Bad Expression Format : {}\n on line {}, col {} in file {}", m_CurrentExpressionLevel, m_Position.Line, m_Position.Col, m_CompilerInstance->InFilePath);
	}

	bool Lexer::IsInsideExpression() const
	{
		return m_CurrentExpressionLevel > 0;
	}

	bool Lexer::IsOperator(char op) const
	{
		return (op == '+' || op == '-' || op == '*' || 
			op == '%' || op == '=' || op == '<' || 
			op == '>' || op == '!' || op == '&' || 
			op == '|' || op == '^' || op == '~' || 
			op == '[' || op == '(' || op == '?' || 
			op == ',' || op == '.' || op == '/');
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

	bool Lexer::IsKeyword(const std::string& str) const
	{
		static const std::vector<std::string> keywords = {
			"auto", "break", "case", "char",
			"const", "continue", "default", "do",
			"double", "else", "enum", "extern",
			"float", "for", "goto", "if",
			"int", "long", "register", "return",
			"short", "signed", "sizeof", "static",
			"struct", "switch", "typedef", "union",
			"unsigned", "void", "volatile", "while",
			"include", "restrict", "__ignore_typecheck",
		};
		return (std::find(keywords.begin(), keywords.end(), str) != keywords.end());
	}

	bool Lexer::IsHexNumber(char c) const
	{
		c = std::tolower(c);
		return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
	}

	bool Lexer::IsBinaryNumber(const std::string& str) const
	{
		for (const auto& ch : str)
			if (ch != '0' && ch != '1')
				return false;
		return true;
	}

	char Lexer::CharToEscapedChar(char c) const
	{
		char ch = 0;
		switch (c)
		{
		case 'n':
			ch = '\n';
			break;
		case 't':
			ch = '\t';
			break;
		case '\\':
			ch = '\\';
			break;
		case '\'':
			ch = '\'';
			break;
		}
		return ch;
	}
}
