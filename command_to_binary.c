#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#ifndef COMMNAD_TO_BINARY_C
#define COMMNAD_TO_BINARY_C
#include "command_to_binary.h"
#include "run_1_am_file_line_to_command_processing.h"
#include "command_line.h"
#endif

/*this function converting the command struct to binary
returns 1 if error, 0 if okay and 2 for stack overflow.*/
int command_to_binary(struct Command_Line* current_command, char *source_file_name, int line_num)
{
	
	if(ic + dc > MAX_ADDRESS)
	{
	
		fprintf(stderr, "\nError! The code is too long.\n");
		current_command->error = 1;
		return 2;
	}
	if(check_error(current_command) == 1)
	{
		return 1;
	}
	
	check_label(current_command);

	if(check_instruction(current_command) == 0)
	{
		check_source_op(current_command, line_num);
	 	check_target_op(current_command, line_num);
		word_assigning(1, 3);
		ic++;
		if(check_extra_words_instruction(current_command) == -1)
		{
			current_command->error = 1;
			return 1;
		}
		reset_line_array();
	}else if(check_extern_entry_label(current_command) == 1)
			{
	 			strcpy(current_command->label, current_command->first_argument);
				if(search_label(current_command) == -1)
				{
					current_command->error = 1;
					error_flag_line = 1;
					fprintf(stderr, "Label '%s' already declared in file '%s' as extern.\n", current_command->first_argument, source_file_name);
					return 1;
				}else if(search_label(current_command) != -2)
						{
							add_undeclare_label(current_command->first_argument);
							label_table[lc].extern_entry = 1;
							label_table[lc].declaration = -1;
							++lc;
						}
			}else if(check_extern_entry_label(current_command) == 2)
					{
						strcpy(current_command->label, current_command->first_argument);
						if(search_label(current_command) == 0)
						{
							add_undeclare_label(current_command->first_argument);
							label_table[lc].extern_entry = 2;
							++lc;
						}
					}else{
							check_data(current_command);
						}
			if(current_command->comment_or_empty_line == 1)
			{
				return 0;
			}
	return 0;
	
}

void resert_label_table()
{
	memset(label_table, 0, sizeof(label_table));
}

void reset_variables(int *ic, int *dc, int *lc, int *error_flag_line, int *num_of_macros, int *word_counter, int *macro_flag)
{
	*ic = *dc = *lc = *error_flag_line = *num_of_macros = *word_counter = 0;
	*macro_flag = 0;
}

void entry_file_creation(int lc, int error_flag_line, char *source_file_name)
{
	int j = 0;
	FILE *entry_ptr;

	while(j < lc )
		{
			if(error_flag_line == 1)
			{
				++j;
				continue;
			}
			if(label_table[j].extern_entry == 2)
			{
				source_file_name = strtok(source_file_name, ".");
				strcat(source_file_name, ".ent");
				entry_ptr = fopen(source_file_name, "w+");
				if(entry_ptr == NULL)
				{
					fprintf(stderr, "\nError: something went wrong with creating entry file - '%s'. please try again\n\n",source_file_name);
					exit(-1);
				}
				fputs(label_table[j].name, entry_ptr);
				fputs("\t", entry_ptr);
				fprintf(entry_ptr, "\t");
				fprintf(entry_ptr, "%d\n", label_table[j].address);
				fclose(entry_ptr);
			}
			++j;	
				
		}

	
	
}

void reset_line_array()
{
	int i;

	for(i = 0; i < 12; i++)
	{
		binary_line[i] = 0;
	}
}

void word_assigning(int num, int n)
{
	int i;
	int j;

	if(n == 3)
	{
		if(num == 1)
		{
			for(i = 0, j = 11; i <= 11; i++, j--)
			{
				instructions_array[ic][i] = binary_line[j];
			}
		}

		if(num == 2)
		{
			for(i = 0, j = 11; i <= 11; i++, j--)
			{
				instructions_array[ic][i] = extra_word[j];
			}
		}
	}else {
			if(num == 1)
			{
				for(i = 0, j = 11; i <= 11; i++, j--)
				{
					data_array[dc][i] = binary_line[j];
				}
			}

			if(num == 2)
			{
				for(i = 0, j = 11; i <= 11; i++, j--)
				{
					data_array[dc][i] = extra_word[j];
				}
			}
		}
}

int is_number(char s[])
{
	int i;

	if(s[0] == '+' || s[0] == '-')
	{
		for(i = 0; s[i] != '\0'; i++)
		{
			if(i != 0 && isdigit(s[i]) == 0)
			{
				return 0;
			}
		}
	}else { 	
				for(i = 0; s[i] != '\0'; i++)
				{
					if(isdigit(s[i]) == 0)
					{
						return 0;
					}
				}
			}
	return 1;
}

int check_addressing_method(char *var, int line_num)
{
	int temp;
	int i = 0;

	if(is_number(var) == 1)
	{
		return 1;		
	}
	if(var[0] == '@')
	{
		var = var+2;
		temp = atoi(var);

		if(var[-1] != 'r' || temp > 7 || temp < 0)
		{
			fprintf(stderr, "\nError in line %d: Invalid register - '%s'", line_num -1 , var);
			error_flag_line = 1;
			return -1;
		}
		return 5;
	}
	while(var[i] != '\n' && var[i] != EOF && var[i] != '\0')
	{
		if((var[i] < '0' || var[i] > '9') && (var[i] < 'A' || var[i] > 'Z') && (var[i] < 'a' || var[i] > 'z'))
		{
			fprintf(stderr, "\nError in line %d: Invalid charater in - '%s'.", line_num -1 , var);
			error_flag_line = 1;
			return -1;
		}
		++i;
	}
	return 3;
}

int check_activity_op(int op)
{
	if(op == 1)
	{
		if(binary_line[11] == 1)
		{
			return 5;
		}
		if(binary_line[10] == 1)
		{
			return 3;
		}
		if(binary_line[9] == 1)
		{
			return 1;
		}
	}

	if(op == 2)
	{
		if(binary_line[4] == 1)
		{
			return 5;
		}
		if(binary_line[3] == 1)
		{
			return 3;
		}
		if(binary_line[2] == 1)
		{
			return 1;
		}
	}
	return 0;
}

void two_registers_word(Command_Line *cur_com)
{
	int reg_1_num;
	int reg_2_num;
	int i;
	int j;
	i = 7;
	j = 3;

	reg_1_num = (cur_com->first_argument[2]) - '0';
	reg_2_num = (cur_com->second_argument[2]) - '0';

	while(i <= 9)
	{
		extra_word[i] = instruction_to_binary[reg_1_num][j];

		i++;
		j--;
	}

	i = 2;
	j = 3;

	while(i <= 4)
	{
		extra_word[i] = instruction_to_binary[reg_2_num][j];

		i++;
		j--;
	}
}

void convert_number_to_binary(int num)
{
	int i;
	int abs_num = abs(num);
	int one_flag = 0;

	for(i = 0; abs_num > 0;)
	{
		extra_word[i++] = abs_num % 2;
		abs_num /= 2;
	}
	if(num < 0)
	{
		i = 0;
		while(i <= 11)
		{
			if(one_flag == 0)
			{
				if(extra_word[i] == 0)
				{
					i++;
					continue;
				}
				if(extra_word[i] == 1)
				{
					one_flag = 1;
					i++;
					continue;
				}
			}
			if(extra_word[i] == 0)
			{
				extra_word[i] = 1;
			}else extra_word[i] = 0;
			i++;
		}
	}
}

void convert_number_to_binary_from_string(char *n)
{
	int i;
	int num = atoi(n);
	int abs_num = abs(num);
	int one_flag = 0;

	for(i = 2; abs_num > 0;)
	{
		extra_word[i++] = abs_num % 2;
		abs_num /= 2;
	}
	if(num < 0)
	{
		i = 2;
		while(i <= 11)
		{
			if(one_flag == 0)
			{
				if(extra_word[i] == 0)
				{
					i++;
					continue;
				}
				if(extra_word[i] == 1)
				{
					one_flag = 1;
					i++;
					continue;
				}
			}
			if(extra_word[i] == 0)
			{
				extra_word[i] = 1;
			}else extra_word[i] = 0;
			i++;

		}
	}
}

void insert_register_num(char *n, int start, int current_line)
{
	int num = n[2] - '0';

	if(num < 0 || num > 7)
	{
		fprintf(stderr, "Error in line %d: Invalid register number: %d.\n", current_line,num);
		error_flag_line = 1;
		return;
	}
	
	while(num > 0)
	{
		extra_word[start++] = num % 2;
		num /= 2;
	}
}

void reset_extra_word_array()
{
	int i;
	
	for (i = 0; i <= 11; i++)
	{
		extra_word[i] = 0;
	}
}

void insert_source_register(Command_Line *cur_com)
{
	
}

int label_index(Command_Line *cur_com, int var)
{
	int i;

	if(var == 1)
	{
		for(i = 0; i < lc; i++)
		{
			if(!strcmp(label_table[i].name, cur_com->first_argument))
			{
				return i;
			}
		}
		return -1;
	}else 
		{
			for(i = 0; i < lc; i++)
			{
				if(!strcmp(label_table[i].name, cur_com->second_argument))
				{
					return i;
				}
			}
			return -1;
		}
}

void add_undeclare_label(char *lab)
{
		strcpy(label_table[lc].name, lab); 
		label_table[lc].declaration = -1;

}

int check_extra_words_instruction(Command_Line *cur_com)
{
	int source_op_flag = 0;
	int target_op_flag = 0;

	if(cur_com->command_name_enum == 14 || cur_com->command_name_enum == 15)
	{
		return 0;
	}

	source_op_flag = check_activity_op(1);
	target_op_flag = check_activity_op(2);
	
	if((cur_com->command_name_enum > 1 && cur_com->command_name_enum < 12 && target_op_flag == 1) || (cur_com->command_name_enum == 0 && target_op_flag == 1) || (cur_com->command_name_enum == 13 && target_op_flag == 1))
	{
		fprintf(stderr, "\nError in line %d: Invalid target operand.\n", cur_com -> current_line);
		return -1;
	}
	if((cur_com->command_name_enum > 3 && cur_com->command_name_enum < 6 && source_op_flag != 0) || (cur_com->command_name_enum > 6 && cur_com->command_name_enum < 16 && source_op_flag != 0))
	{
		fprintf(stderr, "\nError in line %d: Invalid source operand.\n", cur_com -> current_line);
		return -1;
	}
	if((cur_com->command_name_enum > 3 && cur_com->command_name_enum < 6) || (cur_com->command_name_enum > 6 && cur_com->command_name_enum < 14))
	{
		if(target_op_flag == 1)
		{
			convert_number_to_binary_from_string(cur_com->first_argument);
			word_assigning(2, 3);
			++ic;
			reset_extra_word_array();
			return 0;
		}

		if(target_op_flag == 5)
		{
			insert_register_num(cur_com->first_argument, 2, cur_com -> current_line);
			word_assigning(2, 3);
			++ic;
			reset_extra_word_array();
			return 0;
		}
		if(target_op_flag == 3) 
		{
			extra_word[0] = 2;
			extra_word[1] = label_index(cur_com, 1);
			if(extra_word[1] == -1)
			{
				add_undeclare_label(cur_com->first_argument);
				extra_word[1] = lc;
				++lc;
			}
			word_assigning(2, 3);
			++ic;
			reset_extra_word_array();
			return 0;
		}
	}else {
			if(source_op_flag == 5 && target_op_flag == 5)
			{
				two_registers_word(cur_com);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();
				return 0;
			}

			if(source_op_flag == 5 && target_op_flag == 1)
			{
				insert_register_num(cur_com->first_argument, 7, cur_com -> current_line);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();

				convert_number_to_binary_from_string(cur_com->second_argument);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();
				return 0;
			}

			if(source_op_flag == 5 && target_op_flag == 3)
			{
				insert_register_num(cur_com->first_argument, 7, cur_com -> current_line);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();

				extra_word[0] = 2;
				extra_word[1] = label_index(cur_com, 2);
				if(extra_word[1] == -1)
				{
					add_undeclare_label(cur_com->second_argument);
					extra_word[1] = lc;
					++lc;
				}
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();
				return 0;
			}

			if(source_op_flag == 3 && target_op_flag == 5)
			{
				extra_word[0] = 2;
				extra_word[1] = label_index(cur_com, 1);
				if(extra_word[1] == -1)
				{
					add_undeclare_label(cur_com->first_argument);
					extra_word[1] = lc;
					++lc;
				}
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();

				insert_register_num(cur_com->second_argument, 2, cur_com -> current_line);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();
				return 0;
			}

			if(source_op_flag == 3 && target_op_flag == 3)
			{
				extra_word[0] = 2;
				extra_word[1] = label_index(cur_com, 1);
				if(extra_word[1] == -1)
				{
					add_undeclare_label(cur_com->first_argument);
					extra_word[1] = lc;
					++lc;
				}
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();

				extra_word[0] = 2;
				extra_word[1] = label_index(cur_com, 2);
				if(extra_word[1] == -1)
				{
					add_undeclare_label(cur_com->second_argument);
					extra_word[1] = lc;
					++lc;
				}
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();
				return 0;
			}

			if(source_op_flag == 3 && target_op_flag == 1)
			{
				extra_word[0] = 2;
				extra_word[1] = label_index(cur_com, 1);
				if(extra_word[1] == -1)
				{
					add_undeclare_label(cur_com->first_argument);
					extra_word[1] = lc;
					++lc;
				}
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();

				convert_number_to_binary_from_string(cur_com->second_argument);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();
				return 0;
			}

			if(source_op_flag == 1 && target_op_flag == 5)
			{
				convert_number_to_binary_from_string(cur_com->first_argument);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();

				insert_register_num(cur_com->second_argument, 2, cur_com -> current_line);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();
				return 0;
			}

			if(source_op_flag == 1 && target_op_flag == 3)
			{
				convert_number_to_binary_from_string(cur_com->first_argument);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();

				extra_word[0] = 2;
				extra_word[1] = label_index(cur_com, 2);
				if(extra_word[1] == -1)
				{
					add_undeclare_label(cur_com->second_argument);
					extra_word[1] = lc;
					++lc;
				}
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();
				return 0;
			}

			if(source_op_flag == 1 && target_op_flag == 1)
			{
				convert_number_to_binary_from_string(cur_com->first_argument);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();

				convert_number_to_binary_from_string(cur_com->second_argument);
				word_assigning(2, 3);
				++ic;
				reset_extra_word_array();
				return 0;
			}
		}
	return 0;
}

int check_error(Command_Line *cur_com)
{
	if(cur_com->error == 1)
	{
		return 1;
	}

	
	return 0;
}

void string_line(Command_Line *cur_com)
{
	
}

void check_target_op(Command_Line *cur_com, int line_num)
{
	int i;
	int j;
	int add_method_num;
	i = 2;
	j = 3;

	if(cur_com->command_name_enum > 13 || cur_com->command_name_enum < 0)
	{
		return;
	}
	if(cur_com->command_name_enum > 6 || cur_com->command_name_enum == 5 || cur_com->command_name_enum == 4)
	{
		add_method_num = check_addressing_method(cur_com->first_argument, line_num);
	}else{ add_method_num = check_addressing_method(cur_com->second_argument, line_num);
		}

	if(add_method_num == -1)
	{
		error_flag_line = 1;
		return;
	}

	if(add_method_num == 5)
	{
		while(i <= 4)
		{
			binary_line[i] = instruction_to_binary[5][j];

			i++;
			j--;
		}
	}
	if(add_method_num == 1)
	{
		while(i <= 4)
		{
			binary_line[i] = instruction_to_binary[1][j];

			i++;
			j--;
		}
	}
	if(add_method_num == 3)
	{
		while(i <= 4)
		{
			binary_line[i] = instruction_to_binary[3][j];

			i++;
			j--;
		}
	}
}

void check_source_op(Command_Line *cur_com, int line_num)
{
	int i;
	int j;
	int add_method_num;
	i = 9;
	j = 3;
	add_method_num = check_addressing_method(cur_com->first_argument, line_num);

	if(add_method_num == -1)
	{
		error_flag_line = 1;
		return;
	}

	if(cur_com->command_name_enum < 0 || (cur_com->command_name_enum < 6 && cur_com->command_name_enum > 3) || (cur_com->command_name_enum > 6 && cur_com->command_name_enum < 23))
	{
		return;
	}
	
	if(add_method_num == 5)
	{
		while(i <= 11)
		{
			binary_line[i] = instruction_to_binary[5][j];

			i++;
			j--;
		}
	}
	if(add_method_num == 1)
	{
		while(i <= 11)
		{
			binary_line[i] = instruction_to_binary[1][j];

			i++;
			j--;
		}
	}
	if(add_method_num == 3)
	{
		while(i <= 11)
		{
			binary_line[i] = instruction_to_binary[3][j];

			i++;
			j--;
		}
	}

}

int check_instruction(Command_Line *cur_com)
{
	int i;
	int j;
	i = 5;
	j = 3;

	if(cur_com->command_name_enum < 0 || cur_com->command_name_enum > 15)
	{
		return -1;
	}

	while(i <= 8)
	{
		binary_line[i] = instruction_to_binary[cur_com->command_name_enum][j];

		i++;
		j--;
	}
	return 0;
}

int check_extern_entry_label(Command_Line *cur_com)
{

	if(cur_com->command_name_enum == 22)
	{
		return 1; 
	}

	if(cur_com->command_name_enum == 23)
	{
		return 2;
	}

	return 0;
}


int check_instruction_or_data_lable(Command_Line *cur_com)
{
	if(cur_com->command_name_enum <= 15)
	{
		return 1;
	}
	if(cur_com->command_name_enum > 19 && cur_com->command_name_enum < 24)
	{
		return 2;
	}
	return 0;
}

int search_label(Command_Line *cur_com)
{
	int i;

	for(i = 0; i < lc; i++)
	{

		if(!(strcmp(cur_com->label, label_table[i].name)))
		{
			if(label_table[i].declaration == 0)
			{
				if(cur_com->command_name_enum == 23)
				{
					label_table[i].extern_entry = 2;
					return -3;
				}
				return -1;
			}
			if(label_table[i].declaration == -1)
			{
				if(cur_com->command_name_enum == 22)
				{
					label_table[i].extern_entry = 1;
					return -2;
				}else return i+1;
			}
		}
	}
	return 0;
}

void check_label(Command_Line *cur_com)
{
	int i;

	if(strlen(cur_com->label) == 0 || cur_com->command_name_enum == 23 || cur_com->command_name_enum == 22)
	{
		return;
	}
	if((i = search_label(cur_com)) == -1)
	{
		fprintf(stderr, "lable name: %s already exit.\n", cur_com->label);
		fprintf(stderr, "Outputs file will not be created.\n");
		error_flag_line = 1;
		return;
	}
	if(i == -2)
	{
		fprintf(stderr, "lable name: %s declared as extern.\n", cur_com->label);
		fprintf(stderr, "Outputs file will not be created.\n");
		error_flag_line = 1;
		return;
	}

	if(i != 0)
	{
		if(label_table[i-1].extern_entry == 1)
		{
			error_flag_line = 1;
			fprintf(stderr, "Label '%s' already declared as extern.\n", cur_com->label);
			return;
		}
		label_table[i-1].instructions_data = check_instruction_or_data_lable(cur_com);

		label_table[i-1].declaration = 0;

		strcpy(label_table[lc].name, cur_com->label);

		label_table[i-1].address = label_table[i-1].instructions_data == 1 ? ic : dc;

	}else
		{
			label_table[lc].instructions_data = check_instruction_or_data_lable(cur_com);
	
			label_table[lc].extern_entry = check_extern_entry_label(cur_com);

			label_table[lc].declaration = 0;

			strcpy(label_table[lc].name, cur_com->label);
			label_table[lc].address = label_table[lc].instructions_data == 1 ? ic : dc;
			
			lc += 1;
		}

	return;
}

void data_code(Command_Line *cur_com)
{
	int i;
	int end;
	i = 0;
	end = cur_com->arguments_in_data_array;

	while(i < end)
	{
		if(cur_com->data_array[i] > MAX_NUMBER || cur_com->data_array[i] < MIN_NUMBER)
		{
			error_flag_line = 1;
			return;
		}
		convert_number_to_binary(cur_com->data_array[i]);
		word_assigning(2, 4);
		++dc;
		reset_extra_word_array();
		++i;
	}
}

void convert_char_to_binary(char c)
{
	int int_c = (int)(c);

	convert_number_to_binary(int_c);
}

void string_code(Command_Line *cur_com)
{
	int i;
	i = 0;

	while(cur_com->first_argument[i] != '\0')
	{
		convert_char_to_binary(cur_com->first_argument[i]);
		word_assigning(2, 4);
		++dc;
		reset_extra_word_array();
		++i;
	}
	word_assigning(2, 4);
	++dc;
}

int check_data(Command_Line *cur_com)
{
	int com_enum;
	com_enum = cur_com->command_name_enum;

	if(com_enum < 20 || com_enum > 23)
	{
		return -1;
	}

	
	switch(com_enum)
	{
		case 20:
				data_code(cur_com);
				break;
		case 21:
				string_code(cur_com);
				break;
				
				
	}
	return 0;
}

int check_error_operand(Command_Line *cur_com, int line_num)
{
	int command_number = cur_com->command_name_enum;
	int add_first_argument = check_addressing_method(cur_com->first_argument, line_num);
	int add_second_argument = check_addressing_method(cur_com->second_argument, line_num);

	if(add_first_argument == -1 || add_second_argument == -1)
	{
		error_flag_line = 1;
		return -1;
	}

	if((command_number == 0 || command_number == 2 || command_number == 3 || command_number == 6) && add_second_argument == 1)
	{
		fprintf(stderr, "Error in line %d: Invalid second argument type!\n", cur_com -> current_line);
		error_flag_line = 1;
		return -1;
	}

	if(((command_number >= 4 && command_number <= 5) || (command_number >= 7 && command_number <= 11) || command_number == 13) && add_first_argument == 1)
	{
		fprintf(stderr, "Error in line %d: Invalid argument type!\n", cur_com -> current_line);
		error_flag_line = 1;
		return -1;
	}
	return 0;
}


void check_label_table()
{
	int i;


	for(i = 0; i < lc; i++)
	{
		if(label_table[i].extern_entry == 1 && label_table[i].declaration == 0)
		{
			fprintf(stderr, "\nLabel '%s' already declared as extern.\n", label_table[i].name);
			error_flag_line = 1;
			return;
		}
		if((label_table[i].extern_entry == 2 || label_table[i].extern_entry == 0) && label_table[i].declaration == -1)
		{
			fprintf(stderr, "\nError: label '%s' have to be declared in the current file.\n", label_table[i].name);
			error_flag_line = 1;
			return;
		}

	}
}

void reset_instructions_array()
{
	int i;
	int j;

	for(i = 0; i < ic; i++)
	{
		for(j = 0; j < 12; j++)
		{
			instructions_array[i][j] = 0;
		}
		
	}
}

void reset_data_array()
{
	int i;
	int j;

	for(i = 0; i < ic; i++)
	{
		for(j = 0; j < 12; j++)
		{
			data_array[i][j] = 0;
		}
		
	}
}

void conver_label_word_to_binary(int address, int loc)
{
	int i;

	for(i = 9; address > 0;)
	{
		instructions_array[loc][i--] = address % 2;
		address /= 2;
	}
	instructions_array[loc][11] = 0;
	instructions_array[loc][10] = 1;
}

void add_to_extern_file(int line, char *lab_name, char *source_file_name, FILE **extern_ptr)
{
	if(error_flag_line == 1)
	{
		return;
	}
	if(*extern_ptr == NULL)
	{
		source_file_name[strlen(source_file_name)-3] = '\0';
		*extern_ptr = fopen(strcat(source_file_name, ".ext"), "w");

		if(*extern_ptr == NULL)
		{
			fprintf(stderr, "\nError: something went wrong with creating extern file - '%s'.\n\n", source_file_name);
			error_flag_line = 1;
			return;
		}
	}
	fputs(lab_name, *extern_ptr);
	fputs("\t", *extern_ptr);
	fprintf(*extern_ptr, "%d\n", line+99);
}

void convert_label_to_binary(char *source_file_name, FILE** extern_ptr)
{
	int i;

	for(i = 0; i < lc; i++)
	{
		if(label_table[i].extern_entry == 1)
		{
			continue;
		}
		if(label_table[i].instructions_data == 1)
		{
			label_table[i].address += 100;
		}else label_table[i].address += 100 + ic;
	}

	for(i = 0; i < ic; i++)
	{
		if(instructions_array[i][11] != 2)
		{
			continue;
		}
		if(label_table[instructions_array[i][10]].extern_entry == 1)
		{
			add_to_extern_file(i+1, label_table[instructions_array[i][10]].name, source_file_name, &(*extern_ptr));
			instructions_array[i][10] = 0;
			instructions_array[i][11] = 1;
			continue;
		}
		conver_label_word_to_binary(label_table[instructions_array[i][10]].address, i);
	}
}
