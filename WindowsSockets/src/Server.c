#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define DEFAULT_PORT "27015"
#define SUCCESS 0
#define DATABUFFERSIZE 1024


int main(){
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
    SocketInfo.ai_flags = AI_PASSIVE;
    //[Unsure] : Fills remaning info in addrinfo struct
    int iStructInitResult = getaddrinfo(NULL, DEFAULT_PORT, &SocketInfo, &HostResponseInfo);
    if(iStructInitResult!= SUCCESS){
        printf("GetAddr Struct Init Failed\nError Code: %d" ,iStructInitResult);
    }

    //Listen for client connections
    SOCKET ListenSocket = INVALID_SOCKET;               //Socket that listens for connection
    ListenSocket = socket(
        HostResponseInfo->ai_family,
        HostResponseInfo->ai_socktype,
        HostResponseInfo->ai_protocol);

    //Error Checks
    if(ListenSocket == INVALID_SOCKET){
        printf("Socket Error\nERROR CODE: %d", WSAGetLastError());
        freeaddrinfo(HostResponseInfo);
        WSACleanup();
    }

    //-----------------------Binding Socket-----------------
    int iBindResult = bind(
        ListenSocket,
        HostResponseInfo->ai_addr,
        (int)HostResponseInfo->ai_addrlen);
    if(iBindResult == SOCKET_ERROR){
        printf("Binding Failed\nERROR CODE: %d",WSAGetLastError());
        freeaddrinfo(HostResponseInfo);
        closesocket(ListenSocket);
        WSACleanup();
    }
    //Once Binded we no longer require HostResponseInfo
    freeaddrinfo(HostResponseInfo);

    //------------------------------Listen----------------------
    if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
        printf( "Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(ListenSocket);
        WSACleanup();
    }
    //--------------------------Accept-------------------------
    SOCKET  ClientSocket;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET){
        printf("Socket Connection Accept Failed\nERROR CODE: %d", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
    }

    //-------------------------Sending/Recieving Data----------------
    char RecvDataBuffer[DATABUFFERSIZE];
    int iSendResult;
    int iRecvResult;
    do{
        iRecvResult = recv(ClientSocket, RecvDataBuffer, DATABUFFERSIZE, 0);
        if(iRecvResult > 0){
            printf("Bytes Received: %d\n", iRecvResult);


        //Echo Server Code
            iSendResult = send(ClientSocket,RecvDataBuffer, DATABUFFERSIZE, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        } 

        else if (iRecvResult == 0){
            printf("Connection closing...\n");
        }

        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while(iRecvResult > 0);
    for(int i = 0; i< 11; i++){
        printf("%c",RecvDataBuffer[i]);
    }

    //----------------Disconnecting-----------------
    int iDisconnectResult = shutdown(ClientSocket, SD_SEND);
    if(iDisconnectResult == SOCKET_ERROR ){
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
