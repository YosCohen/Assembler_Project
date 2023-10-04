#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#define MAX_LABEL_LENGTH 31
#define PRINT_ALLOCATION_ERROR printf("\n Program memory error, terminating program, please try again");

/*======================================================================*/
/* ------- Run 1 .am file to command_line processing definitions ------ */
/*=====================================================================*/
/*declared in command_line.h*/
struct Command_Line;

/* processes am_file to a binary lines and inputs it in instruction_array and data_array, also adds labels to label table
does not translate the label adresses to binary, this is done in a latter stage*/
int proccess_am_to_binary_without_labels(char *source_file_name, FILE *am_ptr);

/* -- command_line_init_command --
Input:
Command_Line* current_command
initializes all the fields of the current_command struct
*/
void command_line_init_command(struct Command_Line* current_command);

/*checks if given char pointer is allocated properly*/
void check_allocation_error(char*);

/*checks if current line in the FILE* is a comment line*/
int is_comment_line(FILE*);

/* checks curent line of am file for comment/empty line, size limitations and illegal first characers*/
int check_line_before_first_string(struct Command_Line*,FILE* , int);

/*check that ther is no extra text after command that gets zero args*/
int check_text_after_command_with_zero_arguments(FILE*, int );

/*processes the arguments of the current line and inputs the values to the coressponding Command_Line fields*/
int get_and_set_arguments(struct Command_Line*, FILE*, int );

/*gets the .string comand argument and inputs to command_line.first_argument*/
int process_and_set_string_arguments(struct Command_Line* , FILE* , int );

/* gets the argument give after .string*/
int get_string_argument_from_input(char**, FILE*, int );

/* processes and sets one arugment to Command_Line.first_argument*/
int process_and_set_one_argument(struct Command_Line* , FILE* , int );

/*processes and sets the first argument to Command_Line*/
void process_and_set_first_argument(struct Command_Line* , FILE* , int );

/*processes two arugments from am file line and sets them to the command_line fields*/
int process_and_set_two_arguments(struct Command_Line* , FILE* , int );

/*processes the second argument from the current line in am file and sets the command_line.second_argument value*/
void process_and_set_second_argument(struct Command_Line* , FILE* , int );

/*gets the label from current line in the am file and sets the current_command.label value*/
int get_and_set_label(struct Command_Line* current_command, FILE* am_file, int line_num);

/*gets the commnad name from current line in the am file and sets the value in command_line to match*/
int get_and_set_command_name(struct Command_Line* current_command, FILE* am_file, int line_num);

/*gets a string and checks if there is a semicolon in the middle of it*/
int check_semicolon_in_middle_of_string(char* string_to_check);

/*gets the arguments after .data command in current line of am file and inputs them to the data argument array*/
int get_and_set_data_arguments(struct Command_Line* , FILE* , int );

/* goes to the next number in the .data argument*/
int go_to_next_number_to_input(FILE*, int);

/*gets a string and checks if it is a valid number format*/
int check_valid_number(char*, int);

/*checks that the area after the command up to first argument in the current line of am file is valid syntax wise*/
int check_area_after_command_name(struct Command_Line* current_command, FILE* am_file, int);

/*checks that there is and there is only space up to next non space char, and that it exists before new line*/
int check_only_space_to_next_char(FILE* am_file);

/*checks that current line in am file is not over 80 chars long*/
int line_over_80_chars(FILE* am_file, int line_num);

/*
Gets a command_line struct, a file pointer that points to the am file, ad the line number
processes current line and fills the corresponding command_line fields 
*/
int process_input_line_to_command_struct(struct Command_Line*, FILE*, int );


#endif
