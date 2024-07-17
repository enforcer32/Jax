#pragma once

#include "Jax/Compiler/Common.h"

namespace JAX
{
	enum class NodeType
	{
		Number,
		String,
		Variable,
		VariableList,
		Identifier,
		Expression,
		ExpressionParantheses,
		Function,
		Body,
		Struct,
		Union,
		Label,
		Unary,
		Ternary,
		Bracket,
		Cast,
		StatementReturn,
		StatementSwitch,
		StatementCase,
		StatementDefault,
		StatementIf,
		StatementElse,
		StatementFor,
		StatementWhile,
		StatementDoWhile,
		StatementContinue,
		StatementBreak,
		StatementGoto,
		Blank,
	};

	struct Node
	{
		NodeType Type;
		uint32_t Flags;
		Position Position;

		struct NodeMetadata
		{
			Node* Function;
			Node* Body;
		} Metadata;

		union
		{
			char CVal;
			const char* SVal;
			unsigned int INum;
			unsigned long LNum;
			unsigned long long LLNum;
			void* Any;
		};
	};
}
