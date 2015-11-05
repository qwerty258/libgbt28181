#include "workingThread.h"
#include "clientConfigDefine.h"
#include <Windows.h>

void* register_working_thread(void* arg)
{
    client_configurations* thread_parameter = (client_configurations*)arg;
    int result;

    while(thread_parameter->thread_loop)
    {
        osip_usleep((thread_parameter->expiration_interval - 10) * 1000 * 1000);
        eXosip_lock(thread_parameter->exosip_context);
        result = eXosip_register_send_register(
            thread_parameter->exosip_context,
            thread_parameter->registration_ID,
            NULL);
        if(0 > result)
        {
            osip_thread_exit();
            return NULL;
        }
        eXosip_unlock(thread_parameter->exosip_context);
    }

    osip_thread_exit();
    return NULL;
}

void* event_working_thread(void* arg)
{
    client_configurations* thread_parameter = (client_configurations*)arg;
    eXosip_event_t* event = NULL;

    while(thread_parameter->thread_loop)
    {
        event = eXosip_event_wait(thread_parameter->exosip_context, 0, 1);
        if(NULL == event)
        {
            osip_usleep(100000);
            continue;
        }
        eXosip_automatic_action(thread_parameter->exosip_context);
        switch(event->type)
        {
            case EXOSIP_REGISTRATION_SUCCESS:
                printf("registrered successfully\n");
                thread_parameter->keepalive_thread = osip_thread_create(20000, keepalive_working_thread, thread_parameter);
                if(NULL == thread_parameter->keepalive_thread)
                {
                    // to do: handle error
                }
                break;
            case EXOSIP_REGISTRATION_FAILURE:
                printf("registrered failed\n");
            default:
                printf("recieved unknown eXosip event (type, did, cid) = (%d, %d, %d)\n", event->type, event->did, event->cid);
                break;
        }
        eXosip_event_free(event);
    }

    osip_thread_exit();
    return NULL;
}

void* keepalive_working_thread(void* arg)
{
    client_configurations* thread_parameter = (client_configurations*)arg;
    int result = 0;
    osip_message_t* keepalive_message = NULL;
    char* from = NULL;
    char* to = NULL;
    char* message_body = NULL;

    from = osip_malloc(512);
    to = osip_malloc(512);
    message_body = osip_malloc(512);

    if(NULL == from || NULL == to || NULL == message_body)
    {
        osip_free(from);
        osip_free(to);
        osip_free(message_body);
        return NULL;
    }

    snprintf(from, 512, "sip:%s@%s", thread_parameter->client_user_name, thread_parameter->client_IP);

    snprintf(to, 512, "sip:%s@%s", thread_parameter->server_ID, thread_parameter->server_IP);

    result = eXosip_message_build_request(
        thread_parameter->exosip_context,
        &keepalive_message,
        "MESSAGE",
        to,
        from,
        NULL);
    if(OSIP_SUCCESS != result)
    {
        // to do: handle error
        return NULL;
    }

    result = osip_message_set_content_type(keepalive_message, "application/MANSCDP+xml");
    if(OSIP_SUCCESS != result)
    {
        // to do: handle error
        return NULL;
    }

    while(thread_parameter->thread_loop)
    {
        snprintf(message_body, 512, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<Notify>\r\n<CmdType>Keepalive</CmdType>\r\n<SN>%u</SN>\r\n<DeviceID>%s</DeviceID><Status>OK</Status></Notify>", thread_parameter->MANSCDP_SN, thread_parameter->client_user_name);

        result = osip_message_set_body(keepalive_message, message_body, strnlen(message_body, 512));
        if(OSIP_SUCCESS != result)
        {
            // to do: handle error
            return NULL;
        }

        eXosip_lock(thread_parameter->exosip_context);
        result = eXosip_message_send_request(thread_parameter->exosip_context, keepalive_message);
        eXosip_unlock(thread_parameter->exosip_context);
        if(OSIP_SUCCESS != result)
        {
            // to do: handle error
        }

        EnterCriticalSection(&thread_parameter->critical_section);
        thread_parameter->MANSCDP_SN++;
        LeaveCriticalSection(&thread_parameter->critical_section);

        osip_usleep(thread_parameter->heartbeat_interval * 1000 * 1000);
    }

    osip_message_free(keepalive_message);

    osip_free(from);
    osip_free(to);
    osip_free(message_body);

    return NULL;
}