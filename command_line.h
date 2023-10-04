
/*==============================================*/
/* -------- Command_Line definitions ---------- */
/*==============================================*/
enum command_enum { error = -1, 
					mov, cmp, add, sub, not_enum,
					clr, lea, inc, dec, jmp, bne,
					red, prn, jsr, rts, stop,
					data = 20, string, extern_enum, entry };

enum expected_arguments_enum { zero = 0, one, two, data_argument, string_argument};

typedef struct Command_Line 
	{
		char* label;
		char* command_name;
		enum command_enum command_name_enum;
		char* first_argument;
		char* second_argument;
		enum expected_arguments_enum expected_arguments;	
		int error; 
		int comment_or_empty_line; 
		int *data_array; 
		int arguments_in_data_array;
		int current_line;
	
	} Command_Line ;

/* -- Command_Line struct setters --*/
int command_line_set_label(Command_Line*, char*, int);
int command_line_set_command_name(Command_Line*, char* , int );
void command_line_set_first_argument(Command_Line* ,char*);
void command_line_set_second_argument(Command_Line* ,char*);
void command_line_add_number_to_data_array(Command_Line* , char* );

/*gets a command name and returns corresponding command_enum*/
enum command_enum get_command_enum(char*);

/*gets a Command_Line struct with a command name set and sets the command_line.expected_arguments to match*/
void set_expected_arguments(Command_Line*);



