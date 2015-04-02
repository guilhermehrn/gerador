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

int binToInt(char* bin) {
	int tam, fator, inteiro;
	tam = strlen(bin);
	fator = 1;
	inteiro = 0;

	printf("%d\n", tam);

	for (; tam > 0; tam--) {
		if (bin[tam - 1] - 48)
			inteiro += fator;

		fator = 2 * fator;
	}

	return inteiro;
}

/* --------------------------------------------------------------------------------
 Função que retorna a quantidade de dígitos de um número.
 Entrada: um inteiro.
 Retorna: um inteiro com a quantidade de dígitos.
 ------------------------------------------------------------------------------*/
int numeroDigitos(int numero) {
	int i;

	/* Avaliando a quantidade de divisões possíveis por 10 */
	for (i = 0; numero > 0; i++)
		numero /= 10;

	return i;
}

/* --------------------------------------------------------------------------------
 Função que transforma um número em uma string.
 Entrada: um inteiro.
 Retorna: a string equivalente ao número.
 ------------------------------------------------------------------------------*/
char*
intToChar(int numero) {
	/* String para guardar o número escrito */
	char* palavra;

	/* Alocando memória para a quantidade exata de dígitos */
	palavra = malloc(sizeof(char) * numeroDigitos(numero));

	sprintf(palavra, "%d", numero);

	return palavra;
}

char* intToBinario(unsigned int numero, int tamanho) {
	int i, resto, tam;
	char *aux = (char*) malloc(sizeof(char) * 25);
	char *aux2 = (char*) malloc(sizeof(char) * 2);

	tam = tamanho;
	for (i = 0; i < 25; i++) {
		aux[i] = '0';
	}
	if (numero == 1) {
		aux[tamanho - 1] = '1';
	} else {
		do {
			tam--;
			resto = numero % 2;
			sprintf(aux2, "%d", resto);
			aux[tam] = aux2[0];

			numero = numero / 2;

		} while (numero >= 1);
	}

	for (i = tamanho; i < 25; i++) {
		aux[i] = '\0';
	}
	return aux;
}


/* --------------------------------------------------------------------------------
 Função que gera a base de tuplas.
 Recebe: a
 ------------------------------------------------------------------------------*/
void geraBaseTupla(long int qtTuplas, int maxTuplasInsert, int maxTuplasFile) {
	int i, j;
	/* País do qual os registros estão sendo impressos */
	int paisAtual = 0;
	/* Contador do número do arquivo em qual o dados estão sendo gravados */
	int contadorSQL = 1;
	/* Variável para acompanhar a criação dos dados */
	int fator = qtTuplas / 10;
	/* Variável para colocar os dados nas tuplas */
	unsigned int dado = 1;
	/* Variável que vai armazenar um país aleatório */
	unsigned int aleatorioPais;
	/* Variável que vai armazenar um sexo aleatório */
	unsigned int aleatorioSexo;
	/* Variável que vai controlar se um separador homem/mulher já foi usado em cada arquivo */
	int separador = 0;


	char *binario = (char *) malloc(sizeof(char) * 25);


	/* Vetor para fazer o sorteo da quantidade de tuplas de cada país */
	int* pais = malloc(sizeof(int) * 256);

	/* Vetor para fazer o sorteo da quantidade de tuplas de cada sexo */
	long int* sexoPais = malloc(sizeof(long int) * 256);

	/* String usada para abrir os arquivos binários */
	char* saidaBin = malloc(sizeof(char) * 20);
	/* String usada para abrir os arquivos SQL */
	char* saidaSQL = malloc(sizeof(char) * 20);

	/* Ponteiro para os arquivos binários */
	FILE* bin;
	/* ponteiro para os arquivos SQL */
	FILE* sql;

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

	for (j = 0; j < 256; j++)
		printf("%d ", pais[j]);
	printf("\n\n");

	for (j = 0; j < 256; j++)
		printf("%lu ", sexoPais[j]);
	printf("\n\n");

	/* Abrindo o primeiro arquivo de registros binários */
	strcpy(saidaBin, "bin/");
	strcat(saidaBin, intToChar(paisAtual));
	strcat(saidaBin, ".pais");
	bin = fopen(saidaBin, "w");

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

			//printf("%d\n", paisAtual); // Use esse print para ver os país que tem registro(s)

			/* Verificando se não ultrapassou a quantidade de países, não deve cair aqui */
			if (paisAtual > 256) {
				printf("ERRO: número do país atual ultrapassou o limite.\n");
				exit(EXIT_FAILURE);
			}

			/* Abrindo o arquivo que tem registro */
			strcpy(saidaBin, "bin/");
			strcat(saidaBin, intToChar(paisAtual));
			strcat(saidaBin, ".pais");
			bin = fopen(saidaBin, "w");

			/* Zera o separador para poder colocar o mesmo no próximo arquivo */
			separador = 0;
		}

		/* Essa parte cria um novo arquivo SQL para que não fiquem muito grande */
		if (i % maxTuplasFile == 0 && i != 0) {
			/* Colocando o fim ao último arquivo sql */
			fprintf(sql, ";\n");

			/* fechando o arquivo sql para começar um novo */
			fclose(sql);

			/* Abrindo um arquivo sql */
			strcpy(saidaSQL, "sql/");
			strcat(saidaSQL, intToChar(contadorSQL));
			strcat(saidaSQL, ".sql");
			sql = fopen(saidaSQL, "w");

			fprintf(sql,
					"INSERT INTO pessoas (sexo,idade,renda_mensal,escolaridade,idioma,pais,localizador) VALUES ");
		}

		/* Verificando se alcançou o máximo de tuplas por INSERT */
		else if (i % maxTuplasInsert == 0) {
			if (i != 0)
				fprintf(sql, ";\n");
			fprintf(sql,
					"INSERT INTO pessoas (sexo,idade,renda_mensal,escolaridade,idioma,pais,localizador) VALUES ");
		}

		else
			fprintf(sql, ",");

		/* Imprime em tela a quantidade de tuplas já processadas */
		if (i % fator == 0 && i != 0)
			printf("(%d tuplas)\n", i);

		/* Separador no binário entre homens e mulheres */
		if (!sexoPais[paisAtual] && !separador) {
			fprintf(bin, "\n");
			separador = 1;
		}

		/* Sexo */
		dado = rand() % 2;
		binario = intToBinario(dado,1);
		fprintf(sql, "(b'%s',", binario);
		//fprintf (bin,"%X ", dado);

		/* Idade */
		dado = rand() % 128;
		binario = intToBinario(dado,7);
		fprintf(sql, "b'%s',", binario);
		//fprintf(bin, "%X ", dado);

		/* Renda */
		dado = rand() % 1024;
		binario = intToBinario(dado,10);
		fprintf(sql, "b'%s',", binario);
		//fprintf(bin, "%X ", dado);

		/* Escolaridade */
		dado = rand() % 4;
		binario = intToBinario(dado,2);
		fprintf(sql, "b'%s',", binario);
		//fprintf(bin, "%X ", dado);

		/* Idioma */
		dado = rand() % 4096;
		binario = intToBinario(dado,12);
		fprintf(sql, "b'%s',", binario);
		//fprintf(bin, "%X ", dado);

		/* País */
		dado = rand() % 256;
		binario = intToBinario(dado,8);
		fprintf(sql, "b'%s',", binario);
		//fprintf (bin, "%X ", dado); //Não precisa pois os registros estão separados por esse valor

		/* Localizador */
		dado = rand() % 16777216;
		binario = intToBinario(dado,24);
		fprintf(sql, "b'%s')", binario);
		//fprintf(bin, "%X\n", dado);

		/* Decrementando a quantidade de registros de um país */
		pais[paisAtual] = pais[paisAtual] - 1;

		sexoPais[paisAtual] = sexoPais[paisAtual] - 1;
	}

	fprintf(sql, ";\n");
	fclose(sql);
	fclose(bin);

	free(saidaBin);
	free(saidaSQL);
	free(pais);
}

int main(int argc, char* argv[]) {
	/* Quantidade de registros a serem gerados */
	long int qtTuplas = 0;
	/* Máximo de tuplas por INSERT */
	int maxTuplasInsert = 2000;
	/* Máximo de tuplas por arquivo */
	int maxTuplasFile = 5000000;

	/* Verificando se foram passados parâmetros para o programa */
	if (argc < 2)
		instrucoes();

	/* Lendo os parâmetros recebidos */
	recebeParametros(argc, argv, &qtTuplas, &maxTuplasInsert, &maxTuplasFile);

	/* Estrutura para medir o tempo */
	struct timeval tempo;
	/* Variável para armazenar o tempo inicial */
	double tInicial;
	/* Variável para armazenar o tempo final */
	double tFinal;
	/* Arquivo para armazenar o tempo final */
	FILE* resultadoTempo = fopen("resultadoTempo.txt", "w");

	/* Início da cronometragem de tempo da etapa de Descritor */
	gettimeofday(&tempo, NULL);
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
