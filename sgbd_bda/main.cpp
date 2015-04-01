/*
 * main.cpp
 *
 *  Created on: Mar 24, 2015
 *      Author: guilherme
 */
#include<iostream>
#include<fstream>
#include<string>

using namespace std;

int main(int argc, char *argv[]) {

	cout<<"TP1 DE BANCO DE DADOS AVANÇADO"<<endl;
	cout<<"SISTEMAS DE IFORMAÇÂO DCC"<<endl;
	cout<<"Nomes: Guilherme Henrique Rodrigues Nascimento"<<endl;
	cout<<"       Italo"<<endl<<endl;

	cout<<"1>SELECT país, sexo, count(*)\n  FROM pessoas \n  GROUP BY país, sexo;"<<endl<<endl;
	cout<<"2>SELECT país, sexo,idade, count(*) \n  FROM pessoas \n  GROUP BY país, sexo, idade;"<<endl<<endl;
	cout<<"3>SELECT país, sexo, avg(salario) \n  FROM pessoas \n  GROUP BY país, sexo;"<<endl<<endl;
	cout<<"4>SELECT país, sexo, avg(idade) \n  FROM pessoas \n  GROUP BY país, sexo;"<<endl<<endl;
	cout<<"5>SELECT país, sexo, count(*) \n  FROM pessoas \n  WHERE país = 15 \n  GROUP BY país, sexo;"<<endl<<endl;
	cout<<"6>SELECT país, sexo, count(*) \n  FROM pessoas \n  WHERE país = 15 AND sexo = 1;"<<endl<<endl;
	cout<<"7>SELECT país, sexo, count(*) \n  FROM pessoas \n  WHERE país >=0 AND país <=15 \n  GROUP BY país, sexo;"<<endl<<endl;

	int a;
	cout<<"Insira algum valor"<<endl;
	cin>>a;

	return 0;
}
