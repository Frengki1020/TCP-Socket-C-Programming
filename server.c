#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "defined.h"
void writefile(int sockfd, FILE *fp);
	ssize_t total=0;
void writefile(int sockfd, FILE *fp){
	ssize_t n;
	char buff[MAX_LINE] = {0};
	while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0){
	//while((n=recv(buff,sizeof(char),MAX_LINE,fp)) >0){
		total+=n;
		if (n == -1){
			perror("Receive File Error");
			exit(1);
		}
		if (fwrite(buff, sizeof(char), n, fp) != n){
		//if(fwrite(sockfd,buff,n,fp) != n){
			perror("Write File Error");
			exit(1);
		}
		memset(buff, 0, MAX_LINE);
	}
}
int main(int argc, char *argv[]){
	int sockfd;
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Socket Gagal Dibuat");
		exit(1);
	}
	else{
		printf("Socket Sukses Dibuat!\n");
	}
	struct sockaddr_in clientaddr, serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORT);
	//Server Bind untuk Client
	if(bind(sockfd, (const struct sockaddr *) &serveraddr,sizeof(serveraddr)) == -1){
		perror("Bind Gagal!\n");
		exit(1);
	}
	else{
		printf("BIND Sukses!\n");
	}
	//Server Listen untuk Client
	if(listen(sockfd, PORT) == -1){
		perror("Listen Gagal!\n");
		exit(1);
	}
	else{
		printf("Listen For Client!\n");
	}
	//Server Terima Request dari Client 
	socklen_t addrlen = sizeof(clientaddr);
	int connfd;
	if((connfd= accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen)) == -1){
		perror("Koneksi Gagal!\n");
		exit(1);
	}
	else{
		printf("Koneksi Sukses!\n");
	}
	close(sockfd);
	char filename[BUFF_SIZE] = {0};
	if (recv(connfd, filename, BUFF_SIZE, 0) == -1){
		perror("Filename Tidak dapat Diterima!\n");
		exit(1);
	}
	FILE *fp = fopen(filename, "wb");
	if (fp == NULL){
		perror("File Tidak dapat Dibuka!\n");
		exit(1);
	}
	char addr[INET_ADDRSTRLEN];
	printf("Start receive file: %s from %s\n", filename, inet_ntop(AF_INET, &clientaddr.sin_addr, addr, INET_ADDRSTRLEN));
	writefile(connfd, fp);
	//printf("Receive Success, NumBytes = %ld\n", total);
	fclose(fp);
	close(connfd);
	return 0;
}
