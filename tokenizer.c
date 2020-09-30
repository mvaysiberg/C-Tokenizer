#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>

typedef struct _tokenProperties {
    int endpos;
    char* tokenName;
} tokenProperties;

tokenProperties parse_word(char* tokenString, int x);
tokenProperties parse_digit(char* tokenString, int x);
char* isKeyword(char* str);


int main(int argc, char ** argv){
	return 0;
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
    //if the token is a keyword then we return keyword as its type
    char* tokenKeyword = isKeyword(token);
    if (tokenKeyword != NULL){
        ret.tokenName = tokenKeyword;
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
        //If the second char is not X or x, eliminate hex
        }else if(x == start + 1 && toupper(tokenString[x]) != 'X'){
            possibleHex = 0;
        }
        //If the char breaks the token, leave the while loop to end token
        if (!(isdigit(tokenString[x]) || (isalpha(tokenString[x]) && possibleHex) || (possibleDecimal && toupper(tokenString[x])=='E') || tokenString[x] == '.' || (x == ePos + 1 && (tokenString[x] == '+' || tokenString[x] == '-')))){
            break;
        }if(tokenString[x] == '.'){
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
        if (possibleHex && !isxdigit(tokenString[x])){
            break;
        }
        if(toupper(tokenString[x]) == 'E' && (!isdigit(tokenString[x+1]) || tokenString[x+1] != '+' || tokenString[x+1] != '-')){
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
char* isKeyword(char* str){
    //returns whether the contents of str are a C keyword
     if (strcmp(str, "sizeof") == 0){
        return "sizeof";
    }else if(strcmp(str, "break") == 0){
        return "break";
    }else if(strcmp(str, "case") == 0){
        return "case";
    }else if(strcmp(str, "char") == 0){
        return "char";
    }else if(strcmp(str, "const") == 0){
        return "const";
    }else if(strcmp(str, "continue") == 0){
        return "continue";
    }else if(strcmp(str, "default") == 0){
        return "default";
    }else if(strcmp(str, "do") == 0){
       return "do";
    }else if(strcmp(str, "int") == 0){
        return "int";
    }else if(strcmp(str, "long") == 0){
        return "long";
    }else if(strcmp(str, "register") == 0){
        return "register";
    }else if(strcmp(str, "return") == 0){
        return "return";
    }else if(strcmp(str, "short") == 0){
        return "short";
    }else if(strcmp(str, "signed") == 0){
        return "signed";
    }else if(strcmp(str, "sizeof") == 0){
        return "sizeof";
    }else if(strcmp(str, "static") == 0){
        return "static";
    }else if(strcmp(str, "struct") == 0){
        return "struct";
    }else if(strcmp(str, "switch") == 0){
        return "switch";
    }else if(strcmp(str, "typedef") == 0){
        return "typedef";
    }else if(strcmp(str, "union") == 0){
        return "union";
    }else if(strcmp(str, "unsigned") == 0){
        return "unsigned";
    }else if(strcmp(str, "void") == 0){
        return "void";
    }else if(strcmp(str, "volatile") == 0){
        return "volatile";
    }else if(strcmp(str, "while") == 0){
        return "while";
    }else if(strcmp(str, "double") == 0){
        return "double";
    }else if(strcmp(str, "else") == 0){
        return "else";
    }else if(strcmp(str, "enum") == 0){
        return "enum";
    }else if(strcmp(str, "extern") == 0){
        return "extern";
    }else if(strcmp(str, "float") == 0){
        return "float";
    }else if(strcmp(str, "for") == 0){
        return "for";
    }else if(strcmp(str, "goto") == 0){
        return "goto";
    }else if(strcmp(str, "if") == 0){
        return "if";
    }else{
        return NULL;
    }
}