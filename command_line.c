/* this file contatins all the functions that set the different fields in the command line struct*/
#include "run_1_am_file_line_to_command_processing.h"
#include "input_buffer_processing.h"
#include "command_line.h"



/* gets command line pointer, char pointer that should point to a string of label, and the linenumber
inputs the given string to the command_line.label
checks the input 
if the label name is invalid prints a message with the line number and returns 1
if all is good returns 0
*/
int command_line_set_label(Command_Line *current_command, char* label, int line_num)
{
	int char_index;
	int string_length = strlen(label);
	char current_char;


	/*empty label name*/
	if( string_length  == 0)
	{
		fprintf(stderr,"\nError in line %d: Excpected label name before smemicolon ':'.", line_num);
		return 1;
	}

	/*string check before setting the attribute*/
	if( string_length  > MAX_LABEL_LENGTH)
	{
		fprintf(stderr,"\nError in line %d: label name \"%s\" is to long. Maximum label length:%d.", line_num, label, MAX_LABEL_LENGTH);
		return 1;
	}
	
	/*first char is a letter check*/
	if(!isalpha(*label))
	{
		fprintf(stderr,"\nError in line %d: label name \"%s\".Label must start with a letter.",line_num, label);
		return 1;
	}
		
	/*all chars are letters or numbers check*/
	for(char_index = 1; char_index < string_length; char_index++)
	{
		current_char = *(label + char_index);
		if(!isalpha(current_char) && !isdigit(current_char))
		{
			fprintf(stderr,"\nError in line %d: label name \"%s\".Labels can contain numbers and letters only.",line_num, label);
			return 1;
		}
	}

	/*string is valid*/
	/*setting the .label to be the input_string*/
	(*current_command).label = malloc((string_length+1)*sizeof(char));
	check_allocation_error((*current_command).label);
	strcpy((*current_command).label, label);
	*(current_command -> label + string_length + 1 ) = '\0' ;

	return 0;

}/*command_line_set_label*/


/* -- command_line_set_command_name -- 
gets a Command_Line pointer, a char pinter to what's supposed to be the commad name, and the line number
checks the command name of it's a valid data / instruction command 
inputs the command name to current_command.command_name
sets current_command.command_name_enum and current_command.excpected_arguments to match.
if no errors found returns 0, if found errors returns 1.
*/
int command_line_set_command_name(Command_Line* current_command, char* command_name, int line_num)
{
	int command_length = strlen(command_name);
	(*current_command).command_name_enum = get_command_enum(command_name); /*-1 if error*/

	if((*current_command).command_name_enum == error)	
	{
		fprintf(stderr,"\nError in line %d: \"%s\" is an invalid command name.",line_num, command_name);
		return 1;		
	}

	set_expected_arguments(current_command);

	/*setting the .label to be the input_string*/
	(*current_command).command_name = malloc(command_length*sizeof(char));
	check_allocation_error((*current_command).command_name);
	strcpy((*current_command).command_name, command_name);

	/* add number of excpected values*/

	return 0;
}/*command_line_set_command_name*/


/* -- command_line_set_first_argument--
gets a Command_Line pointer and a string to be inputed that has passed all syntax checks
sets current_comman.first_argument = string_to_input
*/
void command_line_set_first_argument(Command_Line *current_command,char *string_to_input)
{
	strcpy(current_command -> first_argument, string_to_input);
}/*command_line_set_first_argument*/


/* -- command_line_set_secibd_argument--
gets a Command_Line pointer and a string to be inputed that has passed all syntax checks
sets current_comman.second_argument = string_to_input
*/
void command_line_set_second_argument(Command_Line *current_command,char *string_to_input)
{
	strcpy(current_command -> second_argument, string_to_input);
}/*command_line_set_second_argument*/


/* - command_line_add_number_to_data_array 
input: command_line pointer, argument_string which is in valid number format
inputs the number in the command_line.data_array
*/ 
void command_line_add_number_to_data_array(Command_Line* current_command, char* argument_string)
{
	int current_index = (*current_command).arguments_in_data_array + 1;
	(*current_command).data_array = (int*)realloc( (*current_command).data_array, (current_index * sizeof(int)));

	
	/* allocation error */
	if ((*current_command).data_array == NULL)
	{
		fprintf(stderr,"\nProgram memory error, please re-run.\n");
		exit(0);
	} 
	
	*((*current_command).data_array + current_index - 1) = atoi(argument_string);
	(*current_command).arguments_in_data_array++;
}/*command_line_add_number_to_data_array*/




/* gets a Command_Line pointer  with a set .command name and sets the .expected arguments to match*/
void set_expected_arguments(Command_Line *current_command)
{
	switch ((int)((*current_command).command_name_enum))
	{

		case (int)mov:
		case (int)cmp:
		case (int)add:
		case (int)sub:
		case (int)lea:	
				(*current_command).expected_arguments = two;
				break;
 
		case (int)extern_enum:
		case (int)entry:	
		case (int)not_enum:
		case (int)clr:
		case (int)inc:
		case (int)dec:
		case (int)jmp:
		case (int)bne:
		case (int)red:
		case (int)prn:
		case (int)jsr:	
				(*current_command).expected_arguments = one;
				break; 

		case (int)rts:
		case (int)stop:
				(*current_command).expected_arguments = zero;
				break; 

		case (int)data:
				(*current_command).expected_arguments = data_argument;
				break;

		case (int)string:
				(*current_command).expected_arguments = string_argument;
				break;

	}/*switch*/
}/*set_expected_arguments(Command_Line* current_command)*/

/* -- get_command_enum
gets a string of command_name
returns corresponding comman_enum
*/
enum command_enum get_command_enum(char* command_name)
{
	
	if(strcmp(command_name,"mov") == 0)
		return mov;

	if(strcmp(command_name,"cmp") == 0)
		return cmp;

	if(strcmp(command_name,"add") == 0)
		return add;

	if(strcmp(command_name,"sub") == 0)
		return sub;

	if(strcmp(command_name,"not") == 0)
		return not_enum;

	if(strcmp(command_name,"clr") == 0)
		return clr;

	if(strcmp(command_name,"lea") == 0)
		return lea;

	if(strcmp(command_name,"inc") == 0)
		return inc;

	if(strcmp(command_name,"dec") == 0)
		return dec;

	if(strcmp(command_name,"jmp") == 0)
		return jmp;

	if(strcmp(command_name,"bne") == 0)
		return bne;

	if(strcmp(command_name,"red") == 0)
		return red;

	if(strcmp(command_name,"prn") == 0)
		return prn;

	if(strcmp(command_name,"jsr") == 0)
		return jsr;

	if(strcmp(command_name,"rts") == 0)
		return rts;

	if(strcmp(command_name,"stop") == 0)
		return stop;

	if(strcmp(command_name,".data") == 0)
		return data;

	if(strcmp(command_name,".string") == 0)
		return string;

	if(strcmp(command_name,".extern") == 0)
		return extern_enum;

	if(strcmp(command_name,".entry") == 0)
		return entry;

	return error;

}


/* -- command_line_init_command --
Input:
Command_Line* current_command
initializes all the fields of the current_command struct
*/
void command_line_init_command(Command_Line* current_command)
{

	current_command -> label = malloc(sizeof(char));		
	current_command -> command_name = malloc(sizeof(char));
	current_command -> command_name_enum = -1;
	current_command -> expected_arguments = -1;
	current_command -> error = 0;
	current_command -> current_line = 0;
	current_command -> comment_or_empty_line = 0;
	current_command -> arguments_in_data_array = 0;
	current_command -> first_argument = malloc(sizeof(char));
	current_command -> second_argument = malloc(sizeof(char));		
	current_command -> data_array = malloc(sizeof(int));
	

	*(current_command -> label) = '\0';	
	*(current_command -> command_name) = '\0';
	*(current_command -> first_argument) = '\0';
	*(current_command -> second_argument) = '\0';		
	*(current_command -> data_array) = '\0';

}/*command_line_init_command*/

