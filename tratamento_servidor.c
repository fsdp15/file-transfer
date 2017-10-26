void servidor_cd(unsigned char *diretorio_cd) {
	int i = chdir(diretorio_cd);
	if (!i) envia_mensagem("", 0, 0, OK);
	else envia_mensagem("", 0, 0, ERRO);
}

void servidor_ls(mensagem *recebida) {
	int i;
	unsigned char dados[59];
	unsigned int tamanho = 1;
	unsigned int sequencia = 0;
	FILE *texto;

	if (!strcmp(recebida->dados, "-la")) 
		system("ls -la > arquivo.txt");
	else if (!strcmp(recebida->dados, "-l"))
		system("ls -l > arquivo.txt");
	else system("ls > arquivo.txt");

	texto = fopen("arquivo.txt", "r");
	while (tamanho) {
		for (i = 0; i < 59; i++)
			dados[i] = 0;
		tamanho = fread(dados, 1, 59, texto);
		if (tamanho) envia_mensagem(dados, tamanho, sequencia, MOSTRA_NA_TELA);
		sequencia++;
		if (sequencia > 31) sequencia = 0;
		}
	envia_mensagem("", 0, 0, FIM);
	system("rm arquivo.txt");
	fclose(texto);
}

void servidor_nome() {
	if ((arquivo = fopen(nome_arquivo, "w")) == NULL)
		envia_mensagem("", 0, 0, ERRO);
	else {
		envia_mensagem("", 0, 0, OK);
	}
}

void servidor_tamanho(mensagem *recebida) {
	char *ptr;
	unsigned long espaco_disponivel = espaco_livre();
	unsigned long tamanho_arquivo = strtol(recebida->dados, &ptr, 10);
	if (espaco_disponivel > tamanho_arquivo) {
		envia_mensagem("", 0, 0, OK);
	}
	else envia_mensagem("", 0, 0, ERRO);
}

void servidor_put(mensagem *recebida) {
	fwrite(recebida->dados, 1, recebida->tamanho, arquivo);
	do {
		recebida = recebe();
		if (recebida->marca == MARCA) {
			if (recebida->crc == calcula_crc(recebida)) {
				envia_mensagem("", 0, 0, ACK);
				if (recebida->tipo == PUT) 
					fwrite(recebida->dados, 1, recebida->tamanho, arquivo);
			}
			else {
				puts("Erro, CRC invalido");
				envia_mensagem("", 0, 0, NACK);
			}

		}
		else puts("Recebi uma mensagem que nao eh pra mim");

	} while (recebida->tipo != FIM);
	fclose(arquivo);
	puts("Arquivo recebido com sucesso!");
	printf("\n");
}

void servidor_get (mensagem *recebida) {
	int recebi = 0 ;
	FILE *get = NULL;
	if ((get = fopen(recebida->dados, "r")) == NULL)
		envia_mensagem("", 0, 0, ERRO);
	else {
		char *tamanho = calcula_tamanho(recebida->dados);
		if (tamanho != NULL) {
			envia_mensagem(tamanho, strlen(tamanho), 0, TAMANHO);
		}
		else envia_mensagem("", 0, 0, ERRO);
	}

	while (!recebi) {
		recebida = recebe();
		if (recebida->marca == MARCA) {
			if (recebida->crc == calcula_crc(recebida)) {
				envia_mensagem("", 0, 0, ACK);
				if (recebida->tipo == ERRO) {
					puts("Erro no recebimento do arquivo");
					recebi = 1;
				}
				else if (recebida->tipo == OK) {
					int i;
					unsigned char dados[59];
					int tamanho = 1;
					int sequencia = 0;

					while (tamanho) {
						for (i = 0; i < 59; i++)
							dados[i] = 0;
						tamanho = fread(dados, 1, 59, get);
						if (tamanho) envia_mensagem(dados, tamanho, sequencia, GET);
							sequencia++;
						if (sequencia > 31) sequencia = 0;
					}
					envia_mensagem("", 0, 0, FIM);
					puts("Arquivo transmitido com sucesso");
					recebi = 1;
					}
				else {
					puts("Erro misterioso");
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
}

void servidor_put_grupo() {
	envia_mensagem("", 0, 0, OK);
	mensagem *recebida, *recebida2, *recebida3;
	do {
		recebida = recebe();
		if (recebida->marca == MARCA) {
			if (recebida->crc == calcula_crc(recebida)) {
				if (recebida->tipo == NOME) {
					envia_mensagem("", 0, 0, ACK);
					strcpy(nome_arquivo, recebida->dados);
					servidor_nome();
					int recebi = 0;
					while (!recebi) {
						recebida2 = recebe();
						if (recebida->tipo == TAMANHO) {
							envia_mensagem("", 0, 0, ACK);
							servidor_tamanho(recebida2);
							int recebi2 = 0;
							while(!recebi2) {
								recebida3 = recebe();
								if (recebida->tipo == PUT) {
									envia_mensagem("", 0, 0, ACK);
									servidor_put(recebida3);
									recebi2 = 1;
								}
							}
							recebi = 1;
						}
					}
				}
			}
			else {
				puts("Erro, CRC invalido");
				envia_mensagem("", 0, 0, NACK);
			}
		}
		else puts("Recebi uma mensagem que nao eh pra mim");
	} while (recebida->tipo != FIM_GRUPO);
	puts("Fim de transmissao de grupo");
}