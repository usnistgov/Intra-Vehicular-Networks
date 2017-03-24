// TCP server program tcp_server.cc
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
 
using namespace std;
 
int main() {
int sockfd, newsockfd, portno;
socklen_t cli;
char buffer[256];
struct sockaddr_in serv_addr, cli_addr;
int n;
sockfd = socket(AF_INET, SOCK_STREAM, 0);
bzero((char *) &serv_addr, sizeof(serv_addr));
portno = 3333;
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port = htons(portno);
bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
listen(sockfd,1);
cli = sizeof(cli_addr);
newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli);
bzero(buffer,256);
n = read(newsockfd,buffer,255);
cout<<"Message Received from Client : "<<buffer<<"\n\n";
n=send(newsockfd,"Your Message Received",21,0);
close(newsockfd);
close(sockfd);
return 0;
}

