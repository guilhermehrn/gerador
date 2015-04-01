/*
 * admBase.cpp
 *
 *  Created on: Mar 24, 2015
 *      Author: guilherme
 */

#include "AdmBase.h"

AdmBase::AdmBase(string caminhoBase) {
	//incializa a consulta.
	this->consulta = "select from";

	//inicializa o caminho da base
	this->caminhoBase = caminhoBase;

	//Inicializa os rotulos da tabela
	this->atributos[0].rotulo = "sexo";
	this->atributos[0].tamanho = 1;

	this->atributos[1].rotulo = "idade";
	this->atributos[1].tamanho = 7;

	this->atributos[2].rotulo = "rendamensal";
	this->atributos[2].tamanho = 10;

	this->atributos[3].rotulo = "escolaridade";
	this->atributos[3].tamanho = 2;

	this->atributos[4].rotulo = "idioma";
	this->atributos[4].tamanho = 12;

	this->atributos[5].rotulo = "pais";
	this->atributos[5].tamanho = 8;

	this->atributos[6].rotulo = "localizador";
	this->atributos[6].tamanho = 24;

	//inicializa a estrutura de gramatica com as paralavras chaves aceitas
	this->palavasKey.palavraReser[0] = "select";
	this->palavasKey.palavraReser[2] = "from";
	this->palavasKey.palavraReser[3] = "where";
	this->palavasKey.palavraReser[4] = "group by";

	//inicializa a estutura gramatica com os operadires.
	this->palavasKey.operadores[0] = "and";
	this->palavasKey.operadores[1] = "or";
	this->palavasKey.operadores[2] = "<=";
	this->palavasKey.operadores[3] = "=>";
	this->palavasKey.operadores[4] = "=";
	this->palavasKey.operadores[6] = "count(*)";
	this->palavasKey.operadores[7] = "avg";
}

void AdmBase::modificarConsulta(string consulta) {
	this->consulta = consulta;
}

string AdmBase::retornarRotuloIndex(int i) {
	return this->atributos[i].rotulo;
}

int AdmBase::retornarIdexRotulo(string rotulo) {
	int aux = -1;
	for (int i = 0; i < 7; i++) {
		if (this->atributos[i].rotulo == rotulo) {
			aux = i;
		}
	}
	return aux;
}

int AdmBase::retornarTamanhoRotulo(string rotulo) {
	int i = retornarIdexRotulo(rotulo);
	return this->atributos[i].tamanho;
}

int AdmBase::retornarTamanhoRotuloIndex(int i) {
	return this->atributos[i].tamanho;
}

AdmBase::~AdmBase() {
// TODO Auto-generated destructor stub
}

