asssembler:  main.o command_line.o run_1_am_file_line_to_command_processing.o input_buffer_processing.o command_to_binary.o macros.o generate_ob_file.o
	gcc -g  -ansi -Wall -pedantic main.o macros.o command_to_binary.o command_line.o run_1_am_file_line_to_command_processing.o input_buffer_processing.o  generate_ob_file.o -o assembler -lm

main.o: main.c run_1_am_file_line_to_command_processing.h input_buffer_processing.h command_to_binary.h macros.h command_line.h
	gcc -g  -ansi -Wall -pedantic -c main.c -o main.o

command_line.o: command_line.c run_1_am_file_line_to_command_processing.h input_buffer_processing.h command_line.h
	gcc -g  -ansi -Wall -pedantic -c command_line.c -o command_line.o

run_1_am_file_line_to_command_processing.o: run_1_am_file_line_to_command_processing.c run_1_am_file_line_to_command_processing.h input_buffer_processing.h command_line.h
	gcc -g  -ansi -Wall -pedantic -c run_1_am_file_line_to_command_processing.c -o run_1_am_file_line_to_command_processing.o

input_buffer_processing.o: input_buffer_processing.c input_buffer_processing.h run_1_am_file_line_to_command_processing.h command_line.h
	gcc -g  -ansi -Wall -pedantic -c input_buffer_processing.c -o input_buffer_processing.o

command_to_binary.o: command_to_binary.c command_to_binary.h run_1_am_file_line_to_command_processing.h command_line.h
	gcc -g -ansi -Wall -pedantic -c command_to_binary.c -o command_to_binary.o

macros.o: macro_process.c macros.h 
	gcc -g -ansi -Wall -pedantic -c macro_process.c -o macros.o

generate_ob_file.o: generate_ob_file.c generate_ob_file.h command_line.h run_1_am_file_line_to_command_processing.h
	gcc -g -ansi -Wall -pedantic -c generate_ob_file.c -o generate_ob_file.o

