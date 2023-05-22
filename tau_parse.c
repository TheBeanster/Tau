#include "tau_parse.h"



static Tau_ExprNode* parse_expression(Tau_State* state, const Tau_Token* begin, Tau_Token** end)
{
	Tau_List l_operands = { 0 };
	Tau_List l_operators = { 0 };
	Tau_List l_all = { 0 };

	/* 5 + 2 * (3 + test(1, 3 + var[0])) */

	Tau_Token* i;
	for (i = begin;; i = i->next)
	{
		switch (i->type)
		{
		case Tau_TT_OPERATOR:

			break;

		case Tau_TT_KEYWORD:
			if (i->keywordid == Tau_KW_FALSE || i->keywordid == Tau_KW_TRUE)
				printf("true or false keyword\n");
			else
				goto expr_end;
			break;

		case Tau_TT_SEPARATOR:

			break;

		default:
			break;
		}
	}

expr_end:
	*end = i;
	return NULL;
}



static Tau_StatementNode* parse_if_statement(Tau_State* state, const Tau_Token* begin, Tau_Token** end)
{
	Tau_ExprNode* condition = parse_expression(state, begin->next, end);

	if (!end || (*end)->keywordid != Tau_KW_THEN)
	{
		printf("If statement condition ended bad\n");
		Tau_DestroyExpression(condition);
		return NULL;
	}

	Tau_StatementNode* stmt = Tau_ALLOC_TYPE(Tau_StatementNode);
	stmt->type = Tau_ST_IF;
	stmt->stmt_if.condition = condition;

	return stmt;
}



static Tau_StatementNode* parse_statement(Tau_State* state, const Tau_Token* begin, Tau_Token** end)
{
	printf(" > Parsing statement starting with '");
	Tau_PrintToken(begin);
	printf("'\n");

	Tau_StatementNode* stmt = NULL;
	switch (begin->keywordid)
	{
	case Tau_KW_IF:
		stmt = parse_if_statement(state, begin, end);
		break;

	default:
		break;
	}

	if (!stmt)
	{
		printf("Couldn't parse statement\n");
		return NULL;
	}

	return stmt;
}



static Tau_Body* parse_body(Tau_State* state, const Tau_Token* begin, Tau_Token** end)
{
	Tau_Body* body = Tau_ALLOC_TYPE(Tau_Body);
	Tau_Token* i = begin;
	while (i)
	{
		Tau_StatementNode* stmt = parse_statement(state, i, &i);
		if (!stmt)
		{
			*end = i;
			return stmt;
		}
		Tau_PushBackList(&body->statements, stmt);
	}

	/* Reached end of tokens list */
	*end = NULL;
	return body;
}



Tau_Body* Tau_ParseSourceCode(Tau_State* state, const char* sourcecode)
{
	printf("Parsing source code\n");
	Tau_PrintSourceCode(sourcecode);

	Tau_List tokens = { 0 };
	Tau_ParseSourcecodeTokens(state, &tokens, sourcecode);

	for (Tau_Token* i = tokens.begin; i; i = i->next)
	{
		Tau_PrintToken(i);
		if (i->type != Tau_TT_ENDLINE)
			printf(" ");
	}
	putchar('\n');
	/* The source code is now separated into tokens */

	Tau_Token* endtoken;
	Tau_Body* body = parse_body(state, tokens.begin, &endtoken);
	if (endtoken != NULL)
	{
		Tau_PUSHTOKENERROR(endtoken, "Sourcecode parsing ended early");
		//Tau_DestroyBody(body);
		body = NULL;
	}

	Tau_PrintAllStateMessages(state);

	return body;
	
on_fail:

	return NULL;
}






void Tau_PrintSourceCode(const char* const sourcecode)
{
	int linecount = 1;
	for (char* c = sourcecode; *c != '\0'; c++)
		if (*c == '\n') linecount++;
	
	/* Find how many digits the linenumber counter will need */
	int linenum_digits;
	int power = 10;
	int digits;
	for (digits = 0; digits < 6; digits++)
	{
		if (linecount / power < 1) break;
		power *= 10;
	}
	power = 10;

	char* c = sourcecode;
	for (int i = 1; i <= linecount; i++)
	{
		if (i / power >= 1) { digits--; power *= 10; }
		printf("| ");
		for (int j = 0; j < digits; j++) putchar(' ');
		printf("%i | ", i);

		for (;; c++)
		{
			if (*c == '\0') goto end;
			if (*c == '\t')
				printf("    ");
			else
				putchar(*c);
			if (*c == '\n') break;
		}
		*c++;
	}
end:
	putchar('\n');
}