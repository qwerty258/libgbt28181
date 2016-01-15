#ifndef _LIBGBT28181CLIENT_H_
#define _LIBGBT28181CLIENT_H_

#include "exportSymbolsAsCStyle.h"

C_EXPORT_BEGIN

#include "callbackDefines.h"
#include "errorCodeDefines.h"

typedef struct _PTZ_control_data
{
    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;
    uint8_t zoom_in;
    uint8_t zoom_out;
    uint8_t relative_pan_speed;
    uint8_t relative_tilt_speed;
    uint8_t relative_zoom_speed;
}PTZ_control_data;

EXPORT_SYMBOLS_API char* never_use_this;

EXPORT_SYMBOLS_API int GBT28181_client_initial(void);

EXPORT_SYMBOLS_API int GBT28181_set_client_name(char* SIP_user_name);

EXPORT_SYMBOLS_API int GBT28181_set_client_ID(char* SIP_user_ID);

EXPORT_SYMBOLS_API int GBT28181_set_client_password(char* SIP_password);

EXPORT_SYMBOLS_API int GBT28181_set_client_IP(char* client_IP);

EXPORT_SYMBOLS_API int GBT28181_set_client_port(int local_port);

EXPORT_SYMBOLS_API int GBT28181_set_SIP_server_ID(char* server_ID);

EXPORT_SYMBOLS_API int GBT28181_set_SIP_server_domain(char* server_domain);

EXPORT_SYMBOLS_API int GBT28181_set_SIP_server_IP(char* server_IP);

EXPORT_SYMBOLS_API int GBT28181_set_SIP_server_port(int server_port);

EXPORT_SYMBOLS_API int GBT28181_set_expiration_interval(int seconds);

EXPORT_SYMBOLS_API int GBT28181_set_heartbeat_interval(int seconds);

EXPORT_SYMBOLS_API int GBT28181_set_max_time_out_number(int max_number);

#define GBT28181_AF_INET    2
#define GBT28181_AF_INET6   23

EXPORT_SYMBOLS_API int GBT28181_set_address_family(int address_family);

#define GBT28181_IPPROTO_TCP    6
#define GBT28181_IPPROTO_UDP    17

EXPORT_SYMBOLS_API int GBT28181_set_IP_protocol(int protocol);

EXPORT_SYMBOLS_API int GBT28181_set_query_device_info_callback(function_query_device_info_callback cb);

EXPORT_SYMBOLS_API int GBT28181_set_query_device_status_callback(function_query_device_status_callback cb);

EXPORT_SYMBOLS_API int GBT28181_set_query_catalog_callback(function_query_catalog_callback cb);

EXPORT_SYMBOLS_API int GBT28181_set_max_number_of_live_video(uint32_t max_number);

EXPORT_SYMBOLS_API int GBT28181_client_go_online(void);

EXPORT_SYMBOLS_API int GBT28181_query_device_info(char* target_sip_user_name, char* target_IP, uint16_t port);

EXPORT_SYMBOLS_API int GBT28181_query_device_status(char* target_sip_user_name, char* target_IP, uint16_t port);

EXPORT_SYMBOLS_API int GBT28181_query_catalog(char* target_sip_user_name, char* target_IP, uint16_t port);

EXPORT_SYMBOLS_API int GBT28181_PTZ_control(char* target_sip_user_name, char* target_IP, uint16_t port, PTZ_control_data* p_PTZ_control_data);

EXPORT_SYMBOLS_API int GBT28181_get_idle_real_time_stream_handle(uint32_t* handle);

EXPORT_SYMBOLS_API int GBT28181_set_RTP_port(uint32_t handle, uint16_t port);

EXPORT_SYMBOLS_API int GBT28181_set_RTP_protocol(uint32_t handle, int protocol);

EXPORT_SYMBOLS_API int GBT28181_set_playing_hwnd(uint32_t handle, void* hWnd);

EXPORT_SYMBOLS_API int GBT28181_get_real_time_stream(uint32_t handle, char* target_sip_user_name, char* target_IP, uint16_t port);

EXPORT_SYMBOLS_API int GBT28181_close_real_time_stream(uint32_t handle);

EXPORT_SYMBOLS_API int GBT28181_free_client(void);

C_EXPORT_END

#endif // !_LIBGBT28181CLIENT_H_



