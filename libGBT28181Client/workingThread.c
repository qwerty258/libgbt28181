#include "workingThread.h"
#include "clientConfigDefine.h"
#include <Windows.h>

void* register_thread(void* arg)
{
    client_configurations* thread_parameter = (client_configurations*)arg;
    int result;

    while(thread_parameter->thread_loop)
    {
        Sleep((thread_parameter->expiration_interval - 10) * 1000);
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

void* event_thread(void* arg)
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