#include<sys/types.h>
#include<netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
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

static volatile int running = 1;
jmp_buf jbuf;


#include <arpa/inet.h>

void sigterm1(int signum)
{
	running = 0;
	printf("Caught signal OTHER1%d\n",signum);

}

void sigterm2(int signum)
{
	running = 0;
	printf("Caught signal OTHER2 %d\n",signum);

}

void sigterm3(int signum)
{

	if(signum == SIGINT)
	{
		printf("Program restarting\n");
		siglongjmp(jbuf,5);
	}
	return;
}



void signal_callback_handler(int signum){

	printf("Caught signal SIGPIPE %d\n",signum);
}



int main(int argc, char *argv[])
{
	int vflag = 0;
	int bflag = 0;
	char *cvalue = NULL;
	int index;
	int c;

	opterr = 0;

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
	printf ("aflag = %d, bflag = %d, cvalue = %s\n",
			vflag, bflag, cvalue);
	for (index = optind; index < argc; index++)
		printf ("Non-option argument %s\n", argv[index]);
	signal(SIGTERM, sigterm1);
	signal(SIGHUP, sigterm2);
	signal(SIGINT, sigterm3);
	signal(SIGPIPE, signal_callback_handler);

	int s; /* can raw socket */
		int required_mtu;
		int mtu;
		int enable_canfd = 1;
		struct sockaddr_can addr;
		struct canfd_frame frame;
		struct ifreq ifr;

		int sockfd, newsockfd, portno;
		socklen_t cli;
		char buffer[1024];
		char interface[6] = "vcan0";
		struct sockaddr_in serv_addr, cli_addr;
		int n;
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		bzero((char *) &serv_addr, sizeof(serv_addr));
		portno = 3333;
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portno);
		bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
			cli = sizeof(cli_addr);
		while (1)


		{
		listen (sockfd, 1);


		newsockfd = accept (sockfd,(struct sockaddr *) &cli_addr, &cli);

		bzero(buffer,1024);
		n = read(newsockfd,buffer,1024);

		printf("%s\n",buffer);
		fflush(stdout);
		char * pch;
		char dev[1000];
		char message[1000];
		char sz[1000];

		pch = strtok (buffer," ,.-");
		int i=0;
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
		n =send(newsockfd,"Your Message Received",21,0);
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
		if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
			perror("socket");
			return 1;
		}
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
		setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
		if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("bind");
			return 1;
		}
		if (write(s, &frame, required_mtu) != required_mtu) {
			perror("write");
			return 1;
		}
	}
				close(sockfd);
				close(s);
	return 0;
}
