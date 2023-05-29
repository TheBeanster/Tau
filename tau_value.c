#include "tau_value.h"

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



void Tau_VAdd(Tau_Value* val1, const Tau_Value* val2)
{
	switch (val1->type)
	{
	Tau_TYPE_NUMBER:
		switch (val2->type)
		{
		Tau_TYPE_NUMBER: val1->v_number += val2->v_number; break;
		Tau_TYPE_BOOLEAN: val1->v_number += val2->v_bool; break;
		Tau_TYPE_STRING:
		{
			int newlen = val1->v_string.length + val2->v_string.length;
			char* newstring = Tau_MALLOC(newlen + 1); /* Null terminator */
			newstring = strcat()

		}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
