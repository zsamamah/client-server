#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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
  for (int i = 0; i < len; i++) 
    result[i] = str[i] ^ key[i];
  return result;
}

int main(int argc, char *argv[]){

/* client <p/c> <server_IP> <server_port> <date/time> */
  char* p_or_c = argv[1];
  int is_encrypted = strcmp("c", p_or_c)==0?1:strcmp("p", p_or_c)==0?0:-1;
  if(is_encrypted<0)
  {
  	printf("[-]Wrong parameter, not \"p\" or \"c\"\n");
  	exit(0);
  }
  char* ip = argv[2];
  int port = atoi(argv[3]);
  
  char* msg = argv[4];

/*  
  printf("p/c => %s \n", p_or_c);
  printf("ip => %s \n", ip);
  printf("port => %d \n", port);
  printf("msg => %s \n", msg);
  exit(0);
*/
  

  int sock;
  struct sockaddr_in addr;
  socklen_t addr_size;
  char buffer[1024];
  int n;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0){
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);

  int conn = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
  if(conn==-1){
  	printf("[-]Server down.\n",conn);
  	exit(0);
  }
  else
	printf("[+]Connected to the server.\n");


  printf("Client: %s\n", msg);
  send(sock, p_or_c, strlen(p_or_c), 0);
  if(is_encrypted==1){
  	char* key = generate_key(msg);
  	char* enc = enc_dec(msg, key);
  	send(sock, enc, strlen(enc), 0);
  	bzero(buffer, 1024);
  	recv(sock, buffer, sizeof(buffer), 0);
  	char* key2 = generate_key(buffer);
  	char* dec = enc_dec(buffer, key2);
  	printf("Server: pln->%s dec->%s\n", buffer, dec);
  }
  else{
  	send(sock, msg, strlen(msg), 0);
  	bzero(buffer, 1024);
	recv(sock, buffer, sizeof(buffer), 0);
	printf("Server: %s\n", buffer);
  }


  close(sock);
  printf("[+]Disconnected from the server.\n");

  return 0;

}










