#include "DynamicArray.h"
#include <stdlib.h>
#include <string.h>

typedef struct _dynamic_array_context
{
    void** elements;
    uint32_t number;
    uint32_t size;
}dynamic_array_context;

dy_array_handle get_dy_array_handle()
{
    dynamic_array_context* p_context = malloc(sizeof(dynamic_array_context));
    if(NULL != p_context)
    {
        p_context->elements = calloc(1, 1);
        p_context->number = 0;
        p_context->size = 0;
    }
    return p_context;
}

int add_dy_array_element(dy_array_handle handle, void* element)
{
    dynamic_array_context* p_context = handle;
    void** temp = NULL;
    if(NULL == p_context)
    {
        return -1;
    }
    if(p_context->number < p_context->size)
    {
        p_context->elements[p_context->number] = element;
        p_context->number++;
    }
    else if(p_context->number == p_context->size)
    {
        temp = realloc(p_context->elements, p_context->size + 10);
        if(NULL != temp)
        {
            p_context->elements = temp;
            p_context->elements[p_context->number] = element;
            p_context->number++;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

uint32_t get_dy_array_element_number(dy_array_handle handle)
{
    dynamic_array_context* p_context = handle;
    if(NULL == p_context)
    {
        return -1;
    }
    return p_context->number;
}

void* get_dy_array_element_by_index(dy_array_handle handle, uint32_t index)
{
    dynamic_array_context* p_context = handle;
    if(NULL == p_context || index >= p_context->number)
    {
        return NULL;
    }
    return p_context->elements[index];
}

void delete_dy_array_element_by_index(dy_array_handle handle, uint32_t index)
{
    dynamic_array_context* p_context = handle;
    if(NULL == p_context || index >= p_context->number)
    {
        return;
    }
    for(uint32_t i = index; i < p_context->number; i++)
    {
        p_context->elements[i] = p_context->elements[i + 1];
    }
    p_context->number--;
}

void close_dy_array_handle(dy_array_handle handle)
{
    dynamic_array_context* p_context = handle;
    if(NULL != p_context)
    {
        free(p_context->elements);
        free(p_context);
    }
}