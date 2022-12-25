#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

char* generate_key(char* str) {
  int len = strlen(str);
  char* hex_str = malloc(len * 2 + 1);
  for (int i = 0; i < len * 2; i += 2) {
    sprintf(hex_str + i, "C5");
  }
  return hex_str;
}

char* enc_dec(char* str, char* key) {
    int len = strlen(str);
  char* result = malloc(len + 1);
//  printf("enc: ");
  for (int i = 0; i < len; i++) {
    result[i] = str[i] ^ key[i];
    //printf("%c",result[i]);
    }
  return result;
}

void send_message(int is_encrypted,int is_date, char* buffer, int client_sock)
{
    if(is_date==-1){
    	bzero(buffer, 1024);
    	strcpy(buffer, "Wrong Message");
    	send(client_sock, buffer, strlen(buffer), 0);
    	if(is_encrypted==1){
    		char* key = generate_key(buffer);
  		char* enc = enc_dec(buffer, key);
    	}
    	printf("Server: %s\n", buffer);
    }
    
    else if(is_date==0){
    	char time_now[8];
    	time_t secs = time(0);
    	struct tm *local = localtime(&secs);
    	sprintf(time_now, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
    	if(is_encrypted==1){
    		char* key = generate_key(time_now);
  		char* enc = enc_dec(time_now, key);
  		char* dec = enc_dec(enc, key);
  		send(client_sock, enc, strlen(time_now), 0);
  		printf("Server: pln->%s enc->%s dec->\n", time_now, enc, dec);
    	}
    	else
    		send(client_sock, time_now, strlen(time_now), 0);
    	printf("Server: %s\n", time_now);
    }
    
    else if(is_date==1){
    	char time_now[10];
    	time_t secs = time(0);
    	struct tm *local = localtime(&secs);
    	sprintf(time_now, "%02d/%02d/%04d", local->tm_mday, local->tm_mon, local->tm_year+1900);
    	if(is_encrypted==1){
    		char* key = generate_key(time_now);
  		char* enc = enc_dec(time_now, key);
  		char* dec = enc_dec(enc, key);
  		send(client_sock, enc, strlen(enc), 0);
  		printf("Server: pln->%s enc->%s dec->%s\n", time_now, enc, dec);
  		printf("msg->%s key->%s\n",time_now,key);
    	}
    	else
    		send(client_sock, time_now, strlen(time_now), 0);
    	printf("Server: %s\n", time_now);
    }

}

int main(int argc, char *argv[]){

/* server <port _number > */

  if(!argv[1])
  {
  	printf("[-]You should provide a port \n");
  	exit(0);
  }

  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);

  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  char buffer[1024];
  char msg[1024];
  int n;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0){
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (n < 0){
    perror("[-]Bind error");
    exit(1);
  }
  printf("[+]Bind to the port number: %d\n", port);

  listen(server_sock, 5);
  printf("Listening...\n\n");


    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
    printf("[+]Client connected.\n");

    bzero(buffer, 1024);
    recv(client_sock, buffer, sizeof(buffer), 0);
    int is_encrypted = strcmp("c", buffer)==0?1:strcmp("p", buffer)==0?0:-1;
    
    bzero(buffer, 1024);
    recv(client_sock, buffer, sizeof(buffer), 0);
    int is_date;
    
    printf("Client: %s\n", buffer);
    
    if(is_encrypted==1){
    	char* key = generate_key(buffer);
  	char* dec = enc_dec(buffer, key);
  	is_date = strcmp("date", dec)==0?1:strcmp("time", dec)==0?0:-1;
  	free(key);
  	free(dec);
  	send_message(1,is_date, dec, client_sock);
    }
    else{
    	is_date = strcmp("date", buffer)==0?1:strcmp("time", buffer)==0?0:-1;
    	send_message(0,is_date, buffer, client_sock);
    	printf("Server: %s \n",buffer);
    }
    
    close(client_sock);
    printf("[+]Client disconnected.\n\n");

  return 0;
}













