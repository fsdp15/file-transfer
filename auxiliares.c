const unsigned char CRC7_POLY = 0x91;
 
unsigned char getCRC(unsigned char message[], unsigned char length) 
{
  unsigned char i, j, crc = 0;
 
  for (i = 0; i < length; i++)
  {
    crc ^= message[i];
    for (j = 0; j < 8; j++)
    {
      if (crc & 1)
        crc ^= CRC7_POLY;
      crc >>= 1;
    }
  }
  return crc;
} 

unsigned char calcula_crc(mensagem* nova_mensagem) {
   unsigned char *aux;
   unsigned char *calcular;

   aux = calloc(1, sizeof(mensagem));
   calcular = calloc(1, sizeof(mensagem)-2);

   memcpy(aux, nova_mensagem, sizeof(mensagem));
   memcpy(calcular, aux + 1, sizeof(mensagem) - 2); // Retirando o campo Marca e CRC da mensagem

   return getCRC(calcular, sizeof(mensagem) - 2);
}

unsigned char* calcula_tamanho(unsigned char* nomeArq) { // Calcula o tamanho do arquivo e converte para uma string
  long int size;
  unsigned char* temp;
  temp = calloc(1, 31);
  FILE* arq;
  arq = fopen(nomeArq, "r");
  fseek(arq, 0L, SEEK_END);
  size = ftell(arq);
  sprintf(temp, "%ld", size);
  fclose(arq);
  return temp;
}

long espaco_livre() // Calcula o espaco livre em disco usando starvfs
{
  char cwd[1024];
   if (!(getcwd(cwd, sizeof(cwd)) != NULL)) {
      return -1;
    }

  struct statvfs stat;
  
  if (statvfs(cwd, &stat) != 0) {
    return -1;
  }

  return stat.f_bsize * stat.f_bavail;
}