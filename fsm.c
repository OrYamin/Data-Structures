#include <string.h> /*strlen*/

#include "fsm.h"

state transition_table[5][2] = 
{
	{S0, REJECT},    /* Transitions from START */
	{ACCEPT, S1},        /* Transitions from S0 */
	{ACCEPT, S1},        /* Transitions from S1 */
	{ACCEPT, S1}, /* Transitions from ACCEPT (final accepting state) */
	{REJECT, REJECT} /* Transitions from REJECT (non-accepting state */
};


state SwitchNextState(state current_state, char input) 
{
	switch (current_state) 
	{
		case START:
			return (input == '0') ? S0 : REJECT;
		case S0:
			return (input == '0') ? ACCEPT : S1;
		case S1:
			return (input == '0') ? ACCEPT : S1;
		case ACCEPT:
			return (input == '0') ? ACCEPT : S1;
		case REJECT:
			return REJECT;
		default:
			return REJECT;
	}
}


state TableNextState(state current_state, char input)
{
	int inputIndex = input - '0';
	return transition_table[current_state][inputIndex];
}


int IsAccepted(next_function state_transition, const char *input_string)
{
	state current_state = START;
	size_t j = 0;
	
	for (j = 0; j < strlen(input_string); j++) 
		{
			current_state = state_transition(current_state, input_string[j]);
		}
		
	return current_state == ACCEPT;
}

