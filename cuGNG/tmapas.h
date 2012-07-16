//---------------------------------------------------------------------------
#ifndef tmapasH
#define tmapasH
//---------------------------------------------------------------------------
//#endif

#include <stdlib.h>
#include <vector>
#include <time.h>
#include <math.h>
#include <opencv/cv.h>
#include "Neurona.h"
#include "ColorRGB.h"

using namespace std;

#define MAX_COLOR 255
#define COLOR 0
#define TRUE 1
#define FALSE 0
#define PI 3.1415926
#define ALEAT(x) (int)(((float)rand()/RAND_MAX)*x)

//#define MAX_NEURONAS 1000

class Neurona;

typedef struct estructuraelementoLista
{
  float x,y,z;
} patron;

class ConfiguracionMapa
{
	public:

		// Default constructor
		ConfiguracionMapa()
		{}

		ConfiguracionMapa(int MAX_NEURONAS,bool cudaEnabled,bool hibrido,int device_id,int tipo,int NUM_PUNTOS,float PORC_APRENDIZAJE,
			int NUM_ENTRADAS,int MAX_ANTIGUEDAD,float COEF_GANADORA,float COEF_VECINAS,float ALFA,float BETA,float COEF_TODAS,	int UMBRAL_DATOS) : 
				MAX_NEURONAS(MAX_NEURONAS),cudaEnabled(cudaEnabled),hibrido(hibrido),device_id(device_id),tipo(tipo),NUM_PUNTOS(NUM_PUNTOS),PORC_APRENDIZAJE(PORC_APRENDIZAJE),
				NUM_ENTRADAS(NUM_ENTRADAS), MAX_ANTIGUEDAD(MAX_ANTIGUEDAD), COEF_GANADORA(COEF_GANADORA), COEF_VECINAS(COEF_VECINAS), ALFA(ALFA), BETA(BETA), COEF_TODAS(COEF_TODAS), UMBRAL_DATOS(UMBRAL_DATOS)
		  {
			LIM_TIEMPO_NG=0;
			ANCHO_MAPA=0;
			ALTO_MAPA=0;
			MAX_ITERACION=0;
			LIMITE=0;
			LAMBDA_I=0.0;
			LAMBDA_F=0.0;
			EPSILON_I=0.0;
			EPSILON_F=0.0;
			EDAD_I=0.0;
			EDAD_F=0.0;
			UMBRAL=0.0;

			TAM_INSERCION=0;
			LIMITE_TIEMPO=0;
			AUTOCOMPLETAR=false;
			ELIMINAR_ARISTAS_INCORRECTAS=false;
		  }

		ConfiguracionMapa(int defaultTypeParameters)
		{
			if( defaultTypeParameters == 1 )
			{
				// parametros de aprendizaje
				PORC_APRENDIZAJE=1;
				ELIMINAR_ARISTAS_INCORRECTAS=false;
	
				// Num entradas
				NUM_ENTRADAS=500;
				COEF_GANADORA=(float)0.1;
				COEF_VECINAS=(float)0.01;
				ALFA=0.0;
				BETA=0.0;

				// parametros configuracion NG
				MAX_NEURONAS=300;
				MAX_ITERACION=MAX_NEURONAS;
				LAMBDA_I=(float) MAX_NEURONAS;
				LAMBDA_F=0.01;
				EPSILON_I=0.5;
				EPSILON_F=0.0;
				EDAD_I=20;
				EDAD_F=200;

				COEF_TODAS=(float)0;
				UMBRAL_DATOS=127;
				//--------------------

				LIM_TIEMPO_NG=0;
				ANCHO_MAPA=7;//10;
				ALTO_MAPA=7;//10;
				LIMITE=100;
				NUM_PUNTOS=0;
				MAX_ANTIGUEDAD=120;
				UMBRAL=80;

				cudaEnabled=false;
				hibrido=false;
			}
		}

		virtual ~ConfiguracionMapa(){};
		 
		// Variables
		
		int MAX_NEURONAS;
		bool cudaEnabled;
		bool hibrido;
		int device_id;
		int tipo;
		int NUM_PUNTOS;

		float PORC_APRENDIZAJE;
		int NUM_ENTRADAS;
		int MAX_ANTIGUEDAD;
		float COEF_GANADORA;
		float COEF_VECINAS;
		float ALFA;
		float BETA;
		float COEF_TODAS;
		int UMBRAL_DATOS;
		//--------------------

		int LIM_TIEMPO_NG;
		int ANCHO_MAPA;
		int ALTO_MAPA;
		int MAX_ITERACION;
		int LIMITE;
		float LAMBDA_I;
		float LAMBDA_F;
		float EPSILON_I;
		float EPSILON_F;
		float EDAD_I;
		float EDAD_F;

		int UMBRAL;
		//int ANCHO;
		//int ALTO;

		// variables de configuracion de la GNG
		int TAM_INSERCION;
		int LIMITE_TIEMPO;
		bool AUTOCOMPLETAR;
		bool ELIMINAR_ARISTAS_INCORRECTAS;
};

class Mapa
{
public:
    // funciones generales de creacion/destruccion y cargar y abrir los mapas
	virtual ~Mapa();
	Mapa(int maxneuronas,bool cudaEnabled,bool hibrido,int num_puntos,int thread_id);
	Mapa(Mapa *,int);
    void Inicializar();
    void Abrir(const char *nombre);
    void Guardar(char *nombre);
	void Mapa::GuardarXYZ(char *nombre);

    // funciones de aprendizaje
    void EliminarAristasIncorrectas();
    void AsignarParametrosAprendizaje(vector<cv::Point3f> datos_entrada,vector<cv::Vec3b> input_color,int num_entradas, bool eliminar_aristas_incorrectas,
                                      float porc_aprendizaje, int umbral_datos,int num_puntos);
    // GNG
    void AsignarParametrosGNG(int max_antiguedad, float coef_ganadora,
		 					  float coef_vecinas, float alfa, float beta);
    void InsertarNeuronaGNG();
    void EliminarNeuronasGNG();
    void EliminarAristasGNG();
    void AprenderGNG();
	void AprenderGNG_GPU();
	void AprenderGNG_HYBRID();
	void AjustarGNG();
	void BuscarNeuronasGanadoras(int &iminimo,int &iminimo2,float &minimo,float &minimo2,float x,float y,float z);
	
	// learned map bounds
	float XMin() const;
	float XMax() const;
	float YMin() const;
	float YMax() const;
	float ZMin() const;
	float ZMax() const;

	cv::Point3f centroid() const;
	void Mapa::sort();

	float getError();
	float getLearningError() const;

	Neurona *neurona;
	int num_neuronas;

	float *nube_puntos;
	std::vector<cv::Point3f> input_pcl;
	std::vector<cv::Vec3b> input_color;
	
	int *datos_coord_seg;	
	ConfiguracionMapa config;

	// Quantization error
	float error;

	// learning error
	float l_error;

   	//FILE *fi_debug;
};

#endif

