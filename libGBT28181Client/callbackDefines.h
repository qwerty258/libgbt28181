#ifndef _CALLBACK_DEFINES_H_
#define _CALLBACK_DEFINES_H_

#include <stdbool.h>

typedef enum _MANSCDP_result_type
{
    MANSCDP_OK,
    MANSCDP_ERROR
}result_type;

typedef void(*function_query_device_info_callback)(char* device_ID, char* device_type, char* manufacturer, char* model, char* firmware, unsigned long long max_camera, unsigned long long max_alarm);

typedef void(*function_query_device_status_callback)(char* device_ID, bool online, result_type status);

#endif // !_CALLBACK_DEFINES_H_
