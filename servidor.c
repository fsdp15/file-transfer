unsigned char nome_arquivo[59]; // Variáveis globais usadas para o PUT
FILE *arquivo;

void inicia_modo_servidor() {
	while(1) {
		mensagem *recebida;
		recebida = recebe();

		// Servidor sempre está escutando e envia um ack sempre que identifica uma mensagem

		if (recebida->marca == MARCA) {
			if(recebida->crc == calcula_crc(recebida)) {
				if (recebida->tipo == CD) {
					envia_mensagem("", 0, 0, ACK);
					unsigned char diretorio_cd[59];
					strcpy(diretorio_cd, recebida->dados);
					servidor_cd(diretorio_cd);
				}
				else if (recebida->tipo == LS) {
					envia_mensagem("", 0, 0, ACK);
					servidor_ls(recebida);
				}
				else if (recebida->tipo == NOME) {
					envia_mensagem("", 0, 0, ACK);
					strcpy(nome_arquivo, recebida->dados);
					servidor_nome();
				}
				else if (recebida->tipo == TAMANHO) {
					envia_mensagem("", 0, 0, ACK);
					servidor_tamanho(recebida);
				}
				else if (recebida->tipo == PUT) {
					envia_mensagem("", 0, 0, ACK);
					servidor_put(recebida);
				}
				else if (recebida->tipo == GET) {
					envia_mensagem("", 0, 0, ACK);
					servidor_get(recebida);
				}
				else if (recebida->tipo == PUT_GRUPO) {
					envia_mensagem("", 0, 0, ACK);
					servidor_put_grupo();
				}
			}
			else {
				puts("Servidor recebeu mensagem com CRC invalido");
				envia_mensagem("", 0, 0, NACK);
			}
		}

		else {
		puts("Servidor recebeu uma mensagem com identificacao desconhecida");
	}
		printf("\n");
		free(recebida);
	}
}