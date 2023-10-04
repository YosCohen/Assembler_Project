#ifndef COMMAND_TO_BINARY_H
#define COMMAND_TO_BINARY_H

#define MAX_NUMBER 2047
#define MIN_NUMBER -2048
#define MAX_ADDRESS 924

#define RESET_VARIABLES resert_label_table(); reset_instructions_array(); reset_data_array(); reset_variables(&ic, &dc, &lc, &error_flag_line, &num_of_macros, &word_counter, &macro_flag); extern_ptr = NULL; free(source_file_name);

struct Command_Line; 

#ifndef LAMA_H
#define LAMA_H
extern int ic;
extern int dc;
extern int lc;

extern int instruction_to_binary[16][4];
#endif
int binary_line[12];
int extra_word[12];

typedef struct {
	char name[100];
	int address;
	int instructions_data;
	int extern_entry;
	int declaration;
} Label; 

Label label_table[924];


int error_flag_line;
int instructions_array[924][12];
int data_array[924][12];

/*this functions rest label table*/
void resert_label_table();

/*this function converting struct command line to binary*/
int command_to_binary(struct Command_Line*, char *, int);

/*this function reset the variables*/
void reset_variables(int*, int*, int*, int*, int*, int*, int*);

/*this function creating entry file, in case of need*/
void entry_file_creation(int, int, char*);

/*reset the line array to zeros*/
void reset_line_array();

/*this function creating a new word in case of both source and target are registers (addressing #5)*/
void two_registers_word(struct Command_Line*);


/*this function is checking if extra word(s) should be implemented*/
int check_extra_words_instruction(struct Command_Line*);

/*this function is checking if the error flag is on in the command struct*/
int check_error(struct Command_Line*);

/*this function is implements the target operand to binary*/
void check_target_op(struct Command_Line*, int);

/*this function is implements the source operand to binary*/
void check_source_op(struct Command_Line*, int);

/*this functions is checking if the current line is a instructions line*/
int check_instruction(struct Command_Line*);

/*this functions is checking if the current label is extern/entry/non of them*/
int check_extern_entry_label(struct Command_Line*);

/*this functions is checking if the current label is delacred in instruction/data line*/
int check_instruction_or_data_lable(struct Command_Line*);

/*this function is checking if the current lable is exist*/
int search_label(struct Command_Line*);

/*this function implement a new word from string line*/
void string_code(struct Command_Line*);

/*this function checking if the current line is a data type*/
int check_data(struct Command_Line*);

/*Validation to the operands in instruction line*/
int check_error_operand(struct Command_Line*, int);

/*this function is implement a data line*/
void data_code(struct Command_Line*);

/*this function implement a new lable*/
void check_label(struct Command_Line*);

/*this function checking if the given label is exist in the label table. if yes - return his index, otherwise return -1*/
int label_index(struct Command_Line*, int);

/*command_to_binary.h  and command_to_binary.c content*/
/*reset the line array to zeros*/
void reset_extra_word_array();

/*insert the word to the relevant array*/
void word_assigning(int, int);

/*the function is checking if the given string is a number*/
int is_number(char[]);

/*this function checking the addressing method number (shitat mion) of the given argument*/
int check_addressing_method(char*, int);

/*this function checking which operand is actived, source/target operand*/
int check_activity_op(int);

/*this function converting the given number to binary, including negative number by Two's complement method*/
void convert_number_to_binary(int);

/*this function converting the given string to binary, including negative number by Two's complement method*/
void convert_number_to_binary_from_string(char*);

/*this function inserting the register number to the relevant operand (source/target)*/
void insert_register_num(char*, int, int);

/*this function add an undelacred label to the lable table*/
void add_undeclare_label(char*);

/*this function converting the given char to binary*/
void convert_char_to_binary(char);

/*this function reset data array*/
void reset_data_array();

/*this function reset instructions array*/
void reset_instructions_array();

/*this function checking the label table*/
void check_label_table();

/*this function converting label word to binary*/
void conver_label_word_to_binary(int, int);

/*this function converting label from label table to binary*/
void convert_label_to_binary(char *, FILE**);

/*this function creating extern file, in case of need*/
void add_to_extern_file(int , char *, char *, FILE **);

#endif
