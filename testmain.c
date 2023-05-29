#include <stdio.h>

#include "tau_state.h"
#include "tau_parse.h"

static char* read_code_from_file(const char* const filename)
{
	FILE* file = fopen(filename, "r");
	if (!file)
	{
		return NULL;
	}

	int length = 0;
	while (!feof(file))
	{
		fgetc(file);
		length++;
	}

	char* filebuffer = malloc(length + 1);
	rewind(file);
	int pos = 0;
	while (!feof(file))
	{
		char c = fgetc(file);
		filebuffer[pos] = c;
		pos++;
	}

	filebuffer[pos - 1] = '\0';
	fclose(file);
	return filebuffer;
}

int main(int argc, char** argv)
{
	Tau_State* state = Tau_CreateState();

	char* code = read_code_from_file("test.tau");
	Tau_ParseSourceCode(state, code);
	free(code);

	printf("\n\n\nValue testing:\n");

	Tau_Value v1 = { Tau_TYPE_NUMBER, .v_number ={ 2 } };
	Tau_Value v2 = { Tau_TYPE_NUMBER, .v_number={ -0.0 } };
	Tau_DoArithmeticOp(state, Tau_OP_DIV, &v1, &v2);
	char str[100];
	Tau_ValueString(&v1, str);
	printf("v1 = %s\n", str);

	Tau_PrintAllStateMessages(state);
	Tau_DestroyState(state);

	return 0;
}
