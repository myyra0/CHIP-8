#include <stdio.h>

#include "stack.h"

#define MAX 16

static int stack[MAX];
static int top = -1;

// Push an element onto the stack
void push(int value)
{
    if (top == MAX - 1)
    {
        printf("\nStack Overflow");
    }
    else
    {
        stack[++top] = value;
    }
}

// Pop an element from the stack
int pop()
{
    if (top == -1)
    {
        printf("\nStack Underflow");
        return -1;
    }
    else
    {
        return stack[top--];
    }
}

// Peek at the top element of the stack
void peek()
{
    if (top == -1)
    {
        printf("\nStack is Empty");
    }
    else
    {
        printf("\nTop Element is: %d", stack[top]);
    }
}

// Display all elements in the stack
void display()
{
    if (top == -1)
    {
        printf("\nStack is Empty");
    }
    else
    {
        printf("\nStack Elements:\n");
        for (int i = 0; i <= top; i++)
        {
            printf("%d ", stack[i]);
        }
        printf("\n");
    }
}
