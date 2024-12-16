/*
Code by: Or Yamin
Project: Calculator
Date: 24/06/2024
Review by: Uriah Hazan
Review date: 27/06/2024
Approved by: Uriah Hazan
Approved date: 27/06/2024
*/

/************************************* HEADERS ******************************/
#include <math.h>    /* pow */
#include <stdlib.h>  /* strtod */
#include <string.h>  /* strlen */
#include <assert.h>  /* assert */

#include "stack.h"
#include "calculator.h"

#define NUM_STATES 2
#define ASCII_RANGE 128
#define STACK_SIZE 25

typedef double (*operation_t)(double, double);

/* Operation functions */
static double Add(double a, double b) { return a + b; }
static double Subtract(double a, double b) { return a - b; }
static double Multiply(double a, double b) { return a * b; }
static double Divide(double a, double b) { return a / b; }
static double Power(double a, double b) { return pow(a, b); }

/* Operator lookup table */
typedef struct 
{
	operation_t func;
	int precedence;
}operator_lut_t;

enum precedence
{
	INVALID = -1,
	LOWEST_PRECEDENCE = 1,
	MIDDLE_PRECEDENCE = 2,
	HIGHEST_PRECEDENCE = 3
};

typedef enum 
{
	WAIT_FOR_NUMBER = 0,
	WAIT_FOR_OPERATOR = 1,
	STATE_ERROR = 2
}state_t;

typedef struct 
{
	state_t next_state;
	e_status_t (*handler)(const char **);
} fsm_entry_t;

/* Forward declarations */
static e_status_t HandleNumber(const char **expression);
static e_status_t HandleOperator(const char **expression);
static e_status_t HandleOpenParen(const char **expression);
static e_status_t HandleCloseParen(const char **expression);
static e_status_t HandleError(const char **expression);
static e_status_t HandleSpace(const char **expression);
static e_status_t HandleEnd(double *result);

static int GetOperatorPrecedence(char op);
static int ExecuteOperator(void);

static int InitStacks(size_t size);
static void InitLUTs(void);
static void DestroyStacks(void);

/* FSM transition table */
static fsm_entry_t fsm[NUM_STATES][ASCII_RANGE];
static operator_lut_t operators[ASCII_RANGE];

typedef struct 
{
	stack_t *operators;
	stack_t *operands;
}stacks_t;

static stacks_t stacks = {NULL, NULL};
static int match_paren = 0;
static int is_init = 0;

/************************************ CALCULATOR ******************************/
e_status_t Calculate(const char *expression, double *result) 
{
	state_t state = WAIT_FOR_NUMBER;
	e_status_t status = CALC_SUCCESS;
	
	assert(NULL != expression);
	assert(NULL != result);
	
	if (0 != InitStacks(STACK_SIZE)) 
    {
		return CALC_FAILURE;
    }

	InitLUTs();
	match_paren = 0;
	
	while ('\0' != *expression && state != STATE_ERROR && CALC_MATH_ERROR != status) 
	{
		int index = (int)*expression;
		status = (fsm[state][index].handler(&expression));
		state = (fsm[state][index].next_state);
	}
	
	if(CALC_MATH_ERROR == status)
	{
		DestroyStacks();
		return CALC_MATH_ERROR;
	}	
	
	if(state == STATE_ERROR || state != WAIT_FOR_OPERATOR || 0 != match_paren)
	{
		DestroyStacks();
		return CALC_SYNTAX_ERROR;
	}
	
	return HandleEnd(result);
}

/***************************** Handlers functions **************************/
static e_status_t HandleEnd(double *result)
{
	int status = CALC_SUCCESS;
	
	assert(NULL != result);
	
	while (!StackIsEmpty(stacks.operators)) 
	{
		status = ExecuteOperator();
		if(CALC_MATH_ERROR == status)
		{
			DestroyStacks();
			return CALC_MATH_ERROR;
		}
	}

	*result = *(double *)StackPeek(stacks.operands);
	
	DestroyStacks();
	
	return status;
}

static e_status_t HandleSpace(const char **expression)
{
	assert(NULL != expression);
	
	++(*expression);
	
	return CALC_SUCCESS;
}

static e_status_t HandleError(const char **expression)
{
	assert(NULL != expression);
	
	(void)expression;
	
	return CALC_SYNTAX_ERROR;	
}

static e_status_t HandleNumber(const char **expression) 
{
	char *endptr;
	double number = 0;
	
	assert(NULL != expression);
	
	number = strtod(*expression, &endptr);
	*expression = endptr;
	StackPush(stacks.operands, &number);
	
	return CALC_SUCCESS;
}

static e_status_t HandleOperator(const char **expression) 
{
	int status = CALC_SUCCESS;
	char op = 0;
	int precedence = 0;
	
	assert(NULL != expression);
	
	op = **expression;
	precedence = GetOperatorPrecedence(op);
	
	while (!StackIsEmpty(stacks.operators) 
	&& precedence <= GetOperatorPrecedence(*(char *)StackPeek(stacks.operators)) 
	&& precedence != HIGHEST_PRECEDENCE && status != CALC_MATH_ERROR) 
	{
		status = ExecuteOperator();
	}
	StackPush(stacks.operators, &op);
	(*expression)++;
	
	return status;
}

static e_status_t HandleOpenParen(const char **expression) 
{
	char op = 0;
	
	assert(NULL != expression);
	
	op = **expression;
	++match_paren;
	StackPush(stacks.operators, &op);
	(*expression)++;
	
	return CALC_SUCCESS;
}

static e_status_t HandleCloseParen(const char **expression) 
{
	int status = CALC_SUCCESS;
	
	assert(NULL != expression);
	
	while(!StackIsEmpty(stacks.operators) && *(char *)StackPeek(stacks.operators) != '(') 
	{
		status = ExecuteOperator();
	}
	
	if(!StackIsEmpty(stacks.operators))
	{
		StackPop(stacks.operators);
	}
	
	--match_paren;
	(*expression)++;
	
	return status;
}

/***************************** stack Operations functions *********************/
static int GetOperatorPrecedence(char op) 
{
	return operators[(int)op].precedence;
}

static int ExecuteOperator(void) 
{
	char op = 0;
	double b = 0;
	double a = 0;
	double result = 0;
	
	op = *(char *)StackPeek(stacks.operators);
	StackPop(stacks.operators);

	b = *(double *)StackPeek(stacks.operands);
	StackPop(stacks.operands);
	
	a = *(double *)StackPeek(stacks.operands);
	StackPop(stacks.operands);

	if(0 == b && '/' == op)
	{
		return CALC_MATH_ERROR; 
	}
	
	result = (operators[(int)op].func)(a, b);   
    StackPush(stacks.operands, &result);
    
    return CALC_SUCCESS;
}

/******************************** Init functions ****************************/
static int InitStacks(size_t size) 
{
	stacks.operators = StackCreate(size, sizeof(char));
	if (NULL == stacks.operators) 
	{
		return CALC_FAILURE;
	}

	stacks.operands = StackCreate(size, sizeof(double));
	if (NULL == stacks.operands) 
	{
		StackDestroy(stacks.operators);
		return CALC_FAILURE;
	}

	return CALC_SUCCESS;
}

static void DestroyStacks(void) 
{
	StackDestroy(stacks.operators);
	StackDestroy(stacks.operands);
}

static void InitLUTs(void)
{
	size_t i = 0;
	
	if(1 == is_init)
	{
		return;
	}
	
	is_init = 1;
	for(; i < ASCII_RANGE; ++i)
	{
		fsm[WAIT_FOR_NUMBER][i].next_state = STATE_ERROR;
		fsm[WAIT_FOR_NUMBER][i].handler = HandleError;
		fsm[WAIT_FOR_OPERATOR][i].next_state = STATE_ERROR;
		fsm[WAIT_FOR_OPERATOR][i].handler = HandleError;
	}
	
	for(i = '0'; i <= '9'; ++i)
	{
		fsm[WAIT_FOR_NUMBER][i].next_state = WAIT_FOR_OPERATOR;
		fsm[WAIT_FOR_NUMBER][i].handler = HandleNumber;
	}
	
	fsm[WAIT_FOR_NUMBER]['-'].next_state = WAIT_FOR_OPERATOR;
	fsm[WAIT_FOR_NUMBER]['-'].handler = HandleNumber;
	
	fsm[WAIT_FOR_NUMBER]['+'].next_state = WAIT_FOR_OPERATOR;
	fsm[WAIT_FOR_NUMBER]['+'].handler = HandleNumber;
	
	fsm[WAIT_FOR_NUMBER]['('].next_state = WAIT_FOR_NUMBER;
	fsm[WAIT_FOR_NUMBER]['('].handler = HandleOpenParen;
	
	fsm[WAIT_FOR_NUMBER]['\0'].next_state = STATE_ERROR;
	fsm[WAIT_FOR_NUMBER]['\0'].handler = HandleError;
	
	fsm[WAIT_FOR_NUMBER][' '].next_state = WAIT_FOR_NUMBER;
	fsm[WAIT_FOR_NUMBER][' '].handler = HandleSpace;
	
	fsm[WAIT_FOR_NUMBER]['\t'].next_state = WAIT_FOR_NUMBER;
	fsm[WAIT_FOR_NUMBER]['\t'].handler = HandleSpace;
	
	fsm[WAIT_FOR_NUMBER]['\n'].next_state = WAIT_FOR_OPERATOR;
	fsm[WAIT_FOR_NUMBER]['\n'].handler = HandleSpace;
	
	
	fsm[WAIT_FOR_OPERATOR][')'].next_state = WAIT_FOR_OPERATOR;
	fsm[WAIT_FOR_OPERATOR][')'].handler = HandleCloseParen;
	
	fsm[WAIT_FOR_OPERATOR]['*'].next_state = WAIT_FOR_NUMBER;
	fsm[WAIT_FOR_OPERATOR]['*'].handler = HandleOperator;
	
	fsm[WAIT_FOR_OPERATOR]['/'].next_state = WAIT_FOR_NUMBER;
	fsm[WAIT_FOR_OPERATOR]['/'].handler = HandleOperator;
	
	fsm[WAIT_FOR_OPERATOR]['+'].next_state = WAIT_FOR_NUMBER;
	fsm[WAIT_FOR_OPERATOR]['+'].handler = HandleOperator;
	
	fsm[WAIT_FOR_OPERATOR]['-'].next_state = WAIT_FOR_NUMBER;
	fsm[WAIT_FOR_OPERATOR]['-'].handler = HandleOperator;
	
	fsm[WAIT_FOR_OPERATOR]['^'].next_state = WAIT_FOR_NUMBER;
	fsm[WAIT_FOR_OPERATOR]['^'].handler = HandleOperator;
	
	fsm[WAIT_FOR_OPERATOR]['\0'].next_state = WAIT_FOR_NUMBER;
	fsm[WAIT_FOR_OPERATOR]['\0'].handler = HandleError;
	
	fsm[WAIT_FOR_OPERATOR][' '].next_state = WAIT_FOR_OPERATOR;
	fsm[WAIT_FOR_OPERATOR][' '].handler = HandleSpace;
	
	fsm[WAIT_FOR_OPERATOR]['\t'].next_state = WAIT_FOR_OPERATOR;
	fsm[WAIT_FOR_OPERATOR]['\t'].handler = HandleSpace;
	
	fsm[WAIT_FOR_OPERATOR]['\n'].next_state = WAIT_FOR_OPERATOR;
	fsm[WAIT_FOR_OPERATOR]['\n'].handler = HandleSpace;
	
	
	/* Operaots LUT initialization */
	for(i = 0; i < ASCII_RANGE; ++i)
	{
		operators[i].func = NULL;
		operators[i].precedence = INVALID;
	}
	
	operators['*'].func = Multiply;
	operators['*'].precedence = MIDDLE_PRECEDENCE;
	
	operators['/'].func = Divide;
	operators['/'].precedence = MIDDLE_PRECEDENCE;
	
	operators['+'].func = Add;
	operators['+'].precedence = LOWEST_PRECEDENCE;
	
	operators['-'].func = Subtract;
	operators['-'].precedence = LOWEST_PRECEDENCE;
	
	operators['^'].func = Power;
	operators['^'].precedence = HIGHEST_PRECEDENCE;
}


