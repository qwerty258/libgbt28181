#ifndef _CALLBACK_DEFINES_H_
#define _CALLBACK_DEFINES_H_

typedef void(*function_query_deviceInfo_callback)(char* device_ID, char* device_type, char* manufacturer, char* model, char* firmware, unsigned long long max_camera, unsigned long long max_alarm);

#endif // !_CALLBACK_DEFINES_H_
