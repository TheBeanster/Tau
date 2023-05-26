#ifndef _Tau_OBJECT_H_
#define _Tau_OBJECT_H_

#include "tau_utility.h"

typedef enum
{
	Tau_TYPE_NUMBER,
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
		struct
		{
			int length;
			char* chars;
		} v_string;
		Tau_List v_array;
		struct
		{
			Tau_List members; /* List of struct members */
		} v_struct;
		// Tau_Function
	};
} Tau_Value;

typedef struct Tau_ArrayMember
{
	Tau_LISTLINKSHEADER(Tau_ArrayMember);
	Tau_Value value;
} Tau_ArrayMember;

typedef struct Tau_StructMember
{
	Tau_LISTLINKSHEADER(Tau_StructMember);
	char* name;
	Tau_Value value;
} Tau_StructMember;



typedef struct Tau_Object
{
	Tau_LISTLINKSHEADER(Tau_Object);
	int refcount;
	Tau_Value value;
} Tau_Object;

typedef struct Tau_Variable
{
	Tau_LISTLINKSHEADER(Tau_Variable);
	char* name;
	Tau_Object* object;
} Tau_Variable;

#endif