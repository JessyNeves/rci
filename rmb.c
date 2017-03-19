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
	int fd, n, addrlen;
	struct sockaddr_in addr;
	struct hostent *h; //variavel do tipo hostent
	struct in_addr *a; //variavel do tipo in_addr
	int maxsize = 128;
	char hostname[30];
	char buffer[maxsize];




if(argc<2){
	printf("\n WARNING! No Arguments \n");
		if((h=gethostbyname("tejo.tecnico.ulisboa.pt"))==NULL) exit(1);//error

		printf("official host name: %s\n", h->h_name);
		a = (struct in_addr*)h->h_addr_list[0];
		//printf("internet address: %s (%08lX)\n",inet_ntoa(*a),(long unsigned int)ntohl(a->s_addr));

		// UDP socket ; SENDTO

		fd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);  //UDP socket number stored in file descriptor
			if(fd==-1){
				printf("ERROR\n");
				exit(1);//error
			}
			memset((void*)&addr,(int)'\0',sizeof(addr)); //clears the vector
			addr.sin_family=AF_INET; //protocol UDP
			addr.sin_addr= *a; //protocol UDP IP of the destiny
			addr.sin_port=htons(59000); //predefined UDP port

		while(1){
			char tmp[maxsize];
			memset(buffer,'\0',maxsize); //clears the buffer
			memset(tmp,'\0',maxsize); //clears the buffer


			int recfrom=0;
			char message[140];
		    fgets(buffer, maxsize , stdin);
			recfrom=strlen(buffer);

			if(strcmp(buffer, "show_servers\n")==0) strcpy(buffer, "GET_SERVERS");
			if(strcmp(buffer, "exit\n")==0)exit(0);

			strncpy(tmp, buffer, 21);
			n=strcmp(tmp, "show_latest_messages ");
			if(n==0){
				printf("Número de Mensagens a Apresentar: %c%c\n", buffer[21], buffer[22]);

			}



			sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
			n=recvfrom(fd,buffer,maxsize,0,(struct sockaddr*)&addr,&addrlen);
			if(n==-1) exit(1);
			//n=n-2;

			write(1,"Echo: ",6);//stdout
			write(1,buffer,n);
			write(1,"\n",2);

			//Se tiver 17 ou + caracteres = REG function

			if(recfrom>=17)
			{
			n=sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
			if(n==-1)exit(1);
			}
//			else
//			{
//				//outros comandos quaisquer
//
//					n=sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
//					if(n==-1)exit(1);
//					addrlen=sizeof(addr);
//
//					n=recvfrom(fd,buffer,maxsize,0,(struct sockaddr*)&addr,&addrlen);
//					if(n==-1) exit(1);
//					n=n-2;
//
//					write(1,"Echo: ",6);//stdout
//					write(1,buffer,n);
//					write(1,"\n",2);
//			}
		close(fd);
		exit(0);
				}
			}
	}


