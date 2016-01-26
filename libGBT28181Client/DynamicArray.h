#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include <stdint.h>

typedef void* dy_array_handle;

dy_array_handle get_dy_array_handle();

int32_t add_dy_array_element(dy_array_handle handle, void* element);

uint32_t get_dy_array_element_number(dy_array_handle handle);

void* get_dy_array_element_by_index(dy_array_handle handle, uint32_t index);

void delete_dy_array_element_by_index(dy_array_handle handle, uint32_t index);

void close_dy_array_handle(dy_array_handle handle);

#endif // !_DYNAMIC_ARRAY_H_
