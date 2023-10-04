#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#ifndef MACRO_PROCESS_C
#define MACRO_PROCESS_C
#include "macros.h"
#include "command_line.h"
#endif

/* gets the input array of files names and the current index, spreads the macros and generates the .am file
returns 0 if failed in generating file */
int	spread_macros_and_generate_am_file(char *argv[], int i, char  *source_file_name, FILE **source_ptr, FILE **am_ptr)
{

	FILE *prepro_ptr;
	/*manipulate and opening source file includes 'am' file creation*/
	if(open_validation_file_name(argv[i], source_file_name, source_ptr, am_ptr) != 0)
	{
		return 0;
	}
	
	/*collecting the macros*/
	read_macros(*source_ptr, *am_ptr);
	close_two_ptr(source_ptr, am_ptr);

	spread_macros_preparation(am_ptr, source_file_name, &prepro_ptr, argv[i]);
	spread_macros(*am_ptr, prepro_ptr);
	close_two_ptr(am_ptr, &prepro_ptr);

	memset(macros, 0, sizeof(macros));

	return 1;
}/*spread_macros_and_generate_am_file*/

/*this function make preparation to the spread macros stage*/
void spread_macros_preparation(FILE **am_ptr, char *source_file_name, FILE **prepro_ptr, char* file_name)
{
	/*opening am file with differnt mode*/
	*am_ptr = fopen(source_file_name, "r+");
	*prepro_ptr = fopen(strcat(source_file_name, ".am"), "w");
}

/*the function closing the given FILE pointers*/
void close_two_ptr(FILE **first_ptr, FILE **second_ptr)
{
	fclose(*first_ptr);
	fclose(*second_ptr);
}

/*the function checking and opening the current file*/
int open_validation_file_name(char *file_name, char *source_file_name, FILE **source_ptr, FILE **am_ptr)
{

	strncpy(source_file_name, file_name, strlen(file_name)+5);
	*source_ptr = fopen(strcat(source_file_name, ".as"), "r"); 

	if(check_open_file(*source_ptr, source_file_name) == -1)
	{
		return -1;
	}
	source_file_name[strlen(source_file_name)-3] = '\0';
	printf("\n=================================================\n");
	printf("\nStarting compliation process for '%s'.as file:\n", source_file_name);
	*am_ptr = fopen(source_file_name, "w");
	if(check_open_file(*am_ptr, source_file_name) == -1)
	{
		return -1;
	}

	return 0;
}

/*This function receiving a string, remove before and after white space(s) and return his pointer*/
char *trimwhitespace(char *str)
{
  char *end;

  /*Trim leading space*/
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  /*All spaces?*/
    return str;

  /*Trim trailing space*/
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  /*Write new null terminator character*/
  end[1] = '\0';

  return str;
}


/*macro name validation: the function checking if the first word in the current line is a valid macro*/
int check_macro_name(char *name)
{
	char rest_line[MAX_LINE_LENGTH];
	char macro_name[MAX_LINE_LENGTH];
	int i;

	memset(rest_line, 0, sizeof(char));
	sscanf(name, "%s %[^\n]", macro_name, rest_line);

	if(strlen(rest_line) > 0)
	{
		fprintf(stderr, "Error: Extra characters after macro's name declaration.\n");
		exit(-1); 
	}
	for(i = 0; i < NUM_OF_INVALID_MACRO_NAME; i++)
	{
		if(strcmp(action_names[i], name) == 0)
		{
			return -1;
		}
	}
	for(i = 0; i <= num_of_macros; i++)
	{
		if(strcmp(macros[i].name, name) == 0)
		{
			return -2;
		}
	}
	return 0;
}

/*this function copying the given current line into the given file*/
void copy_line(char *current_line, FILE *des_file)
{
	fputs(current_line, des_file);
}


/*this function receiving two files, source and temporary.
additionally, she reading the macros from the source files while copying the text to temporary file not including the macro*/
void read_macros(FILE *source_ptr, FILE *am_ptr)
{
	char temp [5];
	int res;
	char current_line[MAX_LINE_LENGTH];
	char first_word[MAX_LINE_LENGTH];
	char macro_name[MAX_LINE_LENGTH];

	while(fgets(current_line, MAX_LINE_LENGTH, source_ptr) != NULL)
		{

			memset(first_word, 0, sizeof(char));
			memset(macro_name, 0, sizeof(char));

			/*separating the first word from the rest of the sentence*/
			sscanf(current_line, "%s %[^\n]", first_word, macro_name);

			if(first_word[0] == ';')
				continue;

			/*checking if we're in the middle of building a macro*/
			if(macro_flag)
			{
				/*checking if the build completed*/
				if(strcmp("endmcro", first_word) == 0)
					{
						/*checking whether unnecessary characters exist*/
						if(strlen(macro_name) > 0)
						{
							fprintf(stderr, "Error: Extra characters after 'endmcro'.\n");
							exit(-1);
						}
						macro_flag = OFF;
						num_of_macros += 1;
						continue;
					}
				/*adding text to the macro*/
				strcat(macros[num_of_macros].value, current_line);
				continue;
			}
			
			/*checking if current line defining a new macro*/
			if(strcmp("mcro", first_word) == 0)
			{	
				/*macro's name validation*/
				res = check_macro_name(macro_name); 
				if(res == -1)
				{
					fprintf(stderr, "Invalid macro name.\n");
					fprintf(stderr, "Macro's name should be unique'.\n");
					exit(-1);
				}else if(res == -2)
						{
							fprintf(stderr, "Invalid macro name.\n");
							fprintf(stderr, "Macro's name already exist.\n");
							exit(-1);
						}else
							{
							macro_flag = ON;
							/*marco's name definition*/
							strcpy(macros[num_of_macros].name, macro_name);	
							sscanf(current_line, "%s %[^\n]", temp, macros[num_of_macros].name);
							}
				continue;
			}
			/*copying the current line to the temporary file*/
			copy_line(current_line, am_ptr);
		}
}


/*this function receiving two files, temporary and final 'am'.
additionally, she spreading the macros from the temporary file while copying the text to final 'am' file not including the macro's names*/
void spread_macros(FILE *am_ptr, FILE *prepro_ptr)
{
	char current_line[MAX_LINE_LENGTH];
	char first_word[MAX_LINE_LENGTH];
	char rest_sen[MAX_LINE_LENGTH];
	int macro_ind;

	while(fgets(current_line, MAX_LINE_LENGTH, am_ptr) != NULL)
	{

		/*separating the first word from the rest of the sentence*/
		sscanf(current_line, "%s %[^\n]", first_word, rest_sen);
		trimwhitespace(first_word);
		trimwhitespace(rest_sen);

		/*checking whether the first word is a macro*/
		if((macro_ind = is_macro(first_word)) != -1)
		{
			/*copying the macro to the final 'am' file, where is the specific macro appear*/
			fputs(macros[macro_ind].value, prepro_ptr);

			/*checking whether unnecessary characters exist*/
			if(strlen(rest_sen) > 1)
			{
				fputs(rest_sen, prepro_ptr);
				fputs("\n", prepro_ptr);
			}
			continue;
		}
		/*copying the current line to the final 'am' file*/
		copy_line(current_line, prepro_ptr);
		
		/*resetting the rest_string variable*/
		rest_string(rest_sen);
	}
}

/*thie function restting the given string*/
void rest_string(char *string)
{
	int i;
	for(i = 0; i < MAX_LINE_LENGTH; i++)
	{
		string[i] = (int)NULL;
	}
}

/*this function checking if the string is a exists macro*/
int is_macro(char *str)
{
	int i;

	for(i = 0; i < 50; i++)
	{
		if(strcmp(macros[i].name, str) == 0)
		{
			return i;
		}
	}
	return -1;
}

/*checking the file(s) amount.
in case of zero file the function will print an error and return -1*/
int input_length_check(int len)
{
	if(len < 2)
	{
		fprintf(stderr, "One argument expected\n");
		return -1;
	}
	return 0;
}

int check_open_file(FILE *source_ptr, char *source_file_name)
{
	if(source_ptr == NULL)
		{
			fprintf(stderr, "\nError: something went wrong with open the file - '%s'.\n\n", source_file_name);
			return -1;;
		}
	return 0;
}
