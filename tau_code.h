#ifndef _Tau_CODE_H_
#define _Tau_CODE_H_

#include "tau_utility.h"



typedef char Tau_KeywordID;
enum
{
	Tau_KW_IF,
	Tau_KW_THEN,
	Tau_KW_ELSE,
	Tau_KW_FOR,
	Tau_KW_DO,
	Tau_KW_WHILE,
	Tau_KW_FUNCTION,
	Tau_KW_RETURN,
	Tau_KW_FALSE,
	Tau_KW_TRUE,
	Tau_KW_END,
	Tau_NUM_KEYWORDS,
	Tau_KW_NULL = -1, /* Invalid or non keyword */
};
extern const char* tau_keyword_strings[Tau_NUM_KEYWORDS];
extern const char* tau_keywordid_names[Tau_NUM_KEYWORDS];

/**
 * Checks the keyword id of a string.
 * @param string String to check
 * @return The ID of the keyword or Tau_KW_NULL if not keyword
 */
Tau_KeywordID Tau_GetKeyword(const char* string);



typedef char Tau_OperatorID;
enum
{
	Tau_OP_ASSIGN,					/* '=' */
	Tau_OP_ASSIGNADDITION,			/* '+=' */
	Tau_OP_ASSIGNSUBTRACTION,		/* '-=' */
	Tau_OP_ASSIGNMULTIPLICATION,	/* '*=' */
	Tau_OP_ASSIGNDIVISION,			/* '/=' */

	Tau_OP_NOT,						/* '!' Logical not */
	Tau_OP_AND,						/* '&' Logical and */
	Tau_OP_OR,						/* '|' Logical or */
	Tau_OP_XOR,						/* '^' Logical xor */
	Tau_OP_NEGATIVE,				/* '-' Unary negative */

	Tau_OP_EQUALITY,				/* '==' */
	Tau_OP_INEQUALITY,				/* '!=' */
	Tau_OP_LESS,					/* '<'  */
	Tau_OP_GREATER,					/* '>'  */
	Tau_OP_LESSEQUAL,				/* '<=' */
	Tau_OP_GREATEREQUAL,			/* '>=' */

	Tau_OP_ADDITION,				/* '+' Arithmetic add */
	Tau_OP_SUBTRACTION,				/* '-' Arithmetic subtract */
	Tau_OP_MULTIPLICATION,			/* '*' Arithmetic multiply */
	Tau_OP_DIVISION,				/* '/' Arithmetic divide */

	Tau_OP_MEMBERACCESS,			/* '.' */
	Tau_OP_ARRAYACCESS,				/* '[]' */

	Tau_NUM_OPERATORS,
	Tau_OP_NULL = -1,				/* Invalid or non operator */
};
extern const char* tau_operator_strings[Tau_NUM_OPERATORS];
extern const char* tau_operatorid_names[Tau_NUM_OPERATORS];

/**
 * Checks the operator id of a string.
 * @param string String to check
 * @return The ID of the operator or Tau_OP_NULL if not operator
 */
Tau_OperatorID Tau_GetOperator(const char* string);

#define Tau_IsAssignmentOp(op)	((op) >= Tau_OP_ASSIGN		|| (op) <= Tau_OP_ASSIGNDIVISION)
#define Tau_IsLogicalOp(op)		((op) >= Tau_OP_NOT			|| (op) <= Tau_OP_XOR)
#define Tau_IsRelationalOp(op)	((op) >= Tau_OP_EQUALITY	|| (op) <= Tau_OP_GREATEREQUAL)
#define Tau_IsArithmeticOp(op)	((op) >= Tau_OP_ADDITION	|| (op) <= Tau_OP_DIVISION)
#define Tau_IsUnaryOp(op)		((op) == Tau_OP_NOT			|| (op) == Tau_OP_NEGATIVE)



typedef char Tau_SeparatorID;
enum
{
	Tau_SP_LPAREN,		/* '(' */
	Tau_SP_RPAREN,		/* ')' */
	Tau_SP_LBRACKET,	/* '[' */
	Tau_SP_RBRACKET,	/* ']' */
	Tau_SP_LBRACE,		/* '{' */
	Tau_SP_RBRACE,		/* '}' */
	Tau_SP_COMMA,		/* ',' */
	Tau_NUM_SEPARATORS,
	Tau_SP_NULL = -1,	/* Invalid or non separator */
};
extern const char tau_separator_chars[Tau_NUM_SEPARATORS];
extern const char* tau_separatorid_names[Tau_NUM_SEPARATORS];





typedef char Tau_ExprNodeType;
enum
{
	Tau_ET_OPERATOR,
	Tau_ET_NUMBERLITERAL,
	Tau_ET_STRINGLITERAL,
	Tau_ET_BOOLLITERAL,
	Tau_ET_OBJECT,
	Tau_ET_VARIABLE,
	Tau_ET_CLOSURE,
	Tau_ET_FUNCTIONCALL,
	Tau_NUM_EXPRNODETYPES
};
extern const char* tau_exprnodetype_names[Tau_NUM_EXPRNODETYPES];

typedef struct
{
	Tau_ExprNodeType type;
	union
	{
		struct
		{
			Tau_OperatorID id;
			struct Tau_ExprNode* left;
			struct Tau_ExprNode* right;
		} op;
		Tau_Float num;
		Tau_Bool boolean;
		struct
		{
			char* chars;
			int len;
		} str;
		//Tau_Function* closure;
		char* variable;
		struct
		{
			char* identifier;
			int numargs;
			char** argnames;
		} functioncall;
	};
} Tau_ExprNode;

void Tau_DestroyExpression(Tau_ExprNode* expr);



typedef enum
{
	Tau_ST_EXPRESSION,
	Tau_ST_IF,
	Tau_ST_FOR,
	Tau_ST_WHILE,
	Tau_ST_DOWHILE,
	Tau_ST_SCOPE,
	Tau_NUM_STATEMENTNODETYPES
} Tau_StatementNodeType;
extern const char* tau_statementnodetype_names[Tau_NUM_STATEMENTNODETYPES];

/**
 * Checks the separator id of a char.
 * @param c Character to check
 * @return The ID of the separator or Tau_SP_NULL if not separator char
 */
Tau_SeparatorID Tau_GetSeparator(char c);



typedef struct Tau_Body
{
	Tau_List statements;
} Tau_Body;

typedef struct Tau_StatementNode
{
	Tau_LISTLINKSHEADER(Tau_StatementNode);
	Tau_StatementNodeType type;
	union
	{
		struct
		{
			Tau_ExprNode* expression;
		} stmt_expr;
		struct
		{
			Tau_ExprNode* condition;
			Tau_Body* body_ontrue;
			Tau_Body* body_onfalse;
		} stmt_if;
		struct
		{
			Tau_ExprNode* init;
			Tau_ExprNode* condition;
			Tau_ExprNode* loop;
			Tau_Body* body;
		} stmt_for;
		struct
		{
			Tau_ExprNode* condition;
			Tau_Body* body;
		} stmt_while;
		struct
		{
			Tau_ExprNode* condition;
			Tau_Body* body;
		} stmt_dowhile;
		struct
		{
			Tau_Body* body;
		} stmt_scope;
	};
} Tau_StatementNode;

#endif