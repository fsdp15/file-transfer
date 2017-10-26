#include "includes.h"

int main() {
	int opcao;
	abre_raw_socket();

	puts("Selecione o modo:");
	puts("1 - Servidor");
	puts("2 - Cliente");
	puts("0 - Sair");

	scanf("%d", &opcao);

	switch (opcao) {
		case 1:
		inicia_modo_servidor();
		break;

		case 2:
		inicia_modo_cliente();
		break;

		case 0: 
		exit(0);

		default:
		puts("Opcao invalida!");
	}

}
