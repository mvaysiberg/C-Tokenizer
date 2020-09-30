#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>

typedef struct _tokenProperties {
    int endpos;
    char* tokenName;
    int dynamicTN;
} tokenProperties;

tokenProperties parse_word(char* tokenString, int x);
int isKeyword(char* str);


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
    tokenProperties ret = {x, "word", 0};
    //if the token is a keyword then we return keyword as its type
    if (isKeyword(token)){
        ret.tokenName = token;
        ret.dynamicTN = 1;
        //token will be freed in main
    }else{
        //token is no longer being used
        free(token);
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
    }else if(strcmp(str, "sizeof") == 0){
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