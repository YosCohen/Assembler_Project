#include "run_1_am_file_line_to_command_processing.h"
#include "input_buffer_processing.h"
#include "command_line.h"


/*
Gets a command_line struct, a file pointer that points to the am file, ad the line number
processes current line and fills the corresponding command_line fields 
Returns 1 if:
- reached new line '\n'

Returns 2 if:
- reached EOF
*/
int process_input_line_to_command_struct(Command_Line *current_command, FILE *am_file, int line_num)
{
	
	/* ----- Variable init ----- */
	int error = 0;
	char* current_string = malloc(sizeof(char));	
	check_allocation_error(current_string);
	

	/* ----- Main Process ------ */

	/* -- pre label --*/
	current_command -> current_line = line_num;

	error = check_line_before_first_string(current_command ,am_file, line_num);
	if(error)
		return current_command -> error = go_to_next_line(am_file);

	if(current_command -> comment_or_empty_line)
		return go_to_next_line(am_file);
	
	/* -- label -- */
	error = get_and_set_label(current_command, am_file, line_num);
	if(error)
		return current_command -> error = go_to_next_line(am_file);
	
	/* -- command name --*/
	error = get_and_set_command_name(current_command, am_file, line_num);
	if(error)
		return current_command -> error = go_to_next_line(am_file);
	
	/*-- arguments -- */
	error = get_and_set_arguments(current_command, am_file, line_num);
	if(error)
		return current_command -> error = go_to_next_line(am_file);/*returns 1 - new line,  or 2 - EOF*/
	
	/* -- done with current line --*/
	return go_to_next_line(am_file);

}/*process_input_line_to_command_struct*/



/*
gets Command_Line pointer
gets FILE* pointer to a file that next char in buffer is first char after command
gets int of line_num
gets next string
checks if next string is valid command name
if valid command name, sets current_command.command_name, command_name_enum, expected_argumetns to match and returns 0
if has errors reutrns 1
*/
int get_and_set_command_name(Command_Line* current_command, FILE* am_file, int line_num)
{
	int error;
	char* current_string = malloc(sizeof(char));	
	check_allocation_error(current_string);

	get_next_string(&current_string, am_file); /*current string should be label or command name*/
	error = command_line_set_command_name(current_command, current_string, line_num);
	if(error)
		return 1;

	return 0;
}/*get_and_set_command_name*/

/*
gets Command_Line pointer
gets FILE* pointer to a file that next char in buffer is first char after command
gets int of line_num
gets first string and checks if it is label, if not returns the string to am_file and returns 0
checks valid label name, and that there is text after label seperated with space and no commas in between
if all valid returns 0
if not valid returns 1
if reached \n or EOF ungets it to the am_file and returns 1
*/
int get_and_set_label(Command_Line* current_command, FILE* am_file, int line_num)
{
	int string_length;	
	int is_label;
	int error;
	char current_char;
	char* current_string = malloc(sizeof(char));	
	check_allocation_error(current_string);

	get_next_string(&current_string, am_file); /*current string should be label or command name*/
	string_length = strlen(current_string);

	/*checking space after semicolon*/
	error = check_semicolon_in_middle_of_string(current_string);
	if(error)
	{
		printf("\nError in line %d: expected space after semicolon.",line_num);
		return 1;
	}

	is_label = (*(current_string + string_length -1) == ':');
	
	if(is_label)
	{
		*(current_string + string_length -1) = '\0';
		/*inputting the label name to current_command.label*/
		error = command_line_set_label(current_command, current_string, line_num);
		if(error)
			return 1;

		/*checks that area between semicolon and command name is clear*/
		error = check_only_space_to_next_char(am_file);
		if(error)
		{
			fprintf(stderr,"\nError in line %d: ", line_num);
			switch(error)
			{
				case 1:		
				case 2:
						fprintf(stderr,"expected command after label.");	
						return 1;				
				case 3:
						fprintf(stderr,"expected space after label.");	
						return 1;
			}/*switch*/
		}/*if error*/

		/*check no comma*/
		current_char = getc(am_file);
		if(current_char == ',')
		{
			fprintf(stderr,"\nError in line %d: illegal comma after label name.", line_num);
			return 1;
		}

		ungetc(current_char, am_file);
		return 0;
	}/*if label*/

	/*not label, returning the string because it's command*/
	push_string_back(current_string, am_file);
	return 0;
}/*get_and_set_label*/

/* gets stinrg and checks if there is a semicolon in the middle*/
int check_semicolon_in_middle_of_string(char* string_to_check)
{
	int string_length = strlen(string_to_check);
	int i;

	for(i = 0 ; i < string_length - 1; i++)
	{
		if(*(string_to_check + i) == ':')
			return 1;
	}

	return 0;

}

/*
gets FILE*
checks that there is and there is only space up to next non space char, and that it exists before new line
if stumbles across new line before non space char reutrns 1
if stumbles across EOF before non space char returns 2
if does not find space up to next non space char returns 3
ungetc last char found 
*/
int check_only_space_to_next_char(FILE* am_file)
{
	char current_char = (char)getc(am_file);
	int found_space = 0;
	

	while(isspace(current_char) && 
		  current_char != EOF&&
		  current_char != '\n')
	{
		found_space = 1;
		current_char = (char)getc(am_file);
	}

	if(current_char == '\n')
	{
		ungetc(current_char, am_file);
		return 1;
	}

	if(current_char == EOF)
	{
		ungetc(current_char, am_file);
		return 2;
	}

	if(!found_space)
	{	
		ungetc(current_char, am_file);
		return 3;
	}

	ungetc(current_char, am_file);
	return 0;
}/*check_only_space_to_next_char*/


/* -- get_and_set_arguments --
gets Command_Line pointer that has label, and commmand already initialized
gets FILE* pointer to a file that next char in buffer is first char after command
gets int of line_num
calls the relevent get argumets and set argument function according to the command name
returns 0 if all went well
returns 1 if there was an error
*/
int get_and_set_arguments(Command_Line *current_command, FILE *am_file, int line_num)
{
	
	switch((int)(current_command -> expected_arguments))
	{
		case (int)data_argument:
			return get_and_set_data_arguments(current_command, am_file, line_num);

		case (int)string_argument:
			return process_and_set_string_arguments(current_command, am_file, line_num);

		case (int)zero:
			return check_text_after_command_with_zero_arguments(am_file, line_num);

		case (int)one:
			return process_and_set_one_argument(current_command, am_file, line_num);

		case (int)two:
			return process_and_set_two_arguments(current_command, am_file, line_num);	
	}

	return 0;
}/*get_and_set_arguments*/

/* -- process_and_set_one_argument --
gets: Command_Line pointer, FILE* am_file pointer, int line_num
checks area between command name and first argument is valid
checks that there is a first argument
check no text after first argument
sets the current_command.first_arugment to match
returns 1 if there was an error
returns 0 if no errors 
*/
int process_and_set_one_argument(Command_Line* current_command, FILE* am_file, int line_num)
{ 
 	int error = 0;
	int reached_end_of_line;

	error = check_area_after_command_name(current_command, am_file, line_num);
	if(error)
		return 1;
		
	process_and_set_first_argument(current_command, am_file, line_num);
	
	/*no text after fist argument check*/
	reached_end_of_line = go_to_next_non_space_char(am_file);
	if(!reached_end_of_line)
	{
		fprintf(stderr,"\nError in line %d: extra text at end of command.",line_num);
		return 1;
	}

	return 0;
}/* process_and_set_one_argument */


/*gets current_command, am_file pointer and line_num
checks if there is a space and no comma up to next string wich is supposed ot be the first argument
and that that argument sexists
returns 1 if error
returns 0 if all good
ungets last char
*/
int check_area_after_command_name(Command_Line* current_command, FILE* am_file, int line_num)
{
	int error;
	char current_char;
	error = check_only_space_to_next_char(am_file);
	if(error)
	{
		fprintf(stderr,"\nError in line %d: ", line_num);
		switch(error)
		{
			case 1:		
			case 2:
					fprintf(stderr,"expected argument after '%s'.",current_command -> command_name);	
					return 1;				
			case 3:
					fprintf(stderr,"expected space after '%s'.",current_command -> command_name);	
					return 1;
		}/*switch*/
	}/*if error*/

	/*check no comma*/
	current_char = getc(am_file);
	if(current_char == ',')
	{
		fprintf(stderr,"\nError in line %d: illegal comma after command name", line_num);
		return 1;
	}

	ungetc(current_char, am_file);
	return 0;
}/*check_area_after_command_name*/


/* -- process_and_set_two_arguments --
gets: Command_Line pointer, FILE* am_file pointer, int line_num
checks area between command name and first argument is valid
checks that there is a first argument
checks area between two arguments is valid
checks that there is a second argument
checks that there is no text after second argument
sets the current_command.first_arugment & second_argument to match
returns 1 if there was an error
returns 0 if no errors 
*/
int process_and_set_two_arguments(Command_Line* current_command, FILE* am_file, int line_num)
{ 
 	int error = 0;
	int reached_end_of_line = 0;
	char current_char;
		
	error = check_area_after_command_name(current_command, am_file, line_num);
	if(error)
		return 1;

	/*first argument*/
	process_and_set_first_argument(current_command, am_file, line_num);
	
	/*checking valid area between two argument*/
	reached_end_of_line = go_to_next_non_space_char(am_file);
	if(reached_end_of_line)
	{
		fprintf(stderr,"\nError in line %d: expected another argument after %s",line_num, current_command -> command_name);
		return 1;
	}

	current_char = getc(am_file);
	if(current_char != ',')
	{
		fprintf(stderr,"\nError in line %d: expeceted ',' between arguments'n'",line_num);
		ungetc(current_char, am_file);				
		return 1;
	}

	reached_end_of_line = go_to_next_non_space_char(am_file);
	if(reached_end_of_line)
	{
		fprintf(stderr,"\nError in line %d: expected another argument after %s",line_num, current_command -> command_name);
		return 1;
	} 
		
	
	/*second argument*/
	process_and_set_second_argument(current_command, am_file, line_num);


	/*no text after second argument check*/
	reached_end_of_line = go_to_next_non_space_char(am_file);
	if(!reached_end_of_line)
	{
		fprintf(stderr,"\nError in line %d: extra text at end of command.",line_num);
		return 1;
	}

	return 0;
}/* process_and_set_one_argument */


/* -- process_and_set_second_argument --
gets: Command_Line pointer, FILE* am_file pointer, int line_num
checks area between first and second argument is valid
checks that there is a second argument
sets the current_command.second_arugment to match
returns 1 if there was an error
returns 0 if no errors 
*/
void process_and_set_second_argument(Command_Line* current_command, FILE* am_file, int line_num)
{	
			
	char* string_to_input = malloc(sizeof(char));
	check_allocation_error(string_to_input);


	go_to_next_non_space_char(am_file);
	get_next_string(&string_to_input, am_file);
	command_line_set_second_argument(current_command, string_to_input);

	free(string_to_input);
}/* process_and_set_second_argument*/




/* -- process_and_set_first_argument --
gets: Command_Line pointer, FILE* am_file pointer, int line_num
gets the next string and sets the current_command.first_arugment to match
*/
void process_and_set_first_argument(Command_Line* current_command, FILE* am_file, int line_num)
{

	char* string_to_input = malloc(sizeof(char));
	check_allocation_error(string_to_input);
	go_to_next_non_space_char(am_file);
	get_next_string(&string_to_input, am_file);
	command_line_set_first_argument(current_command, string_to_input);
	free(string_to_input);

}/*process_and_set_first_argument*/



/* process_and_set_string_arguments --
gets
gets Command_Line pointer that has label, and commmand already initialized to .string
gets FILE* pointer to a file that next char in buffer is the first char of the argument
gets int of line_num
gets the string arguemnt, checks it, and inputs it to the Command_Line sturct
returns 0 if all went well
returns 1 if there was an error
*/
int process_and_set_string_arguments(Command_Line* current_command, FILE* am_file, int line_num)
{
	
	int error;
	int reached_end_of_line;
	
	
	char *string_to_input = malloc(sizeof(char));
	check_allocation_error(string_to_input);
	
	/* area between .string and arguments*/
	error = check_area_after_command_name(current_command, am_file, line_num);
	if(error)
		return 1;

	/*getting the string argument and checking the syntax*/
	error = get_string_argument_from_input(&string_to_input, am_file, line_num);
	if(error)
		return 1;
	
	/*checking extra text*/
	reached_end_of_line = go_to_next_non_space_char(am_file);
	if(!reached_end_of_line)
	{
		fprintf(stderr,"\nError in line %d: extra text at end of command.",line_num);
		return 1;
	}
	
	command_line_set_first_argument(current_command, string_to_input);
	free(string_to_input);
	return 0;
}/* process_and_set_string_arguments*/


/* gets the argument give after .string*/
int get_string_argument_from_input(char **string_to_input, FILE* am_file, int line_num)
{
	int error = 0;
	int char_count = 0;
	char current_char;

	error = go_to_next_non_space_char(am_file);
	if(error)
	{
		fprintf(stderr,"\nError in line %d: excpected string after '.string'.",line_num);
		return 1;
	}	
	
	current_char = (char)getc(am_file);
	
	if(current_char != '"')
	{
		fprintf(stderr,"\nError in line %d: expected \" before string argument.", line_num);
		return 1;
	}
	
	current_char = (char)getc(am_file);
	if(current_char == '"')
	{
		fprintf(stderr,"\nError in line %d: expected text between \"\"", line_num);
		return 1;
	}

	while(current_char != '"' &&
		  current_char != EOF &&
		  current_char != '\n')
	{
		*string_to_input = realloc(*string_to_input, (char_count + 1) *sizeof(char));
		check_allocation_error(*string_to_input);
		*(*string_to_input + char_count) = current_char;
		char_count++;
		current_char = getc(am_file);	
	}

	if(current_char == EOF || current_char == '\n')
	{
		fprintf(stderr,"\nError in line %d: .string text argument must end with \".",line_num);
		ungetc(current_char,am_file);
		return 1;
	}

	*(*string_to_input + char_count) = '\0';
	return 0;
}/*get_string_argument_from_input*/


/*recives FILE pointer with current char at buffer as next char after command with zero arguments and line num 
checks that there is no text up to end of line, 
if all is good, returns 0
if stumbles upon a char returns 1 and prints out an error message
*/
int check_text_after_command_with_zero_arguments(FILE* am_file, int line_num)
{
	int there_is_no_text_after_command = go_to_next_non_space_char(am_file);
	
	/*reached end of row without any text*/
	if(there_is_no_text_after_command)
		return 0;
	
	/*else*/
	fprintf(stderr,"\nError in line %d: extra text at end of command.",line_num);
	return 1;
}/*check_text_after_command_with_zero_arguments*/




/*checks the syntax before reaching the label
1. if a comment line - sets current_command -> comment_or_empty_line = 1 and returns 0
2. if empty line returns - sets current_command -> comment_or_empty_line = 1 and returns 0
3. if starts with a comma  or ':' returns 1 
4. if starts with a letter, number - returns 0.
5. if line over 80 returns 1
*/
int check_line_before_first_string(Command_Line* current_command,FILE* am_file, int line_num)
{
	char current_char;
	int empty_line, comment_line;

	/*ilegal first char error*/
	current_char = getc(am_file);
	if(current_char == ',')
	{
		fprintf(stderr,"\nError in line %d: illegal comma.",line_num);
		return 1;
	}

	if(current_char == ':')
	{
		fprintf(stderr,"\nError in line %d: expected label name before ':'",line_num);
		return 1;
	}
	ungetc(current_char,am_file);

	/*line over 80 chars check*/
	if(line_over_80_chars(am_file, line_num))
		return 1;


	comment_line = is_comment_line(am_file);
	empty_line = go_to_next_non_space_char(am_file);

	if(comment_line || empty_line)
		current_command -> comment_or_empty_line = 1;
	
	return 0;
}


/*recives FILE*
checks if the current line is a comment line
if no comment line, returns 0
if comment line returns 1
*/
int is_comment_line(FILE *am_file)
{
	char current_char;
	
	current_char = getc(am_file);
	if(current_char == ';')
		return 1;
	
	ungetc(current_char,am_file);
	return 0;

}/*is_comment_line*/




/*checks if given string that was allocated did not end in error*/
void check_allocation_error(char* string_to_check)
{
	if (string_to_check == NULL)	
	{
			PRINT_ALLOCATION_ERROR;
			exit(-1);/* todo check if okay*/
	}
}/*check_allocation_error*/


/* -- get_and_set_data_argumetns --
input: Command_Line *current_command, FILE *am_file, int line_num
next getc: first non space char after .data command, already checked that there is some argument there
checks all arguments if valid and inputs them as values in current_command.data_array
returns 0 if all is good 
returns 1 if had error, prints message 
*/
int get_and_set_data_arguments(Command_Line* current_command, FILE* am_file, int line_num)
{
	/* -- Variable initialization --*/
	int error = 0;	
	int end_of_line = 0;

	char *argument_string = malloc(sizeof(char));	
	check_allocation_error(argument_string);

	(*current_command).arguments_in_data_array = 0;
	
	/* area between .data and arguments */
	error = check_area_after_command_name(current_command, am_file, line_num);
	if(error)
		return 1; 

	/* -- Main loop: going throught the arguments in the string--*/
	while(!end_of_line)
	{
		get_next_string(&argument_string, am_file); 
		error = check_valid_number(argument_string, line_num);
		if(error)
			return 1;

		command_line_add_number_to_data_array(current_command, argument_string);

		/*after the number*/
		end_of_line = go_to_next_non_space_char(am_file);

		if(!end_of_line)
			error = go_to_next_number_to_input(am_file, line_num);
		
		if(error)
			return 1;		
	}/*while !end_of_line*/

	return 0;
}/*get_and_set_data_argumetns*/






/* go_to_next_number_to_input
recives a am_file that next char in buffer is first non space char after number for .data argument
checks if:
	- there is a comma between this and next string (wich is supposed to be the next number to input)
	- there is only one comma up to next string 
	- there is a string after the comma
if found error in any of the above returns 1
if no errors found returns 0
*/
int go_to_next_number_to_input(FILE* am_file, int line_num)
{
	char current_char;
	int end_of_line;
	
	/*expecting comma*/
	current_char = (char)getc(am_file);
	if(current_char != ',')
	{
		fprintf(stderr,"\nError in line %d: expected comma between arguments",line_num);
		return 1;
	}

	/*-- expecting number after comma --*/
	end_of_line = go_to_next_non_space_char(am_file);
	
	/*comma after last number*/
	if(end_of_line)
	{
		fprintf(stderr,"\nError in line %d: extra text at end of command.",line_num);	
		return 1;
	}		
	
	current_char = (char)getc(am_file);
	if(current_char ==',')
	{
		fprintf(stderr,"\nError in line %d: two consecutive commas.",line_num);		
		return 1;
	}

	ungetc(current_char,am_file);
	return 0;
}









/* -- check_valid_number -- 
gets a string and line_num
checks if the string is a valid number
if not valid number fomrat prints message and returns 1
if valid number format returns 0*/
int check_valid_number(char *argument_string, int line_num)
{
	char current_char = (*argument_string);
	int i = 1;

	 /*first char*/
	if(current_char == '+' || current_char == '-' )
	{
		current_char = *(argument_string + 1);
		if(current_char == '\0') /*argument is just '+' or '-'*/
		{
			fprintf(stderr,"\nError in line %d: '%s' is not a valid number format.",line_num, argument_string);
			return 1;
		}
		i++;
	}

	/* rest of the string */
	while(current_char != '\0')/*end of string*/
	{
		if(!isdigit(current_char))
		{
			fprintf(stderr,"\nError in line %d: '%s' is not a valid number format.",line_num, argument_string);
			return 1;
		}
		current_char = *(argument_string + i);
		i++;
	}

	return 0;
}/*check_valid_number*/

/* -- line_over_80_chars
gets FILE* and line_num int
checks if current line in input buffer is max 80 chars long (not including \n or EOF at end of line)
if over 80 chars prints error message and returns 1
else reutnrs 0
ungets last char if error
*/
int line_over_80_chars(FILE* am_file, int line_num)
{
	
	char chars[83];
	char current_char = 'a';
	
	int i=0;
	while(current_char != EOF && current_char !='\n')
	{
		current_char = (char)getc(am_file);	
		chars[i] = current_char;
		i++;
	
		if(i>81)/*81 because 81st char might be \n */
		{
			fprintf(stderr,"\nError in line %d: line cannot exceed over 80 characters.",line_num);
			ungetc(current_char, am_file);
			return 1;		
		}
	}
	
	/*ungetting the chars*/
	i--;
	while(i != -1)
	{
		current_char = chars[i];
		ungetc(current_char, am_file);
		i--;
	}

	return 0;
}/*line_over_80_chars*/
