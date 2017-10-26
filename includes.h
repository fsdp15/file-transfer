#define ACK 0
#define NACK 1
#define OK 2
#define FIM 7
#define FIM_GRUPO 8
#define CD 10
#define LS 11
#define PUT 12
#define PUT_GRUPO 13
#define GET 14
#define GET_GRUPO 15
#define MOSTRA_NA_TELA 20
#define TAMANHO 21
#define NOME 22
#define ERRO 31
#define MARCA 126

int soquete;
struct sockaddr_ll endereco;

typedef struct mensagem {
	unsigned char marca;
	unsigned char tamanho :6;
	unsigned char sequencia :5;
	unsigned char tipo :5;
	unsigned char dados[59];
	unsigned char crc;
} mensagem;

void abre_raw_socket();
void inicia_modo_servidor();
void servidor_cd(unsigned char *diretorio_cd);
void servidor_ls(mensagem *recebida);
void servidor_nome();
void servidor_tamanho(mensagem *recebida);
void servidor_put(mensagem *recebida);
void servidor_get (mensagem *recebida);
void servidor_put_grupo();
unsigned char getCRC(unsigned char message[], unsigned char length);
unsigned char calcula_crc(mensagem* nova_mensagem);
unsigned char* calcula_tamanho(unsigned char* nomeArq);
long espaco_livre();
void inicia_modo_cliente();
int envia(unsigned char* buff, int tam);
mensagem* recebe();
void envia_mensagem(unsigned char *dados, int tamanho, int sequencia, int tipo);
void verifica_comando(unsigned char *comando);
void resultado_ls();
void resultado_cd();
void resultado_put(unsigned char *nome_arquivo);
void resultado_get(mensagem *recebida, unsigned char *nome_arquivo);
void ls (unsigned char *comando);
void cd (unsigned char *comando);
void put(unsigned char *comando);
void get(unsigned char * comando);
void put_grupo(unsigned char *comando);

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <signal.h> 
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <assert.h> 
#include <stddef.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <ctype.h>
#include <sys/poll.h>
#include <time.h>
#include "auxiliares.c"
#include "raw_socket.c"
#include "conexoes.c"
#include "cliente.c"
#include "servidor.c"
#include "tratamento_servidor.c"
#include "resultados_cliente.c"
