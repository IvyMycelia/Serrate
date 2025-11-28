/*

This is the Entry Point for the Serrate Compiler.
    - Does all the things a normal main.c would do — Initializes the program, gathers arguments, calls everything else, yada yada.
    - Also Stage-0 Error Handling (Otherwise I'd Lose My Mind)

Author: Ivy Mycelia <3
Date: 23:37, 26/11/2025
Language: C99 <—— For compatibility with most systems

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "lexer.h"
#include "ast.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    if (argc < 2) { fprintf(stderr, "Usage: %s <filename>\n", argv[0]); return 1; }

    // Information Flags
    const char *version_flags[2] = {"v", "version"};
    const char *help_flags[2] = {"h", "help"};
    
    // Set argument values
    char *flag1 = argc > 1 ? argv[1] : NULL;
    char *output_file_name = argc > 2 ? argv[2] : "output.c";

    // Check if first argument is an information flag
    if (argc > 1) {
        while (flag1 && *flag1 == '-') flag1++;
        for (int i = 0; i < 2; i++) {
            if (!strcmp(flag1, help_flags[i])) {
                printf(
                    "Help: idk yet lol\n"
                );
                return 0;
            } else if (!strcmp(flag1, version_flags[i])) {
                printf(
                    "Version: 0.0.1\n"
                );
                return 0;
            }
        }
    }



    /*
        Start the process to compile the file
    */

    // Get source file
    
    int file = open(flag1, O_RDONLY);

    if (file == -1) { fprintf(stderr, "Could not open file\n"); return 2; }

    // Get file size
    struct stat size;
    if (fstat(file, &size) == -1) { perror("Could not get file size\n"); close(file); return 2; }
    size_t file_size = size.st_size;

    // Allocate exact memory needed, and get content of source file
    char *source_code = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, file, 0);
    if (source_code == MAP_FAILED) { fprintf(stderr, "Could not allocate memory\n"); close(file); return -1; }

    write(STDOUT_FILENO, source_code, file_size); // Print source file contents
    printf("\n");
    




    // Initialize Lexer & Parser
    Lexer lexer;
    Lexer_init(&lexer, source_code);

    Parser parser;
    parser_init(&parser, &lexer);

    Node* ast = parse_program(&parser);

    print_ast(ast, 0);
    free_ast(ast);



    // Clear allocated memory and close the file

    munmap(source_code, file_size);
    close(file);






    /*
        Write output from compilation to output file
    */

    // Get size of output file's soon-to-be data
    const char *output_data = "int main() { return 0; }\n";
    size_t output_size = strlen(output_data);

    // Create / find output file to write to
    int output_file = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_file == -1) { perror("Could not create output file"); return 2; }

    // Loop through output file until all of data is written
    size_t total = 0;
    while (total < output_size) {
        ssize_t n = write(output_file, output_data + total, output_size - total);
        if (n <= 0) { perror("write"); close(output_file); return 1; }
        total += n;
    }




    // Exit program
    close(output_file);

    return 0;
}