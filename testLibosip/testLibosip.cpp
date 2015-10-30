// testLibosip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <eXosip2\eXosip.h>
#include <WinSock2.h>

int _tmain(int argc, _TCHAR* argv[])
{
    int result = 0;
    int registerID = 0;
    eXosip_t *ctx = eXosip_malloc();
    osip_message_t* p_osip_register_message = NULL;
    if(NULL != ctx)
    {
        result = eXosip_init(ctx);
        if(0 == result)
        {
            result = eXosip_listen_addr(ctx, IPPROTO_UDP, NULL, 5060, AF_INET, 0);
            if(0 == result)
            {
                eXosip_lock(ctx);

                registerID = eXosip_register_build_initial_register(ctx, "sip:7968435@192.168.10.29", "sip:34020000002000000001@192.168.10.67", NULL, 3600, &p_osip_register_message);
                if(registerID>=0)
                {
                    result = eXosip_register_send_register(ctx, registerID, p_osip_register_message);
                }
                eXosip_unlock(ctx);
            }
        }
        eXosip_quit(ctx);
    }
    osip_free(ctx);
    return 0;
}