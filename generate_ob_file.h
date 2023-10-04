/***********************
generate_ob_file.h
includes all the required functions for generating an .ob file using the data and instruction binary line arrays
***********************/

/* --- process_instruction_and_data_arrays_to_base_64_ob_file ---
gets a file name for the new .ob file
gets instruction and data arrays representing the binary rows of the program
generates a file_name.ob file with the rows as the binary rows translated to base 64
*/
void generate_ob_file(char* , int [][12],int [][12], int , int);


/* --- process_binary_line_to_base_64_string ---
gets an int array of 12 bits, and a char pointer, 
translates the array to base 64 and inputs the result to the string 
note! the array is representing an upside down binary line, meaning that binary_line[11] is the 0 bit of the array and
binary_line[0] is the 11 bit of the array.
*/
char num_to_base_64_char(int num);

/* num_to_base_64_char 
gets an int number between 0-63) (does not check)
returns a char of the base 64 equivelent
*/
void process_binary_line_to_base_64_string(int[12], char*);


/* add_array_to_ob_file 
gets a FILE pointer to ob_file, and array of 12 bit binary lines and the number of lines in the array
translates the binary lines to base 64 and adds them to the file
*/
void add_array_to_ob_file(FILE*, int[][12], int);


/* add_instruction_and_data_count_to_first_line
gets a pointer to .ob file and the value of instruction and data lines 
adds the values to the first line of the .ob file
*/
void add_instruction_and_data_count_to_first_line(FILE* , int , int );

/*add_num_to_file
gets a FILE pointer to .ob file and an int value
adds the int value to the .ob file
*/
void add_num_to_file(FILE* , int );


