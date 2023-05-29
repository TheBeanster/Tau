#ifndef _Tau_STATE_H_
#define _Tau_STATE_H_

#include "tau_utility.h"
#include "tau_value.h"

typedef struct
{
	Tau_List codebodies;
	Tau_List messages;

	Tau_List values; /* List of Tau_RTValue. All allocated values in use */
	Tau_List global_variables; /* List of all global variables in the state */
} Tau_State;

Tau_State* Tau_CreateState();
void Tau_DestroyState(Tau_State* state);


typedef enum
{
	STATEMSG_LOG,
	STATEMSG_SYNTAXERROR,
	STATEMSG_RUNTIMEERROR
} Tau_StateMessageType;

typedef struct Tau_StateMessage
{
	Tau_LISTLINKSHEADER(Tau_StateMessage);
	Tau_StateMessageType type;
	int linenum;
	char* msg;
} Tau_StateMessage;



/** 
 * @brief Push a message onto a state's message stack.
 * @param state The state to push onto.
 * @param type What kind of message this is, which will change the kind of header on the message when printed.
 * @param linenum The linenumber where the message was triggered.
 * @param msg Pointer to a string, this will be copied for the message so remember to free things.
 */
void Tau_PushStateMessage(Tau_State* state, Tau_StateMessageType type, int linenum, char* msg);

#define Tau_PUSHCONSTSYNTAXERROR(msg) Tau_PushStateMessage(state, STATEMSG_SYNTAXERROR, linenum, msg)

#define Tau_PUSHSYNTAXERROR(format, ...)					\
	char _msg[120];											\
	sprintf(_msg, format, __VA_ARGS__);						\
	Tau_PushStateMessage(state, STATEMSG_SYNTAXERROR, linenum, _msg)

#define Tau_PUSHTOKENERROR(token, format, ...)				\
	char _msg[120];											\
	sprintf(_msg, format, __VA_ARGS__);						\
	Tau_PushStateMessage(state, STATEMSG_SYNTAXERROR, token->linenum, _msg)

void Tau_PrintAllStateMessages(Tau_State* state);

#endif