

 
#include<stdio.h>
#include<string.h>   
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>    
#include<pthread.h> 
#define N 20

struct user
{
	char name[15];
	int socket;
};
typedef struct user user; 

struct pthread_arg{
	int index;
	user *users;
};
typedef struct pthread_arg pthread_arg;

void *connection_handler(void *);
char *trimwhitespace(char *str); 
 
int main(int argc , char *argv[])
{
	char username[15];
    int sockfd , newsockfd , clilen, portno,numOfSockets=0,i,con=0;
    struct sockaddr_in server , client;
	pthread_t thread_id;
    user *users; 
	users = (user*)malloc(N*sizeof(user));
	 
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		return 1;
	} 
	portno = atoi(argv[1]);
	
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1)
    {
		perror("ERROR Could not create socket");
		return 1;
    }
	
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( portno );
	
    if( bind(sockfd,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("ERROR bind failed");
        return 1;
    }
    listen(sockfd , 5);
	clilen = sizeof(client);
	
    puts("Waiting for incoming connections...");
		
    while(1)
    {
		//sleep
		
		newsockfd = accept(sockfd, (struct sockaddr *)&client, (socklen_t*)&c);
		bzero(username,15);
		
		read(newsockfd,username,15);
		strcpy(username,trimwhitespace(username));
		printf("%s connected\n",username);
		for(i =0;i<numOfSockets;i++){
			if(strcmp(username,users[i].name)==0) {
				con=1;
				users[i].socket = newsockfd;
				printf("%s found in names\n",username);
				break;
			}
		}
		
		if(con==0) {
			numOfSockets++;  
			users[numOfSockets-1].socket = newsockfd;
			strcpy(users[numOfSockets-1].name,username);
			strcpy(users[numOfSockets-1].name,trimwhitespace(users[numOfSockets-1].name));
			//printf("%s %i\n",users[numOfSockets-1].name,users[numOfSockets-1].socket);
		}
		else con=0;
		
		pthread_arg pa;
		pa.index = numOfSockets-1;
		pa.users = users;
		
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &pa) < 0)
        {
            perror("ERROR Could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
    }
     
    if (newsockfd < 0)
    {
        perror("Accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *arg)
{   
    FILE *file;
    int sock;
    int nr,nw,i;
    char buffer[256],receiver[15],*local,str[255];
	user *users;
	pthread_arg pa = *(pthread_arg*)arg;
	users = pa.users;
	local = users[pa.index].name;	
	sock = users[pa.index].socket;
	printf("%s %i\n",local,sock);
	
	close(sockfd);
	write(sock,"Welcome\n",8);
	 
	strcpy(str,local);
	strcat(str,".txt");
	file = fopen(str,"a+");
	while(!feof(file)){
		nr = fread(buffer,255,1,file);
		strcpy(buffer,trimwhitespace(buffer));
		if(nr>0) write(sock, buffer, strlen(buffer));	
		else write(sock,"No new message\n",15);
	}	  
	fclose(file);
	fclose(fopen(str,"wb"));
			
	while(1){
		bzero(buffer,256);
		bzero(receiver,15);
		bzero(str,255);
		nr = 0;
		nr = recv(sock,buffer,255,0);		
		if (nr < 0){
		  perror("ERROR reading from socket");
		  break;
		}
		else if(nr ==0){
			printf("%s disconnected\n",local);
			fflush(stdout);
			break;
		}
		printf("**0\n");
		for(i=0;i<15;i++){
			if(buffer[i]==32) {
				memcpy(receiver, &buffer[1], i-1);
				memcpy(buffer,&buffer[i+1],255-i);
				strcpy(buffer, trimwhitespace(buffer));
				break;
			}
		}
		
		strcpy(receiver,trimwhitespace(receiver));
		if(strlen(receiver)==0) continue;
		strcpy(str,local);
		strcat(str,": ");
		strcat(str,buffer);
		strcat(str,"\n");
		strcpy(buffer,str);
		strcpy(buffer,trimwhitespace(buffer));
		//printf("%s\n",buffer);
		
		//write to socket the message				
		for(i =0;i<N;i++){
			if (strlen(users[i].name)==0) break;
			if (strlen(receiver)!=0 && strcmp(users[i].name,receiver)==0) {	
				printf("found recv\n");
				write(users[i].socket,buffer,strlen(buffer));
				printf("%s..... %s %i\n",buffer,users[i].name,users[i].socket);
				if(nw<=0){
					printf("**2\n");
					strcpy(str,receiver);
					strcat(str,".txt");					
					file = fopen(str,"a");
					fwrite(buffer,1,sizeof(buffer),file);
					fclose(file);
				}
				break;
			}
		}					
	}
	bzero(buffer,256);
	bzero(receiver,15);
	bzero(str,255);
	close(sock);
	
	    
    pthread_exit(0);
	
} 

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
