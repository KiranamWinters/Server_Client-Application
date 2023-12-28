#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define DEFAULT_PORT "27015"
#define RECVBUFFERSIZE 1024
#define SUCCESS 0

int main(int argc, char* argv[]){
    //-----------------Init Winsock-------------------
    WSADATA wsaData;
    int iWinSockInitResult = WSAStartup((2,2),&wsaData);
    if(iWinSockInitResult != SUCCESS){
        printf("WinSock Initialization Failed\nERROR CODE : %d", iWinSockInitResult);
    }

    //---------------Creating WebSocket-----------------
    struct addrinfo *HostResponseInfo = NULL;
    struct addrinfo *ptr = NULL;

    struct addrinfo SocketInfo;
    ZeroMemory(&SocketInfo, sizeof(SocketInfo));
    SocketInfo.ai_family = AF_INET;                      //ipv4
    //SOCKDGRAM for UDP 
    //SOCKSTREAM for TCP
    SocketInfo.ai_socktype = SOCK_STREAM;
    SocketInfo.ai_protocol = IPPROTO_TCP;
    //[Unsure] : Fills remaning info in addrinfo struct
    int iStructInitResult = getaddrinfo(argv[1], DEFAULT_PORT, &SocketInfo, &HostResponseInfo);
    if(iStructInitResult!= SUCCESS){
        printf("GetAddr Struct Init Failed\nError Code: %d" ,iStructInitResult);
    }
    SOCKET ConnectSocket = INVALID_SOCKET; 
    ptr = HostResponseInfo;
    ConnectSocket = socket(
        ptr->ai_family,
        ptr->ai_socktype, 
        ptr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET) {
    printf("Error at socket(): %ld\n", WSAGetLastError());
    freeaddrinfo(HostResponseInfo);
    WSACleanup();
    }

    //------------------Connect to server-----------------
    int iConnectResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iConnectResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }
    freeaddrinfo(HostResponseInfo);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
    }
    //----------------Send/Recv--------------------------
    const char* sendbuf = "Hello World";
    char recvbuf[RECVBUFFERSIZE];

    int iSendRecvResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);
    if (iSendRecvResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
    };
    printf("Bytes Sent: %d\n",iSendRecvResult);
    iSendRecvResult = shutdown(ConnectSocket, SD_SEND);
    if (iSendRecvResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
    }
    do{
        iSendRecvResult = recv(ConnectSocket, recvbuf, RECVBUFFERSIZE, 0);
        if (iSendRecvResult > 0)
            printf("Bytes received: %d\n", iSendRecvResult);
        else if (iSendRecvResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    }while(iSendRecvResult > 0);

    for(int i = 0; i< 11; i++){
        printf("%c",recvbuf[i]);
    }


    return 0;
}