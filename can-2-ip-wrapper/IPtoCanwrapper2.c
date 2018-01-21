#include<sys/types.h>
#include<netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h> //common
#include <linux/can.h>
#include <linux/can/raw.h>
#include "lib.h"
#include <stdint.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <net/if.h>
#include "terminal.h"
#include<netdb.h>
#include <setjmp.h>
#include <arpa/inet.h>
// general params

//static volatile int running = 1;

// close socket params

int sockfd;
int s;


// retrieve params vars

int vflag = 0;
int bflag = 0;
char *cvalue = NULL;
//int index;
int c;
char *candevice = NULL;


// initialize CAN socket

int s; /* can raw socket */
int required_mtu;
int mtu;
int enable_canfd = 1;
struct sockaddr_can addr;
struct canfd_frame frame;
struct ifreq ifr;


// init IP socket


int sockfd, newsockfd, portno;
socklen_t cli;
char buffer[1024];
char interface[6] = "vcan0";
struct sockaddr_in serv_addr, cli_addr;
int n;



// receive and buffer IP params

char * pch;
char dev[1000];
char message[1000];
char sz[1000];
int i=0;











void retrieve_params(int argc, char **argv)
{

	printf(" retrieve params \n");

	while ((c = getopt (argc, argv, "vbc:")) != -1)
		switch (c)
		{
		case 'v':
			vflag = 1;
			break;
		case 'b':
			bflag = 1;
			break;
		case 'c':
			cvalue = optarg;
			break;
		case '?':
			if (optopt == 'c')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr,
						"Unknown option character `\\x%x'.\n",
						optopt);
			return 1;
		default:
			abort ();
		}

}


//
//void init_socket_IP()
//
//{
//	serv_addr.sin_family = AF_INET;
//	serv_addr.sin_addr.s_addr = INADDR_ANY;
//	serv_addr.sin_port = htons(portno);
//	sockfd = socket(AF_INET, SOCK_STREAM, 0);
//	bzero((char *) &serv_addr, sizeof(serv_addr));
//	serv_addr.sin_family = AF_INET;
//			serv_addr.sin_addr.s_addr = INADDR_ANY;
//			serv_addr.sin_port = htons(portno);
//			bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
//			cli = sizeof(cli_addr);
//}
//

//
//void receive_and_buffer_IP_frame(int sockfd)
//
//{
//
//			//	close(sockfd);
//
//
//}


void CAN_SENDER(int sockfd, int s,char *message)


{
	printf(" receive ip buffer  \n");


	printf (" step1  \n ");
	portno = 3333;

	serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
				serv_addr.sin_addr.s_addr = INADDR_ANY;
				serv_addr.sin_port = htons(portno);
				bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
				cli = sizeof(cli_addr);
				printf (" step2  \n ");
	          listen (sockfd, 1);

		//	fd_set set;
	//		struct timeval timeout;
//			//int rv;
//			FD_ZERO(&set); /* clear the set */
//			FD_SET(sockfd, &set); /* add our file descriptor to the set */
//			timeout.tv_sec = 5;
//			timeout.tv_usec = 0;
//			rv = select(sockfd + 1, &set, NULL, NULL, &timeout);
//			if(rv == -1)
//					{
//						perror("select"); /* an error accured */
//						return 1;
//					}
//					else if(rv == 0)
//					{
//						close(sockfd);
//						newsockfd = accept (sockfd,(struct sockaddr *) &cli_addr, &cli);
//						if(vflag==1)
//						{
//							printf("timeout occurred ... closing broken connection and creating a new one \n"); /* a timeout occured */
//						}
//					}
//					else
						newsockfd = accept (sockfd,(struct sockaddr *) &cli_addr, &cli);


						printf (" step3  \n ");

					if(vflag==1)
					{
						printf(" connection established to the server and accepting incoming messages \n"); /* a timeout occured */
					}
					bzero(buffer,1024);
					n = read(newsockfd,buffer,1024);
					if(vflag==1)
					{
						printf(" storing incoming IP messages in a buffer \n"); /* a timeout occured */
					}


					printf (" step4  \n ");



					printf("%s\n",buffer);





					fflush(stdout);
			pch = strtok (buffer," ,.-");


			printf (" step5  \n ");

				while (pch != NULL)
				{
					if (i==2)
					{
						memset(dev, '\0', sizeof(dev));
						strcpy(dev, pch);
					}
					else if (i==4)
					{
						memset(sz, '\0', sizeof(sz));
						strcpy(sz, pch);
					}
					else if (i==6)
					{
						memset(message, '\0', sizeof(message));
						strcpy(message, pch);
					}
					pch = strtok (NULL, " ,:\"");
					i++;
				}

				printf (" step6  \n ");


				n =send(newsockfd,"Your Message Received",21,0);


	printf(" can send 2 \n");
	required_mtu = parse_canframe(/*argv[2]*/ message , &frame);
	if (!required_mtu){
		fprintf(stderr, "\nWrong CAN-frame format! Try:\n\n");
		fprintf(stderr, "    <can_id>#{R|data}          for CAN 2.0 frames\n");
		fprintf(stderr, "    <can_id>##<flags>{data}    for CAN FD frames\n\n");
		fprintf(stderr, "<can_id> can have 3 (SFF) or 8 (EFF) hex chars\n");
		fprintf(stderr, "{data} has 0..8 (0..64 CAN FD) ASCII hex-values (optionally");
		fprintf(stderr, " separated by '.')\n");
		fprintf(stderr, "<flags> a single ASCII Hex value (0 .. F) which defines");
		fprintf(stderr, " canfd_frame.flags\n\n");
		fprintf(stderr, "e.g. 5A1#11.2233.44556677.88 / 123#DEADBEEF / 5AA# / ");
		fprintf(stderr, "123##1 / 213##311\n     1F334455#1122334455667788 / 123#R ");
		fprintf(stderr, "for remote transmission request.\n\n");
		return 1;
	}


	printf(" can send 3 \n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		return 1;
	}


	printf(" can send 4 \n");

	strncpy(ifr.ifr_name,"vcan0", IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
	if (!ifr.ifr_ifindex) {
		perror("if_nametoindex");
		return 1;
	}
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	if (required_mtu > CAN_MTU) {
		/* check if the frame fits into the CAN netdevice */
		if (ioctl(s, SIOCGIFMTU, &ifr) < 0) {
			perror("SIOCGIFMTU");
			return 1;
		}
		mtu = ifr.ifr_mtu;
		if (mtu != CANFD_MTU) {
			printf("CAN interface ist not CAN FD capable - sorry.\n");
			return 1;
		}
		/* interface is ok - try to switch the socket into CAN FD mode */
		if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
				&enable_canfd, sizeof(enable_canfd))){
			printf("error when enabling CAN FD support\n");
			return 1;
		}
		/* ensure discrete CAN FD length values 0..8, 12, 16, 20, 24, 32, 64 */
		frame.len = can_dlc2len(can_len2dlc(frame.len));
	}

	printf(" can send 5 \n");

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}


	printf(" can send 6 \n");

	if (write(s, &frame, required_mtu) != required_mtu) {
		perror("write");
		return 1;
	}
	close(s);
}


void close_IP_CAN_sockets(int s1 ,int s2 )

{


	printf(" close sockets \n");
	                close(s1);
					close(s2);
}



int main(int argc, char **argv) {


	printf(" main \n");
//////////////////////////// retrieve params
retrieve_params(argc,argv);
//////////////////////////// init socket CAN
//init_socket_CAN(argc,argv);
//////////////////////////// init socket IP

//////////////////////////// control loop start
while (1)


	{

///////////////////////// receive and buffer IP frame
//init_socket_IP();
//receive_and_buffer_IP_frame(sockfd);
//////////////////////////// pass buffer to CAN socket and send it

//////////////////////////// control loop end
CAN_SENDER(sockfd,s,message);

	}

//////////////////////////// close sockets
close_IP_CAN_sockets(s,sockfd);
return 0;
}
