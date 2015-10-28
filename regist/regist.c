/*
/ ---------------------------------------------------------------------------------- -
I am copying here a basic application to send register message.
you could do other messages using similar mechainsm.This will compile and work for register.amar

/ ----------------------------------------------
*/
#include <stdlib.h>
#include <sys/types.h>
#include <WinSock2.h>

#include <osip2/osip.h>

#define MESSAGE_MAX_LENGTH          4000
#define MAX_ADDR_STR                128
#define MESSAGE_ENTRY_MAX_LENGTH    256
#define SIP_PORT                    5060
#define EXPIRES_TIME_INSECS         3600

#define USER_ID     "7929"
#define SIP_PROXY   "sip:192.168.10.67"
#define SIP_FROM    "sip:7929@3402000000"
#define SIP_TO      "sip:7929@3402000000"
#define SIP_CONTACT "sip:7929@3402000000"
#define LOCAL_IP    "192.168.10.29"

osip_t *osip;
int sipSock;

int networkInit()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    struct sockaddr_in address;
    if((sipSock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("networkInit: error opening socket");
        return -1;
    }
    address.sin_family = PF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(SIP_PORT);
    printf("sipSock = %d\n", sipSock);
    if(bind(sipSock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("networkInit: error binding socket");
        return -1;
    }

    return 0;
}

int networkMsgSend(int sock, char *msgP, int msgLen, char *host, int port)
{
    struct sockaddr_in address;
    address.sin_family = PF_INET;
    address.sin_addr.s_addr = inet_addr(host);
    address.sin_port = htons(port);
    if(sendto(sock, msgP, msgLen, 0, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("networkMsgSend: sendto error");
        return -1;
    }
    return 0;
}

int networkMsgRecv(int sock, char *msgP, int msgLen, struct sockaddr_in *address)
{
    int addrLen;
    int dataLen;

    dataLen = recvfrom(sock, msgP, msgLen, 0, (struct sockaddr *)address, &addrLen);
    if(dataLen < 0)
    {
        perror("networkMsgRecv: recvfrom error: %d", WSAGetLastError());
        return -1;
    }
    return dataLen;
}

int SendMsg(osip_transaction_t *tr, osip_message_t *sip, char *host, int port, int out_socket)
{
    int len = 0;
    char *msgP;
    size_t msgLen;
    int i;
    int status;

    printf("SendMsg\n");

    if((i = osip_message_to_str(sip, &msgP, &msgLen)) != 0)
    {
        OSIP_TRACE(osip_trace(__FILE__, __LINE__, OSIP_BUG, NULL, "failed to convert message\n"));
        return -1;
    }
    if(!networkMsgSend(sipSock, msgP, strlen(msgP), host, port))
        OSIP_TRACE(osip_trace(__FILE__, __LINE__, OSIP_INFO1, NULL, "Time: Udp message sent: \n%s\n", msgP));

    return 0;
}


void cb_rcvICTRes(int type, osip_transaction_t *pott, osip_message_t *pomt)
{
    printf("cb_rcvICTRes\n");
}

void cb_rcvNICTRes(int type, osip_transaction_t *pott, osip_message_t *pomt)
{
    printf("cb_rcvNICTRes\n");
}

void cb_rcvreq(int type, osip_transaction_t *pott, osip_message_t *pomt)
{
    printf("cb_rcvreq\n");
}


void setCallbacks(osip_t *osip)
{
    osip_set_cb_send_message(osip, SendMsg);
    osip_set_message_callback(osip, OSIP_ICT_STATUS_1XX_RECEIVED, cb_rcvICTRes);
    osip_set_message_callback(osip, OSIP_NICT_STATUS_1XX_RECEIVED, cb_rcvNICTRes);
    osip_set_message_callback(osip, OSIP_IST_INVITE_RECEIVED, cb_rcvreq);
}

int AddSupportedMethods(osip_message_t *msgPtr)
{
    osip_message_set_allow(msgPtr, "INVITE");
    osip_message_set_allow(msgPtr, "INFO");
    osip_message_set_allow(msgPtr, "ACK");
    osip_message_set_allow(msgPtr, "CANCEL");
    osip_message_set_allow(msgPtr, "BYE");

    return 0;
}

int bSipSend(osip_message_t    *msgPtr, osip_fsm_type_t   transactionType)
{
    int status;
    osip_transaction_t *transactionPtr;
    osip_event_t       *sipeventPtr;

    if((status = osip_transaction_init(&transactionPtr, transactionType, osip, msgPtr)) != 0)
    {
        printf("Failed to init transaction %d", status);
        return -1;
    }

    if((sipeventPtr = osip_new_outgoing_sipmessage(msgPtr)) == NULL)
    {
        printf("Can't allocate message");
        osip_message_free(msgPtr);
        return -1;
    }

    sipeventPtr->transactionid = transactionPtr->transactionid;


    if((status = osip_message_force_update(msgPtr)) != 0)
    {
        printf("Failed force update", status);
        osip_message_free(msgPtr);
        return -1;
    }

    if((status = osip_transaction_add_event(transactionPtr, sipeventPtr)) != 0)
    {
        printf("Can't add event");
        osip_message_free(msgPtr);
        return -1;
    }

    return 0;

}

int bSipRegisterBuild(osip_message_t **regMsgPtrPtr)
{
    static int gSeqNum = 1;
    int status;
    char *callidNumberStr = NULL;
    char *seqNumStr = NULL;
    osip_call_id_t *callidPtr;
    char temp[MESSAGE_ENTRY_MAX_LENGTH];
    char sipPort[MESSAGE_ENTRY_MAX_LENGTH];
    osip_cseq_t *cseqPtr;
    unsigned int number;
    osip_message_t     *regMsgPtr;
    char expires[10];

    if((status = osip_message_init(&regMsgPtr)) != 0)
    {
        OSIP_TRACE(osip_trace(__FILE__, __LINE__, OSIP_BUG, NULL, "Can't init message!\n"));
        return -1;
    }
    osip_message_set_method(regMsgPtr, osip_strdup("REGISTER"));

    osip_uri_init(&(regMsgPtr->req_uri));
    if((status = osip_uri_parse(regMsgPtr->req_uri, SIP_PROXY)) != 0)
    {
        OSIP_TRACE(osip_trace(__FILE__, __LINE__, OSIP_BUG, NULL, "uri parse failed!\n"));
        osip_message_free(regMsgPtr);
        return -1;
    }
    osip_message_set_version(regMsgPtr, osip_strdup("SIP/2.0"));
    osip_message_set_status_code(regMsgPtr, 0);
    osip_message_set_reason_phrase(regMsgPtr, NULL);

    osip_message_set_to(regMsgPtr, SIP_TO);
    osip_message_set_from(regMsgPtr, SIP_FROM);

    if((status = osip_call_id_init(&callidPtr)) != 0)
    {
        OSIP_TRACE(osip_trace(__FILE__, __LINE__, OSIP_BUG, NULL, "call id failed!\n"));
        osip_message_free(regMsgPtr);
        return -1;
    }
    callidNumberStr = (char *)osip_malloc(MAX_ADDR_STR);
    number = osip_build_random_number();
    sprintf(callidNumberStr, "%u", number);
    osip_call_id_set_number(callidPtr, callidNumberStr);

    osip_call_id_set_host(callidPtr, osip_strdup(LOCAL_IP));

    regMsgPtr->call_id = callidPtr;

    if((status = osip_cseq_init(&cseqPtr)) != 0)
    {
        OSIP_TRACE(osip_trace(__FILE__, __LINE__, OSIP_BUG, NULL, "seq init failed!\n"));
        osip_message_free(regMsgPtr);
        return -1;
    }
    gSeqNum++;
    seqNumStr = (char *)osip_malloc(MAX_ADDR_STR);
    sprintf(seqNumStr, "%i", gSeqNum);
    osip_cseq_set_number(cseqPtr, seqNumStr);
    osip_cseq_set_method(cseqPtr, osip_strdup("REGISTER"));
    regMsgPtr->cseq = cseqPtr;

    osip_message_set_max_forwards(regMsgPtr, "70");

    sprintf(sipPort, "%i", SIP_PORT);
    sprintf(temp, "SIP/2.0/%s %s;branch=z9hG4bK%u", "UDP", LOCAL_IP, osip_build_random_number());
    osip_message_set_via(regMsgPtr, temp);

    osip_message_set_contact(regMsgPtr, SIP_CONTACT);
    sprintf(expires, "%i", EXPIRES_TIME_INSECS);
    osip_message_set_expires(regMsgPtr, expires);

    osip_message_set_content_length(regMsgPtr, "0");

    osip_message_set_user_agent(regMsgPtr, "osip");

    AddSupportedMethods(regMsgPtr);
    *regMsgPtrPtr = regMsgPtr;
    return 0;
}

int bSipRegister(void *cookie)
{
    osip_message_t *regMsgPtr;

    if(bSipRegisterBuild(&regMsgPtr) != 0)
    {
        printf("Error building register message!");
        return -1;
    }

    if(bSipSend(regMsgPtr, NICT) != 0)
    {
        printf("Error sending message!");
        return -1;
    }
    return 0;
}

void processSipMsg()
{
    int port;
    char host[256];
    char msg[MESSAGE_MAX_LENGTH];
    int msgLen;
    osip_event_t *sipevent;
    osip_transaction_t *transaction = NULL;
    struct sockaddr_in sa;
    int status;

    if((msgLen = networkMsgRecv(sipSock, msg, MESSAGE_MAX_LENGTH, &sa)) > 0)
    {
        printf("processSipMsg: RECEIVED MSG\n");
        printf("%s\n", msg);
        sipevent = osip_parse(msg, msgLen);
        if((sipevent == NULL) || (sipevent->sip == NULL))
        {
            printf("Could not parse SIP message\n");
            osip_event_free(sipevent);
            return;
        }
    }

    char* IP[20];

    strncpy(IP, inet_ntoa(sa.sin_addr), 20);

    osip_message_fix_last_via_header(sipevent->sip, IP, ntohs(sa.sin_port));
    if((status = osip_find_transaction_and_add_event(osip, sipevent)) != 0)
    {
        printf("New transaction!\n");
        if(MSG_IS_REQUEST(sipevent->sip))
        {
            printf("Got New Request\n");;
        }
        else if(MSG_IS_RESPONSE(sipevent->sip))
        {
            printf("Bad Message:%s\n", msg);
            osip_event_free(sipevent);
        }
        else
        {
            printf("Unsupported message:%s\n", msg);
            osip_event_free(sipevent);
        }
    }
}

int main()
{
    int i, result;
    fd_set readfds;
    struct timeval tv;
    printf("Initializing OSIP\n");
    TRACE_INITIALIZE(END_TRACE_LEVEL, NULL);
    if(networkInit() < 0)
    {
        printf("ERROR Initializing NETWORK\n");
        return -1;
    }
    i = osip_init(&osip);
    if(i != 0)
        return -1;
    printf("Setting Callbacks\n");
    setCallbacks(osip);
    printf("Entering Main loop 1\n");
    OSIP_TRACE(osip_trace(__FILE__, __LINE__, OSIP_BUG, NULL, "Check OSIP_TRACE init\n"));
    bSipRegister("This is Test Cookie");
    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(sipSock, &readfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        result = select(FD_SETSIZE, &readfds, 0, 0, &tv);
        if(result < 0)
        {
            perror("main: select error");
            exit(1);
        }
        if(FD_ISSET(sipSock, &readfds))
        {
            printf("main: Received SIP message\n");
            processSipMsg();
        }
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