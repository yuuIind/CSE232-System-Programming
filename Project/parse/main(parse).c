#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#define LSIZ 256
#define FIELD_MAX 10
#define FIELD_CHAR_MAX 6

int current_line = 0;
char field[FIELD_MAX][FIELD_CHAR_MAX+1];

void parse(char* filename);
void lineTrimmer(char *line);

int main(int argc, char *argv[]) {
    printf("\tPARSING STARTS:\n");
    for (int i = 1; i < 25; i++){
        current_line = i;
        parse("f1_extended.txt");
        printf("\tLine %d: ", current_line);
        for(int i=0; i < 10; i++){
            if (field[i][0] != '\0'){
                printf("'%s', ", field[i]);
            }
        }
        printf("\n");
    }
    printf("\n\tSuccesfully executed");
    return 0;
}

void parse(char* filename){
	/*
        The program is read line by line. parse() function parses only the 
        current program line and stores its fields to the following data 
        structure:
            char field[10][7];
        
        Examples:
                L1: LDAA #20H -> 'L1:' 'LDAA' '#20H'
                STAA 2000H -> 'STAA' '2000H'
                #M1 -> 'M1'
                #M2 2,3 -> 'M2' '2' '3'
                #if ($2=’ABC’) M3 200, 300 -> '#if' '$2' 'ABC' 'M3' '200' '300'
        
		
		NOTES: 
        	1- **program** is read. So, the macro definitions at the start of 
				file are ignored and parsing is only performed for the lines
				after 'PROG' line for the current line.
        
        	2- The data structure 'field' is globally defined.
        
        	3- The data structure 'field' is gets cleared every time parse is 
				called.
        
        	4- To find current line, parse uses global variable 'current_line'
				is used.
        
        	5- Helper function 'lineTrimmer' is used to clean whitespaces in the 
            	lines.
            

        BUGS OR PROBLEMS:
			1- When parsing conditional macros, if (’) is used instead of ('), 
				parses incorrectly due to encoding
    */
	bool start_parsing = false;
	FILE *f = fopen(filename,"r");
	if (f == NULL) {
        printf("Failed to open the file.\n");
		fclose(f);
		return;
    }

	char line[LSIZ];
    int line_number = 1;
    
	while(fgets(line, LSIZ, f)){
		lineTrimmer(line);
		if(strcmp(line, "PROG") == 0){
            start_parsing = true;
			line_number++;
            continue;  // Skip the 'PROG' line itself
        }

		if(!start_parsing){
			line_number++;
            continue;  // Skip lines before the 'PROG' line
        }

		// skip line if it's not the current line
		if(line_number != current_line){ 
			line_number++;
			continue;
		}
		
		// PARSING OPERATIONS FOR CURRENT LINE STARTS HERE
		memset(field, '\0', sizeof(field)); // clear the previous data
		char* token = strtok(line, " \t\n");  // Tokenize the input string
		int field_index=0;
		// Parse the tokens
		while(token != NULL){
			if(token[0] == '#'){
            	if(strcmp(token, "#if") == 0){

            	    strcpy(field[field_index], token); // '#if' is added to field
            	    field_index++;

					// Move to next token
                	token = strtok(NULL, " \t\n"); // this token is condition
                	
                	// Find the positions of "$" and "="
                    char *dollar_sign = strchr(token, '$');
                    char *equal_sign = strchr(token, '=');

                    // Copy the first operand and assign it to field[1]
					// first operand is between "$" and "=", "=" isn't included
                    strncpy(field[field_index], dollar_sign, equal_sign - dollar_sign);
                    field[field_index][equal_sign - dollar_sign] = '\0';  // Null-terminate the string
                    field_index++;

                	// Find the ending point of second operand
                    char *second_cnd = equal_sign + 2;
                    char *prnth_sign = strchr(token, ')') - 1;

                    // Copy the second operand and assign it to field[2]
					// first operand is between '\'' and ")", ")" isn't included
					// index['\''] = index['='] + 1
                    strncpy(field[field_index], second_cnd, prnth_sign - second_cnd);
                    field[field_index][prnth_sign - second_cnd] = '\0';  // Null-terminate the string
                    field_index++;
            	} 
				else{
				    if(isdigit(token[1])){ // if token is not macro, '#' stays
				        strcpy(field[field_index], token);
            	        field_index++;
				    }
				    else{ // if token is a macro, Remove the '#'
				        strncpy(field[field_index], token+1, strlen(token));
				        field_index++;
				    }
            	}
        	}
			else{
            	// Check if the token contains a comma
                char* commaToken = strchr(token, ',');
                if (commaToken != NULL){
                    // Token contains a comma, split it into multiple tokens
                    char* subToken = strtok(token, ",");
                    while (subToken != NULL) {
                        strcpy(field[field_index], subToken);
                        field_index++;
                        subToken = strtok(NULL, ",");
                    }
                }
                else {
                    strcpy(field[field_index], token);
                    field_index++;
                }
        	}
        	token = strtok(NULL, " \t\n");  // Move to next token
        	if((token != NULL)&&(field_index >= 10)){
        	    printf("Error: Number of fields exceeds the limit");
        	    return;
        	}
		}
		break; // parsing is done for this line, exit loop.
	}
	
	fclose(f);
	return;
}

void lineTrimmer(char *line) {
    // Remove leading whitespace
    int i = 0;
    while (isspace(line[i])){
        i++;
    }

    // Move non-whitespace characters to the beginning of the string
    int j = 0;
    while (line[i]) {
        line[j] = line[i];
        i++;
        j++;
    }
    line[j] = '\0'; // Add null terminator

    // Remove trailing whitespace
    i = strlen(line) - 1;
    while (i >= 0 && isspace(line[i])) {
        line[i] = '\0';
        i--;
    }
}