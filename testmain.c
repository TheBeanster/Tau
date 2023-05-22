#include <stdio.h>

#include "tau_state.h"
#include "tau_parse.h"

int main(int argc, char** argv)
{
	Tau_State* state = Tau_CreateState();

	Tau_ParseSourceCode(state, "if true == 5 then\n\tprint('xumn')\nend");

	Tau_DestroyState(state);

	return 0;
}
