int envia(unsigned char* buff, int tam) {
	int retorno;
	socklen_t len;
	len = sizeof(struct sockaddr_ll);
	
	retorno = sendto(soquete, (unsigned char *) buff, tam, 0, (struct sockaddr*) &endereco,
			len);
	return retorno;
}

mensagem* recebe() {
	unsigned char local[sizeof(mensagem)];
	unsigned char* vect;
	vect = calloc(1, sizeof(mensagem)); // Buffer para receber os dados

	recv(soquete, (unsigned char *) vect, sizeof(local), 0x0);
	return (mensagem*) vect; // Cast do buffer para o tipo mensagem
}

 void envia_mensagem(unsigned char *dados, int tamanho, int sequencia, int tipo) {
	int tentativas = 0;
	int i, retorno_poll, timeout, size, teste_ack;
	teste_ack = 0;
	retorno_poll = 0;
	struct pollfd pollt; // Polling para checar se o socket estabeleceu conexão
	pollt.fd = soquete;
	pollt.events = POLLIN;
	retorno_poll  = 0;
	timeout = 0;
	clock_t inicio;
	double tempo_gasto = 0.0;

	int len = sizeof(mensagem);

	mensagem *nova_mensagem, *ack;
	unsigned char* buffer;
	buffer = calloc(1, len); // Setando todos os bits do buffer como 0
	nova_mensagem = calloc(1, len); // Setando todos os bits da mensagem como 0
	nova_mensagem->marca = MARCA;
	nova_mensagem->tamanho = tamanho;
	nova_mensagem->sequencia = sequencia;
	nova_mensagem->tipo = tipo;

	for (i = 0; i < tamanho; i++) {
		nova_mensagem->dados[i] = dados[i];
	}

	nova_mensagem->crc = calcula_crc(nova_mensagem);
	memcpy(buffer, nova_mensagem, len);

	if (tipo == ACK || tipo == NACK) {
		envia(buffer, sizeof(mensagem)); 
	}

		/* Como o ack não precisa de resposta da outra máquina, apenas o envia.
		Se ele falhar, há timeout */

	 else { // Para outras mensagens há o para e espera. Envia a mensagem e espera um ack
		while (!teste_ack) {
			while(!retorno_poll && tentativas <= 20) {
				inicio = clock();
				envia(buffer, sizeof(mensagem)); 
				retorno_poll = poll(&pollt, 1, 1000); // Verifica retorno e timeout
				if (retorno_poll == 0) {
					printf("Erro na tentativa de conexao %d de 20\n", tentativas);
					tentativas++;
				}
				// Como retorno_poll pode ser 1 mesmo sem conexão entre as duas máquinas (por interferência no cabo, por exemplo),
				// há um timer que verifica se já faz muito tempo que o ack não veio
			}
			if (tentativas > 20) {
				puts("Erro: nao conseguiu estabelecer conexao");
				exit(0);
			}

			ack = recebe();

			tempo_gasto = (double)(clock() - inicio) / CLOCKS_PER_SEC;
			if (ack->marca == MARCA && ack->tipo == ACK) teste_ack = 1;
			 else if (tempo_gasto > 0.0001)  {
				printf("Erro na tentativa de conexao %d de 20\n", tentativas);
				tentativas++;
				tempo_gasto = 0.0;
				retorno_poll = 0;
			}  
		}
	}
} 