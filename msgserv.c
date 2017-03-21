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
#include <time.h>
struct hostent *gethostbyname(const char *name);
extern int errno;

// argv[1] = name of the server
// argv[2] = IP do servidor de mensagens
// argv[3] = Porto UDP Cliente
// argv[4] = Porto TCP Cliente
// argv[5] = IP do Servidor Identidades				OPCIONAL
// argv[6] = Porto UDP do servidor identidades		OPCIONAL
// argv[7] = Numero máx de mensagens gravadas		OPCIONAL
// argv[8] = intervalo refresh Do servidoR			OPCIONAL


int main(int argc, char *argv[]){
	int fd,fd2,fd3, n, addrlen;
	struct sockaddr_in addr;
	struct sockaddr_in addr_tcp;

	struct hostent *h; //variavel do tipo hostent
	struct hostent *t; // hostent for TCP
	struct in_addr *b; //in_addr for TCP
	struct in_addr *a; //variavel do tipo in_addr
	int maxsize = 128;
	int recfrom=0, ret, maxfd;
	int i=1;
	char hostname[30];
	char buffer[maxsize];
	char refresh[maxsize];

	fd_set rfds, fds;
	struct timeval time;
	time.tv_sec = 15;
	time.tv_usec = 0;


	// argv[1] = name of the message server
	// argv[2] = IP do servidor de mensagens
	// argv[3] = Porto UDP Cliente
	// argv[4] = Porto TCP Cliente
	//se argc < 4 nao foram introduzidos argumentos obrigatorios, kill
	if(argc < 4) exit(0);
	if(argc != 8){
		printf("Too few arguments. Assuming values\n");
		if((h=gethostbyname("tejo.tecnico.ulisboa.pt"))==NULL){
			printf("\nCan't gethostbyname!\n");
			exit(1);//error
		}
/////////SOCKET UDP //////////////////////
		a = (struct in_addr*)h->h_addr_list[0];
		fd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); //UDP
		fd2=socket(AF_INET,SOCK_STREAM,0);
		fd3=socket(AF_INET,SOCK_STREAM,0);	//TCP
//TCP
		if(fd==-1)exit(1);//error
		if(fd2==-1)exit(1);//error

///////////////////		UDP		///////////////////////////////////
	memset((void*)&addr_tcp,(int)'\0',sizeof(addr_tcp)); //clears address
	memset((void*)&addr,(int)'\0',sizeof(addr)); //clears the vector
	addr.sin_family=AF_INET; //protocol UDP
	addr.sin_addr= *a; //protocol UDP IP of the destiny
	addr.sin_port=htons(59000); //protocol UDP port
/////////////////////////////////////////////////////


	//TCP family specification
		addr_tcp.sin_family=AF_INET;
		addr_tcp.sin_addr.s_addr=htonl(INADDR_ANY);
		addr_tcp.sin_port=htons(59000);
		if(bind(fd2,(struct sockaddr*)&addr_tcp,sizeof(addr_tcp))==-1)exit(0);
		if(listen(fd2,5)==-1){
			printf("error Listen\n");
			exit(1);//error
		}

		//Gathering information from other TCP msgserv running
				t = gethostbyname("tejo.tecnico.ulisboa.pt");
				b = (struct in_addr*)t->h_addr_list[0];
				printf("internet address: %s (%08lX)\n",inet_ntoa(*b),(long unsigned int)ntohl(b->s_addr));
		//Gathering information from other TCP msgserv running

				//Connecting to TCP Server
						n=connect(fd3,(struct sockaddr*)&addr_tcp,sizeof(addr_tcp));
						if(n==-1){
							printf("Connection Error - Unable to Connect n=-1\n");
							exit(1);
						}else{
							printf("TCP Connection Established\n");
							printf("----------------------------\n");
						}

			//registering the message server upon start of the program ////////////
			strcat(buffer, "REG ");
			while(i<5){
				strcat(buffer, argv[i]);
				if(i==4) break;
				strcat(buffer, ";");
				i++;
			}
			n=sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
			i=1;
			if(n==-1){
					printf("Couldn't send\n");
					exit(1);
					}
			// No recfrom function because ID server does not respond in this case
			///////////////////////////////////////////////////////////////

			////////////////////Request for other message servers connected//////////////////
			strcpy(buffer, "GET_SERVERS\n");
			n=sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
			if(n==-1)exit(1);
			addrlen=sizeof(addr);
			n=recvfrom(fd,buffer,maxsize,0,(struct sockaddr*)&addr,&addrlen);
			if(n==-1) exit(1);
			printf("%s", buffer);
			////////////////////////////////////////////////////////////////////////////////



			while(1){

				start:
				memset(buffer,'\0',maxsize); //clears the buffer
				FD_ZERO(&rfds);
				FD_SET(fd, &rfds);
				FD_SET(0, &fds);
				maxfd = fd+1;

				ret = select(1, &fds, NULL, NULL, &time);
				if(ret==0){
					////REFRESHING SERVER////
								strcat(buffer, "REG ");
								while(i<5){
									strcat(buffer, argv[i]);
									if(i==4) break;
									strcat(buffer, ";");
									i++;
								}
								n=sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
								i=1;
								if(n==-1){
										printf("Couldn't send\n");
										exit(1);
										}
					/////////////////////////
					goto start;
				}
				if(ret) gets(buffer);
				recfrom=strlen(buffer);

				//Se recfrom>= 17 caracteres quer dizer que é o comando REG ou um comando errado que será prontamente rejeitado pelo serv indentidades
				if(recfrom>=17)
				{
					n=sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
					if(n==-1){
						printf("Couldn't send\n");
						exit(1);
					}
				}
				else
				{
					//outros comandos quaisquer

						n=sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
						if(n==-1)exit(1);
						addrlen=sizeof(addr);

						n=recvfrom(fd,buffer,maxsize,0,(struct sockaddr*)&addr,&addrlen);
						if(n==-1) exit(1);
						n=n-2;
						write(1,"Echo: ",6);//stdout
						write(1,buffer,n);
						write(1,"\n",2);
				}

			}

			close(fd);
			exit(0);
	}


}

