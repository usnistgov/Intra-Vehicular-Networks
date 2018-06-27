/*
 * cangen.c - CAN frames generator for testing purposes
 *
 * Copyright (c) 2002-2007 Volkswagen Group Electronic Research
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Volkswagen nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * Alternatively, provided that this notice is retained in full, this
 * software may be distributed under the terms of the GNU General
 * Public License ("GPL") version 2, in which case the provisions of the
 * GPL apply INSTEAD OF those given above.
 *
 * The provided data structures and external interfaces from this code
 * are not restricted to be used by modules with a GPL compatible license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * Send feedback to <linux-can@vger.kernel.org>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "terminal.h"
#include "lib.h"
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>

// general methods

static volatile int running = 1;
fd_set rdfs;
int idx;
//retrieve_params variables

int vflag = 0;
unsigned char logfrmt = 0;
char host[100];
char *candevice = NULL;
#define MAXSOCK 32

// INIT_CANSOCKET_variables

int currmax, numfilter;
extern int optind, opterr, optopt;
#define MAXSOCK 32
char *ptr, *nptr;
int nbytes, i;
int s[MAXSOCK];
static char *cmdlinename[MAXSOCK];
static int max_devname_len;
struct sockaddr_can addr;
struct ifreq ifr;
#define ANYDEV "any"
int currmax, numfilter;
struct can_filter *rfilter;
can_err_mask_t err_mask;
int join_filter;
const int canfd_on = 1;
int rcvbuf_size = 0;
unsigned char timestamp = 0;
unsigned char log = 0;
unsigned char dropmonitor = 0;
struct iovec iov;
struct msghdr msg;
struct canfd_frame frame;
char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32 ))];
// INIT IP SOCKET VARIABLES


int sockfd, portno/*, n*/;
struct sockaddr_in serv_addr;
struct hostent *server;


// receive and buffer can message variables

int maxdlen;
char json[1024];

#define MAXIFNAMES 30
static int dindex[MAXIFNAMES];
static char devname[MAXIFNAMES][IFNAMSIZ + 1];
//



void retrieve_params(int argc, char *argv[])
{

	int index;
	int opt;
	opterr = 0;

	while ((opt = getopt(argc, argv, "vLh:")) != -1)

		switch (opt) {
		case 'v':
			vflag = 1;
			break;
		case 'h':
			memset(host, '\0', sizeof(host));
			strcpy(host, optarg);
			break;
		case 'L':
			logfrmt = 1;
			break;
		default:
			abort ();
		}



}

void init_socket_CAN(int argc, char **argv)
{

	if(vflag==1)
			{
				printf (" initialize CAN socket  \n ");
			}


	currmax = argc - optind; /* find real number of CAN devices */

	if (currmax > MAXSOCK) {
		fprintf(stderr, "More than %d CAN devices given on commandline!\n",
				MAXSOCK);
		return 1;
	}

/////////////////// reading parameters

	for (i = 0; i < currmax; i++) {

		ptr = argv[optind + i];
		nptr = strchr(ptr, ',');
#ifdef DEBUG
		printf("open %d '%s'.\n", i, ptr);
#endif
		s[i] = socket(PF_CAN, SOCK_RAW, CAN_RAW);
		if (s[i] < 0) {
			perror("socket");
			return 1;
		}
		cmdlinename[i] = ptr; /* save pointer to cmdline name of this socket */
		if (nptr)
			nbytes = nptr - ptr; /* interface name is up the first ',' */
		else
			nbytes = strlen(ptr); /* no ',' found => no filter definitions */
		if (nbytes >= IFNAMSIZ) {
			fprintf(stderr, "name of CAN device '%s' is too long!\n", ptr);
			return 1;
		}
		if (nbytes > max_devname_len)
			max_devname_len = nbytes; /* for nice printing */
		addr.can_family = AF_CAN;
		memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
		strncpy(ifr.ifr_name, ptr, nbytes);
#ifdef DEBUG
		printf("using interface name '%s'.\n", ifr.ifr_name);
#endif
		if (strcmp(ANYDEV, ifr.ifr_name)) {
			if (ioctl(s[i], SIOCGIFINDEX, &ifr) < 0) {
				perror("SIOCGIFINDEX");
				exit(1);
			}
			addr.can_ifindex = ifr.ifr_ifindex;
		} else
			addr.can_ifindex = 0; /* any can interface */
		if (nptr) {
			numfilter = 0;
			ptr = nptr;
			while (ptr) {
				numfilter++;
				ptr++; /* hop behind the ',' */
				ptr = strchr(ptr, ','); /* exit condition */
			}
			rfilter = malloc(sizeof(struct can_filter) * numfilter);
			if (!rfilter) {
				fprintf(stderr, "Failed to create filter space!\n");
				return 1;
			}
			numfilter = 0;
			err_mask = 0;
			join_filter = 0;
			while (nptr) {
				ptr = nptr + 1; /* hop behind the ',' */
				nptr = strchr(ptr, ','); /* update exit condition */
				if (sscanf(ptr, "%x:%x", &rfilter[numfilter].can_id,
						&rfilter[numfilter].can_mask) == 2) {
					rfilter[numfilter].can_mask &= ~CAN_ERR_FLAG;
					numfilter++;
				} else if (sscanf(ptr, "%x~%x", &rfilter[numfilter].can_id,
						&rfilter[numfilter].can_mask) == 2) {
					rfilter[numfilter].can_id |= CAN_INV_FILTER;
					rfilter[numfilter].can_mask &= ~CAN_ERR_FLAG;
					numfilter++;
				} else if (*ptr == 'j' || *ptr == 'J') {
					join_filter = 1;
				} else if (sscanf(ptr, "#%x", &err_mask) != 1) {
					fprintf(stderr, "Error in filter option parsing: '%s'\n",
							ptr);
					return 1;
				}
			}
			if (err_mask)
				setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask,
						sizeof(err_mask));
			if (join_filter
					&& setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_JOIN_FILTERS,
							&join_filter, sizeof(join_filter)) < 0) {
				perror(
						"setsockopt CAN_RAW_JOIN_FILTERS not supported by your Linux Kernel");
				return 1;
			}
			if (numfilter)
				setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_FILTER, rfilter,
						numfilter * sizeof(struct can_filter));
			free(rfilter);
		} /* if (nptr) */
		/* try to switch the socket into CAN FD mode */
		setsockopt(s[i], SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on,
				sizeof(canfd_on));
		if (rcvbuf_size) {
			int curr_rcvbuf_size;
			socklen_t curr_rcvbuf_size_len = sizeof(curr_rcvbuf_size);
			/* try SO_RCVBUFFORCE first, if we run with CAP_NET_ADMIN */
			if (setsockopt(s[i], SOL_SOCKET, SO_RCVBUFFORCE, &rcvbuf_size,
					sizeof(rcvbuf_size)) < 0) {
#ifdef DEBUG
				printf("SO_RCVBUFFORCE failed so try SO_RCVBUF ...\n");
#endif
				if (setsockopt(s[i], SOL_SOCKET, SO_RCVBUF, &rcvbuf_size,
						sizeof(rcvbuf_size)) < 0) {
					perror("setsockopt SO_RCVBUF");
					return 1;
				}
				if (getsockopt(s[i], SOL_SOCKET, SO_RCVBUF, &curr_rcvbuf_size,
						&curr_rcvbuf_size_len) < 0) {
					perror("getsockopt SO_RCVBUF");
					return 1;
				}
				if (!i && curr_rcvbuf_size < rcvbuf_size * 2)
					fprintf(stderr, "The socket receive buffer size was "
							"adjusted due to /proc/sys/net/core/rmem_max.\n");
			}
		}
		if (timestamp || log || logfrmt) {
			const int timestamp_on = 1;
			if (setsockopt(s[i], SOL_SOCKET, SO_TIMESTAMP, &timestamp_on,
					sizeof(timestamp_on)) < 0) {
				perror("setsockopt SO_TIMESTAMP");
				return 1;
			}
		}
		if (dropmonitor) {
			const int dropmonitor_on = 1;
			if (setsockopt(s[i], SOL_SOCKET, SO_RXQ_OVFL, &dropmonitor_on,
					sizeof(dropmonitor_on)) < 0) {
				perror(
						"setsockopt SO_RXQ_OVFL not supported by your Linux Kernel");
				return 1;
			}
		}
		if (bind(s[i], (struct sockaddr *) &addr, sizeof(addr)) < 0) {
			perror("bind");
			return 1;
		}
	}

	iov.iov_base = &frame;
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;
	FD_ZERO(&rdfs);

for (i = 0; i < currmax; i++)
FD_SET(s[i], &rdfs);
iov.iov_len = sizeof(frame);
msg.msg_namelen = sizeof(addr);
msg.msg_controllen = sizeof(ctrlmsg);
msg.msg_flags = 0;


}

void init_socket_IP()

{




	portno = 3333;
	if(vflag==1)
				{
					printf (" creating  IP SOCKET  \n ");
				}
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(vflag==1)
				{
					printf (" Retreiving host name\n ");
				}
	server = gethostbyname(host);
	if(vflag==1)
				{
					printf (" Host \"%s\" identified  \n ");
					printf (" Preparing the IP structure \n ");
				}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	//serv_addr.sin_family = PF_INET;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr,
	server->h_length);
	serv_addr.sin_port = htons(portno);
	if(vflag==1)
					{
						printf (" Socket initialized\n ");
					}

	}


int Can_interface_to_CAN_socket_binder(int ifidx, int socket) {



	if(vflag==1)
			{
				printf (" CAN interface to CAN socket binding  \n ");
			}


	int i;
	struct ifreq ifr;
	for (i = 0; i < MAXIFNAMES; i++) {
		if (dindex[i] == ifidx)
			return i;
	}
	for (i = 0; i < MAXIFNAMES; i++) {
		if (dindex[i]) {
			ifr.ifr_ifindex = dindex[i];
			if (ioctl(socket, SIOCGIFNAME, &ifr) < 0)
				dindex[i] = 0;
		}
	}
	for (i = 0; i < MAXIFNAMES; i++)
		if (!dindex[i]) /* free entry */
			break;
	if (i == MAXIFNAMES) {
		fprintf(stderr, "Interface index cache only supports %d interfaces.\n",
		MAXIFNAMES);
		exit(1);
	}

	dindex[i] = ifidx;
	ifr.ifr_ifindex = ifidx;
	if (ioctl(socket, SIOCGIFNAME, &ifr) < 0)
		perror("SIOCGIFNAME");
	if (max_devname_len < strlen(ifr.ifr_name))
		max_devname_len = strlen(ifr.ifr_name);
	strcpy(devname[i], ifr.ifr_name);
#ifdef DEBUG
	printf("new index %d (%s)\n", i, devname[i]);
#endif
	return i;
}

void receive_and_buffer_CAN_frame(int s)


{

	if(vflag==1)
			{
				printf (" receive and buffer CAN frame  \n ");
			}


	nbytes = recvmsg(s, &msg, 0);
					idx = Can_interface_to_CAN_socket_binder(addr.can_ifindex, s);
									if ((size_t)nbytes == CAN_MTU)
										maxdlen = CAN_MAX_DLEN;
									else if ((size_t)nbytes == CANFD_MTU)
										maxdlen = CANFD_MAX_DLEN;
									else {
										fprintf(stderr, "read: incomplete CAN frame\n");
										return 1;
									}
					char buf[CL_CFSZ]; /* max length */
					sprint_canframe(buf, &frame, 0, maxdlen);
					snprintf(json, sizeof(json),
							"{ \"dev\": \"%s\", \"len\": %d, \"message\": \"%s\" }",
							devname[idx], max_devname_len, buf);
				//	free(*buf);


					//free();
			///////		/
		/// free(&frame,frame.len);
}





void message_display (struct canfd_frame n)


{

	//if (logfrmt) {
	char buf[CL_CFSZ];
    sprint_canframe(buf, &n, 0, maxdlen);
    snprintf(json, sizeof(json),
    						"{ \"dev\": \"%s\", \"len\": %d, \"message\": \"%s\" }",
    						devname[idx], max_devname_len, buf);
    if(vflag==1)
    			{
    	printf("%s\n", json);
    			}

	//}
}





void IP_SENDER(int k, char* omega,char*host )

{

	if(vflag==1)
			{
				printf (" connect to remote host %s   \n ", host);
			}

	//alpha:
	connect(sockfd, (struct serv_addr *) &serv_addr, sizeof(serv_addr));
	if(vflag==1)
				{
					printf (" send IP packet %s   \n ", omega);
				}
		send(k, omega, strlen(omega), 0);
		bzero(omega, 256);
     //   free(k);

}


void close_IP_CAN_sockets(int s1,int s2)
{

	if(vflag==1)
			{
				printf (" Close IP and CAN sockets  \n ");
			}



close(s1);
close(s2);
}


int main(int argc, char **argv) {

//////////////////////////// retrieve params
retrieve_params(argc,argv);
//////////////////////////// init socket CAN
init_socket_CAN(argc,argv);
//////////////////////////// init socket IP

////////////////////////// while running condition start
while (running) {

		//		if (FD_ISSET(s[0], &rdfs)) {
///////////////////////// receive and buffer CAN frame
receive_and_buffer_CAN_frame(s[0]);


//////////////////////////// pass buffer to IP socket and send it
init_socket_IP();
message_display(frame);
usleep(50 * 1000);
IP_SENDER(sockfd,json,host);
//////////////////////////// display received CAN MESSAGE

////////////////////////// while running condition end
		         //             	}

	}
//////////////////////////// close sockets
//for (i = 0; i < currmax; i++)
//close_IP_CAN_sockets(s[i],sockfd);
return 0;
}
