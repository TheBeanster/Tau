#ifndef _Tau_OBJECT_H_
#define _Tau_OBJECT_H_

#include "tau_utility.h"

typedef enum
{
	Tau_TYPE_NUMBER,
	Tau_TYPE_BOOLEAN,
	Tau_TYPE_STRING,
	Tau_TYPE_ARRAY,
	Tau_TYPE_OBJECT,
	Tau_TYPE_FUNCTION,
} Tau_ValueType;

typedef struct Tau_Value
{
	Tau_ValueType type;
	union
	{
		Tau_Float v_number;
		Tau_Bool v_bool;
		struct
		{
			int length;
			char* chars;
		} v_string;
		Tau_List v_array;
		struct
		{
			Tau_List members; /* List of object members */
		} v_object;
		// Tau_Function
	};
} Tau_Value;

typedef struct Tau_ArrayMember
{
	Tau_LISTLINKSHEADER(Tau_ArrayMember);
	Tau_Value value;
} Tau_ArrayMember;

typedef struct Tau_ObjectMember
{
	Tau_LISTLINKSHEADER(Tau_ObjectMember);
	char* name;
	Tau_Value value;
} Tau_ObjectMember;

Tau_Value* Tau_CreateValue(Tau_ValueType type);
void Tau_ClearValue(Tau_Value* val);
void Tau_DestroyValue(Tau_Value* val);

void Tau_VAdd(Tau_Value* val1, const Tau_Value* val2);



typedef struct Tau_RTValue
{
	Tau_LISTLINKSHEADER(Tau_RTValue);
	int refcount;
	Tau_Value value;
} Tau_RTValue;

typedef struct Tau_Variable
{
	Tau_LISTLINKSHEADER(Tau_Variable);
	char* name;
	Tau_RTValue* value;
} Tau_Variable;

#endif