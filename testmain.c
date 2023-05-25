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

	Tau_DestroyState(state);

	return 0;
}
