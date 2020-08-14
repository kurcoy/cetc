/*header file for linux*/
//#include <unistd.h> /* read, write, close */
//#include <sys/socket.h> /* socket, connect */
//#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
//#include <netdb.h> /* struct hostent, gethostbyname */

/*header file for windows*/   
#include <ansi_c.h>    
#include <stdio.h>
#include <WinSock2.h>
#include <ansi_c.h>
#include <ansi_c.h>
#include <ansi_c.h>

#pragma comment(lib,"Ws2_32.lib ")

#include <ansi_c.h>     

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memcpy, memset */
#include <toolbox\toolbox.h>    

void error(const char *msg) { perror(msg); exit(0); }

int post(char *file, char *host, int port, char *dir) {
	

	char header[]	=	"POST %s HTTP/1.1\r\n"
      						"Host: %s\r\n"
      						"Connection: keep-alive\r\n"
  								"Content-Type: multipart/form-data; boundary=\"partsep\"\r\n"
									"Content-Length: %d\r\n"
									"\r\n";
	char pre[] = 		"--partsep\r\n"
									"Content-Disposition: form-data; name=\"MAX_FILE_SIZE\"\r\n"
									"\r\n"
									"300000\r\n";

	char data[] = 	"--partsep\r\n"
      						"Content-Type: application/octet-stream\r\n"
      						"Content-Disposition: form-data; name=\"userfile\"; filename=\"test.dat\"\r\n"
      						"\r\n"
									"Test Data";
	
	char footer[] = "\r\n"
									"--partsep--\r\n"
									"\r\n";
	
	struct hostent *server;
	struct sockaddr_in serv_addr;
	int sockfd, bytes, sent, received, total, data_len; 
	char *bname, *filename;
	char message[1024],response[4096];

	
	printf("Posting file \"%s\" to \"%s\"...\n", file, host);
	

	filename = StrDup(file);
	bname = "userfile";
	data_len = strlen(pre) + strlen(data) + strlen(footer);
	sprintf(message, header, dir, host, data_len);
	
	/* create the socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR: opening socket");

	/* lookup the ip address */
	server = gethostbyname(host);
	if (server == NULL) error("ERROR, no such host");
	
	/* fill in the structure */
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);
	
	/* connect the socket */
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
     error("ERROR connecting");

	/* send header */
	sent = send(sockfd, message, strlen(message), 0);
	if (sent <= 0)
		error("ERROR failed to send");
	
	/* send pre_data */
	sent = send(sockfd, pre, strlen(pre),0);	
	if (sent <= 0)
		error("ERROR failed to send");
	
	/* send data */
	sent = send(sockfd, data, strlen(data),0);
	if (sent <= 0)
		error("ERROR failed to send");

	/* send footer */
	sent = send(sockfd, footer, strlen(footer),0);
	if (sent <= 0)
		error("ERROR failed to send");

	/* send test_msg */
	//sent = send(sockfd, test_msg, strlen(test_msg),0);
	//if (sent <= 0)
	//	error("ERROR failed to send");

	/* print full message sent*/
	printf("%s", message);
	printf("%s", pre);
	printf("%s", data);
	printf("%s", footer);
	//printf("%s", test_msg);

	/* receive the response */
	memset(response,0,sizeof(response));
	total = sizeof(response)-1;
	received = 0;
	do {
		bytes = read(sockfd,response+received,total-received);
		if (bytes < 0)
			error("ERROR reading response from socket");
		if (bytes == 0)
			break;
		received+=bytes;
	} while (received < total);
	
	if (received == total)
		error("ERROR storing complete response from socket");

	printf("%d bytes received in response...\n", received);
	printf("%s\n", response);
	
  /* close the socket */
  close(sockfd);
	
	return 0;;
}

void usage(char *argv0) {
	printf("USAGE: %s <file> <host> <port> <dir>\n", argv0 );
	exit(0);
}

int main(int argc, char *argv[]) {
	if (argc < 5)
		usage(argv[0]);

	post(argv[1], argv[2], atoi(argv[3]), argv[4]);
	return 0;
}
