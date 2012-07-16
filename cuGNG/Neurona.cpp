#include "Neurona.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Neurona Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Neurona::Neurona()
{
	utilizada=false;
	error=0;
    vecinas.erase(vecinas.begin(),vecinas.end());
}

Neurona::~Neurona()
{

}

void Neurona::inicializar_pesos(float lax, float lay, float laz)
{
	pesos[0]=lax;
	pesos[1]=lay;
    pesos[2]=laz;
    return;
}


float Neurona::Distancia_2(float x, float y, float z)
{
	float distancia;
	distancia=(pesos[0]-x)*(pesos[0]-x)+(pesos[1]-y)*(pesos[1]-y)+(pesos[2]-z)*(pesos[2]-z);
	return distancia;
}

void Neurona::ModificarPesos(float x,float y,float z,float coef)
{
 	pesos[0]+=coef*(x-pesos[0]);
	pesos[1]+=coef*(y-pesos[1]);
	pesos[2]+=coef*(z-pesos[2]);
    return;
}

void Neurona::AnadirVecina(int vecina)
{
	VECINAS aux_vecina;

	aux_vecina.vecina=vecina;
	aux_vecina.antiguedad=0;

	vecinas.push_back(aux_vecina);
    return;
}

void Neurona::EliminarVecina(int vecina)
{
	VECTOR::iterator vecina2;

	for (vecina2=vecinas.begin();vecina2!=vecinas.end();vecina2++)
		if (vecina==(*vecina2).vecina)
		{
			vecinas.erase(vecina2);
			return;
		}
    return;
}

void Neurona::IncrementarEdad()
{
	VECTOR::iterator vecina;

	for (vecina=vecinas.begin();vecina!=vecinas.end();vecina++)
		(*vecina).antiguedad++;
    return;
}

void Neurona::CrearArista(int vecina)
{
	VECINAS aux_vecina;
	VECTOR::iterator vecina2;
	char encontrada=false;

	for (vecina2=vecinas.begin();vecina2!=vecinas.end();vecina2++)
	{
		if (vecina==(*vecina2).vecina)
		{
			encontrada=true;
			(*vecina2).antiguedad=0;
			break;
		}
	}

	// Si no existia la arista
	if (encontrada==false)
	{
		aux_vecina.vecina=vecina;
		aux_vecina.antiguedad=0;
		vecinas.push_back(aux_vecina);
	}
  return;
}
