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
if(!bool_initialed)                     \
{                                       \
    return GBT28181_CLIENT_UNINITIALED; \
}

client_configurations global_client_configurations;

LIBGBT28181CLIENT_API int GBT28181_client_initial(void)
{
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
    global_client_configurations.registration_message = NULL;
    global_client_configurations.registration_ID = 0;
    global_client_configurations.register_thread = NULL;
    global_client_configurations.event_thread = NULL;
    global_client_configurations.keepalive_thread = NULL;
    global_client_configurations.thread_loop = false;
    global_client_configurations.MANSCDP_SN = 1;
    CHECK_NULL_AND_RETURN(global_client_configurations.exosip_context)
}

LIBGBT28181CLIENT_API int GBT28181_set_client_name(char* SIP_user_name)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    CHECK_NULL_PARAMETER(SIP_user_name)

    global_client_configurations.client_user_name = osip_strdup(SIP_user_name);

    CHECK_NULL_AND_RETURN(global_client_configurations.client_user_name)
}

LIBGBT28181CLIENT_API int GBT28181_set_client_ID(char* SIP_user_ID)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    CHECK_NULL_PARAMETER(SIP_user_ID)

    global_client_configurations.client_authentication_ID = osip_strdup(SIP_user_ID);

    CHECK_NULL_AND_RETURN(global_client_configurations.client_authentication_ID)
}

LIBGBT28181CLIENT_API int GBT28181_set_client_password(char* SIP_password)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    CHECK_NULL_PARAMETER(SIP_password)

    global_client_configurations.client_password = osip_strdup(SIP_password);

    CHECK_NULL_AND_RETURN(global_client_configurations.client_password)
}

LIBGBT28181CLIENT_API int GBT28181_set_client_IP(char* client_IP)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    CHECK_NULL_PARAMETER(client_IP);

    global_client_configurations.client_IP = osip_strdup(client_IP);

    CHECK_NULL_AND_RETURN(global_client_configurations.client_IP);
}

LIBGBT28181CLIENT_API int GBT28181_set_client_port(int local_port)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    global_client_configurations.client_port = local_port;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_SIP_server_ID(char* server_ID)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    CHECK_NULL_PARAMETER(server_ID)

    global_client_configurations.server_ID = osip_strdup(server_ID);

    CHECK_NULL_AND_RETURN(global_client_configurations.server_ID)
}

LIBGBT28181CLIENT_API int GBT28181_set_SIP_server_domain(char* server_domain)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    CHECK_NULL_PARAMETER(server_domain)

    global_client_configurations.server_domain = osip_strdup(server_domain);

    CHECK_NULL_AND_RETURN(global_client_configurations.server_domain)
}

LIBGBT28181CLIENT_API int GBT28181_set_SIP_server_IP(char* server_IP)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    CHECK_NULL_PARAMETER(server_IP);

    global_client_configurations.server_IP = osip_strdup(server_IP);

    CHECK_NULL_AND_RETURN(global_client_configurations.server_IP)
}

LIBGBT28181CLIENT_API int GBT28181_set_SIP_server_port(int server_port)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    global_client_configurations.server_port = server_port;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_expiration_interval(int seconds)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    if(seconds < 3600)
    {
        return GBT28181_INTERVAL_TOO_SMALL;
    }

    global_client_configurations.expiration_interval = seconds;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_heartbeat_interval(int seconds)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    if(seconds < 60)
    {
        return GBT28181_INTERVAL_TOO_SMALL;
    }

    global_client_configurations.heartbeat_interval = seconds;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_max_time_out_number(int max_number)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    if(max_number < 3)
    {
        return GBT28181_INTERVAL_TOO_SMALL;
    }

    global_client_configurations.max_time_out_number = max_number;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_address_family(int address_family)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    if(address_family < 2 || 30 < address_family)
    {
        return GBT28181_BADPARAMETER;
    }

    global_client_configurations.address_family = address_family;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_set_IP_protocol(int protocol)
{
    CHECK_INITIALED(global_client_configurations.initialed)

    if(protocol < 6 || 30 < protocol)
    {
        return GBT28181_BADPARAMETER;
    }

    global_client_configurations.protocol = protocol;

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_client_go_online(void)
{
    CHECK_INITIALED(global_client_configurations.initialed)

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
        &global_client_configurations.registration_message);
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
        global_client_configurations.registration_message);
    if(GBT28181_SUCCESS != result)
    {
        return result;
    }

    global_client_configurations.thread_loop = true;

    global_client_configurations.register_thread = osip_thread_create(20000, register_working_thread, &global_client_configurations);
    if(NULL == global_client_configurations.register_thread)
    {
        return GBT28181_THREAD_CREATE_FAILED;
    }

    global_client_configurations.event_thread = osip_thread_create(20000, event_working_thread, &global_client_configurations);
    if(NULL == global_client_configurations.event_thread)
    {
        return GBT28181_THREAD_CREATE_FAILED;
    }

    return GBT28181_SUCCESS;
}

LIBGBT28181CLIENT_API int GBT28181_free_client(void)
{
    CHECK_INITIALED(global_client_configurations.initialed)

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

    osip_message_free(global_client_configurations.registration_message);

    return GBT28181_SUCCESS;
}
