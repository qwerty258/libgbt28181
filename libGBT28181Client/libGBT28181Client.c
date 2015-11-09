#include "libGBT28181Client.h"
#include "clientConfigDefine.h"
#include "workingThread.h"

#define CHECK_NULL_AND_RETURN(pointer)  \
if(NULL == (pointer))                   \
{                                       \
    return GBT28181_NOMEM;              \
}                                       \
else                                    \
{                                       \
    return GBT28181_SUCCESS;            \
}

#define CHECK_NULL_PARAMETER(pointer)   \
if(NULL == (pointer))                   \
{                                       \
    return GBT28181_BADPARAMETER;       \
}

#define CHECK_INITIALED(bool_initialed) \
if(!(bool_initialed))                   \
{                                       \
    return GBT28181_CLIENT_UNINITIALED; \
}

#define CHECK_CALLBACK(pointer)         \
if(NULL == (pointer))                   \
{                                       \
    return GBT28181_CALLBACK_NOTSET;    \
}

#define CHECK_ONLINE_NO_SET(bool_online)    \
if((bool_online))                           \
{                                           \
    return GBT28181_ALREADY_ONLINE;         \
}

#define CHECK_MUST_ON_LINE(bool_online) \
if(!(bool_online))                      \
{                                       \
    return GBT28181_MUST_ONLINE;        \
}

client_configurations global_client_configurations;

LIBGBT28181CLIENT_API int GBT28181_client_initial(void)
{
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.online = false;
    global_client_configurations.initialed = true;
    InitializeCriticalSectionAndSpinCount(&global_client_configurations.critical_section, 4000);
    global_client_configurations.client_user_name = NULL;
    global_client_configurations.client_authentication_ID = NULL;
    global_client_configurations.client_password = NULL;
    global_client_configurations.client_IP = NULL;
    global_client_configurations.client_port = 5060;
    global_client_configurations.server_ID = NULL;
    global_client_configurations.server_domain = NULL;
    global_client_configurations.server_IP = NULL;
    global_client_configurations.server_port = 5060;
    global_client_configurations.expiration_interval = 3600;
    global_client_configurations.heartbeat_interval = 60;
    global_client_configurations.max_time_out_number = 3;
    global_client_configurations.address_family = GBT28181_AF_INET;
    global_client_configurations.protocol = GBT28181_IPPROTO_UDP;
    global_client_configurations.exosip_context = eXosip_malloc();
    global_client_configurations.registration_ID = 0;
    global_client_configurations.thread_loop = false;
    global_client_configurations.MANSCDP_SN = 1;
    global_client_configurations.give_out_query_device_info_result = NULL;

    CHECK_NULL_AND_RETURN(global_client_configurations.exosip_context);
}

LIBGBT28181CLIENT_API int GBT28181_set_client_name(char* SIP_user_name)
{
    CHECK_NULL_PARAMETER(SIP_user_name);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.client_user_name = osip_strdup(SIP_user_name);

    CHECK_NULL_AND_RETURN(global_client_configurations.client_user_name);
}

LIBGBT28181CLIENT_API int GBT28181_set_client_ID(char* SIP_user_ID)
{
    CHECK_NULL_PARAMETER(SIP_user_ID);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.client_authentication_ID = osip_strdup(SIP_user_ID);

    CHECK_NULL_AND_RETURN(global_client_configurations.client_authentication_ID);
}

LIBGBT28181CLIENT_API int GBT28181_set_client_password(char* SIP_password)
{
    CHECK_NULL_PARAMETER(SIP_password);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.client_password = osip_strdup(SIP_password);

    CHECK_NULL_AND_RETURN(global_client_configurations.client_password);
}

LIBGBT28181CLIENT_API int GBT28181_set_client_IP(char* client_IP)
{
    CHECK_NULL_PARAMETER(client_IP);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.client_IP = osip_strdup(client_IP);

    CHECK_NULL_AND_RETURN(global_client_configurations.client_IP);
}

LIBGBT28181CLIENT_API int GBT28181_set_client_port(int local_port)
{
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.client_port = local_port;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_SIP_server_ID(char* server_ID)
{
    CHECK_NULL_PARAMETER(server_ID);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.server_ID = osip_strdup(server_ID);

    CHECK_NULL_AND_RETURN(global_client_configurations.server_ID);
}

LIBGBT28181CLIENT_API int GBT28181_set_SIP_server_domain(char* server_domain)
{
    CHECK_NULL_PARAMETER(server_domain);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.server_domain = osip_strdup(server_domain);

    CHECK_NULL_AND_RETURN(global_client_configurations.server_domain);
}

LIBGBT28181CLIENT_API int GBT28181_set_SIP_server_IP(char* server_IP)
{
    CHECK_NULL_PARAMETER(server_IP);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.server_IP = osip_strdup(server_IP);

    CHECK_NULL_AND_RETURN(global_client_configurations.server_IP);
}

LIBGBT28181CLIENT_API int GBT28181_set_SIP_server_port(int server_port)
{
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.server_port = server_port;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_expiration_interval(int seconds)
{
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    if(seconds < 3600)
    {
        return GBT28181_INTERVAL_TOO_SMALL;
    }

    global_client_configurations.expiration_interval = seconds;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_heartbeat_interval(int seconds)
{
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    if(seconds < 60)
    {
        return GBT28181_INTERVAL_TOO_SMALL;
    }

    global_client_configurations.heartbeat_interval = seconds;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_max_time_out_number(int max_number)
{
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    if(max_number < 3)
    {
        return GBT28181_INTERVAL_TOO_SMALL;
    }

    global_client_configurations.max_time_out_number = max_number;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_address_family(int address_family)
{
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    if(address_family < 2 || 30 < address_family)
    {
        return GBT28181_BADPARAMETER;
    }

    global_client_configurations.address_family = address_family;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_IP_protocol(int protocol)
{
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    if(protocol < 6 || 30 < protocol)
    {
        return GBT28181_BADPARAMETER;
    }

    global_client_configurations.protocol = protocol;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_client_go_online(void)
{
    CHECK_INITIALED(global_client_configurations.initialed);

    if(NULL == global_client_configurations.client_user_name ||
       NULL == global_client_configurations.client_authentication_ID ||
       NULL == global_client_configurations.client_password ||
       NULL == global_client_configurations.client_IP ||
       NULL == global_client_configurations.server_ID ||
       NULL == global_client_configurations.server_domain ||
       NULL == global_client_configurations.server_IP)
    {
        return GBT28181_ESSENTIAL_CLIENT_PARAMETER_ERROR;
    }

    int result = 0;
    osip_message_t* registration_message = NULL;
    char* from = osip_malloc(512);
    char* proxy = osip_malloc(512);

    if(NULL == from || NULL == proxy)
    {
        osip_free(from);
        osip_free(proxy);
        return GBT28181_NOMEM;
    }

    result = eXosip_init(global_client_configurations.exosip_context);
    if(GBT28181_SUCCESS != result)
    {
        return result;
    }

    result = eXosip_listen_addr(
        global_client_configurations.exosip_context,
        global_client_configurations.protocol,
        NULL,
        global_client_configurations.client_port,
        global_client_configurations.address_family,
        0);
    if(GBT28181_SUCCESS != result)
    {
        eXosip_quit(global_client_configurations.exosip_context);
        return result;
    }

    eXosip_set_user_agent(global_client_configurations.exosip_context, "libGBT28181");

    result = eXosip_add_authentication_info(
        global_client_configurations.exosip_context,
        global_client_configurations.client_user_name,
        global_client_configurations.client_authentication_ID,
        global_client_configurations.client_password,
        NULL,
        NULL);
    if(GBT28181_SUCCESS != result)
    {
        return result;
    }

    snprintf(
        from,
        256,
        "sip:%s@%s",
        global_client_configurations.client_user_name,
        global_client_configurations.client_IP);

    snprintf(
        proxy,
        256,
        "sip:%s@%s",
        global_client_configurations.server_ID,
        global_client_configurations.server_IP);

    global_client_configurations.registration_ID = eXosip_register_build_initial_register(
        global_client_configurations.exosip_context,
        from,
        proxy,
        NULL,
        global_client_configurations.expiration_interval,
        &registration_message);
    if(global_client_configurations.registration_ID < 1)
    {
        osip_free(from);
        osip_free(proxy);
        return GBT28181_UNDEFINED_ERROR;
    }

    osip_free(from);
    osip_free(proxy);

    result = eXosip_register_send_register(
        global_client_configurations.exosip_context,
        global_client_configurations.registration_ID,
        registration_message);
    if(GBT28181_SUCCESS != result)
    {
        return result;
    }

    global_client_configurations.thread_loop = true;

    if(NULL == osip_thread_create(20000, register_working_thread, &global_client_configurations))
    {
        return GBT28181_THREAD_CREATE_FAILED;
    }

    if(NULL == osip_thread_create(20000, event_working_thread, &global_client_configurations))
    {
        return GBT28181_THREAD_CREATE_FAILED;
    }

    global_client_configurations.online = true;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_query_device_info_callback(function_query_device_info_callback cb)
{
    CHECK_NULL_PARAMETER(cb);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.give_out_query_device_info_result = cb;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_query_device_info(char* target_sip_user_name)
{
    CHECK_NULL_PARAMETER(target_sip_user_name);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_MUST_ON_LINE(global_client_configurations.online);
    CHECK_CALLBACK(global_client_configurations.give_out_query_device_info_result);

    int result = OSIP_SUCCESS;
    osip_message_t* query_query_device_message = NULL;
    char* from = osip_malloc(512);
    char* to = osip_malloc(512);
    char* proxy = osip_malloc(512);
    char* message_body = osip_malloc(1500);

    if(NULL == from || NULL == to || NULL == proxy || NULL == message_body)
    {
        osip_free(from);
        osip_free(to);
        osip_free(proxy);
        osip_free(message_body);
        return GBT28181_NOMEM;
    }

    snprintf(from, 512, "sip:%s@%s", global_client_configurations.client_user_name, global_client_configurations.client_IP);
    snprintf(to, 512, "sip:%s@%s", target_sip_user_name, global_client_configurations.server_domain);
    snprintf(proxy, 512, "sip:%s@%s", global_client_configurations.server_ID, global_client_configurations.server_IP);

    result = eXosip_message_build_request(
        global_client_configurations.exosip_context,
        &query_query_device_message,
        "MESSAGE",
        to,
        from,
        proxy);
    if(OSIP_SUCCESS != result)
    {
        osip_free(from);
        osip_free(to);
        osip_free(proxy);
        osip_free(message_body);
        return result;
    }

    result = osip_message_set_content_type(query_query_device_message, "application/MANSCDP+xml");
    if(OSIP_SUCCESS != result)
    {
        osip_free(from);
        osip_free(to);
        osip_free(proxy);
        osip_free(message_body);
        return result;
    }

    snprintf(message_body, 1500, "<?xml version=\"1.0\"?><Query><CmdType>DeviceInfo</CmdType><SN>%u</SN><DeviceID>%s</DeviceID></Query>", global_client_configurations.MANSCDP_SN, target_sip_user_name);

    result = osip_message_set_body(query_query_device_message, message_body, strnlen(message_body, 1500));
    if(OSIP_SUCCESS != result)
    {
        osip_free(from);
        osip_free(to);
        osip_free(proxy);
        osip_free(message_body);
        return result;
    }

    eXosip_lock(global_client_configurations.exosip_context);
    result = eXosip_message_send_request(global_client_configurations.exosip_context, query_query_device_message);
    eXosip_unlock(global_client_configurations.exosip_context);
    if(OSIP_SUCCESS != result)
    {
        osip_free(from);
        osip_free(to);
        osip_free(proxy);
        osip_free(message_body);
        return result;
    }

    EnterCriticalSection(&global_client_configurations.critical_section);
    global_client_configurations.MANSCDP_SN++;
    LeaveCriticalSection(&global_client_configurations.critical_section);

    osip_free(from);
    osip_free(to);
    osip_free(proxy);
    osip_free(message_body);

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_query_device_status_callback(function_query_device_status_callback cb)
{
    CHECK_NULL_PARAMETER(cb);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_ONLINE_NO_SET(global_client_configurations.online);

    global_client_configurations.give_out_query_device_status_result = cb;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_query_device_status(char* target_sip_user_name)
{
    CHECK_NULL_PARAMETER(target_sip_user_name);
    CHECK_INITIALED(global_client_configurations.initialed);
    CHECK_MUST_ON_LINE(global_client_configurations.online);
    CHECK_CALLBACK(global_client_configurations.give_out_query_device_status_result);

    int result = OSIP_SUCCESS;
    osip_message_t* query_device_status_message = NULL;
    char* from = osip_malloc(512);
    char* to = osip_malloc(512);
    char* proxy = osip_malloc(512);
    char* message_body = osip_malloc(1500);

    if(NULL == from || NULL == to || NULL == proxy || NULL == message_body)
    {
        osip_free(from);
        osip_free(to);
        osip_free(proxy);
        osip_free(message_body);
        return GBT28181_NOMEM;
    }

    snprintf(from, 512, "sip:%s@%s", global_client_configurations.client_user_name, global_client_configurations.client_IP);
    snprintf(to, 512, "sip:%s@%s", target_sip_user_name, global_client_configurations.server_domain);
    snprintf(proxy, 512, "sip:%s@%s", global_client_configurations.server_ID, global_client_configurations.server_IP);

    result = eXosip_message_build_request(
        global_client_configurations.exosip_context,
        &query_device_status_message,
        "MESSAGE",
        to,
        from,
        proxy);
    if(OSIP_SUCCESS != result)
    {
        osip_free(from);
        osip_free(to);
        osip_free(proxy);
        osip_free(message_body);
        return result;
    }

    result = osip_message_set_content_type(query_device_status_message, "application/MANSCDP+xml");
    if(OSIP_SUCCESS != result)
    {
        osip_free(from);
        osip_free(to);
        osip_free(proxy);
        osip_free(message_body);
        return result;
    }

    snprintf(message_body, 1500, "<?xml version=\"1.0\"?><Query><CmdType>DeviceStatus</CmdType><SN>%u</SN><DeviceID>%s</DeviceID></Query>", global_client_configurations.MANSCDP_SN, target_sip_user_name);

    result = osip_message_set_body(query_device_status_message, message_body, strnlen(message_body, 1500));
    if(OSIP_SUCCESS != result)
    {
        osip_free(from);
        osip_free(to);
        osip_free(proxy);
        osip_free(message_body);
        return result;
    }

    eXosip_lock(global_client_configurations.exosip_context);
    result = eXosip_message_send_request(global_client_configurations.exosip_context, query_device_status_message);
    eXosip_unlock(global_client_configurations.exosip_context);
    if(OSIP_SUCCESS != result)
    {
        osip_free(from);
        osip_free(to);
        osip_free(proxy);
        osip_free(message_body);
        return result;
    }

    EnterCriticalSection(&global_client_configurations.critical_section);
    global_client_configurations.MANSCDP_SN++;
    LeaveCriticalSection(&global_client_configurations.critical_section);

    osip_free(from);
    osip_free(to);
    osip_free(proxy);
    osip_free(message_body);

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_free_client(void)
{
    CHECK_INITIALED(global_client_configurations.initialed);

    DeleteCriticalSection(&global_client_configurations.critical_section);

    osip_free(global_client_configurations.client_user_name);
    osip_free(global_client_configurations.client_authentication_ID);
    osip_free(global_client_configurations.client_password);
    osip_free(global_client_configurations.client_IP);
    osip_free(global_client_configurations.server_ID);
    osip_free(global_client_configurations.server_domain);
    osip_free(global_client_configurations.server_IP);

    global_client_configurations.thread_loop = false;

    eXosip_quit(global_client_configurations.exosip_context);

    return GBT28181_SUCCESS;
}
