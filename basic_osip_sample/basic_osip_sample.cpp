// basic_osip_sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define ENABLE_TRACE
#include "winsock2.h"
#include "osip2/osip.h"
#include <osip2/osip_mt.h>
#include <iostream>
#include <sstream>
#include <assert.h>

#define UNUSED(x) (void(x))

using namespace std;

static SOCKET g_sock;
static const size_t BUFFSIZE = 1600;  // MTU usually doesn't exceed 1600

int cb_SendMsg(osip_transaction_t *tr, osip_message_t *sip, char *, int port, int out_socket)
{
    UNUSED(out_socket);
    UNUSED(tr);
    UNUSED(port);
    char *msgP;
    size_t msgLen;
    osip_message_to_str(sip, &msgP, &msgLen);

    stringstream ss;
    ss << tr->topvia->host << ':' << tr->topvia->port;
    struct sockaddr addr;
    int addrSize = sizeof(struct sockaddr);
    WSAStringToAddressA(const_cast<LPSTR>(ss.str().c_str()), AF_INET, NULL, &addr, &addrSize);

    sendto(out_socket, msgP, msgLen, 0, &addr, addrSize);
    return 0;
}

int BuildResponse(const osip_message_t *request, osip_message_t **response)
{
    osip_message_t *msg = NULL;
    osip_message_init(&msg);

    osip_from_clone(request->from, &msg->from);
    osip_to_clone(request->to, &msg->to);
    osip_cseq_clone(request->cseq, &msg->cseq);
    osip_call_id_clone(request->call_id, &msg->call_id);

    int pos = 0;//copy vias from request to response
    while(!osip_list_eol(&request->vias, pos))
    {
        osip_via_t *via;
        osip_via_t *via2;

        via = (osip_via_t *)osip_list_get(&request->vias, pos);
        int i = osip_via_clone(via, &via2);
        if(i != 0)
        {
            osip_message_free(msg);
            return i;
        }
        osip_list_add(&(msg->vias), via2, -1);
        pos++;
    }

    osip_to_set_tag(msg->to, osip_strdup("4893693")); // set to tag in response
    osip_message_set_version(msg, osip_strdup("SIP/2.0"));
    osip_message_set_contact(msg, osip_strdup("sip:raymond@127.0.0.1"));
    osip_message_set_user_agent(msg, osip_strdup("Linphone/3.2.1 (eXosip2/3.3.0)"));
    *response = msg;
    return 0;
}

void cb_RcvICTRes(int, osip_transaction_t *, osip_message_t *)
{
    OutputDebugString(TEXT("cb_RcvICTRes fired\r\n"));
}

void cb_RcvNICTRes(int, osip_transaction_t *, osip_message_t *)
{
    OutputDebugString(TEXT("cb_RcvNICTRes fired\r\n"));
}

void cb_ISTTranKill(int, osip_transaction_t *)
{
    OutputDebugString(TEXT("cb_ISTTranKill fired\r\n"));
}

void* Notify(void* arg)
{
    osip_transaction_t *tran = static_cast<osip_transaction_t*>(arg);

    osip_message_t *response = NULL;
    osip_event_t *evt = NULL;
    BuildResponse(tran->orig_request, &response);

    cout << "incoming call from " << tran->from->url << endl << " [a]nswer or [d]ecline?" << endl;
    char act = 'd';
    cin >> act;
    if('a' == act)
    {//accept call
        osip_message_set_status_code(response, SIP_OK);
        osip_message_set_reason_phrase(response, osip_strdup("OK"));
        const char* mime = "application/sdp";
        osip_message_set_body_mime(response, mime, strlen(mime));
        osip_message_set_content_type(response, mime);
        const char* sdp = "v=0\r\n\
                          o=raymond 323456 654323 IN IP4 127.0.0.1\r\n\
                          s=A conversation\r\n\
                          c=IN IP4 127.0.0.1\r\n\
                          t=0 0\r\n\
                          m=audio 7078 RTP/AVP 111 110 0 8 101\r\n\
                          a=rtpmap:111 speex/16000/1\r\n\
                          a=rtpmap:110 speex/8000/1\r\n\
                          a=rtpmap:0 PCMU/8000/1\r\n\
                          a=rtpmap:8 PCMA/8000/1\r\n\
                          a=rtpmap:101 telephone-event/8000\r\n\
                          a=fmtp:101 0-11\r\n";

        osip_message_set_body(response, osip_strdup(sdp), strlen(sdp));
    }
    else
    {//decline call
        osip_message_set_status_code(response, SIP_DECLINE);
        osip_message_set_reason_phrase(response, osip_strdup("Decline"));
        osip_message_set_contact(response, NULL);
    }

    evt = osip_new_outgoing_sipmessage(response);
    evt->transactionid = tran->transactionid;
    osip_transaction_add_event(tran, evt);
    return NULL;
}

void cb_RcvISTReq(int, osip_transaction_t *tran, osip_message_t *msg)
{
    osip_message_t *response = NULL;
    osip_event_t *evt = NULL;

    BuildResponse(msg, &response);//trying
    osip_message_set_status_code(response, SIP_TRYING);
    evt = osip_new_outgoing_sipmessage(response);
    osip_message_set_reason_phrase(response, osip_strdup("Trying"));
    osip_transaction_add_event(tran, evt);

    BuildResponse(msg, &response);//dialog establishement
    osip_message_set_status_code(response, 101);
    evt = osip_new_outgoing_sipmessage(response);
    osip_message_set_reason_phrase(response, osip_strdup("Dialog Establishement"));
    osip_transaction_add_event(tran, evt);

    BuildResponse(msg, &response);//ringing
    osip_message_set_status_code(response, SIP_RINGING);
    evt = osip_new_outgoing_sipmessage(response);
    osip_message_set_reason_phrase(response, osip_strdup("Ringing"));
    osip_transaction_add_event(tran, evt);

    osip_thread_create(0, Notify, tran);// start another thread to notify user the incoming call
}

void SetCallbacks(osip_t *osip)
{
    osip_set_cb_send_message(osip, cb_SendMsg);
    osip_set_message_callback(osip, OSIP_ICT_STATUS_1XX_RECEIVED, cb_RcvICTRes);
    osip_set_message_callback(osip, OSIP_NICT_STATUS_1XX_RECEIVED, cb_RcvNICTRes);
    osip_set_message_callback(osip, OSIP_IST_INVITE_RECEIVED, cb_RcvISTReq);
    osip_set_kill_transaction_callback(osip, OSIP_IST_KILL_TRANSACTION, cb_ISTTranKill);
}

SOCKET InitNet()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    TCHAR ip[] = TEXT("192.168.10.29:5060");
    struct sockaddr addr;
    int addrSize = sizeof(struct sockaddr);
    WSAStringToAddress(ip, AF_INET, NULL, &addr, &addrSize);
    bind(sock, &addr, addrSize);
    return sock;
}

void ProcessNewReq(osip_t* osip, osip_event_t *evt)
{
    osip_transaction_t *tran;
    osip_transaction_init(&tran, IST, osip, evt->sip);
    //osip_transaction_set_in_socket (tran, socket);
    osip_transaction_set_out_socket(tran, g_sock);
    osip_transaction_set_your_instance(tran, osip);// store osip in transaction for later usage
    osip_transaction_add_event(tran, evt);
}

void* TransportFun(void* arg)
{
    int rc;
    osip_t* osip = static_cast<osip_t*>(arg);

    OutputDebugString(TEXT("initialize network\r\n"));
    g_sock = InitNet();
    assert(0 < g_sock);
    char buf[BUFFSIZE];
    while(true)
    {
        struct sockaddr from;
        int addrSize = sizeof(struct sockaddr);
        int len = recvfrom(g_sock, buf, BUFFSIZE, 0, &from, &addrSize);
        if(len < 1)
            continue;
        buf[len] = 0;
        WCHAR addrBuf[1024];
        DWORD addrBufLen;
        WSAAddressToString(&from, addrSize, NULL, addrBuf, &addrBufLen);

        osip_event_t *evt = osip_parse(buf, len);
        rc = osip_find_transaction_and_add_event(osip, evt);
        if(0 != rc)
        {
            OutputDebugString(TEXT("this event has no transaction, create a new one.\r\n"));
            ProcessNewReq(osip, evt);
        }
    }
    return NULL;
}

int main(int argc, _TCHAR* argv[])
{
    UNUSED(argc); UNUSED(argv);
    osip_t* osip = NULL;
    osip_init(&osip);
    SetCallbacks(osip);
    osip_thread_create(0, TransportFun, osip);
    while(true)
    {
        osip_ict_execute(osip);
        osip_ist_execute(osip);
        osip_nict_execute(osip);
        osip_nist_execute(osip);
        osip_timers_ict_execute(osip);
        osip_timers_ist_execute(osip);
        osip_timers_nict_execute(osip);
        osip_timers_nist_execute(osip);
    }
    return 0;
}