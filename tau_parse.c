#include "tau_parse.h"



static Tau_Body* parse_body(Tau_State* state, const Tau_Token* begin, Tau_Token** end);



typedef struct parse_exprnode
{
	Tau_ListNode links_all;
	union
	{
		Tau_ListNode links_operands;
		Tau_ListNode links_operators;
	};
	Tau_ExprNode* node;
	Tau_Token* token; // TODO This is temporary
} parse_exprnode;

#define CREATE_NODE(t)										\
	parse_exprnode* node = Tau_ALLOC_TYPE(parse_exprnode);	\
	node->node = Tau_ALLOC_TYPE(Tau_ExprNode);				\
	node->node->type = t

static parse_exprnode* parse_operator(Tau_Token* token)
{
	parse_exprnode* node = Tau_ALLOC_TYPE(parse_exprnode);
	node->node = Tau_ALLOC_TYPE(Tau_ExprNode);
	node->node->type = Tau_ET_OPERATOR;
	node->node->op.id = token->operatorid;
	return node;
}

static parse_exprnode* parse_identifier(Tau_Token* token)
{
	parse_exprnode* node = Tau_ALLOC_TYPE(parse_exprnode);
	node->node = Tau_ALLOC_TYPE(Tau_ExprNode);
	node->node->type = Tau_ET_VARIABLE;
	node->node->variable = _strdup(token->string);
	return node;
}





#define PUSH_OPERAND(node)									\
	Tau_PushBackList(&l_operands, &node->links_operands);	\
	Tau_PushBackList(&l_all, &node->links_all);				\
	prevwasoperator = Tau_FALSE;								node->token = i

#define PUSH_OPERATOR(node)									\
	Tau_PushBackList(&l_operators, &node->links_operators);	\
	Tau_PushBackList(&l_all, &node->links_all);				\
	prevwasoperator = Tau_TRUE;									node->token = i


/** @brief Parses an expression tree.
 * @param state 
 * @param begin The token where the expression begins. Shouldn't be separator
 * @param readendline If the expression should end on vaild endlines. Should only be true on expression statements. 
 * This is because endlines denote where expression statements end. But in for example, if conditions, the expression always ends on 'then'.
 * @param end The token after the expression ends.
 * @return Pointer to the top node of the expression tree.
 */
static Tau_ExprNode* parse_expression(Tau_State* state, const Tau_Token* begin, const Tau_Bool readendline, Tau_Token** end)
{
	printf(" > Parsing expression starting with '");
	Tau_PrintToken(begin);
	printf("'\n");

	if (!begin) return NULL;

	/* Lists of parse_exprnode in the expression */
	Tau_List l_operands = { 0 };
	Tau_List l_operators = { 0 };
	Tau_List l_all = { 0 }; /* List of all nodes in the expression */

	/* 5 + 2 * (3 + test(1, 3 + var[0])) */
	/* 4 + 2 * (1 + 3\n */

	Tau_Bool prevwasoperator = Tau_FALSE;
	Tau_Token* i = begin;
	while (i)
	{
		switch (i->type)
		{
		case Tau_TT_OPERATOR:
		{
			CREATE_NODE(Tau_ET_OPERATOR);
			node->node->op.id = i->operatorid;
			PUSH_OPERATOR(node);
		}
			break;

		case Tau_TT_KEYWORD:
			if (i->keywordid == Tau_KW_FALSE || i->keywordid == Tau_KW_TRUE)
			{
				CREATE_NODE(Tau_ET_BOOLLITERAL);
				node->node->boolean = i->keywordid != Tau_KW_FALSE;
				PUSH_OPERAND(node);
			}
			else
				/* Any other keyword ends expression */
				if (prevwasoperator) /* Operator without right operand */
				{
					Tau_PUSHTOKENERROR(i, "Operator is missing right operand");
					goto on_fail;
				} else
					goto expr_end;
			break;

		case Tau_TT_SEPARATOR:
			switch (i->separatorid)
			{
			case Tau_SP_COMMA:
			case Tau_SP_RPAREN:
			case Tau_SP_RBRACE:
			case Tau_SP_RBRACKET:
				/* These all end expressions */
				if (prevwasoperator)
				{
					Tau_PUSHTOKENERROR(i, "Operator is missing right operand");
					goto on_fail; /* Operator without right operand */
				} else
					goto expr_end; /* Expression ends on any of , ) ] or } */
				break;
			case Tau_SP_LPAREN:
			{
				printf("Parsing subexpression\n");
				Tau_Token* subexpr_end;
				Tau_ExprNode* subexpr = parse_expression(state, i->next, Tau_FALSE, &subexpr_end);
				if (subexpr && subexpr_end && subexpr_end->separatorid == Tau_SP_RPAREN)
				{
					/* Subexpression ends with matching parentheses */
					parse_exprnode* node = Tau_ALLOC_TYPE(parse_exprnode);
					node->node = subexpr;
					PUSH_OPERAND(node);
					i = subexpr_end;
				} else
				{
					Tau_PUSHTOKENERROR(i, "Subexpression doesn't have closing parenthesis");
					Tau_DestroyExpression(subexpr);
					goto on_fail;
				}
				break;

			}
			default:
				break;
			}
			break;

		case Tau_TT_IDENTIFIER:
		{
			/* Variable OR function call */
			CREATE_NODE(Tau_TT_IDENTIFIER);
			node->node->variable = _strdup(i->string);
			PUSH_OPERAND(node);
		}
			break;

		case Tau_TT_NUMBERLITERAL:
		{
			CREATE_NODE(Tau_ET_NUMBERLITERAL);
			node->node->num = strtof(i->string, NULL);
			PUSH_OPERAND(node);
		}
			break;

		default:
			break;
		}

		i = i->next;
	}

expr_end:

	printf(" > Expression ended on token '");
	Tau_PrintToken(i);
	printf("'\nTokens in expression: ");
	for (parse_exprnode* n = l_all.begin; n; n = n->links_all.next)
	{
		Tau_PrintToken(n->token);
		printf(" ");
	}
	putchar('\n');

	*end = i;
	return ((parse_exprnode*)l_all.begin)->node;

on_fail:

	return NULL;
}



static Tau_StatementNode* parse_if_statement(Tau_State* state, const Tau_Token* begin, Tau_Token** end)
{
	Tau_Token* i = begin;

	/* Parse the if statement condition expression */
	Tau_ExprNode* condition = parse_expression(state, begin->next, Tau_FALSE, &i);
	if (!condition || !i || i->keywordid != Tau_KW_THEN || !i->next)
	{
		Tau_PUSHTOKENERROR(begin, "Couldn't parse if-statement condition");
		goto on_fail;
	}
	i = i->next; /* i is token after 'then' */

	/* Parse the true body */
	Tau_Body* body_ontrue = parse_body(state, i, &i);
	if (!body_ontrue || !i) /* If body couldn't parse, it doesn't end */
	{
		Tau_PUSHTOKENERROR(begin, "Couldn't parse if-statement true body");
		goto on_fail;
	}
	if (!(i->keywordid == Tau_KW_END || i->keywordid == Tau_KW_ELSE)) /* If it doesn't end on 'end' or 'else' */
	{
		Tau_PUSHTOKENERROR(begin, "If-statement body doesn't end on 'end' or 'else'");
		goto on_fail;
	}

	Tau_StatementNode* stmt = Tau_ALLOC_TYPE(Tau_StatementNode);
	stmt->type = Tau_ST_IF;
	stmt->stmt_if.condition = condition;

	return stmt;

on_fail:
	Tau_DestroyExpression(condition);
	*end = NULL;
	return NULL;
}



static Tau_StatementNode* parse_expression_statement(Tau_State* state, Tau_Token* begin, Tau_Token** end)
{
	Tau_Token* i = begin;
	Tau_ExprNode* expression = parse_expression(state, begin, Tau_TRUE, &i);
	if (!expression) return NULL;
	Tau_StatementNode* stmt = Tau_ALLOC_TYPE(Tau_StatementNode);
	stmt->type = Tau_ST_EXPRESSION;
	stmt->stmt_expr.expression = expression;
	*end = i;
	return stmt;
}



static Tau_StatementNode* parse_statement(Tau_State* state, Tau_Token* begin, Tau_Token** end)
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
		stmt = parse_expression_statement(state, begin, end);
		break;
	}

	if (!stmt)
	{
		Tau_PUSHTOKENERROR(begin, "Couldn't parse statement");
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
			return body;
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
		if (!i->lastonline)
			putchar(' ');
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

	printf("\n\n   MESSAGES\n");
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
		c++;
	}
end:
	putchar('\n');
}