bool isValid(char * s)
{
    char stackArray[10000];
    int top = 0;

    for (int i = 0; i < strlen(s); s++)
    {
        if (s[i] == '(' || s[i] == '[' || s[i] == '{')
        {
            top++;
            stackArray[top] = s[i];
            printf("added top bracket\n");
        }
        else if (s[i] == ')' || s[i] == ']' || s[i] == '}')
        {
            if ((stackArray[top] == '(' && s[i] == ')') ||
                (stackArray[top] == '[' && s[i] == ']') ||
                (stackArray[top] == '{' && s[i] == '}'))
            {
                printf("removed top bracket\n");
                stackArray[top--] = '\0';
            }
            else
            {
                return 0;
            }
        }
    }

    printf("top = %d\n", top);

    if (top == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}