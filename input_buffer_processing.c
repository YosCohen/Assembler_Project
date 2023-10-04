#include "run_1_am_file_line_to_command_processing.h"
#include "input_buffer_processing.h"
#include "command_line.h"



/*
Gets next string seperated up until ','  space or EOF and inputs into current_string
if getc(input_buffer) - is space, current_string will be empty
*/
void get_next_string(char **current_string, FILE *input_buffer)
{
	int current_char;
	int string_length = 0;
		
	current_char = getc(input_buffer);

	while( 	current_char != EOF &&
			current_char != (int)(',') &&
			!isspace(current_char))
		 {
			string_length++;
			*current_string =(char*)realloc(*current_string , string_length*sizeof(char));
			check_allocation_error(*current_string);	
			*(*current_string + string_length - 1) = (char)current_char; /*inputting the char in the current position*/
			 current_char = getc(input_buffer);
		 }/*while*/
		
	string_length++;
	*current_string =(char*)realloc(*current_string , string_length*sizeof(char));
	check_allocation_error(*current_string);
	*(*current_string + string_length - 1) = '\0';

	ungetc(current_char, input_buffer);
}/*get_next_string*/


/*
Recives an input of a FILE* 
uses getc until stumbles across a non space char
ungets the last char stumbled upon
returns 1 if reaches \n
returns 2 if reaches EOF
returns 0 otherwise
*/
int go_to_next_non_space_char(FILE* input_buffer)
{
	int current_char = (int)' ';

	while(isspace(current_char))
	{
		if(current_char == (int)'\n')
		{
			ungetc(current_char, input_buffer);
			return 1;
		}
		current_char = getc(input_buffer);
	}

	if(current_char == EOF)
	{		
		ungetc(current_char,input_buffer);
		return 2;
	}
	
	ungetc(current_char,input_buffer);	
	return 0;
}


/* gets and  string and FILE* and ungetcs all the chars of the string from last to first */
void push_string_back(char *string_to_push, FILE* input_buffer)
{
	int i = strlen(string_to_push);
	string_to_push = string_to_push + i -1;
	while(i > 0  )
	{
		ungetc(*(string_to_push), input_buffer);
		string_to_push--;
		i--;
	}
}/*push string back*/


/*
Recives FILE* input_buffer
moves the input buffer pointer to point to the first char of the next line
returns 1 if there is a next line
returns 2 if reached EOF
*/
int go_to_next_line(FILE* input_buffer)
{
	int current_char = getc(input_buffer);
	
	while(1)
	{
		if(current_char == (int)'\n')
			return 1;

		if(current_char == EOF)
			return 2;

		current_char = getc(input_buffer);
	}
}/*go_to_next_line*/
