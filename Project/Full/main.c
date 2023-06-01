#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

struct mac {
char mname[8]; 				//macro name
char param[10][4]; 			//max. 10 parameters, each parameter max. 3 characters
char macro[256]; 			//macro body
};
struct mac buffer[10]; 		// memory buffer for 10 macro definitions
int m_count = 0; 			// counts the number of macro definitions

#define LSIZ 256 
#define FIELD_MAX 10
#define FIELD_CHAR_MAX 6
#define MAX_MACRO_DEF_SIZE 256
#define MAX_PARAM_SIZE 4
#define MAX_PARAMS 15

int current_line = 0;
char field[FIELD_MAX][FIELD_CHAR_MAX+1];
char* inputfile;

int read(char* filename);
void parse(char* filename);
void lineTrimmer(char *line);
void createPT();
void is_macro();
void expand();

struct pt {
char mname[8];
    int nparams;
    char dummy[10][MAX_PARAM_SIZE];
    char actual[10][MAX_PARAM_SIZE];
} ;
struct pt PT;

int g_argc;
char **g_argv;


int main(int argc, char *argv[]) {
	
	g_argc = argc;
    g_argv = argv;
	
	inputfile = argv[1];
	// inputfile = "f1.txt";
    int m_count = read(inputfile);
    printf("\tPARSING STARTS:\n");
    // for i si gideceği miktarın kontronl edilmesi gerek
    int i = 1;
    for (i = 1; i < 30000; i++){
        current_line = i;
        parse(inputfile);
        
        
        if ( field[0][0] != '\0' )
        {
        	 
        	is_macro();
        	
        	if ( strcmp ( field[0],"END" ) == 0 ) return 0;
        	
		}
        
        /*
        printf("\tLine %d: ", current_line);
        for(int i=0; i < 10; i++){
            if (field[i][0] != '\0'){
                printf("'%s', ", field[i]);
            }
        }
        printf("\n");
        */
    }
    printf("\n\tSuccesfully executed");
    return 0;
}

int read(char* filename){
	FILE *f = fopen(filename,"r");
	char line[LSIZ];

	if ( f != NULL){	//read file
		int j = 0;
		while(fgets(line, LSIZ, f) != NULL ){									// Try to find a macro
			char* token = strtok(line, " \t\n");  								// Tokenize the input string
			if ( (token[0] == 'P') && (token[1] == 'R') && (token[2] == 'O') && (token[3] == 'G') ){
							token = NULL;
							break;
				}
			if( strcmp(token, "PROG") == 0 ){
					break;
			}
			while(token != NULL){
				printf("First token: %s", token);
				// Checks if the program has started
				
				if(token[0] == '#' ){			//if not macro skip								
					if ( (token[0] == '#') && (token[1] == 'E') && (token[2] == 'N') && (token[3] == 'D') && (token[4] == 'M') ){
							token = NULL;
							break;
					}
					//printf("--- MACRO NAME ---\n");
					while ( token[j+1] != ':' ){
						printf("%c",token[j+1]);
						buffer[m_count].mname[j] = token[j+1];
						j++;
					}

					token = strtok(NULL, " \t\n"); 
					//printf("\n--- MACRO IDENT ---\n");
					
					if( strcmp(token,"MACRO") != 0){
						//printf("Macro, it is not.");
						break;
					}
					//printf("Macro, it is.\n");
					/*
						Check if the Macro has Parameters
					*/
					printf("\n--- MACRO PARAMETERS ---\n");
					token = strtok(NULL, " \t\n"); 
					char* commaToken = strchr(token, ',');
					j=0;
					memset(buffer[m_count].param, '\0', sizeof(buffer[m_count].param));
					if ( commaToken != NULL ){							// if it has, continue with the loop
						char* subToken = strtok(token, ",");			
						printf("%s\n",subToken);
						while (subToken != NULL) {
							strcpy(buffer[m_count].param[j], subToken);
							printf("Parameter %d: %s\n",j,buffer[m_count].param[j]);
							j++;
							subToken = strtok(NULL, ",");
						}

					}

					/*
						Add the lines into body with \t between each till #ENDM is reached.
					*/
					memset(buffer[m_count].macro, '\0', sizeof(buffer[m_count].macro)); // clear the previous data
					while( fgets(line, LSIZ, f) != NULL ){
						lineTrimmer(line);
						// if we reached end of the macro, break
						if ( (line[0] == '#') && (line[1] == 'E') && (line[2] == 'N') && (line[3] == 'D') && (line[4] == 'M') ){
							break;			
						}
						strcat(buffer[m_count].macro,line);
						//printf("%s",buffer[m_count].macro);		
						strcat(buffer[m_count].macro,"\n");		
						
					}
					m_count++;	
					j=0;
					break;
				}
				else{// if it is not a token, break
					break;
				}

			}
			
		}
		//printf("\n");
		j = 0;
		
		/*
		while( j < m_count ){
			int i = 0;
			printf("----------------------------------------------------------\n");
			printf("|  Macro name:                                           |\n");
			printf("----------------------------------------------------------\n");
			printf("%s\n",buffer[j].mname);
			printf("----------------------------------------------------------\n");
			printf("|  Macro parameters:                                     |\n");
			printf("----------------------------------------------------------\n");
			printf("%s",buffer[j].param[0]);
			while( buffer[j].param[i][0] != '\0' ){
				printf("%s\n",buffer[j].param[i]);
				i++;
			}
			printf("----------------------------------------------------------\n");
			printf("|  Macro body:                                           |\n");
			printf("----------------------------------------------------------\n");
			printf("%s\n\n",buffer[j].macro);
			j++;
			
		}
		*/

		fclose(f);
	}
	else
	{
		printf("unable to open file");
	}
		
	
	return m_count;
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
                #if ($2=â€™ABCâ€™) M3 200, 300 -> '#if' '$2' 'ABC' 'M3' '200' '300'
        
		
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
			1- When parsing conditional macros, if (â€™) is used instead of ('), 
				sometimes parses incorrectly
    */
	bool start_parsing = false;
	FILE *f = fopen(filename,"r");
	/*
	if (f == NULL) {
        printf("Failed to open the file.\n");
		
		return;
    }
    */
    

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
                    char *prnth_sign = strchr(token, ')');

                    // Copy the second operand and assign it to field[2]
					// first operand is between '\'' and ")", ")" isn't included
					// index['\''] = index['='] + 1
                    strncpy(field[field_index], equal_sign+1, prnth_sign - (equal_sign+1));
                    field[field_index][prnth_sign - (equal_sign+1)] = '\0';  // Null-terminate the string
                    field_index++;
            	} 
				else{
				    if(isdigit(token[1])){ // if token is not macro, '#' stays
				        strcpy(field[field_index], token + '\0');
            	        field_index++;
				    }
				    else{ // if token is a macro, Remove the '#'
				        strncpy(field[field_index], token+1, strlen(token));
				        field[field_index][strlen(field[field_index])] = '\0';
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
                        strcpy(field[field_index], subToken + '\0');
                        field_index++;
                        subToken = strtok(NULL, ",");
                    }
                }
                else {
                    strcpy(field[field_index], token + '\0');
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

void createPT() {
   
	// Find the current macro call
   int i = 0;
   //printf("1");
    while (i < m_count && buffer[i].mname != "\0" ) {
       //printf("2");
	    if ( strcmp(field[0], buffer[i].mname) == 0 ) {
            // Macro call found, fill the parameter table (PT)
            
            strcpy(PT.mname, buffer[i].mname);
            PT.nparams = 0; // Initialize the number of parameters to 0

            // Check if the macro has parameters
			//printf("3");
            if (buffer[i].param[0][0] != '\0') {
                int j = 0;
				//printf("4");
                while (buffer[i].param[j][0] != '\0' && j < 10) {
                    strcpy(PT.dummy[j], buffer[i].param[j]);
                    PT.nparams++;
                    j++;
					//printf("5");
                }
            }
			//printf("6");

            // Fill the actual parameters from the parsed fields
            int j = 1;
            while (field[j][0] != '\0' && j < 10) {
                strcpy(PT.actual[j-1], field[j]);
                j++;
				//printf("7");
            }

            break;
        }
        i++;
    } 
	

    // Print the parameter table (PT)
	
    printf("\n--- Parameter Table (PT) ---\n");
    printf("Macro Name: %s\n", PT.mname);
    printf("Number of Parameters: %d\n", PT.nparams);
    printf("Dummy Parameters: ");
    int j = 0;
    for (j = 0; j < PT.nparams; j++) {
        printf("%s ", PT.dummy[j]);
    }
    printf("\nActual Parameters: ");
    for (j = 0; j < PT.nparams; j++) 
	{
		int a = 0;
    	while ( PT.actual[j][a] != '\0' )
    	{
    		printf("%c", PT.actual[j][a]);
    		a++;
    		if ( a >= 4 )
    		{
    			a = 0;
    			break;
			}
		}
		printf(" ");
        // printf("%s\n", PT.actual[j]);
    }
    printf("\n");
}




void is_macro ()
{
	
	FILE *file;
	char filename[256]; // bir dosya ismi iÃ§in yeterince bÃ¼yÃ¼k olmalÄ±
		
	snprintf ( filename, sizeof(filename), "%s.asm", inputfile );
	long size = 0;
	file = fopen ( filename, "r" );
	if ( file != NULL )
	{
		fseek ( file, 0, SEEK_END );  // dosyanÄ±n sonuna git
		size = ftell ( file );   // dosyanÄ±n boyutunu al
		fclose(file);
	}
	
		
	if ( size == 0 ) 
	{
		file = fopen ( filename, "w" );
		fputs ( "PROG\n", file );
		fclose(file);
	}
	
	
	bool macflag = 0;
	bool ifflag = 0;
	int ii = 0;
	for ( ii = 0; ii < 10; ii++ )
	{
	   if ( strcmp ( field[0] , buffer[ii].mname ) == 0 )
	   {
			macflag = 1;
			break;
	   }
	   else if ( strcmp (field[0] , "#if") == 0 )
	   {
		   ifflag = 1;
		   break;
	   }
	}
	
	if ( macflag == 1 )
	{
		expand();
	}
	else if ( ifflag == 1 )
	{
		char tempchar[FIELD_MAX][FIELD_CHAR_MAX+1];
		int i = 0;
		for(i = 0; i < FIELD_MAX; i++) 
		{
        	strcpy(tempchar[i], field[i]);
    	}	

		tempchar[2][0] = ' ';
		tempchar[2][strlen(tempchar[2])-1] = ' ';
		char tempchar2[256];
		memset(tempchar2,'\0',256);
		strncat(tempchar2," \0",2);
		strcat(tempchar2,g_argv[field[1][1] - '0']);
		strncat(tempchar2," \0",2);
		
		// if ( strcmp ( tempchar2[field[1][1] - '0'], tempchar[2] ) == 0 )
		if ( strcmp ( tempchar2, tempchar[2] ) == 0 )
		{
			// Ã§Ä±karÄ±lan eleman sayÄ±sÄ±
			int remove_count = 3;

			// Her elemanÄ± baÅŸa Ã¶teleme
			int i = 0;
			for ( i = 0; i < FIELD_MAX - remove_count; i++ ) 
			{
				strcpy ( field[i], field[i + remove_count] );
				field[i + remove_count][0] = '\0';
			}
			
			expand();
		}
	}
	else
	{
		// macro bulamadÄ±m oyuzden aldÄ±ÄŸÄ±m satÄ±rÄ± aynÄ± ÅŸekilde yazÄ±yorum
		file = fopen ( filename, "a+" ); // dosya adÄ±nÄ± deÄŸiÅŸtirmek gerekebilir

		if (file == NULL) 
		{
			// DosyayÄ± aÃ§amÄ±yorum dosya aÃ§Ä±lamadÄ±ÄŸÄ±nda Ã§alÄ±ÅŸÄ±r
		}
		int i = 0;
		for ( i = 0; i < FIELD_MAX; i++ )
		{
			if ( field[i] == "\0" ) break;
			// fputs ( field[i] + " ", file );
			fputs ( field[i], file );
			fputs ( " ", file );
		}
		
		fputs ( "\n", file );
		fclose ( file );
	}
}


void expand() {
    createPT();
    
    FILE *file;
    char filename[256]; // bir dosya ismi iÃ§in yeterince bÃ¼yÃ¼k olmalÄ±
	snprintf ( filename, sizeof(filename), "%s.asm", inputfile );
	file = fopen ( filename, "a+" );
    
   	
	int start = -1, end = -1;
	
	   
	int iii = 0;
    while ( iii < 10 )
	{
		if ( strcmp ( field[0],buffer[iii].mname ) == 0 )
		{
	        char expandedLine[100];
	        strcpy(expandedLine, buffer[iii].macro);

	        char newStr[1000] = ""; // Yeni string için yeterli boyutta bir array olusturduk
		    int len = strlen(expandedLine);
		    int i, j, start = -1, end = -1, startold = -1;
		
		    for(i = 0; i <= len; i++) 
			{
				
				if ( start != -1 && ( expandedLine[i] == ' ' || expandedLine[i] == '\t' ) )
				{
					startold = start;
					start = i;
					for ( j = startold; j < start; j++ )
					{
						newStr[strlen(newStr)] = expandedLine[j];
						newStr[strlen(newStr)+1] = '\0';
						
					}
					// newStr[strlen(newStr)+1] = '\n';
				}
				
			
				
		        if( ( expandedLine[i] == ' ' || expandedLine[i] == '\t' ) && start == -1) 
				{
		            start = i;
		        } 
		        else if((expandedLine[i] == '\0' || expandedLine[i] == '\n' ) && start != -1) 
				{
		            end = i;
		            // Anahtar kelimeleri kontrol et
		            for(j = 0; j < PT.nparams; j++) 
					{
		                if(strncmp(expandedLine + start + 1, PT.dummy[j], end - start - 1) == 0) 
						{
							strncat(newStr, " \0", 2);
		                    strncat(newStr, PT.actual[j], 4); 
		                    strncat(newStr, " \n", 3);
		                    break;
		                }
		            }
					
		            if(j == PT.nparams) { // Eger eslesme yoksa, orijinal string'i ekle
		                strncat(newStr, expandedLine + start, end - start);
		                strncat(newStr, " \n", 3);
		            }
		
		            start = -1;  // start ve end degerlerini resetle
		            end = -1;
		        }
		        else if ( start == -1 )
		        {
		        	newStr[strlen(newStr)] = expandedLine[i];
					newStr[strlen(newStr)+1] = '\0';
				}
		    }
		
		    
	        
	        int k = 0;
	        while ( k < strlen(newStr) )
	        {
	        	if ( newStr[k] == '\n' )
	        	{
	        		fputs ( "\n", file );
				}
				else
				{
					fputc ( newStr[k], file );
				}
	        	
				k++;	
			}
	    }
	    
	    iii++;
    }
    
    fclose ( file );
}


/*void createPT() {
    // Initialize the PT structure
    strcpy(PT.mname, ""); // Macro name
    PT.nparams = 0; // Number of parameters
    memset(PT.dummy, '\0', sizeof(PT.dummy)); // Clear the dummy parameters
    memset(PT.actual, '\0', sizeof(PT.actual)); // Clear the actual parameters

    // Get the current line from the parsed fields
    char* currentLine = field[0];

    // Check if the current line is a macro call
    if (currentLine[0] == '#') {
        // Find the macro name and parameters
        char* macroName = currentLine + 1; // Exclude the '#' character
        char* commaToken = strchr(macroName, ','); // Check if the macro call has parameters

        // Extract the macro name
        if (commaToken != NULL) {
            // Macro call with parameters
            size_t macroNameLength = commaToken - macroName;
            strncpy(PT.mname, macroName, macroNameLength);
            PT.mname[macroNameLength] = '\0'; // Null-terminate the macro name
        } else {
            // Macro call without parameters
            strcpy(PT.mname, macroName);
        }

        // Extract the parameters
        PT.nparams = 0; // Reset the number of parameters

        if (commaToken != NULL) {
            // Macro call with parameters
            char* parameter = strtok(commaToken + 1, ","); // Start parsing from the first parameter
            while (parameter != NULL && PT.nparams < 10) {
                strncpy(PT.dummy[PT.nparams], parameter, MAX_PARAM_SIZE - 1);
                PT.dummy[PT.nparams][MAX_PARAM_SIZE - 1] = '\0'; // Null-terminate the parameter
                PT.nparams++;
                parameter = strtok(NULL, ",");
            }
        }
    }
	 // Print the populated PT structure
    printf("\tPT:\n");
    printf("\tMacro Name: %s\n", PT.mname);
    printf("\tNumber of Parameters: %d\n", PT.nparams);
    printf("\tDummy Parameters:\n");
    for (int i = 0; i < PT.nparams; i++) {
        printf("\t\tParameter %d: %s\n", i + 1, PT.dummy[i]);
    }
}
*/



