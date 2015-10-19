#include <stdio.h>
#include <WinSock2.h>
int main(int argc, char* argv[])
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in sockSIPServerAddr;
    int length = sizeof(sockaddr_in);
    WSAStringToAddressA("192.168.10.29", AF_INET, NULL, (sockaddr*)&sockSIPServerAddr, &length);
    sockSIPServerAddr.sin_port = htons(5060);

    sockaddr_in sockClientAddr;
    sockClientAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    sockClientAddr.sin_family = AF_INET;
    sockClientAddr.sin_port = htons(20004);

    bind(sock, (sockaddr*)&sockClientAddr, sizeof(sockaddr_in));

    char* message = (char*)malloc(2048);

    sprintf(message, "MESSAGE sip:34020000001320000001@3402000000 SIP/2.0\r\nVia: SIP/2.0/UDP 192.168.10.29:20004;rport;branch=z9hG4bK2976927360\r\nFrom: <sip:34020000002000000009@192.168.10.29>;tag=4644278517\r\nTo: <sip:34020000001320000001@192.168.10.181>\r\nCall-ID:232151665230564\r\nCSeq:1 MESSAGE\r\nMax-Forwards: 70\r\nContact:<sip:34020000002000000009@192.168.10.29>\r\nContent-Type: Application/MANSCDP+xml\r\nContent-Length: 128\r\n\r\n<?xml version=\"1.0\"?><Query><CmdType>DeviceInfo</CmdType><SN>2</SN><DeviceID>34020000001320000001</DeviceID></Query>");

    sendto(sock, message, strlen(message), 0, (sockaddr*)&sockSIPServerAddr, sizeof(sockaddr_in));

    memset(message, 0x0, 2048);

    recvfrom(sock, message, 2048, 0, (sockaddr*)&sockSIPServerAddr, &length);

    printf("%s\r\n\r\n", message);

    memset(message, 0x0, 2048);

    recvfrom(sock, message, 2048, 0, (sockaddr*)&sockSIPServerAddr, &length);

    printf("%s\r\n\r\n", message);


    free(message);

    closesocket(sock);

    WSACleanup();
    return 0;
}

