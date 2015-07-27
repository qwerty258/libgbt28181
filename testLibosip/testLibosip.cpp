// testLibosip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <osip2\osip.h>

int _tmain(int argc, _TCHAR* argv[])
{
    osip_t* p_osip_t;

    if(0 != osip_init(&p_osip_t))
    {
        return -1;
    }

    osip_free(p_osip_t);

    return 0;
}

