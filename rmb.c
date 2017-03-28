//test.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


struct hostent *gethostbyname(const char *name);
extern int errno;
int main(int argc, char *argv[]){
	int fd, fd2, n, addrlen;
	struct sockaddr_in addr;
	struct sockaddr_in addr2;
	struct hostent *h, *s; //variavel do tipo hostent
	struct in_addr *a, *b; //variavel do tipo in_addr
	int maxsize = 128;
	char hostname[30];
	char buffer[maxsize];


if(argc<2){
	printf("\n WARNING! No Arguments \n");
		if((h=gethostbyname("tejo.tecnico.ulisboa.pt"))==NULL) exit(1);//error
		if((s=gethostbyname("localhost"))==NULL) exit(1); //error

		printf("official host name: %s\n", h->h_name); //prints ID server name
		printf("official host name: %s\n", s->h_name); //prints Msgserv server name

		a = (struct in_addr*)h->h_addr_list[0];
		b = (struct in_addr*)s->h_addr_list[0];
		//printf("internet address: %s (%08lX)\n",inet_ntoa(*a),(long unsigned int)ntohl(a->s_addr));

		// UDP sockets CREATION
		fd2=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);  //UDP socket number stored in file descriptor
			if(fd2==-1){
				printf("ERROR\n");
				exit(1);//error
					}
		fd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);  //UDP socket number stored in file descriptor
			if(fd==-1){
				printf("ERROR\n");
				exit(1);//error
			}

			memset((void*)&addr2,(int)'\0',sizeof(addr2)); //clears the vector
			addr2.sin_family=AF_INET; //UDP
			//addr2.sin_addr = *b; //IP
			addr2.sin_port=htons(5001); //PORT

			memset((void*)&addr,(int)'\0',sizeof(addr)); //clears the vector
			addr.sin_family=AF_INET; //protocol UDP
			addr.sin_addr= *a; //protocol UDP IP of the destiny
			addr.sin_port=htons(59000); //predefined UDP port

		while(1){

			int recfrom=0;
			fgets(buffer, maxsize , stdin);
		    if(strcmp(buffer, "exit\n")==0)exit(0);

		    if(strcmp(buffer, "show_servers\n")==0){
		    	strcpy(buffer, "GET_SERVERS");
		    	recfrom=strlen(buffer);
		    	sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
		    	n=recvfrom(fd,buffer,maxsize,0,(struct sockaddr*)&addr,&addrlen);
		    	if(n==-1)exit(1);
		    	write(1,"Echo: ",6);//stdout
		    	write(1,buffer,n);
		    	write(1,"\n",1);
		    }else{
					n=sendto(fd2,buffer,maxsize,0,(struct sockaddr*)&addr2,sizeof(addr2));//send function
					if(n==1)exit(0);
		    	}

		}//while
	}
}//main


