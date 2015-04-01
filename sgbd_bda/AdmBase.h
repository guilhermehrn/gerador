/*
 * admBase.h
 *
 *  Created on: Mar 24, 2015
 *      Author: guilherme
 */

#ifndef ADMBASE_H_
#define ADMBASE_H_
#include <string.h>

/**
 *
 */
using namespace std;

/**
 * Struct que define os rotulos ou atributos da base
 */
struct RotuloBase{
	string rotulo;
	int tamanho;
};

/**
 * Struct que define a Gramatica das consultas aceitas
 */
struct Gramatica{
	string palavraReser[4];
	string operadores [8];
};

class AdmBase {

private:

	string consulta;
	Gramatica palavasKey;
	RotuloBase atributos[7];
	string caminhoBase;

public:
	AdmBase(string caminhoBase);

	void modificarConsulta(string consulta);

	string retornarRotuloIndex(int i);

	int retornarIdexRotulo (string rotulo);

	int retornarTamanhoRotulo(string rotulo);

	int retornarTamanhoRotuloIndex(int i);

	void fazerConsulta(string consulta);


	virtual ~AdmBase();

};


#endif /* ADMBASE_H_ */
