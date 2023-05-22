#include "tau_state.h"



Tau_State* Tau_CreateState()
{
	Tau_State* state = Tau_ALLOC_TYPE(Tau_State);
	return state;
}

void Tau_DestroyState(Tau_State* state)
{

	Tau_FREE(state);
}



void Tau_PushStateMessage(Tau_State* state, Tau_StateMessageType type, int linenum, char* msg)
{
	Tau_StateMessage* message = Tau_ALLOC_TYPE(Tau_StateMessage);
	message->msg = _strdup(msg);
	message->type = type;
	message->linenum = linenum;
	Tau_PushBackList(&state->messages, message);
}

void Tau_PrintAllStateMessages(Tau_State* state)
{
	Tau_StateMessage* i;
	for (Tau_StateMessage* i = state->messages.begin; i != NULL;)
	{
		switch (i->type)
		{
		case STATEMSG_SYNTAXERROR:
			printf("Syntax error on line %i > %s\n", i->linenum, i->msg);
			break;

		default:
			printf("msg > %s\n", i->msg);
			break;
		}

		/* Remove the message */
		Tau_StateMessage* del = i;
		i = i->next;
		Tau_FREE(del->msg);
		Tau_FREE(del);
	}
}
