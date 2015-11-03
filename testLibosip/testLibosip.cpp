// testLibosip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <osip2\osip_mt.h>
#include <eXosip2\eXosip.h>
#include <WinSock2.h>

#define MEMERROR 1000
#define APIERROR 1001

typedef struct regparam_t
{
    eXosip_t* ctx;
    int regid;
    int expiry;
    int auth;
}regparam_t;

void* register_proc(void* arg)
{
    regparam_t* regparam = static_cast<regparam_t*>(arg);
    int result;

    while(true)
    {
        Sleep((regparam->expiry / 2) * 1000);
        eXosip_lock(regparam->ctx);
        result = eXosip_register_send_register(regparam->ctx, regparam->regid, NULL);
        if(0 > result)
        {
            printf("eXosip_register_send_register error, file: %s, line: %d", __FILE__, __LINE__);
            exit(APIERROR);
        }
        regparam->auth = 0;
        eXosip_unlock(regparam->ctx);
    }

    return NULL;
}

int _tmain(int argc, _TCHAR* argv[])
{
    int result = 0;
    regparam_t regparam = {NULL, 0, 3600, 0};
    osip_message_t* reg = NULL;
    struct osip_thread* register_thread = NULL;
    eXosip_event_t* event = NULL;

    eXosip_t* ctx = eXosip_malloc();
    if(NULL == ctx)
    {
        exit(MEMERROR);
    }

    regparam.ctx = ctx;

    result = eXosip_init(ctx);
    if(0 != result)
    {
        printf("eXosip_init error, file: %s, line: %d", __FILE__, __LINE__);
        exit(APIERROR);
    }

    result = eXosip_listen_addr(ctx, IPPROTO_UDP, NULL, 5060, AF_INET, 0);
    if(0 != result)
    {
        printf("eXosip_listen_addr error, file: %s, line: %d", __FILE__, __LINE__);
        exit(APIERROR);
    }

    eXosip_set_user_agent(ctx, "test client");

    result = eXosip_add_authentication_info(ctx, "4768465", "4768465", "123456", NULL, NULL);
    if(0 != result)
    {
        printf("eXosip_add_authentication_info error, file: %s, line: %d", __FILE__, __LINE__);
        exit(APIERROR);
    }


    regparam.regid = eXosip_register_build_initial_register(ctx, "sip:4768465@192.168.10.29", "sip:34020000002000000001@192.168.10.67", NULL, 3600, &reg);
    if(regparam.regid < 1)
    {
        printf("eXosip_register_build_initial_register error, file: %s, line: %d", __FILE__, __LINE__);
        exit(APIERROR);
    }

    result = eXosip_register_send_register(ctx, regparam.regid, reg);
    if(0 != result)
    {
        printf("eXosip_register_send_register error, file: %s, line: %d", __FILE__, __LINE__);
        exit(APIERROR);
    }

    register_thread = osip_thread_create(20000, register_proc, &regparam);
    if(NULL == register_thread)
    {
        printf("osip_thread_create error, file: %s, line: %d", __FILE__, __LINE__);
        exit(APIERROR);
    }

    while(true)
    {
        event = eXosip_event_wait(ctx, 0, 1);
        if(NULL == event)
        {
            osip_usleep(10000);
            continue;
        }
        eXosip_automatic_action(ctx);
        switch(event->type)
        {
            case EXOSIP_REGISTRATION_SUCCESS:
                printf("registrered successfully");
                break;
            case EXOSIP_REGISTRATION_FAILURE:
                regparam.auth = 1;
            default:
                printf("recieved unknown eXosip event (type, did, cid) = (%d, %d, %d)", event->type, event->did, event->cid);
                break;
        }
        eXosip_event_free(event);
    }

    return 0;
}