#include <string.h>
#include <stdlib.h>

bool isPalindrome(int x)
{
    if (x < 0)
    {
        return 0;
    }
    int remainder = 0;
    long int reversed = 0;
    int original = x;

    while (x != 0)
    {
        remainder = x % 10;
        reversed = reversed * 10 + remainder;
        x /= 10;
    }

    bool result = (reversed == original ? 1 : 0);

    return result;
}