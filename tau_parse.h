#ifndef _Tau_PARSE_H_
#define _Tau_PARSE_H_

#include "tau_utility.h"
#include "tau_code.h"
#include "tau_state.h"



typedef char Tau_TokenType;
enum
{
	Tau_TT_KEYWORD,
	Tau_TT_OPERATOR,
	Tau_TT_SEPARATOR,
	Tau_TT_NUMBERLITERAL,
	Tau_TT_STRINGLITERAL,
	Tau_TT_IDENTIFIER,
	Tau_TT_ENDLINE,
	Tau_TT_NULL = -1,		/* Invalid token */
};

typedef struct Tau_Token
{
	Tau_LISTLINKSHEADER(Tau_Token);

	Tau_TokenType	type;
	Tau_KeywordID	keywordid;
	Tau_OperatorID	operatorid;
	Tau_SeparatorID separatorid;

	char* string;
	int linenum;
} Tau_Token;

void Tau_PrintToken(const Tau_Token* token);
void Tau_DestroyToken(Tau_Token* token);



/**
 * @brief Reads through a string character by character and divides it into separate tokens.
 * @param state State to parse in.
 * @param tokens Pointer to an empty list to put the tokens into.
 * @param sourcecode Pointer to a string with Tau source code.
 * @return The number of errors found.
 */
int Tau_ParseSourcecodeTokens(Tau_State* state, Tau_List* tokens, const char* sourcecode);

Tau_Body* Tau_ParseSourceCode(Tau_State* state, const char* sourcecode);

void Tau_PrintCodeTree(const struct Tau_Body* code);

void Tau_PrintSourceCode(const char* const sourcecode);

#endif