/** Copyright (c) 2011, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Andras Biro
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int sockfd[2], portno[2], i;  
  socklen_t clilent[2];
  struct sockaddr_in serv_addr[2], cli_addr[2];
  
  if (argc < 3) {
      fprintf(stderr,"Usage: %s PORTA PORTB\n",argv[0]);
      exit(1);
  }
  for(i=0;i<2;i++){
    portno[i] = atoi(argv[i+1]);
    if(portno[i]<=0||portno[i]>=65535){
      fprintf(stderr, "Port number must be between 1 and 65535\n");
      exit(1);
    }
    sockfd[i]=socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd[i]<0){
      fprintf(stderr, "Can't create socket\n");
      exit(1);
    }
    bzero((char *) &(serv_addr[i]), sizeof(serv_addr[i]));
    serv_addr[i].sin_family = AF_INET;
    serv_addr[i].sin_addr.s_addr = INADDR_ANY;
    serv_addr[i].sin_port = htons(portno[i]);
     
    if (bind(sockfd[i], (struct sockaddr *) &serv_addr[i], sizeof(serv_addr[i])) < 0){
      fprintf(stderr, "Can't bind on port %d\n", portno[i]);
      exit(1);
    }
    listen(sockfd[i],1);
    clilent[i] = sizeof(cli_addr[i]);
  }
  fd_set watchset;
  int maxfd=sockfd[0]>sockfd[1]?sockfd[0]:sockfd[1];
  FD_ZERO(&watchset);
  for(i=0;i<2;i++)
    FD_SET(sockfd[i],&watchset);
  printf("Waiting for clients on port %d and %d\n",portno[0],portno[1]);
  if(select(maxfd+1,&watchset,NULL,NULL,NULL)<0){
    fprintf(stderr,"Select error\n");
    exit(1);
  }
  int connected=-1,notconnected=-1;
  if(FD_ISSET(sockfd[0], &watchset)){
    connected=0;
    notconnected=1;
  }else{
    connected=1;
    notconnected=0;
  }
  int newsockfd[2];
  newsockfd[connected] = accept(sockfd[connected], (struct sockaddr *) &(cli_addr[connected]), &(clilent[connected]));
  if(newsockfd[connected]<0){
    fprintf(stderr,"Can't accept client connection on port %d\n", portno[connected]);
    exit(1);    
  }
  printf("Clinet on port %d connected, waiting for client on port %d\n",portno[connected],portno[notconnected]);
  newsockfd[notconnected] = accept(sockfd[notconnected], (struct sockaddr *) &(cli_addr[notconnected]), &(clilent[notconnected]));
  if(newsockfd[notconnected]<0){
    fprintf(stderr,"Can't accept client connection on port %d\n", portno[notconnected]);
    exit(1);    
  }
  char buffer[256];
  FD_ZERO(&watchset);
  maxfd=newsockfd[0]>newsockfd[1]?newsockfd[0]:newsockfd[1];
  for(i=0;i<2;i++)
    FD_SET(newsockfd[i],&watchset);
  printf("Clinets connected, starting forwarding\n");
  while(FD_ISSET(newsockfd[0],&watchset)||FD_ISSET(newsockfd[1],&watchset)){
    fd_set readfds=watchset;
//    printf("ready\n");
    if(select(maxfd+1,&readfds,NULL,NULL,NULL)<0){
      fprintf(stderr,"Select error\n");
      exit(1);
    }
    int from=-1, to=-1;
    if(FD_ISSET(newsockfd[0], &readfds)){
      from=0;
      to=1;
    } else {
      from=1;
      to=0;
    }
//    printf("%d to %d\n",portno[from],portno[to]);
    int n = read(newsockfd[from],buffer,255);
    if (n < 0) 
      fprintf(stderr,"Can't read from port %d",portno[from]);
    else{
      n = write(newsockfd[to],buffer,n);
      if (n < 0) 
        fprintf(stderr,"Can't write to port %d",portno[to]);
    }
  }
  for(i=0;i<2;i++){
    close(newsockfd[i]);
    close(sockfd[i]);
  }
  return 0;
}