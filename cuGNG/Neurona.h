#ifndef __NEURONA_H_
#define __NEURONA_H_

#include <vector>

using namespace std;

struct VECINAS
{
	int vecina;
	int antiguedad;
    float distancia;
};

struct reduceSTRUCT2
{
	float distancia1;
	int indice1;
	float distancia2;
	int indice2;
};

struct auxVECINA
{
	int indice_vecina;
	int antiguedad;
};

typedef vector<VECINAS> VECTOR;

class Neurona
{
public:
	Neurona();
	virtual ~Neurona();

	// Variables
	float pesos[3];
	
	char utilizada;
	VECTOR vecinas;
	float error;
	float error_parcial;
	int cont_util;
    int color;

	// Funciones
	void inicializar_pesos(float,float,float);
	float Distancia_2(float,float,float);
	void ModificarPesos(float,float,float,float);
	void AnadirVecina(int);
	void EliminarVecina(int);
	void IncrementarEdad();
	void CrearArista(int);
};

#endif