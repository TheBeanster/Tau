#include "tau_value.h"

#include "tau_state.h"
#include <math.h>

const char* const tau_valuetype_names[Tau_NUMTYPES] =
{
	"number",
	"bool",
	"string",
	"array",
	"object",
	"function"
};

Tau_Value* Tau_CreateValue(Tau_ValueType type)
{
	Tau_Value* val = Tau_ALLOC_TYPE(Tau_Value);
	val->type = type;
	return val;
}

void Tau_ClearValue(Tau_Value* val)
{
	switch (val->type)
	{
	Tau_TYPE_STRING:
		Tau_FREE(val->v_string.chars);
		break;

	Tau_TYPE_ARRAY:
		for (Tau_ArrayMember* i = val->v_array.begin; i;)
		{
			Tau_ArrayMember* del = i;
			i = i->next;
			Tau_ClearValue(&del->value);
			Tau_FREE(del);
		}
		break;

	Tau_TYPE_OBJECT:
		for (Tau_ObjectMember* i = val->v_object.members.begin; i;)
		{
			Tau_ObjectMember* del = i;
			i = i->next;
			Tau_ClearValue(&del->value);
			Tau_FREE(del->name);
			Tau_FREE(del);
		}
		break;

	default:
		break;
	}
}

void Tau_DestroyValue(Tau_Value* val)
{
	if (!val) return;
	Tau_ClearValue(&val);
	Tau_FREE(val);
}



int Tau_ValueString(Tau_Value* val, char* str)
{
	switch (val->type)
	{
	case Tau_TYPE_NUMBER: return (val->v_number == (Tau_Int)val->v_number) ?
		sprintf(str, "%d", (Tau_Int)val->v_number) :
		sprintf(str, "%f", val->v_number);
	case Tau_TYPE_BOOL: return sprintf(str, "%s", val->v_bool ? "true" : "false");
	case Tau_TYPE_STRING: return sprintf(str, "%s", val->v_string.chars);
	default:
		exit(-1);
	}
}



Tau_Bool Tau_VAdd(Tau_State* state, Tau_Value* val1, const Tau_Value* val2)
{
	switch (val1->type)
	{
	case Tau_TYPE_BOOL:
		/* Adding a bool turns it into a number */
		val1->type = Tau_TYPE_NUMBER;
		val1->v_number = (val1->v_bool != Tau_FALSE);
		/* Fall through */
	case Tau_TYPE_NUMBER:
		switch (val2->type)
		{
		case Tau_TYPE_NUMBER: val1->v_number += val2->v_number; break;
		case Tau_TYPE_BOOL: val1->v_number += val2->v_bool; break;
		default: goto invalid_type_pair;
		}
		break;

	case Tau_TYPE_STRING:
	{
		char val2string[200];
		int newlen = val1->v_string.length + Tau_ValueString(val2, val2string);
		val1->v_string.chars = Tau_REALLOC(val1->v_string.chars, newlen + 1); /* Null terminator */
		strcat(val1->v_string.chars, val2string);
		val1->v_string.length = newlen;
	}
		break;

	default: goto invalid_type_pair;
	}
	return Tau_TRUE;

invalid_type_pair:
	Tau_PUSHRUNTIMEERROR("Can't add %s to %s", tau_valuetype_names[val2->type], tau_valuetype_names[val1->type]);
	return Tau_FALSE;
}



Tau_Bool Tau_BinaryOp(Tau_State* state, Tau_OperatorID op, Tau_Value* val1, const Tau_Value* val2)
{
	switch (val1->type)
	{
	case Tau_TYPE_BOOL:
		/* Operating on a bool turns it into a number */
		val1->type = Tau_TYPE_NUMBER;
		val1->v_number = (val1->v_bool != Tau_FALSE);
		/* Fall through */
	case Tau_TYPE_NUMBER:
	{
		Tau_Float num2;
		switch (val2->type)
		{
		case Tau_TYPE_NUMBER: num2 = val2->v_number; break;
		case Tau_TYPE_BOOL: num2 = val2->v_bool != Tau_FALSE; break;
		default: goto invalid_type_pair;
		}
		switch (op)
		{
		case Tau_OP_ADDITION: val1->v_number += num2; break;
		case Tau_OP_SUBTRACTION: val1->v_number -= num2; break;
		case Tau_OP_MULTIPLICATION: val1->v_number *= num2; break;
		case Tau_OP_DIVISION: val1->v_number /= num2; break;

		case Tau_OP_AND: val1->v_bool = (val1->v_number && num2); val1->type = Tau_TYPE_BOOL; break;
		case Tau_OP_OR: val1->v_bool = (val1->v_number || num2); val1->type = Tau_TYPE_BOOL; break;
		case Tau_OP_XOR: val1->v_bool = ((Tau_Int)val1->v_number ^ (Tau_Int)num2); val1->type = Tau_TYPE_BOOL; break;

		default:
			break;
		}
	}

	case Tau_TYPE_STRING:
	{
		char val2string[200];
		int newlen = val1->v_string.length + Tau_ValueString(val2, val2string);
		val1->v_string.chars = Tau_REALLOC(val1->v_string.chars, newlen + 1); /* Null terminator */
		strcat(val1->v_string.chars, val2string);
		val1->v_string.length = newlen;
	}
	break;

	default: goto invalid_type_pair;
	}
	return Tau_TRUE;

invalid_type_pair:
	Tau_PUSHRUNTIMEERROR("Can't add %s to %s", tau_valuetype_names[val2->type], tau_valuetype_names[val1->type]);
	return Tau_FALSE;
}
