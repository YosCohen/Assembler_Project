#ifndef MACROS_H
#define MACROS_H

#define SEPARATE_BY_SPACE macro_ptr = strtok(NULL, " ");
#define MAX_LINE_LENGTH 80
#define MAX_FILE_NAME_LENGTH 80
#define NUM_OF_INVALID_MACRO_NAME 28
#define NUM_OF_ACTION 15
#define OFF 0
#define ON 1

extern int num_of_macros;
extern int macro_flag;
extern char *action_names[28];


/*gets .as file name and spreads macros and generates .am file*/
int	spread_macros_and_generate_am_file(char*[], int i, char*, FILE**, FILE**);

void spread_macros_preparation(FILE **, char*, FILE **, char*);

void close_two_ptr(FILE **, FILE **);

/*the function checking and opening the current file*/
int open_validation_file_name(char *, char*, FILE **, FILE**);

/*checking the file(s) amount.
in case of zero file the function will print an error and return -1*/
int input_length_check(int);

/*this function checking if the string is a exists macro*/
int is_macro(char *);

/*macro name validation: the function checking if the first word in the current line is a valid macro*/
int check_macro_name(char[]);

/*this function copying the given current line into the given file*/
void copy_line(char *, FILE *);

/*this function receiving two files, source and temporary.
additionally, she reading the macros from the source files while copying the text to temporary file not including the macro*/
void read_macros(FILE *, FILE *);

/*this function receiving two files, temporary and final 'am'.
additionally, she spreading the macros from the temporary file while copying the text to final 'am' file not including the macro's names*/
void spread_macros(FILE *, FILE *);

/*thie function restting the given string*/
void rest_string(char *);

/*This function receiving a string, remove before and after white space(s) and return his pointer*/
char *trimwhitespace(char *);

int check_open_file(FILE *, char *);


typedef struct {
	char name[256];
	char value[1000];
} Macro;

Macro macros[50];
#endif
