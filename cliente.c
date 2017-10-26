void inicia_modo_cliente() {
	unsigned char comando[256];
	printf("\n");
	while(1) {
		printf("> "); 
		fgets (comando, 256, stdin);
		strtok(comando, "\n");
		printf("\n");
		verifica_comando(comando);
	}
}

void verifica_comando(unsigned char *comando) {
	int len = strlen(comando);
	int ret;
	if (len < 3) 
		puts("Comando invalido!"); 
	else {
		if (comando[0] == 'l' && comando[1] == 's') {
			ls(comando);
		}
		else if (comando[0] == 'c' && comando[1] == 'd') {
			cd(comando);
		}
		else if (comando[0] == 'p' && comando[1] == 'u' && comando[2] == 't' && comando[3] == ' ') {
			put(comando);
		}
		else if (comando[0] == 'g' && comando[1] == 'e' && comando[2] == 't' && comando[3] == ' ') {
			get(comando);
		}
		else if (comando[0] == 'p' && comando[1] == 'u' && comando[2] == 't' && comando[3] == 'g' && comando[4] == ' ') {
			put_grupo(comando);
		}
		else puts("Comando invalido!");
	}
}

void ls (unsigned char *comando) {
	if (comando[2] == 'l') {
		if (comando[3] == ' ' && comando[4] == '-' && comando[5] == 'l') {
			if (comando[6] == 'a' && comando[7] == '\0') {
				system("ls -la");
				printf("\n");
			}
			else if (comando[6] == '\0') {
					system("ls -l");
					printf("\n");
			}
			else puts("Comando invalido!");
		}
		else if (comando[3] == '\0') {
					system("ls");
					printf("\n");
		}
		else puts("Comando invalido!");
	}
			
	else if (comando[2] =='r') {
		if (comando[3] == ' ' && comando[4] == '-' && comando[5] == 'l') {
			if (comando[6] == 'a' && comando[7] == '\0') {
				envia_mensagem("-la", 3, 0, LS);
				resultado_ls();
			}
			else if (comando[6] == '\0') {
				envia_mensagem("-l", 2, 0, LS);
				resultado_ls();
			}
			else puts("Comando invalido!");
		}
		else if (comando[3] == '\0') {
			envia_mensagem("", 0, 0, LS);
			resultado_ls();
		}

		else puts("Comando invalido!");
	}

	else puts("Comando invalido!");
}

void cd (unsigned char *comando) {
	unsigned char cd[59];
	int i = 0;
	strcpy(&cd[0], &comando[4]);
	if (comando[2] == 'l' && comando[3] == ' ') {
		chdir(cd);
	}
	else if (comando[2] == 'r' && comando[3] == ' ') {
		envia_mensagem(cd, strlen(cd), 0, CD);
		resultado_cd();
	}
	else puts("Comando invalido!");
}

void put(unsigned char *comando) {
	unsigned char nome_arquivo[59];
	if (comando[0] == 'p' && comando[1] == 'u' && comando[2] == 't' && comando[3] == ' ')
		strcpy(&nome_arquivo[0], &comando[4]);
	else strcpy(&nome_arquivo[0], &comando[0]);
	envia_mensagem(nome_arquivo, strlen(nome_arquivo), 0, NOME);
	mensagem *recebida;
	int recebi = 0;
	while (!recebi) {
		recebida = recebe();
		if (recebida->marca == MARCA) {
			if (recebida->crc == calcula_crc(recebida)) {
				envia_mensagem("", 0, 0, ACK);
				if (recebida->tipo == ERRO) {
					puts("Erro de permissao");
					recebi = 1;
				}
				else if (recebida->tipo == OK) {
					unsigned char *tamanho = calcula_tamanho(nome_arquivo);
					if (tamanho != NULL) {
						char entrada;
						printf("Tamanho, em bytes, do arquivo: %s\nDeseja continuar?[y/n] ", tamanho);
						scanf("%c", &entrada);
						if (entrada == 'y') {
							envia_mensagem(tamanho, strlen(tamanho), 0, TAMANHO);
							resultado_put(nome_arquivo);
						}
						else {
							puts("Transmissao abortada");
							recebi = 1;
						}
					}
					else puts("Arquivo não existe");
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

void get(unsigned char *comando) {
	unsigned char nome_arquivo[59];
	strcpy(&nome_arquivo[0], &comando[4]);
	envia_mensagem(nome_arquivo, strlen(nome_arquivo), 0, GET);
	mensagem *recebida;
	int recebi = 0;
	while (!recebi) {
		recebida = recebe();
		if (recebida->marca == MARCA) {
			if (recebida->crc == calcula_crc(recebida)) {
				envia_mensagem("", 0, 0, ACK);
				if (recebida->tipo == ERRO) {
					puts("Erro: arquivo nao existe");
					recebi = 1;
				}
				if (recebida->tipo == TAMANHO) {
					resultado_get(recebida, nome_arquivo);
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

void put_grupo(unsigned char *comando) {
	unsigned char nome[10][59];
	int j = 0;
	int i = 0;
	int k = 5;
	while (comando[k] != '\0') {
		if (comando[k] == ' ' && comando[k+1] == '-') {
			nome[i][j] = '\0';
			i++;
			j = 0;
			k = k + 3;
		}
		else {
		nome[i][j] = comando[k];
		j++;
		}
	}
	printf("%s\n", nome[0]);
	printf("%s\n", nome[1]);
	envia_mensagem("", 0, 0, PUT_GRUPO);
	mensagem *recebida;
	int recebi = 0;
	while (!recebi) {
		recebida = recebe();
		if (recebida->marca == MARCA) {
			if (recebida->crc == calcula_crc(recebida)) {
				envia_mensagem("", 0, 0, ACK);
				if (recebida->tipo == OK) {
					for (k = 0; k <= i; k++) {
						put(nome[k]);
					}
					envia_mensagem("", 0, 0, FIM_GRUPO);
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
