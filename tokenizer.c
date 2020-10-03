#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>

//Get the properties of the token after parsing
typedef struct _tokenProperties {
    int endpos;
    char* tokenName;
} tokenProperties;

tokenProperties parse_quote(char* tokenString, int x);
tokenProperties parse_operator(char* tokenString, int x);
tokenProperties parse_word(char* tokenString, int x);
tokenProperties parse_digit(char* tokenString, int x);
int isKeyword(char* str);
void print_token(char* tokenName, char* start, char* end);

//Get command-line input and iterate through the string
int main(int argc, char ** argv){
    //Check if an input string is given
    if (argc == 1){
        printf("No arguments\n");
        exit(0);
    }
    tokenProperties tp;
    int commenting = 0;
    int y = 0;
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
            tp = parse_word(argv[1], y);
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
            print_token(tp.tokenName,&argv[1][y], &argv[1][tp.endpos]);
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
	return 0;
}
tokenProperties parse_quote(char* tokenString, int x){
    char qtype = tokenString[x];
    x+=1;
    tokenProperties ret;
    while (tokenString[x]!= '\0')
    {
        if (tokenString[x] == qtype)
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
tokenProperties parse_word(char* tokenString, int x){
    int start = x;
    //loops through the tokenString until the current char cannot be part of a word
    while (isalnum(tokenString[x])){
        ++x;
    }
    int len = x - start;
    char* token = (char*)(malloc(len + 1));
    strncpy(token,&(tokenString[start]),len);
    token[len] = '\0';
    tokenProperties ret = {x, "word"};
    //if the token is sizeof, the token type is sizeof
    if (strcmp(token,"sizeof") == 0){
        ret.tokenName = "sizeof";
    }
    //if the token is a keyword then we return keyword as its type
    else if (isKeyword(token)){
        ret.tokenName = "keyword";
    }
    free(token);
    return ret;
}
tokenProperties parse_digit(char* tokenString, int x){
    int start = x;
    int possibleFloat = 0;
    int periodPos = -1;
    int possibleOctal = 1;
    int possibleHex =1;
    int possibleDecimal = 1;
    int ePos=-2;

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
        if (!(isdigit(tokenString[x]) || (isalpha(tokenString[x]) && possibleHex) || (possibleFloat && toupper(tokenString[x])=='E') || tokenString[x] == '.' || (x == ePos + 1 && (tokenString[x] == '+' || tokenString[x] == '-')))){
            break;
        }
        if(tokenString[x] == '.'){
            //If it is a possible hex or there was already a period, the token will end
            if (possibleHex || possibleFloat || !isdigit(tokenString[x+1]))
                break;
            periodPos = x;
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
        else if (possibleFloat && toupper(tokenString[x]) == 'E' && ePos == -2){
            ePos = x;
        }
        //If an e occurs more than once
        else if (ePos != -2 && toupper(tokenString[x]) == 'E'){
            break;
        }
        //If characters after ‘+’ or ‘-’ are not valid
        else if((tokenString[x] == '+' || tokenString[x] == '-') && !isdigit(tokenString[x+1])){
            x = ePos;
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
int isKeyword(char* str){
    //returns whether the contents of str are a C keyword
     if (strcmp(str, "sizeof") == 0){
        return 1;
    }else if(strcmp(str, "break") == 0){
        return 1;
    }else if(strcmp(str, "case") == 0){
        return 1;
    }else if(strcmp(str, "char") == 0){
        return 1;
    }else if(strcmp(str, "const") == 0){
        return 1;
    }else if(strcmp(str, "continue") == 0){
        return 1;
    }else if(strcmp(str, "default") == 0){
        return 1;
    }else if(strcmp(str, "do") == 0){
       return 1;
    }else if(strcmp(str, "int") == 0){
        return 1;
    }else if(strcmp(str, "long") == 0){
        return 1;
    }else if(strcmp(str, "register") == 0){
        return 1;
    }else if(strcmp(str, "return") == 0){
        return 1;
    }else if(strcmp(str, "short") == 0){
        return 1;
    }else if(strcmp(str, "signed") == 0){
        return 1;
    }else if(strcmp(str, "static") == 0){
        return 1;
    }else if(strcmp(str, "struct") == 0){
        return 1;
    }else if(strcmp(str, "switch") == 0){
        return 1;
    }else if(strcmp(str, "typedef") == 0){
        return 1;
    }else if(strcmp(str, "union") == 0){
        return 1;
    }else if(strcmp(str, "unsigned") == 0){
        return 1;
    }else if(strcmp(str, "void") == 0){
        return 1;
    }else if(strcmp(str, "volatile") == 0){
        return 1;
    }else if(strcmp(str, "while") == 0){
        return 1;
    }else if(strcmp(str, "double") == 0){
        return 1;
    }else if(strcmp(str, "else") == 0){
        return 1;
    }else if(strcmp(str, "enum") == 0){
        return 1;
    }else if(strcmp(str, "extern") == 0){
        return 1;
    }else if(strcmp(str, "float") == 0){
        return 1;
    }else if(strcmp(str, "for") == 0){
        return 1;
    }else if(strcmp(str, "goto") == 0){
        return 1;
    }else if(strcmp(str, "if") == 0){
        return 1;
    }else{
        return 0;
    }
}

void print_token(char* tokenName, char* start, char* end){
    printf("%s: \"", tokenName);
    //prints each character [start,end)
    for(char * ptr = start; ptr != end; ++ptr){
        printf("%c", *ptr);
    }
    printf("\"\n");
}
