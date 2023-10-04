/********************************
Authors: Eitan & Yossi Cohen
Version: 2023b
*********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "run_1_am_file_line_to_command_processing.h"
#include "input_buffer_processing.h"
#include "command_to_binary.h"
#include "macros.h"
#include "command_line.h"
#include "generate_ob_file.h"


int ic = 0;
int lc = 0;
int dc = 0;
int num_of_macros = 0;
int word_counter = 0;
int macro_flag = OFF;
int instruction_to_binary[16][4] = {{0,0,0,0}, {0,0,0,1}, {0,0,1,0}, {0,0,1,1}, {0,1,0,0}, {0,1,0,1}, 										{0,1,1,0}, {0,1,1,1}, {1,0,0,0}, {1,0,0,1}, {1,0,1,0}, {1,0,1,1}, 										{1,1,0,0}, {1,1,0,1}, {1,1,1,0}, {1,1,1,1}};
char *action_names[28] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", 								"inc", "dec", "jmp", "bne", "red", "prn", "jsr", 								"rts", "stop", ".data", ".string", ".entry", ".extern", "@r0", "@r1",
							"@r2", "@r3", "@r4", "@r5", "@r6", "@r7"};

int main (int argc, char *argv[])
{
	FILE *source_ptr = NULL, *am_ptr, *extern_ptr = NULL;
	int i = 0;
	int number_of_files = argc;
	char *source_file_name;
	
	/*checking if has at least one 'as' file*/
	if(input_length_check(argc)) 
		exit(-1);

	/*preprocessor run for each given '.as' file(s)*/
	for(i = 1; i < number_of_files; i++)
	{	
		/*memory allocation*/
		source_file_name = (char*)malloc((strlen(argv[i])+5) * sizeof(char));
		check_allocation_error(source_file_name);

		if(!spread_macros_and_generate_am_file(argv, i, source_file_name, &source_ptr,  &am_ptr)) /*errors while genrating .am file*/
			continue;
		
		if(proccess_am_to_binary_without_labels(source_file_name, am_ptr) !=0 )
		{
			printf("\n\nCompilation Result: Errors in compiling file '%s'.as\n", source_file_name);
			RESET_VARIABLES
			continue;
		}

		check_label_table();
		convert_label_to_binary(source_file_name, &extern_ptr);
		entry_file_creation(lc, error_flag_line, source_file_name);
		source_file_name = strtok(source_file_name, ".");
		
		if(!error_flag_line)
		{
			generate_ob_file(argv[i], instructions_array, data_array, ic, dc);
			printf("\n\nCompilation Result: File '%s.as' was compilied succesfully.\n",source_file_name);
		}
		else
			printf("\n\nCompilation Result: Errors in compiling file '%s'.as\n", source_file_name);

		/***************closing and reset stage***************/

		/*preparing to next given file*/
		resert_label_table();
		reset_instructions_array();
		reset_data_array();
		reset_variables(&ic, &dc, &lc, &error_flag_line, &num_of_macros, &word_counter, &macro_flag);

		extern_ptr = NULL;
		free(source_file_name);
	}	
	return 0;
}



/*processes am_file to a binary lines and inputs it in instruction_array and data_array, also adds labels to label table
does not translate the label adresses to binary, this is done in a latter stage*/
int proccess_am_to_binary_without_labels(char *source_file_name, FILE *am_ptr)
{
	int reached_end_of_file = 0;
	Command_Line *current_command;
	int line_num = 1;
	int return_value = 0; 
	int error = 0;
	int exceeded_max_word_limit = 0;
	
	am_ptr = fopen(source_file_name, "r+");
	
	
	/*loop that runs through each line of am_file*/
	while(!reached_end_of_file && !exceeded_max_word_limit)
	{
		current_command = malloc(sizeof(Command_Line));
		command_line_init_command(current_command);
 		reached_end_of_file = process_input_line_to_command_struct(current_command, am_ptr, line_num);
		reached_end_of_file--;
		line_num++;
		error = command_to_binary(current_command, source_file_name, line_num);
		if(error)
		{
			if(error == 2)
				exceeded_max_word_limit = 1;
			return_value = 1;
		}
		free(current_command);

	}

	fclose(am_ptr);
	return return_value;	
}
