#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int main()
{
    int a, b, c;
    
    // printf("Enter the three integers:\n");
    
    scanf("%d%d%d", &a, &b, &c);
    
    
    // printf("a=%d\n", a);
    // printf("b=%d\n", b);
    // printf("c=%d\n", c);
    
    if (a+b > c && a+c > b && b+c > a)
    {
        printf("YES");
    }
    else
    {
        printf("NO");
    }
        
    return 0;
}