#ifndef _Tau_OBJECT_H_
#define _Tau_OBJECT_H_

#include "tau_utility.h"
#include "tau_code.h"

typedef enum
{
	Tau_TYPE_NUMBER,
	Tau_TYPE_BOOL,
	Tau_TYPE_STRING,
	Tau_TYPE_ARRAY,
	Tau_TYPE_OBJECT,
	Tau_TYPE_FUNCTION,
	Tau_NUMTYPES
} Tau_ValueType;
extern const char* const tau_valuetype_names[Tau_NUMTYPES];

typedef struct Tau_Value
{
	Tau_ValueType type;
	union
	{
		Tau_Float v_number;
		Tau_Bool v_bool;
		struct
		{
			char* chars;
			int length;
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

int Tau_ValueString(Tau_Value* val, char* str);

Tau_Bool Tau_VAdd(struct Tau_State* state, Tau_Value* val1, const Tau_Value* val2);

/** 
 * @brief Performs an arthmetic operator and mutates val1 with the answer
 * @param state For pushing error messages
 * @param op Either OP_ADD, OP_SUB, OP_MUL or OP_DIV
 * @param val1 Left argument, will be mutated. Make a copy if needed
 * @param val2 Right argument, immutable
 * @return TRUE on success, or FALSE if something went wrong
 */
Tau_Bool Tau_DoArithmeticOp(struct Tau_State* state, Tau_OperatorID op, Tau_Value* val1, const Tau_Value* val2);

Tau_Bool Tau_ValueBool(const Tau_Value* val);
Tau_Float Tau_ValueNumber(const Tau_Value* val);

Tau_Bool Tau_ValueEquals(struct Tau_State* state, const Tau_Value* val1, const Tau_Value* val2);

Tau_Bool Tau_DoLogicalOp(struct Tau_State* state, Tau_OperatorID op, Tau_Value* val1, const Tau_Value* val2);



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