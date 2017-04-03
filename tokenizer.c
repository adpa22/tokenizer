/*
 * tokenizer.c
 */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

struct TokenizerT_ {
	char * str;
	int length;
	int index;
};

typedef enum{zero, octal, decimal, hexadecimal, floattype, mal, floatEtype, floatSigntype} states;

typedef struct TokenizerT_ TokenizerT;

states curState;
/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 * 
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate( char * ts ) {
	TokenizerT *out = malloc(sizeof (TokenizerT));
    out->index = 0; //the set the index to 0
    out->length = strlen(ts);
    out->str = malloc(sizeof(char) * (strlen(ts)+1) ); //add + 1 for the nulbyte
    strcpy(out->str, ts); //copies the string
    if(out != NULL)
        return out;
    else
        return NULL;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy( TokenizerT * tk ) {
	free(tk->str); //free the memory and also set it to NULL
	//tk-> str = NULL;
    free(tk);
    //tk = NULL;
}

//helps getting the next part
char * helper(char * inBetwen, int start, TokenizerT *t){
     char* out = malloc(sizeof(char)*(start+1));
     memcpy(out, inBetwen, sizeof(char)*start);
     out[start] = 0;
     t->index += start; //changes the index to start at the next token
     free(inBetwen);
     return out;
}  

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char *TKGetNextToken( TokenizerT * tk ) {
	//to get start of token
	while (isspace(tk->str[tk->index]) || !isdigit(tk->str[tk->index])){ //skip spaces, non-digits are invalid
		if (tk->str[tk->index] == 0){ //if its null
			return 0;
		} 
		if (isspace(tk->str[tk->index])){ //skip all spaces
			tk->index++;
		}
		if(!isdigit(tk->str[tk->index])){ //skip all invalid tokens
			printf("invalid [0x%x]\n", tk->str[tk->index]);
			tk->index++;
		}
	}

	int endIndex = tk -> index; //will get the end index of the next white space
	int counter = 1;
	while( !isspace(tk->str[endIndex]) ){ //gets the next blank space
        if(tk->str[endIndex] == '\0')  //null checker
            break;
        endIndex++;
        counter++;
    }
    char* between = malloc(sizeof(char)*(counter) ); //the string in between spaces (possible token)
    memcpy(between, &tk->str[tk->index], sizeof(char) *counter ); //copies the string USE STRCPY????

    between[counter-1] = 0; //manually set the null byte. was getting error before
    if(between[0] == '0'){ //get the initial state
        curState = zero;
    }
    else{
        curState = decimal;
    } //ADD ANOTHER IF/ELSE FOR INVALID???
    int i;
    for (i = 1; i < counter; i++){ //start at 1 becasue of the whitespace
    	switch(curState){
    		case zero:
    			if (isdigit(between[i]) && between[i] < '8'){
    				curState = octal;
    				break;
    			}
    			if(between[i] == 'x' || between [i] == 'X'){ //X or x for hex
                    curState = hexadecimal;
                    break;
                }
                if (between[i] == '.'){ //checks for .
                    curState = floattype;
                    break;
                }
                if(between[i]== '8' || between[i]== '9'){ //Ex: 09
                    curState = zero;
                    return helper(between, i, tk);
                }

            case octal:
            	if(between[i] >= '0' && between [i]<'8' && isdigit(between[i])){ //checks for octal conditions
            		curState = octal; //repeated in the FSM
                    break;
                }
                return helper(between, i, tk);

            case decimal:
            	if (between[i] == '.'){ //general floattype
                    curState = floattype;
                    break;
                }
                if (isdigit(between[i]) ){ //already this state
                	curState = decimal; //repetive??
                    break;
                }
                return helper(between, i, tk);

            case hexadecimal: //hex is 0-7 or a-f or A-F
            	if (isdigit(between[i]) || (between[i] >= 'a' && between[i] <= 'f') || (between[i] >= 'A' && between[i] <= 'F')){
            		curState = hexadecimal;
            		break;
            	}
            	if(between[i-1] == 'x' || between[i-1] == 'X'){
                    curState = mal;
                }
                else
                    return helper(between, i, tk);

            case floattype:
            	if (isdigit(between[i]) ){
            		curState = floattype;
                    break; 
                } 
                if(between[i-1] == '.'){ //will change later
                    curState = mal;
                }
                else if(between[i] == 'e' || between[i] == 'E'){ //for that type
                    curState = floatEtype;
                    break;
                }
                else
                    return helper(between, i, tk);

            case floatEtype: //since there are multiple ones that go to float
                if(between[i] == '+' || between[i] == '-'){
                	curState = floatSigntype; //this is for when the + or - is there after E or e
                	break;
                }
                if(between[i-1] == 'E' || between [i-1] == 'e') {//cant have 2 e'es in a row
                    curState = mal;
            	}
                else
                    return helper(between, i, tk);

            case floatSigntype:
            	if (isdigit(between[i])){ //digit loop in the FSM
            		curState = floatSigntype;
            		break;
            	}
            	if(between[i-1] == '+' || between [i-1] == '-'){ //can't have to + or - in a row
                    curState = mal;
                }
                else{
                    return helper(between, i, tk);
                }

            case mal: //tries to get next valid char
                return helper(between, i, tk);
            
    	}
    	if(i+1==counter) //if it's about to reach the end, call it agian so it doesn't malfunction
            return helper(between, i, tk);
    }


  return NULL;
}

char * printer[] = {"zero ", "octal ", "decimal ", "hex ", "float ", "mal ", "integer "}; //helps me print at the end

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */

int main(int argc, char **argv) {
	char * input = argv[1];
	TokenizerT * tok = TKCreate(input);
	char *next = TKGetNextToken(tok);
	while (next != 0){ //while it's not empty, run
		int length = strlen(next);
		if (curState == 6 || curState == 7){ //for the other float cases
			curState = floattype; //this narrows it down
		}
		if(curState == mal){
            printf("mal %c\n" , next[length-1]);
        }
        else{
            printf("%s%s\n", printer[curState], next);
        }
        free (next);
        next = TKGetNextToken(tok);
	}
	TKDestroy(tok);


  return 0;
}