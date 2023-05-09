#include <string.h>

char * intToRoman(int num)
{
    char *return_roman = (char *) malloc(sizeof(char) * 20);
    memset(return_roman, 0, sizeof(char) * 20);
    int i = 0;

    while(num > 0)
    {
        if (num >= 1000)
        {
            return_roman[i++] = 'M';
            num -= 1000;
        }
        else if (num >= 900)
        {
            return_roman[i++] = 'C';
            return_roman[i++] = 'M';
            num -= 900;
        }
        else if (num >= 500)
        {
            return_roman[i++] = 'D';
            num -= 500;
        }
        else if (num >= 400)
        {
            return_roman[i++] = 'C';
            return_roman[i++] = 'D';
            num -= 400;
        }
        else if (num >= 100)
        {
            return_roman[i++] = 'C';
            num -= 100;
        }
        else if (num >= 90)
        {
            return_roman[i++] = 'X';
            return_roman[i++] = 'C';
            num -= 90;
        }
        else if (num >= 50)
        {
            return_roman[i++] = 'L';
            num -= 50;
        }
        else if (num >= 40)
        {
            return_roman[i++] = 'X';
            return_roman[i++] = 'L';
            num -= 40;
        }
        else if (num >= 10)
        {
            return_roman[i++] = 'X';
            num -= 10;
        }
        else if (num >= 9)
        {
            return_roman[i++] = 'I';
            return_roman[i++] = 'X';
            num -= 9;
        }
        else if (num >= 5)
        {
            return_roman[i++] = 'V';
            num -= 5;
        }
        else if (num >= 4)
        {
            return_roman[i++] = 'I';
            return_roman[i++] = 'V';
            num -= 4;
        }
        else if (num >= 1)
        {
            return_roman[i++] = 'I';
            num -= 1;
        }
    }

    return return_roman;
}