#include <stdlib.h>

#ifndef STACK_H
#define STACK_H

// Define the stack structure
typedef struct {
    void **items;
    size_t itemSize;
    int top;
    int capacity;
} Stack;

// Function prototypes
Stack* createStack(size_t itemSize, int capacity);
void resizeStack(Stack *stack, int newCapacity);
void push(Stack *stack, void *item);
void pop(Stack *stack);
void* peek(Stack *stack);

#endif // STACK_H