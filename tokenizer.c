#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Get the properties of the token after parsing
typedef struct _tokenProperties {
    int endpos;
    char* tokenName;
} tokenProperties;

//create nodes for hash table
typedef struct _node{
   char* data;
   struct _node* next;
} node;

tokenProperties parse_quote(char* tokenString, int x);
tokenProperties parse_operator(char* tokenString, int x);
tokenProperties parse_word(char* tokenString, int x, node** hashTable);
tokenProperties parse_digit(char* tokenString, int x);
void print_token(char* tokenName, char* start, char* end);
int getBucket(char string[], int len);
void insertHash(node** hashTable, char string[]);
void makeHash(node** hashTable);
int searchHash(node** hashTable, char string[], int len);
void freeHash(node** hashTable);

//Get command-line input and iterate through the string
//It is assumed that argc = 1 or argc = 2
//Checks the following errors: no input (print No arguments and exit), an unmatched quote (print invalid token), and an endMultiline comment without a startMultiline (print invalid token)
int main(int argc, char ** argv){
    //Check if an input string is given
    if (argc == 1){
        printf("No arguments\n");
        exit(0);
    }
    tokenProperties tp;
    int commenting = 0;
    int y = 0;
    node* hashTable[35];
    for (int i = 0; i < 35; i++)
    {
        hashTable[i] = NULL;
    }
    makeHash(hashTable);
    while (argv[1][y] != '\0')
    {
        if(!commenting && isspace(argv[1][y]))
        {
            y+=1;
            continue;
        }
        else if(!commenting && isdigit(argv[1][y]))
        {
            tp = parse_digit(argv[1], y);
        }
        else if (!commenting && isalpha(argv[1][y]))
        {
            tp = parse_word(argv[1], y, hashTable);
        }
        else if (!commenting && (argv[1][y] == '\"' || argv[1][y] == '\''))
        {
            tp = parse_quote(argv[1], y);
        }
        else
        {
            tp = parse_operator(argv[1], y);
            //if a single line comment is found, exit
            if (strcmp(tp.tokenName, "comment") == 0)
            {
                exit(0);
            }
            //if the start of a multiline comment is found, start commenting
            else if(strcmp(tp.tokenName, "startMultiline") == 0)
            {
                commenting = 1;
                y+=1;
            }
            //if the end of a multiline comment is found, end commenting
            else if (strcmp(tp.tokenName, "endMultiline") == 0)
            {
                //if an end of a multiline comment is found without the start, go to next character
                if(commenting != 1) {
                    tp.tokenName = "Invalid Token";
                }
                commenting = 0;
            }
        }
        //print token
        int tokenLength = tp.endpos-y;
        if (!commenting && tokenLength > 0)
        {
            if (strcmp(tp.tokenName, "endMultiline") != 0)
            {
                print_token(tp.tokenName,&argv[1][y], &argv[1][tp.endpos]);
            }
	        y = tp.endpos;
        }
        //check for unmatched quote
        else if (tp.endpos == -1)
        {
            printf("Invalid token: %s\n", &argv[1][y]);
            exit(0);
        }
        else
        {
            y+=1;
        }
    }
    freeHash(hashTable);
	return 0;
}
//Used to find closing quote
//It is assumed that tokenString[x] is either " or ' (which is guaranteed from main) and that tokenString either contains a closing quote or null terminator
//The argument tokenString is used to pass the entire input string and the argument x is the index where the token starts
//This function returns a struct that describes the end of the token and its name, if the end position == -1, then the quote is not matched
tokenProperties parse_quote(char* tokenString, int x){
    char quoteType = tokenString[x];
    x+=1;
    tokenProperties ret;
    while (tokenString[x]!= '\0')
    {
        if (tokenString[x] == quoteType)
        {
            ret.endpos = x+1;
            ret.tokenName = "quote";
            return ret;
        }
        x+=1;
    }
    ret.endpos = -1;
    ret.tokenName = "Invalid Token";
    return ret;
}
//Determine the operator in the tokenString
//It is assumed that tokenString[x] exists
//The argument tokenString is used to pass the entire input string and the argument x is the index where the token starts
//This function returns a struct that describes the end of the token and its name
tokenProperties parse_operator(char* tokenString, int x) {
    tokenProperties ret = {x, ""};
    switch(tokenString[x]) {
        case '(':
            ret.endpos = x+1;
            ret.tokenName = "left parenthesis";
            return ret;
        case ')':
            ret.endpos = x+1;
            ret.tokenName = "right parenthesis";
            return ret;
        case '[':
            ret.endpos = x+1;
            ret.tokenName = "left bracket";
            return ret;
        case ']':
            ret.endpos = x+1;
            ret.tokenName = "right bracket";
            return ret;
        case ',':
            ret.endpos = x+1;
            ret.tokenName = "comma";
            return ret;
        case '~':
            ret.endpos = x+1;
            ret.tokenName = "1s complement";
            return ret;
        case '?':
            ret.endpos = x+1;
            ret.tokenName = "conditional true";
            return ret;
        case ':':
            ret.endpos = x+1;
            ret.tokenName = "conditional false";
            return ret;
        case '.':
            ret.endpos = x+1;
            ret.tokenName = "structure member";
            return ret;
        case '-':
            if (tokenString[x+1] == '>')
            {
                ret.endpos = x+2;
                ret.tokenName = "structure pointer";
                return ret;
            }
            else if (tokenString[x+1] == '-')
            {
                ret.endpos = x+2;
                ret.tokenName = "decrement";
                return ret;
            }
            else if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "minus equals";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "minus/subtract operator";
                return ret;
            }
        case '+':
            if (tokenString[x+1] == '+')
            {
                ret.endpos = x+2;
                ret.tokenName = "increment";
                return ret;
            }
            else if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "plus equals";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "addition";
                return ret;
            }
        case '>':
            if (tokenString[x+1] == '>' && tokenString[x+2] == '=')
            {
                ret.endpos = x+3;
                ret.tokenName = "shift right equals";
                return ret;
            }
            else if (tokenString[x+1] == '>')
            {
                ret.endpos = x+2;
                ret.tokenName = "shift right";
                return ret;
            }
            else if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "greater than or equal test";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "greater than test";
                return ret;
            }
        case '<':
            if (tokenString[x+1] == '<' && tokenString[x+2] == '=')
            {
                ret.endpos = x+3;
                ret.tokenName = "shift left equals";
                return ret;
            }
            else if (tokenString[x+1] == '<')
            {
                ret.endpos = x+2;
                ret.tokenName = "shift left";
                return ret;
            }
            else if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "less than or equal test";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "less than test";
                return ret;
            }
        case '!':
            if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "inequality test";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "negate";
                return ret;
            }
        case '^':
            if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "bitwise XOR equals";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "bitwise XOR";
                return ret;
            }
        case '|':
            if (tokenString[x+1] == '|')
            {
                ret.endpos = x+2;
                ret.tokenName = "logical OR";
                return ret;
            }
            else if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "bitwise OR equals";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "bitwise OR";
                return ret;
            }
        case '/':
            if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "divide equals";
                return ret;
            }
            else if (tokenString[x+1] == '/')
            {
                ret.endpos = x+2;
                ret.tokenName = "comment";
                return ret;
            }
            else if (tokenString[x+1] == '*')
            {
                ret.endpos = x+2;
                ret.tokenName = "startMultiline";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "division";
                return ret;
            }
        case '*':
            if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "times equals";
                return ret;
            }
            else if (tokenString[x+1] == '/')
            {
                ret.endpos = x+2;
                ret.tokenName = "endMultiline";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "multiply/dereference operator";
                return ret;
            }
        case '=':
            if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "equality test";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "assignment";
                return ret;
            }
        case '&':
            if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "bitwise AND equals";
                return ret;
            }
            else if (tokenString[x+1] == '&')
            {
                ret.endpos = x+2;
                ret.tokenName = "logical AND";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "AND/address operator";
                return ret;
            }
        case '%':
            if (tokenString[x+1] == '=')
            {
                ret.endpos = x+2;
                ret.tokenName = "mod equals";
                return ret;
            }
            else
            {
                ret.endpos = x+1;
                ret.tokenName = "mod";
                return ret;
            }
        default:
            ret.endpos = x+1;
            ret.tokenName = "Invalid Token";
            return ret;
    }
}
//Determine the word in the tokenString
//It is assumed that tokenString[x] is a letter (guaranteed by main) and that tokenString either contains a char that is not a letter or a null terminator
//The argument tokenString is used to pass the entire input string and the argument x is the index where the token starts
//This function returns a struct that describes the end of the token and its name
tokenProperties parse_word(char* tokenString, int x, node** hashTable){
    int start = x;
    //loops through the tokenString until the current char cannot be part of a word
    while (isalnum(tokenString[x])){
        ++x;
    }
    tokenProperties ret = {x, "word"};
    //if the token is sizeof, the token type is sizeof
    if (strncmp("sizeof",&tokenString[start],x-start) == 0){ 
        ret.tokenName = "sizeof";
    }
    //if the token is a keyword then we return keyword as its type
    else if (searchHash(hashTable, &tokenString[start], x-start)){
        ret.tokenName = "keyword";
    }
    return ret;
}
//Determine the type of number in the tokenString
//It is assumed that tokenString[x] is a digit (guaranteed by main) and that tokenString either contains a char that is not numeric or a null terminator
//The argument tokenString is used to pass the entire input string and the argument x is the index where the token starts
//This function returns a struct that describes the end of the token and its name
tokenProperties parse_digit(char* tokenString, int x){
    int start = x;
    int possibleFloat = 0;
    int periodIndex = -1;
    int possibleOctal = 1;
    int possibleHex =1;
    int possibleDecimal = 1;
    int eIndex=-2;

    while (1){
        //If the first char is not 0, eliminate hex and octal
        if (x == start && tokenString[x] != '0'){
            possibleHex = 0;
            possibleOctal = 0;
        //If the second char is not X or x, or if the string is just "0x/X" eliminate hex
        }else if(x == start + 1 && (toupper(tokenString[x]) != 'X' || !isxdigit(tokenString[x+1]))){
            possibleHex = 0;
        }
        //If the char breaks the token, leave the while loop to end token
        if (!(isdigit(tokenString[x]) || (isalpha(tokenString[x]) && possibleHex) || (possibleFloat && toupper(tokenString[x])=='E') || tokenString[x] == '.' || (x == eIndex + 1 && (tokenString[x] == '+' || tokenString[x] == '-')))){
            break;
        }
        if(tokenString[x] == '.'){
            //If it is a possible hex or there was already a period, the token will end
            if (possibleHex || possibleFloat || !isdigit(tokenString[x+1]))
                break;
            periodIndex = x;
            possibleFloat = 1;
        }
        //if any char > 7, eliminate octal
        else if (isdigit(tokenString[x]) && tokenString[x] > '7'){
            possibleOctal = 0;
        }
        //If 2nd character is X, eliminate possibility of octal, decimal, and float(already initialized as 0)
        else if(isalpha(tokenString[x])){
            possibleOctal = 0;
            possibleDecimal = 0;
        }
        //out of hex range
        if (possibleHex && !isxdigit(tokenString[x]) && x != start+1){
            break;
        }
	    //E must be followed by a digit or a +/-
        if(toupper(tokenString[x]) == 'E' && !isdigit(tokenString[x+1]) && tokenString[x+1] != '+' && tokenString[x+1] != '-'){
            break;
        }
        //if an 'E' is encountered for the first time
        else if (possibleFloat && toupper(tokenString[x]) == 'E' && eIndex == -2){
            eIndex = x;
        }
        //If an e occurs more than once
        else if (eIndex != -2 && toupper(tokenString[x]) == 'E'){
            break;
        }
        //If characters after ‘+’ or ‘-’ are not valid
        else if((tokenString[x] == '+' || tokenString[x] == '-') && !isdigit(tokenString[x+1])){
            x = eIndex;
            break;
        }
        ++x;
    }
    tokenProperties ret = {x, ""};
    if (possibleFloat){
        ret.tokenName = "float";
    }else if (possibleHex){
        ret.tokenName = "hexadecimal integer";
    }else if (possibleOctal){
        ret.tokenName = "octal integer";
    }else{
        //possibleDecimal
        ret.tokenName = "decimal integer";
    }
    return ret;
}

//Prints token
//It is assumed that end > start are not NULL (guaranteed by main) and tokenName is not NULL and \0 terminated
//The argument tokenName represents the type of token and the arguments start and end represent the start and end + 1 indicies of the token in the input string
void print_token(char* tokenName, char* start, char* end){
    printf("%s: \"", tokenName);
    //prints each character [start,end)
    for(char * ptr = start; ptr != end; ++ptr){
        printf("%c", *ptr);
    }
    printf("\"\n");
}
//Calculate the bucket in the hash table that corresponds to the string
//It is assumed that string != NULL and len >= 0
//Returns the bucket for string in the hash table
int getBucket(char string[], int len) {
	int sum = 0;
	for(int i = 0; i < len; i++) {
		sum += string[i];
    }
	return sum % 35;
}
//Insert the string into the hash table
//It is assumed that hashTable != NULL (guaranteed by main) and string != NULL (guaranteed by makeHash)
//If there is a memory allocation error, the program exits
void insertHash(node** hashTable, char string[]) {
	int bucket = getBucket(string, strlen(string));
	node* newNode = malloc(sizeof(node));
    if (newNode == NULL){
        printf("Memory allocation error!");
        exit(0);
    }
	newNode->data = string;
	if(hashTable[bucket] == NULL) {
		newNode->next = NULL;
		hashTable[bucket] = newNode;
    }
	else {
		newNode->next = hashTable[bucket];
		hashTable[bucket] = newNode;
    }
}
//Fills hash with C keywords
//It is assumed that hashTable != NULL (guaranteed by main)
void makeHash(node** hashTable) {
    insertHash(hashTable, "auto");
    insertHash(hashTable, "break");
    insertHash(hashTable, "case");
    insertHash(hashTable, "char");
    insertHash(hashTable, "const");
    insertHash(hashTable, "continue");
    insertHash(hashTable, "default");
    insertHash(hashTable, "do");
    insertHash(hashTable, "int");
    insertHash(hashTable, "long");
    insertHash(hashTable, "register");
    insertHash(hashTable, "return");
    insertHash(hashTable, "short");
    insertHash(hashTable, "signed");
    insertHash(hashTable, "static");
    insertHash(hashTable, "struct");
    insertHash(hashTable, "switch");
    insertHash(hashTable, "typedef");
    insertHash(hashTable, "union");
    insertHash(hashTable, "unsigned");
    insertHash(hashTable, "void");
    insertHash(hashTable, "volatile");
    insertHash(hashTable, "while");
    insertHash(hashTable, "double");
    insertHash(hashTable, "else");
    insertHash(hashTable, "enum");
    insertHash(hashTable, "extern");
    insertHash(hashTable, "float");
    insertHash(hashTable, "for");
    insertHash(hashTable, "goto");
    insertHash(hashTable, "if");
}
//Searches hash table for string
//It is assumed that hashTable != NULL (guaranteed by main) and string != NULL (guaranteed by parse_word)
//Returns 1 if string is a keyword, returns 0 if string is not a keyword
int searchHash(node** hashTable, char string[], int len) {
	int bucket = getBucket(string,len);
	node* ptr = hashTable[bucket];
	while (ptr != NULL) {
		if (strncmp(ptr->data, string, len) == 0)
			return 1;
		ptr = ptr->next;
    }
	return 0;
}
//Free all nodes in hash table
//It is assumed that hashTable != NULL (guaranteed by main)
void freeHash(node** hashTable) {
	for (int i = 0; i < 35; i++) {
		node* freePtr = hashTable[i];
		while(freePtr != NULL) {
			node* temp = freePtr->next;
			free(freePtr);
			freePtr = temp;
        }
	}
}
