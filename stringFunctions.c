#include <stdio.h>
#include <stdlib.h>

void mystrcopy(char str1 [], char str2[]){
    int i = 0;
    while(str1[i] != '\0'){
        str2[i] = str1[i];
        i++;
    }
    str2[i] = '\0';
}

int mystrlength(char str []){
    int i = 0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
}

void mystrconcatinate(char str1 [], char str2[])
{
    int i = 0;
    while(str1[i] != '\0'){
        i++;
    }
    int j = 0;
    while (str2[j] != '\0')
    {
    	str1[i] = str2[j];
    	i++;
    	j++;
    } 
    str1[i] = '\0';
}

int my_strcmp(char *strg1, char *strg2)
{
    while( ( *strg1 != '\0' && *strg2 != '\0' ) && *strg1 == *strg2 )
    {
        strg1++;
        strg2++;
    }

    if(*strg1 == *strg2)
    {
        return 0; // strings are identical
    }

    else
    {
        return *strg1 - *strg2;
    }
}
