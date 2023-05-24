#include "tau_parse.h"



void Tau_PrintToken(const Tau_Token* token)
{
	if (!token) return;
	switch (token->type)
	{
	case Tau_TT_KEYWORD:		printf("%s", tau_keywordid_names[token->keywordid]); break;
	case Tau_TT_OPERATOR:		printf("%s", tau_operatorid_names[token->operatorid]); break;
	case Tau_TT_SEPARATOR:		printf("%s", tau_separatorid_names[token->separatorid]); break;
	case Tau_TT_NUMBERLITERAL:	printf("%s", token->string); break;
	case Tau_TT_STRINGLITERAL:	printf("\"%s\"", token->string); break;
	case Tau_TT_IDENTIFIER:		printf("%s", token->string); break;
	default:
		printf("invalid");
		break;
	}
	if (token->lastonline) putchar('\n');
}

void Tau_DestroyToken(Tau_Token* token)
{
	if (token->string)
		Tau_FREE(token->string);
	Tau_FREE(token);
}



typedef enum
{
	CT_NULL,		/* Default type or any char not recognised */
	CT_ALPHA,		/* Letter or underscore */
	CT_NUMBER,
	CT_POINT,
	CT_OPERATOR,
	CT_SEPARATOR,
	CT_SPACER,		/* Space or tab */
	CT_QUOTE,		/* Quotation marks for strings */
	CT_COMMENT,		/* Comments start with # and end with an endline */
	CT_ENDLINE,
} chartype;

#define Tau_IsAlpha(c) (isalpha(c) || c == '_')
#define Tau_IsOperatorChar(c) (strchr("+-/*=<>!&|^", c))
#define Tau_IsQuote(c) (c == '\"' || c == '\'')

static chartype check_chartype(const char c)
{
	if (Tau_IsAlpha(c))				return CT_ALPHA;
	if (isdigit(c))					return CT_NUMBER;
	if (c == '.')					return CT_POINT;
	if (strchr("+-/*=<>!&|^", c))	return CT_OPERATOR;
	if (strchr("()[]{},;", c))		return CT_SEPARATOR;
	if (isblank(c))					return CT_SPACER;
	if (c == '\n' || c == ';')		return CT_ENDLINE; /* Semicolon acts the same as endline */
	if (Tau_IsQuote(c))				return CT_QUOTE;
	if (c == '#')					return CT_COMMENT;
	if (c == '\n')					return CT_ENDLINE;
	return CT_NULL;
}



static Tau_Token* create_token()
{
	Tau_Token* token = Tau_MALLOC(sizeof(Tau_Token));
	token->prev = token->next = NULL;
	token->type = Tau_TT_NULL;
	token->keywordid = Tau_KW_NULL;
	token->operatorid = Tau_OP_NULL;
	token->separatorid = Tau_SP_NULL;
	token->string = NULL;
	token->lastonline = Tau_FALSE;
	return token;
}



static int read_alpha_token(
	Tau_List* tokens,
	const char* sourcecode,
	int start,
	int linenum
)
{
	int end;
	for (int i = start + 1;; i++)
		if (!Tau_IsAlpha(sourcecode[i]) && !isdigit(sourcecode[i]))
		{
			end = i;
			break;
		}

	Tau_Token* token = create_token();
	char* cutstring = Tau_CopyCutString(sourcecode, start, end - start);
	token->keywordid = Tau_GetKeyword(cutstring);
	if (token->keywordid == Tau_KW_NULL)
	{
		token->type = Tau_TT_IDENTIFIER;
		token->string = cutstring;
	} else
	{
		token->type = Tau_TT_KEYWORD;
		token->string = NULL;
		Tau_FREE(cutstring);
	}
	token->linenum = linenum;
	Tau_PushBackList(tokens, token);
	return end;
}



static int read_number_token(
	Tau_State* state,
	Tau_List* tokens,
	const char* sourcecode,
	int start,
	int linenum
)
{
	Tau_Bool pointfound = Tau_FALSE; /* For checking if there are two decimal points in one number */
	int end;
	for (int i = start + 1;; i++)
		if (sourcecode[i] == '.')
		{
			if (pointfound)
			{
				Tau_PUSHCONSTSYNTAXERROR("Two decimal points in one number");
				return ~(i + 1);
			}
			pointfound = Tau_TRUE;
		}
		else if (!isdigit(sourcecode[i]))
			{
				end = i;
				break;
			}

	Tau_Token* token = create_token();
	token->string = Tau_CopyCutString(sourcecode, start, end - start);
	token->type = Tau_TT_NUMBERLITERAL;
	token->linenum = linenum;
	Tau_PushBackList(tokens, token);
	return end;
}



static int read_operator_token(
	Tau_State* state,
	Tau_List* tokens,
	const char* sourcecode,
	int start,
	int linenum
)
{
	int end;
	for (int i = start + 1;; i++)
		if (!Tau_IsOperatorChar(sourcecode[i]))
		{
			end = i;
			break;
		}

	char* cutstring = Tau_CopyCutString(sourcecode, start, end - start);
	Tau_OperatorID op = Tau_GetOperator(cutstring);
	if (op == Tau_OP_NULL)
	{
		Tau_PUSHSYNTAXERROR("Invalid operator '%s'", cutstring);
		Tau_FREE(cutstring);
		return ~end;
	}
	Tau_FREE(cutstring);
	Tau_Token* token = create_token();
	token->operatorid = op;
	token->type = Tau_TT_OPERATOR;
	token->linenum = linenum;
	Tau_PushBackList(tokens, token);
	return end;
}



static int read_separator_token(
	Tau_State* state,
	Tau_List* tokens,
	const char* sourcecode,
	int start,
	int linenum
)
{
	Tau_OperatorID sp = Tau_GetSeparator(sourcecode[start]);
	if (sp == Tau_SP_NULL)
	{
		Tau_PUSHSYNTAXERROR("Invalid separator '%c'", sourcecode[start]);
		return ~(start + 1);
	}
	Tau_Token* token = create_token();
	token->separatorid = sp;
	token->type = Tau_TT_SEPARATOR;
	token->linenum = linenum;
	Tau_PushBackList(tokens, token);
	return start + 1;
}



static int read_string_token(
	Tau_State* state,
	Tau_List* tokens,
	const char* sourcecode,
	int start,
	int linenum
)
{
	int end;
	for (int i = start + 1;; i++)
		if (Tau_IsQuote(sourcecode[i]))
		{
			end = i + 1; /* Plus 1 to include quote mark */
			break;
		} else if (sourcecode[i] == '\n' || sourcecode[i] == '\0')
		{
			Tau_PUSHCONSTSYNTAXERROR("String doesn't have closing quote mark");
			return ~(i + 1);
		}

	Tau_Token* token = create_token();
	token->string = Tau_CopyCutString(sourcecode, start + 1, end - start - 2);
	token->type = Tau_TT_STRINGLITERAL;
	token->linenum = linenum;
	Tau_PushBackList(tokens, token);
	return end;
}



static int read_comment(
	const char* sourcecode,
	int start
)
{
	for (int i = start + 1;; i++)
		if (sourcecode[i] == '\n' || sourcecode[i] == '\0')
		{
			return i + 1; /* Plus 1 to skip \n */
		}
}





int Tau_ParseSourcecodeTokens(Tau_State* state, Tau_List* tokens, const char* sourcecode)
{
	int	linenum = 1;
	int numerrors = 0;
	int i = 0;
	while (1)
	{
		/* Looping over every char in the sourcecode one by one */
		char c = sourcecode[i];
		if (c == '\0') break;

		switch (check_chartype(c))
		{
		case CT_ALPHA:		i = read_alpha_token(tokens, sourcecode, i, linenum); break;
		case CT_NUMBER:		i = read_number_token(state, tokens, sourcecode, i, linenum); break;
		case CT_POINT:		i++; continue; /* No need to check if token is invalid */
		case CT_OPERATOR:	i = read_operator_token(state, tokens, sourcecode, i, linenum); break;
		case CT_SEPARATOR:	i = read_separator_token(state, tokens, sourcecode, i, linenum); break;
		case CT_SPACER:		i++; continue; /* No need to check if token is invalid */
		case CT_QUOTE:		i = read_string_token(state, tokens, sourcecode, i, linenum); break;
		case CT_COMMENT:	i = read_comment(sourcecode, i); continue;

		case CT_ENDLINE:	((Tau_Token*)tokens->end)->lastonline = Tau_TRUE; i++; continue;

		default: /* Invalid character */
		{
			Tau_PUSHSYNTAXERROR("Invalid character '%c'", c);
		}
		}

		if (i < 0) /* Token invalid */
		{
			i = ~i;
			numerrors++;
		}
	}
	return numerrors;
}
