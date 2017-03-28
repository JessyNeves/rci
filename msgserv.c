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

struct messages{
	char message[128];
	int timer;
};


// argv[1] = name of the server
// argv[2] = IP do servidor de mensagens
// argv[3] = Porto UDP Servidor Mensagens Atende
// argv[4] = Porto TCP Servidor Mensagens Atende
// argv[5] = IP do Servidor Identidades				OPCIONAL
// argv[6] = Porto UDP do servidor identidades		OPCIONAL
// argv[7] = Numero máx de mensagens gravadas		OPCIONAL
// argv[8] = intervalo refresh Do servidoR			OPCIONAL


int main(int argc, char *argv[]){

	struct messages array[128];
	int fd,fd2,fd3, fd4, n, addrlen;
	struct sockaddr_in addr;
	struct sockaddr_in addr2;
	struct sockaddr_in addr_tcp;
	struct sockaddr_in addr_tcp2;

	struct hostent *h; //UDP CLIENT
	struct hostent *s; //UDP SERVER
	struct in_addr *a; //in_addr for UDP
	int maxsize = 128;
	int recfrom=0, ret, maxfd;
	int i=1, r=0, j=0;
	char hostname[30];
	char buffer[maxsize];
	char info[maxsize];
	char refresh[maxsize];

	fd_set rfds;
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
		if((h=gethostbyname("tejo.tecnico.ulisboa.pt"))==NULL)exit(1);//error

		/////////SOCKETS UDP/TCP //////////////////////
		a = (struct in_addr*)h->h_addr_list[0];
		fd=socket(AF_INET,SOCK_DGRAM,0);//UDP to Máquina TEJO
		fd2=socket(AF_INET,SOCK_DGRAM,0);//UDP socket for SERVER
		fd3=socket(AF_INET,SOCK_STREAM,0);//TCP
		fd4=socket(AF_INET,SOCK_STREAM,0); //TCP

///////////////////		UDP Structures	/////////////////
	memset((void*)&addr,(int)'\0',sizeof(addr)); //clears the vector
	memset((void*)&addr2,(int)'\0',sizeof(addr2)); //clears the vector

	addr.sin_family=AF_INET; //protocol UDP
	addr.sin_addr= *a; // IP maquina TEJO
	addr.sin_port=htons(59000); //protocol UDP port of Server ID

	//SERVER UDP, accepting datagrams from any address on the system through PORT defined on program call
	addr2.sin_family=AF_INET; //protocol UDP
	addr2.sin_addr.s_addr=htonl(INADDR_ANY);
	addr2.sin_port=htons(atoi(argv[3])); //protocol UDP port of Server ID
	ret=bind(fd2,(struct sockaddr*)&addr2,sizeof(addr2));
	if(ret==-1){
		printf("error: %s\n",strerror(errno));
		exit(1);//error
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
			if(n==-1)exit(1);
	 ///////////////NO RECVFROM BECAUSE IDENTITY WONT RESPOND////////////////////////////

    ////////////////REQUEST OF OTHER SERVERS CONNECTED TO ID SV/////////////////////////
			strcpy(buffer, "GET_SERVERS\n");
			n=sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
			if(n==-1)exit(1);
			addrlen=sizeof(addr);
			n=recvfrom(fd,buffer,maxsize,0,(struct sockaddr*)&addr,&addrlen);
			if(n==-1) exit(1);
			printf("%s", buffer);
			/// STORAGE OF OTHER SERVER'S INFORMATION
			char * servers = buffer;
			int n_servers = 0;
			const char s[2] = ";";
			char *token;
			char *ip;
			char *tcport;
			char *udport;
			int x=0;

			while(servers){
			      char * nextLine = strchr(servers, '\n');
			      if (nextLine) *nextLine = '\0';  // temporarily terminate the current line
			      token = strtok(servers, s);
			      while( token != NULL ){
				      x++;
			    	  if(x==2){
				    	  ip = token;
			    		  printf("%s\n", ip );//IP
			    	  }
			    	  if(x==3){
				    	  udport = token;
			    		  printf("Servidor: %s .... Porta UDP:%s\n", ip,udport);//Porta UDP
			    	  }
			    	  if(x==4){
				    	  tcport = token;
			    		  printf("Servidor: %s .... Porta TCP:%s\n", ip,tcport);//Porta TCP
			    	  }
			          token = strtok(NULL, s);
			        }
			      if (nextLine) *nextLine = '\n';  // then restore newline-char, just to be tidy
			      servers = nextLine ? (nextLine+1) : NULL;
			      x=0;
			   }


			while(1){
				int msgnum=0;
				char info[140];
				char command[30];
				char message[140];

				start:
				memset(buffer,'\0',maxsize); //clears the buffer
				memset(info,'\0',maxsize); //clears the buffer
				memset(message,'\0',maxsize); //clears the buffer

				FD_ZERO(&rfds);
				FD_SET(0, &rfds);
				FD_SET(fd, &rfds);
				FD_SET(fd2, &rfds);
				FD_SET(fd3, &rfds);
				maxfd = fd3+1;

				ret = select(maxfd, &rfds, NULL, NULL, &time);
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
				if(ret){
					n=recvfrom(fd2,buffer,maxsize,0,(struct sockaddr*)&addr2,&addrlen);
					if(n==-1) exit(1);
					sscanf(buffer, "%[^ ] %[^\n]",command, info);
					if(strcmp(command, "show_latest_messages")==0){
						msgnum = atoi(info);
						if(msgnum>r) msgnum = r;
						printf("\nLatest %d messages published:\n", msgnum);
						for(int aux=0; aux<msgnum; aux++){
							if(array[aux].message != NULL) printf("%s\n", array[aux].message);
						}
					}


					if(strcmp(command, "publish")==0){
						if(r>0){
							strcpy(array[r].message,info);
							array[r].timer=array[r-1].timer + 1;
							r++;
							}
						if(r==0){
							strcpy(array[0].message,info);
							array[0].timer = 1;
							r++;
							}
					}

				}
				recfrom=strlen(buffer);
//
//				//Se recfrom>= 17 caracteres quer dizer que é o comando REG ou um comando errado que será prontamente rejeitado pelo serv indentidades
//				if(recfrom>=17)
//				{
//					n=sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
//					if(n==-1){
//						printf("Couldn't send\n");
//						exit(1);
//					}
//				}
//				else{
//					//outros comandos quaisquer
//						n=sendto(fd,buffer,maxsize,0,(struct sockaddr*)&addr,sizeof(addr));//send function
//						if(n==-1)exit(1);
//						addrlen=sizeof(addr);
//
//						n=recvfrom(fd,buffer,maxsize,0,(struct sockaddr*)&addr,&addrlen);
//						if(n==-1) exit(1);
//						n=n-2;
//						write(1,"Echo: ",6);//stdout
//						write(1,buffer,n);
//						write(1,"\n",2);
//				}
			}

			close(fd);
			close(fd2);
			exit(0);
	}


}

