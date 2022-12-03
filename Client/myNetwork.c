#include <stdio.h>
#include <winsock2.h>

struct sockaddr_in server;
int Index = 0;

int init(int port)
{
    WSADATA wsadata;
    int wsaStartUp = WSAStartup(MAKEWORD(2 , 2) , &wsadata);
    if(wsaStartUp!=0)
    {
        printf("WSAStartUp Failed");
        return -1;

    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(port);
    return 0;
}
char *buffer;
char *Send(char *data)
{
   // strcat(data , "\n"); I Deleted This Line Because My Server Does Not Need This.
    buffer = malloc(1000000000);
    memset(buffer , 0 , 1000000000);

    int clientSocket = socket(AF_INET , SOCK_STREAM , 0);
    if(clientSocket == INVALID_SOCKET)
    {
        return buffer;
    }

    int can_connect  = connect(clientSocket , (struct sockaddr_in *)&server , sizeof(server));

    if(can_connect !=0)
    {

        return buffer;
    }

    send(clientSocket , data , strlen(data) , 0 );
    recv(clientSocket , buffer , 99999999 , 0);
    closesocket(clientSocket);


    return buffer;

}

