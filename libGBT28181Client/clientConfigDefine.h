#ifndef _CLIENT_CONFIG_DEFINE_H_
#define _CLIENT_CONFIG_DEFINE_H_

#include "callbackDefines.h"
#include <stdbool.h>
#include <osip2\osip_mt.h>
#include <eXosip2\eXosip.h>
#include <Windows.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif // _MSC_VER

typedef struct _live_video_context
{
    bool live_video_streaming;
    uint16_t port_RTP;
    int call_id;
    char* target_sip_user_name;
    char* target_IP;
    uint16_t port_SIP;
    int dialog_id;
}live_video_context;

typedef struct _client_configurations
{
    bool online;
    bool initialed;
    CRITICAL_SECTION critical_section;
    char* client_user_name;
    char* client_authentication_ID;
    char* client_password;
    char* client_IP;
    int client_port;
    char* server_ID;
    char* server_domain;
    char* server_IP;
    int server_port;
    int expiration_interval;
    int heartbeat_interval;
    int max_time_out_number;
    int address_family;
    int protocol;
    struct eXosip_t* exosip_context;
    int registration_ID;
    bool thread_loop;
    uint64_t MANSCDP_SN;
    function_query_device_info_callback give_out_query_device_info_result;
    function_query_device_status_callback give_out_query_device_status_result;
    function_query_catalog_callback give_out_query_catalog_result;
    uint32_t max_live_video_number;
    live_video_context** live_video_context_pointer_array;
}client_configurations;

typedef enum _MANSCDP_xml_type
{
    MANSCDP_xml_Unknown,
    MANSCDP_xml_Control,
    MANSCDP_xml_Query,
    MANSCDP_xml_Notify,
    MANSCDP_xml_Response
}MANSCDP_xml_type;

typedef enum _MANSCDP_command_type
{
    MANSCDP_command_type_unknown,
    MANSCDP_DeviceInfo,
    MANSCDP_DeviceStatus,
    MANSCDP_Catalog
}MANSCDP_command_type;

typedef struct _MANSCDP_xml
{
    MANSCDP_xml_type xml_type;
    MANSCDP_command_type command_type;
    uint64_t MANSCDP_SN;
    char* DeviceID;
    char* Result;
    char* DeviceType;
    char* Manufacturer;
    char* Model;
    char* Firmware;
    uint64_t MaxCamera;
    uint64_t MaxAlarm;
    uint64_t catalog_sum_num;
    MANSCDP_on_off_line online;
    MANSCDP_result_type status;
    MANSCDP_device* p_MANSCDP_device;
    client_configurations* p_client_configurations; // do not free in free_MANSCDP_xml_pointer
}MANSCDP_xml;

#endif // !_CLIENT_CONFIG_DEFINE_H_

