#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char * longestPalindrome(char * s)
{
    int found = 0, i = 0, j = strlen(s);
    printf("s = %s\n", s);
    char substring[strlen(s)+1];
    char reversed_substring[strlen(s)+1];
    
    while (!found)
    {
        printf("i = %d, j = %d\n", i, j);
        
        printf("string: %s\nreversed string:%s\n", substring, reversed_substring);
        strncpy(substring, s+i, j-i);
        substring[j-i] = '\0';
        
        
        // check if palindrome
        //  reverse substring
        printf("st4rlen(subs) = %ld\n", strlen(substring));
        for (int a = strlen(substring) - 2; a >= 0; a--)
        {
            reversed_substring[strlen(substring) - a - 1] = substring[a];
        }
        reversed_substring[j-i] = '\0';
        printf("string: %s\nreversed string:%s\n", substring, reversed_substring);

        // compare with original
        //if found, return substring
        if (!strcmp(reversed_substring, substring))
        {
            found = 1;
            return substring;
        }

        // else increment and retry bounds
        i++;
        j++;
        
        printf("i = %d, j = %d\n", i, j);

        if(j > strlen(s))
        {
            j = (j-i)-1;
            i = 0;
        }
        
        printf("i = %d, j = %d\n\n", i, j);
    }
    return;
}

// int main()
// {
//     char word[] = "hellolleh";
//     word[strlen(word)] = '\0';
//     printf(longestPalindrome(word));
// }