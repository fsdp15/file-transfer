void resultado_ls() {
	mensagem *recebida;
	FILE *imprime;
	imprime = fopen("imprime.txt", "w");

	do {
		recebida = recebe();
		if (recebida->marca == MARCA) {
			if (recebida->crc == calcula_crc(recebida)) {
				envia_mensagem("", 0, 0, ACK); 
				if (recebida->tipo == MOSTRA_NA_TELA)
					fwrite(recebida->dados, 1, recebida->tamanho, imprime);
			}
			else {
				puts("Erro, CRC invalido");
				envia_mensagem("", 0, 0, NACK);
			}

		}
		else puts("Recebi uma mensagem que nao eh pra mim");

	} while (recebida->tipo != FIM);
	fclose(imprime);
	system("cat imprime.txt");
	system("rm imprime.txt");
	printf("\n");
}

void resultado_cd() {
	int recebi = 0;
	mensagem *recebida;

	while (!recebi) {
		recebida = recebe();
		if (recebida->marca == MARCA) {
			if (recebida->crc == calcula_crc(recebida)) {
					envia_mensagem("", 0, 0, ACK); 
					if (recebida->tipo == ERRO) {
						if (recebida->dados[59] == '0') {
							puts("Erro, sem permissao para acessar o diretorio");
							recebi = 1;
						}
						else if (recebida->dados[59] == '1') {
							puts("Erro, diretorio inexistente");
							recebi = 1;
						}
						else if (recebida->dados[59] == '2') {
							puts("Nao sei o que eh pra fazer aqui");
							recebi = 1;
						}
						else { 
							puts("Erro nao identificado na solitacao");
							recebi = 1;
						}
					}
					else if (recebida->tipo == OK) {
							puts("Diretorio acessado");
							recebi = 1;
						}
						else {
							puts("Recebi uma mensagem que nao se adequa a solicitacao de CD");
						}
			}

			else {
				puts("Erro, CRC invalido");
				envia_mensagem("", 0, 0, NACK);
			}
		}
		else puts("Recebi uma mensagem que nao eh pra mim");
	}
	free(recebida);
}

void resultado_put(unsigned char *nome_arquivo) {
	mensagem *recebida;
	int recebi = 0;
	while (!recebi) {
		recebida = recebe();
		if (recebida->marca == MARCA) {
			if (recebida->crc == calcula_crc(recebida)) {
				envia_mensagem("", 0, 0, ACK); 
				if (recebida->tipo == ERRO) {
					puts("Sem espaço livre no servidor");
					recebi = 1;
				}
				else if (recebida->tipo == OK) {
					int i;
					char dados[59];
					int tamanho = 1;
					int sequencia = 0;
					FILE *arquivo;

					arquivo = fopen(nome_arquivo, "r");
					while (tamanho) {
						for (i = 0; i < 59; i++)
							dados[i] = 0;
						tamanho = fread(dados, 1, 59, arquivo);
						if (tamanho) envia_mensagem(dados, tamanho, sequencia, PUT);
						sequencia++;
						if (sequencia > 31) sequencia = 0;
					}
					envia_mensagem("", 0, 0, FIM);
					puts("Arquivo transmitido com sucesso");
					recebi = 1;
				}
				}
			else {
				puts("Erro, CRC invalido");
				envia_mensagem("", 0, 0, NACK);
			}
		}
		else puts("Recebi uma mensagem que nao eh pra mim");
			}
	free(recebida);
}

void resultado_get(mensagem *recebida, unsigned char *nome_arquivo){
	FILE *get;
	mensagem *recebida2;
	char entrada;
	printf("Tamanho, em bytes, do arquivo: %s\nDeseja continuar?[y/n] ", recebida->dados);
	scanf("%c", &entrada);
	if (entrada != 'y')
		envia_mensagem("", 0, 0, ERRO);
	else {
		get = fopen(nome_arquivo, "w");
		envia_mensagem("", 0, 0, OK);
		do {
			recebida2 = recebe();
			if (recebida2->marca == MARCA) {
				if (recebida2->crc == calcula_crc(recebida2)) {
					envia_mensagem("", 0, 0, ACK);
					if (recebida2->tipo == GET) {
						fwrite(recebida2->dados, 1, recebida2->tamanho, get);
					}
				}
				else {
					puts("Erro, CRC invalido");
					envia_mensagem("", 0, 0, NACK);
				}
			}
			else puts("Recebi uma mensagem que nao eh pra mim");
		} while (recebida2->tipo != FIM);
		fclose(get);
		puts("Arquivo recebido com sucesso!");
		printf("\n");
	}
	free(recebida2);
}