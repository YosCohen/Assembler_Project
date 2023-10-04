/***********************
generate_ob_file.c
includes all the required functions for generating an .ob file using the data and instruction binary line arrays
***********************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "generate_ob_file.h"
#include "run_1_am_file_line_to_command_processing.h"


/* --- process_instruction_and_data_arrays_to_base_64_ob_file ---
gets a file name for the new .ob file
gets instruction and data arrays representing the binary rows of the program
generates a file_name.ob file with the rows as the binary rows translated to base 64
*/
void generate_ob_file(char *file_name, int instruction_array[][12], int data_array[][12], int num_of_instruction_lines, int num_of_data_lines)
{
	char *full_file_name;
	FILE *ob_file;

	full_file_name = malloc((strlen(file_name)+3)*sizeof(char));
	check_allocation_error(full_file_name);
	
	/*creating full file name*/
	full_file_name = strcpy(full_file_name, file_name);
	full_file_name = strcat(full_file_name, ".ob");


	/* -- Creating the new file and inputting data --*/
	ob_file = fopen(full_file_name, "w");

	if(ob_file == NULL)
	{
		fprintf(stderr,"\nERROR! please run again.");
		exit(-1);
	}

	/*adding the first line*/
	add_instruction_and_data_count_to_first_line(ob_file, num_of_instruction_lines, num_of_data_lines);

	/*adding the instruction lines*/	
	add_array_to_ob_file(ob_file, instruction_array, num_of_instruction_lines);
	add_array_to_ob_file(ob_file, data_array, num_of_data_lines);

	fclose(ob_file);
	free(full_file_name);

}

/* add_array_to_ob_file 
gets a FILE pointer to ob_file, and array of 12 bit binary lines and the number of lines in the array
translates the binary lines to base 64 and adds them to the file
*/
void add_array_to_ob_file(FILE *ob_file, int array[][12], int num_of_lines)
{
	int i;
	char *current_line = malloc(3*sizeof(char));
	check_allocation_error(current_line);
	/*adding the data lines*/	
	for(i = 0; i < num_of_lines; i++)
	{		
		process_binary_line_to_base_64_string(array[i], current_line);
		fputs(current_line, ob_file);
		fputs("\n", ob_file );
	}	

}

/* add_instruction_and_data_count_to_first_line
gets a pointer to .ob file and the value of instruction and data lines 
adds the values to the first line of the .ob file
*/
void add_instruction_and_data_count_to_first_line(FILE* ob_file, int num_of_instruction_lines, int num_of_data_lines)
{
	add_num_to_file(ob_file, num_of_instruction_lines);
	fputc((int)' ', ob_file);
	add_num_to_file(ob_file, num_of_data_lines);
	fputc((int)'\n', ob_file);	
}

/*add_num_to_file
gets a FILE pointer to .ob file and an int value
adds the int value to the .ob file
*/
void add_num_to_file(FILE* ob_file, int num)
{
	int largest_ten = 1;
	int digit_char; /* int representation of the char to add*/

	while( (num / largest_ten) > 9 )
		largest_ten *= 10;

	while(largest_ten != 0)
	{
		digit_char = (num / largest_ten) + 48; /*int representati ascii char digit*/
		num = num%10;
		fputc(digit_char,  ob_file);
		largest_ten /= 10;
	}
}/*add_num_to_file*/


/* --- process_binary_line_to_base_64_string ---
gets an int array of 12 bits, and a char pointer, 
translates the array to base 64 and inputs the result to the string 
note! the array is representing an upside down binary line, meaning that binary_line[11] is the 0 bit of the array and
binary_line[0] is the 11 bit of the array.
*/
void process_binary_line_to_base_64_string(int binary_line[12], char *base_64_string)
{

	int right_num = 0;/*representing the 0-5 bits in binary_line in decimal*/
	int left_num = 0;/*representing the 6-11 bits in binary_line in decimal*/
	char right_char, left_char;/*frist_num and second_num in base 64*/
	int current_index = 0;
	int	twos_power = 1; /*for calculating the binary to decimal*/

	/*right num - binary to decimal, loop decending because the binary line is upside down*/
	for(current_index = 11; current_index > 5 ; current_index --)
	{
		right_num += binary_line[current_index]*twos_power;
		twos_power *= 2;
	}

	twos_power = 1;

	/*left num - binary to decimal*/
	for(current_index = 5; current_index >-1 ; current_index --)
	{
		left_num += binary_line[current_index]*twos_power;
		twos_power *= 2;
	}

	left_char = num_to_base_64_char(left_num);
	right_char = num_to_base_64_char(right_num);

	*base_64_string = left_char;
	*(base_64_string + 1) = right_char;
	*(base_64_string + 2) = '\0';

} /*process_binary_line_to_base_64_string*/


/* num_to_base_64_char 
gets an int number between 0-63) (does not check)
returns a char of the base 64 equivelent
*/
char num_to_base_64_char(int num)
{

	if(num < 26)
		return ('A' + num);

	if(num < 52) /*  25 < num < 52 */
		return ('a' + num - 26); /*because num = 26 then char = 'a'*/

	if(num < 62) /* 51 < num < 62*/
		return (num - 4); /*because num = 52 then char = '0' ascii code 48*/

	if(num == 62)
		return '+';

	/*num == 63*/
	return '/';

} /*num_to_base_64_char*/


