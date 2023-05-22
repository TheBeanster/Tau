#include "tau_code.h"



const char* tau_keyword_strings[Tau_NUM_KEYWORDS] =
{
	"if",
	"then",
	"else",
	"for",
	"do",
	"while",
	"function",
	"return",
	"false",
	"true",
	"end"
};

const char* tau_keywordid_names[Tau_NUM_KEYWORDS] =
{
	"KW_IF",
	"KW_THEN",
	"KW_ELSE",
	"KW_FOR",
	"KW_DO",
	"KW_WHILE",
	"KW_FUNCTION",
	"KW_RETURN",
	"KW_FALSE",
	"KW_TRUE",
	"KW_END"
};

Tau_KeywordID Tau_GetKeyword(const char* string)
{
	for (int i = 0; i < Tau_NUM_KEYWORDS; i++)
		if (strcmp(string, tau_keyword_strings[i]) == 0)
			return i;
	return Tau_KW_NULL;
}



const char* tau_operator_strings[Tau_NUM_OPERATORS] =
{
	"=",
	"+=",
	"-=",
	"*=",
	"/=",

	"!",
	"&",
	"|",
	"^",
	"", /* Unary negative has a special case */

	"==",
	"!=",
	"<",
	">",
	"<=",
	">=",

	"+",
	"-",
	"*",
	"/",

	".",
	"", /* Array access has a special case */
};

const char* tau_operatorid_names[Tau_NUM_OPERATORS] =
{
	"OP_ASSIGN",
	"OP_ASSIGNADDITION",
	"OP_ASSIGNSUBTRACTION",
	"OP_ASSIGNMULTIPLICATION",
	"OP_ASSIGNDIVISION",

	"OP_NOT",
	"OP_AND",
	"OP_OR",
	"OP_XOR",
	"OP_NEGATIVE",

	"OP_EQUALITY",
	"OP_INEQUALITY",
	"OP_LESS",
	"OP_GREATER",
	"OP_LESSEQUAL",
	"OP_GREATEREQUAL",

	"OP_ADDITION",
	"OP_SUBTRACTION",
	"OP_MULTIPLICATION",
	"OP_DIVISION",

	"OP_MEMBERACCESS",
	"OP_ARRAYACCESS"
};

Tau_OperatorID Tau_GetOperator(const char* string)
{
	for (int i = 0; i < Tau_NUM_OPERATORS; i++)
		if (strcmp(string, tau_operator_strings[i]) == 0)
			return i;
	return Tau_OP_NULL;
}



const char tau_separator_chars[Tau_NUM_SEPARATORS] =
{
	'(',
	')',
	'[',
	']',
	'{',
	'}',
	',',
};

const char* tau_separatorid_names[Tau_NUM_SEPARATORS] =
{
	"SP_LPAREN",
	"SP_RPAREN",
	"SP_LBRACKET",
	"SP_RBRACKET",
	"SP_LBRACE",
	"SP_RBRACE",
	"SP_COMMA",
};

Tau_SeparatorID Tau_GetSeparator(char c)
{
	for (int i = 0; i < Tau_NUM_SEPARATORS; i++)
		if (c == tau_separator_chars[i])
			return i;
	return Tau_SP_NULL;
}





void Tau_DestroyExpression(Tau_ExprNode* expr)
{
	Tau_FREE(expr);
}