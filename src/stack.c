// Implements a generic stack data structure.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"

// Function to initialize the stack
Stack* createStack(size_t itemSize, int capacity) {
    printf("Creating stack\n");
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->itemSize = itemSize;
    stack->capacity = capacity;
    stack->top = -1;
    // Allocate memory for the stack items
    stack->items = (void **)malloc(capacity * sizeof(void *));
    for (int i = 0; i < capacity; i++) {
        stack->items[i] = malloc(itemSize);
    }
    return stack;
}

// Function to resize the stack
void resizeStack(Stack *stack, int newCapacity) {
    stack->items = (void **)realloc(stack->items, newCapacity * sizeof(void *));
    for (int i = stack->capacity; i < newCapacity; i++) {
        stack->items[i] = malloc(stack->itemSize);
    }
    stack->capacity = newCapacity;
}

// Function to push an item onto the stack
void push(Stack *stack, void *item) {
    if (stack->top == stack->capacity - 1) {
        resizeStack(stack, stack->capacity * 2); // Double the capacity if full
    }
    stack->top++;
    memcpy(stack->items[stack->top], item, stack->itemSize);
}

// Function to pop an item from the stack
void pop(Stack *stack) {
    if (stack->top == -1) {
        fprintf(stderr, "Stack underflow\n");
        return;
    }
    // memcpy(item, stack->items[stack->top], stack->itemSize);
    stack->top--;
}

// Function to peek the top item of the stack
void* peek(Stack *stack) {
    if (stack->top == -1) {
        fprintf(stderr, "Stack is empty\n");
        return NULL;
    }
    return stack->items[stack->top];
}