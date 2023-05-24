#include <stdio.h>

#include "tau_state.h"
#include "tau_parse.h"

int main(int argc, char** argv)
{
	Tau_State* state = Tau_CreateState();

	Tau_ParseSourceCode(state, "(69) if 4 then end");

	Tau_DestroyState(state);

	return 0;
}
