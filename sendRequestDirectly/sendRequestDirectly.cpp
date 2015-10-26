#include <stdio.h>
#include <WinSock2.h>
int main(int argc, char* argv[])
{
    int result = 0;

    WSADATA wsaData;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in sockSIPServerAddr;
    int length = sizeof(sockaddr_in);
    result = WSAStringToAddressA("192.168.10.29", AF_INET, NULL, (sockaddr*)&sockSIPServerAddr, &length);
    sockSIPServerAddr.sin_port = htons(5060);

    sockaddr_in sockClientAddr;
    sockClientAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    sockClientAddr.sin_family = AF_INET;
    sockClientAddr.sin_port = htons(20004);

    result = bind(sock, (sockaddr*)&sockClientAddr, sizeof(sockaddr_in));

    char* message = (char*)malloc(2048);

    result = sprintf(message, "REGISTER sip:34020000002000000001@3402000000 SIP/2.0\r\nVia: SIP/2.0/UDP 192.168.10.29:5060;rport;branch=z9hG4bK1566165472\r\nFrom: <sip:34020000001320000001@3402000000>;tag=377124889\r\nTo: <sip:34020000001320000001@3402000000>\r\nCall-ID: 1830394338\r\nCSeq: 1 REGISTER\r\nContact: <sip:34020000001320000001@192.168.10.29:5060>\r\nMax-Forwards: 70\r\nUser-Agent: Client\r\nExpires: 3600\r\nContent-Length: 0");

    result = sendto(sock, message, strlen(message), 0, (sockaddr*)&sockSIPServerAddr, sizeof(sockaddr_in));

    memset(message, 0x0, 2048);

    result = recvfrom(sock, message, 2048, 0, (sockaddr*)&sockSIPServerAddr, &length);

    printf("%s\r\n\r\n", message);

    memset(message, 0x0, 2048);

    result = recvfrom(sock, message, 2048, 0, (sockaddr*)&sockSIPServerAddr, &length);

    printf("%s\r\n\r\n", message);


    free(message);

    closesocket(sock);

    result = WSACleanup();
    return 0;
}

