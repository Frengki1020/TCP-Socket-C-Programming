#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "defined.h"

void sendfile(FILE *fp, int sockfd);
ssize_t total=0;

//Fungsi untuk Kirim File
void sendfile(FILE *fp, int sockfd) {
  	 int n;
    	char sendline[MAX_LINE] = {0};
    	while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0){
           	total+=n;
        	if (n != MAX_LINE && ferror(fp)){
            		perror("Read File Error!\n");
            		exit(1);
        	}
        	if (send(sockfd, sendline, n, 0) == -1){
            		perror("Can't send file!\n");
            		exit(1);
        	}
		else{
			printf("Send File Success!\n");
		}
        	memset(sendline, 0, MAX_LINE);
    	}
}

//Main Program
int main(int argc, char* argv[]){
    	if (argc != 2){
        	perror("usage: ./client [Nama File]");
        	exit(1);
    	}
    	int sockfd;
	//Client Membuat Socket
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
        	perror("Tidak Dapat Membuat Socket!\n");
        	exit(1);
    	}
	else{
		printf("Socket Sukses Dibuat!\n");
	}
    	struct sockaddr_in serveraddr;
    	memset(&serveraddr, 0, sizeof(serveraddr));
    	serveraddr.sin_family = AF_INET;
    	serveraddr.sin_port = htons(PORT);
    	if (inet_pton(AF_INET, "192.168.43.67", &serveraddr.sin_addr) < 0){
        	perror("IPaddress Convert Error");
        	exit(1);
    	}
	//Client Request Koneksi Ke Server
    	if (connect(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    	{
        	perror("Connect Error!\n");
        	exit(1);
    	}
	else{
		printf("Request Diterima!\n");
	}
	//
    	char *filename = basename(argv[1]);
    	if (filename == NULL){
        	perror("File Name Tidak dapat Diterima!\n");
        	exit(1);
    	}
	//Kirim File
    	char buff[BUFF_SIZE] = {0};
    	strncpy(buff, filename, strlen(filename));
    	if (send(sockfd, buff, BUFF_SIZE, 0) == -1){
        	perror("Filename Tidak dapat Dikirim!\n");
        	exit(1);
    	}
	//Buka File
    	FILE *fp = fopen(argv[1], "rb");
    	if (fp == NULL) {
        	perror("File Tidak dapat Dibuka!\n");
        	exit(1);
    	}
	//Kirim File ke Server
    	sendfile(fp, sockfd);
 	//printf("Send Success, NumBytes = %ld\n", total);
    	fclose(fp);
    	close(sockfd);
    	return 0;
}

