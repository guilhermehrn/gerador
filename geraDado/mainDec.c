/*
 Gerar dados aleatórios para alimentar o banco (6 bilhões de pessoas):
 Sexo (F/M) - 1 bit (oculto)
 Idade (0-127) 7 bits
 Renda Mensal (0-1023) 10 bits
 Escolaridade (0-3) 2 bits
 Idioma (0-4095) 12 bits
 País (0-255) 8 bits (oculto)
 Localizador (coordenadas) 24 bits

 Dados a serem informados:
 -Espaço (tamanho do arquivo binário e do banco de dados)
 -Tempo (Tempo de consulta das consultas, 1ª vez e fazer a média de 3)

 Formato dos dados:

 Posição: 0 1234567 8901234567 89 012345678901 23456789 012345678901234567890123
 Binário: 1 1011001 0000000000 10 000111100011 10000010 000000000100100011010001
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

/* --------------------------------------------------------------------------------
 Função que printa na tela como os parâmetros devem ser passados para o programa.
 ------------------------------------------------------------------------------*/
void instrucoes() {
	fprintf(stdout, "\nPrograma que gera um arquivo de dados binários.");
	fprintf(stdout,
			"\nMétodo de uso:\n./geraBase\n-f <máximo de tuplas por arquivo>");
	fprintf(stdout,
			"\n-i <máximo de tupla por INSERT> \n-q <quantidade de tuplas a serem geradas>\n");
	exit(EXIT_SUCCESS);
}

/* --------------------------------------------------------------------------------
 Função que recebe os parâmetros do programa.
 Recebe: a quantidade de parâmetros, a string de parâmetros, a quantidade de
 tuplas a serem geradas, uma variável para sinalizar se a saída tem que estar
 ordenada e a o nome do arquivo de saída.
 ------------------------------------------------------------------------------*/
void recebeParametros(int argc, char* argv[], long int* qtTuplas,
		int* maxTuplasInsert, int* maxTuplasFile) {

	int opt;

	/* Enquanto houver parâmetros a serem lidos */
	while ((opt = getopt(argc, argv, "i:I:f:F:q:Q:hH")) != -1) {
		switch (opt) {
		/* Caso o usuário solicite as instruções */
		case 'h':
		case 'H':
			instrucoes();
			break;

			/* Quantidade de tuplas */
		case 'q':
		case 'Q':
			*qtTuplas = atoi(optarg);
			break;

			/* Quantidade máxima de tuplas em arquivo */
		case 'f':
		case 'F':
			*maxTuplasFile = atoi(optarg);
			break;

			/* Quantidade máxima de tupla por INSERT */
		case 'i':
		case 'I':
			*maxTuplasInsert = atoi(optarg);
			break;

			/* Qualquer outro parâmetro */
		case '?':
			fprintf(stderr,
					"ERRO: Parâmetro desconhecido. Use -h para ajuda.\n");
			exit(EXIT_FAILURE);
		}
	}

	if (*qtTuplas == 0) {
		fprintf(stdout,
				"\nQuantidade de tuplas não foi especificado. Use o parâmetro -q.\n");
		exit(EXIT_FAILURE);
	}

	if (maxTuplasInsert > maxTuplasFile) {
		fprintf(stderr,
				"A quantidade de tupla em INSERT não pode ser menor que a quantidade de tupla em Arquivo.\n");
		exit(EXIT_FAILURE);
	}
}

/* --------------------------------------------------------------------------------
 Função que retorna a quantidade de dígitos de um número.
 Entrada: um inteiro.
 Retorna: um inteiro com a quantidade de dígitos.
 ------------------------------------------------------------------------------*/
int numeroDigitos(int numero) {
	int i;
	for (i = 0; numero > 0; i++)
		numero /= 10;
	return i;
}

/* --------------------------------------------------------------------------------
 Função que transforma um número em uma string.
 Entrada: um inteiro.
 Retorna: a string equivalente ao número.
 ------------------------------------------------------------------------------*/
char* intToChar(int numero) {

	char* palavra; /* String para guardar o número escrito */

	/* Alocando memória para a quantidade exata de dígitos */
	palavra = malloc(sizeof(char) * numeroDigitos(numero));
	sprintf(palavra, "%d", numero);
	return palavra;
}

/*-------------------------------------------------------------------------------
  Função que transforma um numero inteiro em uma string com o binario correspodente
  Entrada: um inteiro longo a ser trasformado, e o tamanho do binario de saida.
  Retorna: a string com binario equivalente.
 --------------------------------------------------------------------------------*/
void intToBinario(unsigned int numero, int tamanho, char *binario) {
	int i, resto, tam;
	//char *aux = (char*) malloc (sizeof(char) * 25);
	char *aux2=(char*) malloc (sizeof(char) * 2);

	tam = tamanho;
	for (i = 0; i < 25; i++) {
		binario[i] = '0';
	}
	if (numero == 1) {
		binario[tamanho - 1] = '1';
	} else {
		do {
			tam--;
			resto = numero % 2;
			sprintf(aux2, "%d", resto);
			binario[tam] = aux2[0];

			numero = numero / 2;

		} while (numero >= 1);
	}

	for (i = tamanho; i < 25; i++) {
		binario[i] = '\0';
	}
	free(aux2);
}
/* --------------------------------------------------------------------------------
 Função que gera a base de tuplas.
 Recebe: a
 ------------------------------------------------------------------------------*/
void geraBaseTupla(long int qtTuplas, int maxTuplasInsert, int maxTuplasFile) {
	int i, j;
	int paisAtual = 0; /* País do qual os registros estão sendo impressos */
	int contadorSQL = 1; /* Contador do número do arquivo em qual o dados estão sendo gravados */
	int fator = qtTuplas / 10; /* Variável para acompanhar a criação dos dados */
	unsigned long int dado = 1; /* Variável para acompanhar a criação dos dados */
	unsigned int aleatorioPais; /* Variável que vai armazenar um país aleatório */
	unsigned int aleatorioSexo; /* Variável que vai armazenar um sexo aleatório */
	int separador = 0; /* Variável que vai controlar se um separador homem/mulher já foi usado em cada arquivo */
	unsigned long int aux; /*auxliar para ajudar nas operações bit a bit*/
	unsigned long int aux2;/*auxliar para ajudar nas operações bit a bit*/
	unsigned long int buff[1];/*buf para armazenar no arquivo binario*/


	FILE* bin; /* Ponteiro para os arquivos binários */
	FILE* sql; /* ponteiro para os arquivos SQL */

	/*usado para guarda uma string de binarios*/
	char *binario = (char *) malloc(sizeof(char) * 25);

	/* Vetor para fazer o sorteo da quantidade de tuplas de cada país */
	int* pais = (int *) malloc(sizeof(int) * 256);

	/* Vetor para fazer o sorteo da quantidade de tuplas de cada sexo */
	long int* sexoPais = (long int *) malloc(sizeof(long int) * 256);

	/* String usada para abrir os arquivos binários */
	char* saidaBin = (char *) malloc(sizeof(char) * 20);

	/* String usada para abrir os arquivos SQL */
	char* saidaSQL = (char *) malloc(sizeof(char) * 20);

	/* Alimentando o fator aleatoriedade com o tempo atual */
	srand((unsigned) time(NULL));

	/* Zerando o vetor com a quantidade de registros por país
	 e o vetor com a quantidade de mulheres por país */
	for (j = 0; j < 256; j++) {
		pais[j] = 0;
		sexoPais[j] = 0;
	}

	/* Sorteando a quantidade de registros de cada país */
	for (j = 0; j < qtTuplas; j++) {
		aleatorioPais = rand() % 256;
		pais[aleatorioPais] = pais[aleatorioPais] + 1;

		aleatorioSexo = rand() % 2;
		if (aleatorioSexo == 1)
			sexoPais[aleatorioPais] = sexoPais[aleatorioPais] + 1;
	}

	/* Achando o primeiro país com registros */
	while (!pais[paisAtual])
		paisAtual++;
/*
	for (j = 0; j < 256; j++)
		printf("%d ", pais[j]);
	printf("\n\n");

	for (j = 0; j < 256; j++)
		printf("%lu ", sexoPais[j]);
	printf("\n\n");
*/
	/* Abrindo o primeiro arquivo de registros binários */
	strcpy(saidaBin, "bin/");
	strcat(saidaBin, intToChar(paisAtual));
	strcat(saidaBin, ".pais");

	bin = fopen(saidaBin, "wb");

	/* Abrindo o primeiro arquivo de registros sql */
	sql = fopen("sql/0.sql", "w");

	/* Enquanto não atingir a quantidade de tuplas solicitada pelo usuário */
	for (i = 0; i < qtTuplas; i++) {
		/* Essa condicional faz o controle de arquivo em que os registro vão ser guardados,
		 os registro são armazenados nos arquivo de seus países */
		/* Se não houver mais registros para esse país */
		if (!pais[paisAtual]) {
			/* Fechando o arquivo, pois não há mais registros a gravar nele */
			fclose(bin);

			/* Procura o próximo país com algum registro */
			while (pais[paisAtual] == 0)
				paisAtual++;

			//printf("<<<%d>>>\n", paisAtual); // Use esse print para ver os país que tem registro(s)

			/* Verificando se não ultrapassou a quantidade de países, não deve cair aqui */
			if (paisAtual > 256) {
				printf("ERRO: número do país atual ultrapassou o limite.\n");
				exit(EXIT_FAILURE);
			}

			/* Abrindo o arquivo que tem registro */
			strcpy(saidaBin, "bin/");
			strcat(saidaBin, intToChar(paisAtual));
			strcat(saidaBin, ".pais");
			bin = fopen(saidaBin, "wb");

			/* Zera o separador para poder colocar o mesmo no próximo arquivo */
			separador = 0;
		}

		/* Essa parte cria um novo arquivo SQL para que não fiquem muito grande */

		if (i % maxTuplasFile == 0 && i != 0) {
			/* Colocando o fim ao último arquivo sql */
		//	fprintf(sql, ";\n");

			/* fechando o arquivo sql para começar um novo */
		fclose(sql);

			/* Abrindo um arquivo sql */
			strcpy(saidaSQL, "sql/");
			strcat(saidaSQL, intToChar(contadorSQL));
			strcat(saidaSQL, ".sql");
			sql = fopen(saidaSQL, "w");

			contadorSQL ++;

		//	fprintf(sql,
		//			"INSERT INTO pessoas (sexo,idade,renda_mensal,escolaridade,idioma,pais,localizador) VALUES ");
		}

		/* Verificando se alcançou o máximo de tuplas por INSERT */
		//else if (i % maxTuplasInsert == 0) {
		//	if (i != 0)
		//		fprintf(sql, ";\n");
		//	fprintf(sql,
		//			"INSERT INTO pessoas (sexo,idade,renda_mensal,escolaridade,idioma,pais,localizador) VALUES ");
		//}

	//	else
		//	fprintf(sql, ",");

		/* Imprime em tela a quantidade de tuplas já processadas */
		if (i % fator == 0 && i != 0)
			printf("(%d tuplas)\n", i);

		/* Separador no binário entre homens e mulheres */
		//if (!sexoPais[paisAtual] && !separador) {
			//fprintf(bin, "\n");
		//	separador = 1;
		//}

		aux = 0;
		aux2 = 0;
		dado = 0;

		/* Sexo */
		dado = rand() % 2;
		aux2 = dado;
		aux2 = aux2 << 63;
		aux = aux | aux2;
		intToBinario(dado, 1,binario);

		fprintf(sql, "%s,", binario);
		//fprintf (bin,"%u ", dado);

		/* Idade */
		dado = 0;
		aux2 = 0;

		dado = rand() % 128;
		aux2 = dado;
		aux2 = aux2 << 56;
		aux = aux | aux2;
		intToBinario(dado, 7,binario);

		fprintf(sql, "%s,", binario);
		//fprintf(bin, "%u ", dado);

		/* Renda */
		dado = 0;
		aux2 = 0;
		dado = rand() % 1024;
		aux2 = dado;
		aux2 = aux2 << 46;
		aux = aux | aux2;
		intToBinario(dado, 10,binario);
		fprintf(sql, "%s,", binario);
		//fprintf(bin, "%u ", dado);

		/* Escolaridade */
		dado = 0;
		aux2 = 0;
		dado = rand() % 4;
		aux2 = dado;
		aux2 = aux2 << 44;
		aux = aux | aux2;
		intToBinario(dado, 2,binario);
		fprintf(sql, "%s,", binario);
		//fprintf(bin, "%u ", dado);

		/* Idioma */
		dado = 0;
		aux2 = 0;
		dado = rand() % 4096;
		aux2 = dado;
		aux2 = aux2 << 32;
		aux = aux | aux2;
		intToBinario(dado, 12,binario);
		fprintf(sql, "%s,", binario);
		//fprintf(bin, "%u ", dado);

		/* País */
		dado = 0;
		aux2 = 0;
		dado = paisAtual;
		aux2 = dado;
		aux2 = aux2 << 24;
		aux = aux | aux2;
		intToBinario(dado, 8,binario);
		fprintf(sql, "%s,", binario);
		//fprintf (bin, "%u ", dado);

		/* Localizador */
		dado = 0;
		aux2 = 0;
		dado = rand() % 16777216;
		aux2 = dado;
		aux = aux | aux2;
		intToBinario(dado, 24,binario);
		fprintf(sql, "%s", binario);
		//fprintf(bin, "%u\n", dado);

		buff[0] = aux;
		long int g = aux;
		//printf("%lx", g);
		fwrite(&buff[0], 8, 1, bin);

		fprintf(sql, "\n");

		/* Decrementando a quantidade de registros de um país */
		pais[paisAtual] = pais[paisAtual] - 1;

		sexoPais[paisAtual] = sexoPais[paisAtual] - 1;
	}

	//fprintf(sql, ";\n");
	fclose(sql);
	fclose(bin);

	free(saidaBin);
	free(saidaSQL);
	free(pais);
}

int main(int argc, char* argv[]) {
	long int qtTuplas = 6000000000; /*Quantidade de registros a serem gerados*/
	int maxTuplasInsert = 2000; /*Máximo de tuplas por INSERT*/
	int maxTuplasFile = 1000000000;/*Máximo de tuplas por arquivo*/
	double tInicial; /* Variável para armazenar o tempo inicial */
	struct timeval tempo; /* Estrutura para medir o tempo */
	double tFinal; /* Variável para armazenar o tempo final */

	/* Verificando se foram passados parâmetros para o programa */
	//if (argc < 2) {
	//	instrucoes();
	//}

	/* Lendo os parâmetros recebidos */
	//recebeParametros(argc, argv, &qtTuplas, &maxTuplasInsert, &maxTuplasFile);

	/* Arquivo para armazenar o tempo final */
	FILE* resultadoTempo = fopen("resultadoTempo.txt", "w");

	/* Início da cronometragem de tempo da etapa de Descritor */
	gettimeofday (&tempo, NULL);
	tInicial = tempo.tv_sec + (tempo.tv_usec / 1000000.0);

	printf("Iniciando a criação do dataset...\n");

	/* removendo possível arquivos de outras execuções */
	system("rm -R -f bin sql");
	/* criando pasta para as bases */
	system("mkdir -p bin sql");

	/* função que gera o arquivo com o dados em hexadecimal e em SQL */
	geraBaseTupla(qtTuplas, maxTuplasInsert, maxTuplasFile);

	/* Fim da cronometragem */
	gettimeofday(&tempo, NULL);
	tFinal = tempo.tv_sec + (tempo.tv_usec / 1000000.0);

	/* Imprimindo resultado de tempo */
	fprintf(resultadoTempo,
			"Base gerada com sucesso.\nQuantidade de tuplas: %lu\n", qtTuplas);
	fprintf(resultadoTempo, "Tempo: %.4lf segundos.\n\n", tFinal - tInicial);
	fprintf(resultadoTempo,
			"Máximo de tuplas:\n-%d por arquivo.\n-%d por INSERT.\n",
			maxTuplasFile, maxTuplasInsert);

	printf("Fim do processo de criação.\n");

	fclose(resultadoTempo);

	return 0;
}
