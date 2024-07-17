#pragma once

#include <string>
#include <iostream>

namespace JAX
{
	struct Position
	{
		uint32_t Line;
		uint32_t Col;
		std::string FileName;
	};

	enum class TokenType
	{
		Invalid,
		Keyword,
		Identifier,
		Operator,
		Symbol,
		Number,
		String,
		Comment,
		Newline
	};

	struct Token
	{
		TokenType Type;
		uint32_t Flags;
		Position Position;
		union
		{
			char CVal;
			const char* SVal;
			unsigned int INum;
			unsigned long LNum;
			unsigned long long LLNum;
			void* Any;
		};

		bool Whitespace;
		const char* BracketValue;

		Token(TokenType type = TokenType::Invalid)
			: Type(type), Any(nullptr), Flags(0), Whitespace(false)
		{
		}

		operator bool()
		{
			return Type != TokenType::Invalid;
		}

		friend std::ostream& operator<<(std::ostream& os, const Token& token)
		{
			std::string type;
			std::string value = "None";

			switch (token.Type)
			{
			case TokenType::Invalid:
				type = "Invalid";
				break;
			case TokenType::Keyword:
				type = "Keyword";
				break;
			case TokenType::Identifier:
				type = "Identifier";
				break;
			case TokenType::Operator:
				type = "Operator";
				break;
			case TokenType::Symbol:
				type = "Symbol";
				break;
			case TokenType::Number:
				type = "Number";
				value = std::to_string(token.LLNum);
				break;
			case TokenType::String:
				type = "String";
				value = token.SVal;
				break;
			case TokenType::Comment:
				type = "Comment";
				break;
			case TokenType::Newline:
				type = "Newline";
				break;
			default:
				type = "UNKNOWN";
				break;
			}

			os << "Token{Type: " << type << ", Value: " << value << ", Position: {Line: " << token.Position.Line << ", Column: " << token.Position.Col << ", FileName: " << token.Position.FileName << "}, Whitespace: " << token.Whitespace << "}";
			return os;
		}
	};
}
