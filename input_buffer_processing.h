#ifndef INPUT_BUFFER_PROCESSING_H
#define INPUT_BUFFER_PROCESSING_H
/*
Recives FILE* input_buffer
moves the input buffer pointer to point to the first char of the next line
returns 1 if there is a next line
returns 2 if reached EOF
*/
int go_to_next_line(FILE* input_buffer);

/*
Recives an input of a FILE* 
uses getc until stumbles across a non space char
ungets the last char stumbled upon
returns 1 if reaches \n
returns 2 if reaches EOF
returns 0 otherwise
*/
int go_to_next_non_space_char(FILE* input_buffer);

/* -- get_next_string -- 
Gets:
char** current_string
FILE* input_buffer

What does it do:
inputs the current string up to a space char, ',' or EOF to *current_string

Note that current_string is a pointer to a pointer.
*/
void get_next_string(char **current_string, FILE *input_buffer);

/* gets and  string and FILE* and ungetcs all the chars of the string from last to first */
void push_string_back(char *string_to_push, FILE* input_buffer);
#endif
