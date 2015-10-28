// testLibosip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <concurrent_queue.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <osip2\osip.h>
#include <iostream>
using namespace std;

typedef struct container_received_data
{
    char data[1600];
    int data_lenght;
}received_data;

typedef struct container_listening_thread_parameter
{
    SOCKET socket_server;
    bool loop;
    osip_t* p_osip_t;
}listening_thread_parameter;

Concurrency::concurrent_queue<received_data*> received_data_queue;

void osip_nist_register_received(int type, osip_transaction_t*, osip_message_t*);

DWORD WINAPI listening_thread(LPVOID lpParam)
{
    listening_thread_parameter* p_listening_thread_parameter = static_cast<listening_thread_parameter*>(lpParam);
    received_data* p_received_data_temp = new received_data;
    sockaddr_in client_sockaddr;

    int size;

    osip_event_t* p_osip_event_t;
    osip_transaction_t* p_osip_transaction_t;

    int result;

    while(p_listening_thread_parameter->loop)
    {
        memset(p_received_data_temp, 0x0, sizeof(received_data));
        size = sizeof(sockaddr_in);

        p_received_data_temp->data_lenght = recvfrom(p_listening_thread_parameter->socket_server, p_received_data_temp->data, 1600, 0, (sockaddr*)&client_sockaddr, &size);

        if(0 != p_received_data_temp->data_lenght)
        {
            cout << p_received_data_temp->data << endl;
            p_osip_event_t = osip_parse(p_received_data_temp->data, p_received_data_temp->data_lenght);
            if(NULL == p_osip_event_t)
            {
                continue;
            }
            result = osip_find_transaction_and_add_event(p_listening_thread_parameter->p_osip_t, p_osip_event_t);
            if(0 != result)
            {
                p_osip_transaction_t = osip_create_transaction(p_listening_thread_parameter->p_osip_t, p_osip_event_t);
                if(NULL == p_osip_transaction_t)
                {
                    continue;
                }
                //osip_transaction_set_in_socket (tran, socket);
                osip_transaction_set_out_socket(p_osip_transaction_t, p_listening_thread_parameter->socket_server);
                osip_transaction_set_your_instance(p_osip_transaction_t, p_listening_thread_parameter->p_osip_t);// store osip in transaction for later usage
                osip_transaction_add_event(p_osip_transaction_t, p_osip_event_t);
            }
        }
    }

    delete p_received_data_temp;

    return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    osip_t* p_osip_t;

    if(0 != osip_init(&p_osip_t))
    {
        exit(-1);
    }

    // callback called when a SIP message must be sent.
    osip_set_cb_send_message(p_osip_t, NULL);
    // callback called when a SIP transaction is TERMINATED.
    osip_set_kill_transaction_callback(p_osip_t, OSIP_ICT_KILL_TRANSACTION, NULL);
    osip_set_kill_transaction_callback(p_osip_t, OSIP_NIST_KILL_TRANSACTION, NULL);
    osip_set_kill_transaction_callback(p_osip_t, OSIP_NICT_KILL_TRANSACTION, NULL);
    osip_set_kill_transaction_callback(p_osip_t, OSIP_NIST_KILL_TRANSACTION, NULL);
    // callback called when the callback to send message have failed.
    osip_set_transport_error_callback(p_osip_t, OSIP_ICT_TRANSPORT_ERROR, NULL);
    osip_set_transport_error_callback(p_osip_t, OSIP_IST_TRANSPORT_ERROR, NULL);
    osip_set_transport_error_callback(p_osip_t, OSIP_NICT_TRANSPORT_ERROR, NULL);
    osip_set_transport_error_callback(p_osip_t, OSIP_NIST_TRANSPORT_ERROR, NULL);
    // callback called when a received answer has been accepted by the transaction.
    osip_set_message_callback(p_osip_t, OSIP_ICT_STATUS_1XX_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_ICT_STATUS_2XX_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_ICT_STATUS_3XX_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_ICT_STATUS_4XX_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_ICT_STATUS_5XX_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_ICT_STATUS_6XX_RECEIVED, NULL);
    // callback called when a received answer has been accepted by the transaction.
    osip_set_message_callback(p_osip_t, OSIP_NICT_STATUS_1XX_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NICT_STATUS_2XX_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NICT_STATUS_3XX_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NICT_STATUS_4XX_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NICT_STATUS_5XX_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NICT_STATUS_6XX_RECEIVED, NULL);
    // callback called when a received request has been accepted by the transaction.
    osip_set_message_callback(p_osip_t, OSIP_IST_INVITE_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_IST_ACK_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NIST_REGISTER_RECEIVED, osip_nist_register_received);
    osip_set_message_callback(p_osip_t, OSIP_NIST_BYE_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NIST_CANCEL_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NIST_INFO_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NIST_OPTIONS_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NIST_SUBSCRIBE_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NIST_NOTIFY_RECEIVED, NULL);
    osip_set_message_callback(p_osip_t, OSIP_NIST_UNKNOWN_REQUEST_RECEIVED, NULL);

    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;

    if(0 != WSAStartup(wVersionRequested, &wsaData))
    {
        cout << "WSAStartup error" << endl;
        exit(-1);
    }

    SOCKET socket_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(INVALID_SOCKET == socket_server)
    {
        cout << "get socket error" << endl;
        exit(-1);
    }

    sockaddr_in sockaddr_server;
    sockaddr_server.sin_family = AF_INET;
    sockaddr_server.sin_port = htons(5060);
    sockaddr_server.sin_addr.S_un.S_addr = inet_addr("192.168.10.29");

    if(0 != bind(socket_server, (sockaddr*)&sockaddr_server, sizeof(sockaddr_in)))
    {
        cout << "bind error" << endl;
        exit(-1);
    }

    listening_thread_parameter listening_thread_parameter_t;

    listening_thread_parameter_t.loop = true;
    listening_thread_parameter_t.socket_server = socket_server;
    listening_thread_parameter_t.p_osip_t = p_osip_t;

    DWORD threadID;
    HANDLE hThread = CreateThread(NULL, 0, listening_thread, &listening_thread_parameter_t, 0, &threadID);

    while(true)
    {
        osip_ict_execute(p_osip_t);
        osip_ist_execute(p_osip_t);
        osip_nict_execute(p_osip_t);
        osip_nist_execute(p_osip_t);
        osip_timers_ict_execute(p_osip_t);
        osip_timers_ist_execute(p_osip_t);
        osip_timers_nict_execute(p_osip_t);
        osip_timers_nist_execute(p_osip_t);
    }

    WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);

    closesocket(socket_server);

    if(0 != WSACleanup())
    {
        cout << "WSACleanup error" << endl;
        exit(-1);
    }

    osip_free(p_osip_t);

    return 0;
}

void osip_nist_register_received(int type, osip_transaction_t* p_osip_transaction_t, osip_message_t* p_osip_message_t)
{
    cout << "osip_nist_register_received" << endl;
}

int cb_send_message(osip_transaction_t *p_osip_transaction_t, osip_message_t *p_osip_message_t, char *p_char, int a, int b)
{
    osip_transaction_t* p_osip_transaction_t_temp = p_osip_transaction_t;
    osip_message_t* p_osip_message_t_temp = p_osip_message_t;
    char* p_char_temp = p_char;
    int a_temp = a;
    int b_temp = b;
    return 0;
}