#include "tmapas.h"
#include "cuGNG3D.h"
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <float.h>
#include <cuda.h>

#define BOUND_NUM_NEURONS_CUDA 2500
//#define ITERATION_TIME_MESSAGES

//---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Mapa::Mapa(int maxneuronas,bool cudaEnabled,bool hibrido,int num_puntos, int thread_id)
{
	int i;
	
	srand ( time(0) );

	this->config.MAX_NEURONAS=maxneuronas;
	this->config.cudaEnabled = cudaEnabled;
	this->config.hibrido = hibrido;
	this->config.NUM_PUNTOS = num_puntos;

	if(this->config.cudaEnabled)
		cudaEnabled=init_cuda_device(thread_id);

    // Inicializo numeros aleatorios
	srand( (unsigned)time( NULL ) );

	// all neurons are created
	neurona=new Neurona[maxneuronas];
	/*for (i=0;i<MAX_NEURONAS;i++)
         neurona[i]=new Neurona();*/

	num_neuronas=0;

    // variables de configuracion

	// parametros de aprendizaje
	this->config.PORC_APRENDIZAJE=1;
	this->config.NUM_ENTRADAS=2000;
	this->config.ELIMINAR_ARISTAS_INCORRECTAS=true;

	// parametros configuracion GNG
	this->config.MAX_ANTIGUEDAD=250;
	this->config.COEF_GANADORA=(float)0.1;
	this->config.COEF_VECINAS=(float)0.01;
	this->config.ALFA=0.0;
	this->config.BETA=0.0;

	// parametros configuracion NG
	this->config.MAX_ITERACION=maxneuronas;
	this->config.LAMBDA_I=(float)maxneuronas;
	this->config.LAMBDA_F=0.01;
	this->config.EPSILON_I=0.5;
	this->config.EPSILON_F=0.005;
	this->config.EDAD_I=20;
	this->config.EDAD_F=200;
	this->config.LIM_TIEMPO_NG;

	//--------------------
	this->config.COEF_TODAS=(float)0;
	this->config.UMBRAL_DATOS=127;

	this->config.ANCHO_MAPA=7;//10;
	this->config.ALTO_MAPA=7;//10;
	this->config.LIMITE=100;

	this->config.UMBRAL=80;
	//ANCHO=600;//320;
	//ALTO=600;

	/*fi_debug = fopen("debug_messages.txt","a");
	if( fi_debug == NULL )
	{
		printf("Initalizing error debug log file\n");
		exit(-4);
	}*/
}


// constructor de copia
Mapa::Mapa(Mapa *mapa_original,int completo)
{
	int i,j;
	VECTOR::iterator vecina;

	num_neuronas=mapa_original->num_neuronas;

	neurona=new Neurona[mapa_original->config.MAX_NEURONAS];

	for (i=0;i<this->config.MAX_NEURONAS;i++)
	{
		if (mapa_original->neurona[i].utilizada==true)
		{
			neurona[i].utilizada=true;
			neurona[i].error=mapa_original->neurona[i].error;
			neurona[i].error_parcial=mapa_original->neurona[i].error_parcial;
			neurona[i].pesos[0]=mapa_original->neurona[i].pesos[0];
			neurona[i].pesos[1]=mapa_original->neurona[i].pesos[1];
			neurona[i].pesos[2]=mapa_original->neurona[i].pesos[2];
		}
	}

    if (completo==1)
    {
    		for (i=0;i<config.MAX_NEURONAS;i++)
			{
            	if (neurona[i].utilizada==true)
                {
            		for (j=0;j<config.MAX_NEURONAS;j++)
                	{
                    	if ((i!=j)&&(neurona[j].utilizada==true))
                        	neurona[i].AnadirVecina(j);
                	}
                }
            }
    }
    else if (completo==0)
    {
		for (i=0;i<config.MAX_NEURONAS;i++)
		{
			for (vecina=mapa_original->neurona[i].vecinas.begin();vecina!=mapa_original->neurona[i].vecinas.end();vecina++)
			{
				neurona[i].AnadirVecina((*vecina).vecina);
			}
		}
    }
    this->config.tipo=mapa_original->config.tipo;

    // se copian los parametros de configuracion
    input_pcl=mapa_original->input_pcl;
	this->config.NUM_PUNTOS=mapa_original->config.NUM_PUNTOS;
    this->config.PORC_APRENDIZAJE=mapa_original->config.PORC_APRENDIZAJE;
    this->config.NUM_ENTRADAS=mapa_original->config.NUM_ENTRADAS;
    this->config.MAX_ANTIGUEDAD=mapa_original->config.MAX_ANTIGUEDAD;
    this->config.COEF_GANADORA=mapa_original->config.COEF_GANADORA;
    this->config.COEF_VECINAS=mapa_original->config.COEF_VECINAS;
    this->config.ALFA=mapa_original->config.ALFA;
    this->config.BETA=mapa_original->config.BETA;
    this->config.COEF_TODAS=mapa_original->config.COEF_TODAS;
    this->config.UMBRAL_DATOS=mapa_original->config.UMBRAL_DATOS;
    this->config. LIM_TIEMPO_NG=mapa_original->config.LIM_TIEMPO_NG;
    this->config.ANCHO_MAPA=mapa_original->config.ANCHO_MAPA;
    this->config.ALTO_MAPA=mapa_original->config.ALTO_MAPA;
    this->config.MAX_ITERACION=mapa_original->config.MAX_ITERACION;
    this->config.LIMITE=mapa_original->config.LIMITE;
    this->config.LAMBDA_I=mapa_original->config.LAMBDA_I;
    this->config.LAMBDA_F=mapa_original->config.LAMBDA_F;
    this->config.EPSILON_I=mapa_original->config.EPSILON_I;
    this->config.EPSILON_F=mapa_original->config.EPSILON_F;
    this->config.EDAD_I=mapa_original->config.EDAD_I;
    this->config.EDAD_F=mapa_original->config.EDAD_F;
    this->config.UMBRAL=mapa_original->config.UMBRAL;
    //ANCHO=mapa_original->ANCHO;
    //ALTO=mapa_original->ALTO;
    this->config.ELIMINAR_ARISTAS_INCORRECTAS=mapa_original->config.ELIMINAR_ARISTAS_INCORRECTAS;

}


//---------------------------------------------------------------------------

Mapa::~Mapa()
{
	delete [] neurona;
	//if(cudaEnabled)
	/*for (int i=0;i<MAX_NEURONAS;i++)
	delete neurona[i];*/
}

//---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Funciones de inicializacion
//////////////////////////////////////////////////////////////////////

void
Mapa::Inicializar()
{
	int i,j,al;

	// Se crea la topologia inicial con 2 neuronas
		for (i=0;i<2;i++)
		{
			neurona[i].utilizada=true;
			al=(rand() % config.NUM_PUNTOS);
			neurona[i].inicializar_pesos(input_pcl[al].x,input_pcl[al].y,input_pcl[al].z);
		}
		for (i=2;i<config.MAX_NEURONAS;i++)
		{
			neurona[i].utilizada=false;
        }

		num_neuronas=2;

 return;
}


void Mapa::AsignarParametrosAprendizaje(std::vector<cv::Point3f> datos_entrada, std::vector<cv::Vec3b> input_color, int num_entradas, bool eliminar_aristas_incorrectas,
                                        float porc_aprendizaje, int umbral_datos, int num_puntos)
{
  this->config.NUM_PUNTOS = num_puntos;
  this->input_pcl=datos_entrada;
  this->input_color=input_color;
  this->config.NUM_ENTRADAS=num_entradas;
  this->config.ELIMINAR_ARISTAS_INCORRECTAS=eliminar_aristas_incorrectas;
  this->config.PORC_APRENDIZAJE=porc_aprendizaje;
  this->config.UMBRAL_DATOS=umbral_datos;

 return;
}


void Mapa::AsignarParametrosGNG(int max_antiguedad, float coef_ganadora,
								float coef_vecinas, float alfa, float beta)
{
  this->config.MAX_ANTIGUEDAD=max_antiguedad;
  this->config.COEF_GANADORA=coef_ganadora;
  this->config.COEF_VECINAS=coef_vecinas;
  this->config.ALFA=alfa;
  this->config.BETA=beta;
 return;
}


//////////////////////////////////////////////////////////////////////
// Abrir/Guardar mapas
//////////////////////////////////////////////////////////////////////

void Mapa::Abrir(const char *nombre)
{
   FILE *fich;
   int eltipo,num_vecinas,cantidad;
   char tipos[100]="",cuantas[100]="",quevecina[100]="",peso1[100]="",peso2[100]="",peso3[100],svecinas[100]="";
   char queneurona[200]="";

   fich=fopen(nombre,"r");
   if (fich!=NULL){
          fscanf(fich,"%s\n",tipos);
          fscanf(fich,"%s\n",cuantas);
          cantidad=atoi(cuantas);
          if(cantidad==this->config.MAX_NEURONAS)  // el mapa tiene el mismo numero de neuronas
            {
//              num_neuronas=cantidad;
              num_neuronas=0;
              this->config.tipo=0;
              for (int i=0;i<cantidad;i++)
              {
                  sprintf(queneurona,"");
                  fscanf(fich,"%s\n",queneurona);
                  if (strcmp(queneurona,"nousada"))
                  {
                      neurona[num_neuronas].utilizada=true;
                      fscanf(fich,"%s %s %s %s",peso1,peso2,peso3,svecinas);
                      neurona[num_neuronas].pesos[0]=atof(peso1);
                      neurona[num_neuronas].pesos[1]=atof(peso2);
					  neurona[num_neuronas].pesos[2]=atof(peso3);
                      num_vecinas=atoi(svecinas);

                      for (int v=0;v<num_vecinas;v++)
                      {
                          fscanf(fich,"%s",quevecina);
                          neurona[num_neuronas].AnadirVecina(atoi(quevecina));
                      }
                      fscanf(fich,"\n");
                      num_neuronas++;
                  }
            }

           for (int i=num_neuronas;i<cantidad;i++)
                neurona[i].utilizada=false;
            }

   }
   fclose(fich);
}

//---------------------------------------------------------------------------

void Mapa::Guardar(char *nombre)
{
   FILE *fich;
   unsigned int v;
   VECTOR::iterator vecina;

   fich=fopen(nombre,"w");
   if (fich!=NULL){
          fprintf(fich,"%d\n",this->config.tipo);
          fprintf(fich,"%d\n",this->config.MAX_NEURONAS);
          for (int i=0;i<this->config.MAX_NEURONAS;i++)
          {
              if (neurona[i].utilizada==true)
              {
                  fprintf(fich,"n%d\n",i);
                  //for (v=0;v<neurona[i].vecinas.size();v++){};
                  fprintf(fich,"%f %f %f",neurona[i].pesos[0],neurona[i].pesos[1],neurona[i].pesos[2]);
                  for (vecina=neurona[i].vecinas.begin();vecina!=neurona[i].vecinas.end();vecina++)
                      fprintf(fich," %d",(*vecina).vecina);
                  fprintf(fich,"\n");
              }
              //else
                  //fprintf(fich,"nousada\n");
          }
   }
   fclose(fich);
}

void Mapa::GuardarXYZ(char *nombre)
{
   FILE *fich;
   unsigned int v;
   VECTOR::iterator vecina;

   fich=fopen(nombre,"w");
   if (fich!=NULL){
	   fprintf(fich,"%d\n",this->num_neuronas);
          for (int i=0;i<this->config.MAX_NEURONAS;i++)
          {
              if (neurona[i].utilizada==true)
                  fprintf(fich,"%f %f %f\n",neurona[i].pesos[2],neurona[i].pesos[0],neurona[i].pesos[1]);
          }
   }
   fclose(fich);
}

//---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// GNG
//////////////////////////////////////////////////////////////////////

//void Mapa::InsertarNeuronaGNG()
//{
//
//	float maximo;
//	int i;
//	short mas_bmu,mayor_error;
//	VECTOR::iterator vecina;
//	float error;
//
//	// Localizacion de la neurona con mayor error
//	maximo=-1;
//	for (i=0;i<MAX_NEURONAS;i++)
//	{
//		if ((neurona[i].utilizada==true) && (neurona[i].error>maximo))
//		{
//			mas_bmu=i;
//			maximo=neurona[i].error;
//		}
//	}
//
//	// Localizacion de la neurona vecina con mayor error
//	maximo=-1;
//	for (vecina=neurona[mas_bmu].vecinas.begin();vecina!=neurona[mas_bmu].vecinas.end();vecina++)
//	{
//		error=neurona[mas_bmu].error;
//		if (error>maximo)
//		{
//			maximo=error;
//			mayor_error=(*vecina).vecina;
//		}
//	}
//
//	if (maximo<0)
//		mayor_error=mas_bmu;
//
//	// Inserto nueva neurona
//	short nueva=0;
//    while (neurona[nueva].utilizada==true)
//         nueva++;
//
//    neurona[nueva].utilizada=true;
//
//	// Inicializo pesos de la nueva neurona
//	float interpol=0.5; 
//	neurona[nueva].pesos[0]=interpol*(neurona[mayor_error].pesos[0]+neurona[mas_bmu].pesos[0]);
//	neurona[nueva].pesos[1]=interpol*(neurona[mayor_error].pesos[1]+neurona[mas_bmu].pesos[1]);
//	neurona[nueva].pesos[2]=interpol*(neurona[mayor_error].pesos[2]+neurona[mas_bmu].pesos[2]);
//
//	// Creamos las aristas necesarias para mantener la topologia
//	neurona[nueva].AnadirVecina(mas_bmu);
//	neurona[nueva].AnadirVecina(mayor_error);
//    neurona[mas_bmu].AnadirVecina(nueva);
//	neurona[mayor_error].AnadirVecina(nueva);
//    neurona[mas_bmu].EliminarVecina(mayor_error);
//    neurona[mayor_error].EliminarVecina(mas_bmu);
//
//	// Actualizamos los errores de estas neuronas
//	float incr_mas_bmu=-0.5*interpol*neurona[mas_bmu].error;
//	float incr_mayor_error=-0.5*interpol*neurona[mayor_error].error;
//	neurona[mas_bmu].error+=incr_mas_bmu;
//	neurona[mayor_error].error+=incr_mayor_error;
//	neurona[nueva].error=-(incr_mas_bmu+incr_mayor_error);
//}

void Mapa::InsertarNeuronaGNG()
{

	float maximo;
	int i;
	short mas_bmu,mayor_error;
	VECTOR::iterator vecina;
	float error;

	// Localizacion de la neurona con mayor error
	maximo=-1;
	for (i=0;i<this->config.MAX_NEURONAS;i++)
	{
		if ((neurona[i].utilizada==true) && (neurona[i].error>maximo))
		{
			mas_bmu=i;
			maximo=neurona[i].error;
		}
	}

	// Localizacion de la neurona vecina con mayor error
	maximo=-1;
	for (vecina=neurona[mas_bmu].vecinas.begin();vecina!=neurona[mas_bmu].vecinas.end();vecina++)
	{
		error=neurona[mas_bmu].error;
		if (error>maximo)
		{
			maximo=error;
			mayor_error=(*vecina).vecina;
		}
	}

	if (maximo<0)
		mayor_error=mas_bmu;

	// Inserto nueva neurona
	short nueva=0;
    while (neurona[nueva].utilizada==true)
         nueva++;

    neurona[nueva].utilizada=true;

	// Inicializo pesos de la nueva neurona
	float interpol=0.5; //neurona[mas_bmu]->error/(neurona[mas_bmu]->error+neurona[mayor_error]->error);
//	neurona[nueva]->pesos[0]=neurona[mayor_error]->pesos[0]+interpol*(neurona[mas_bmu]->pesos[0]-neurona[mayor_error]->pesos[0]);
//	neurona[nueva]->pesos[1]=neurona[mayor_error]->pesos[1]+interpol*(neurona[mas_bmu]->pesos[1]-neurona[mayor_error]->pesos[1]);
	neurona[nueva].pesos[0]=interpol*(neurona[mayor_error].pesos[0]+neurona[mas_bmu].pesos[0]);
	neurona[nueva].pesos[1]=interpol*(neurona[mayor_error].pesos[1]+neurona[mas_bmu].pesos[1]);
       // neurona[nueva]->pesos[2]=neurona[0]->pesos[2];
        neurona[nueva].pesos[2]=interpol*(neurona[mayor_error].pesos[2]+neurona[mas_bmu].pesos[2]);
// neurona[nueva]->color=GetRed(Mapas->Image1->Canvas,(int)neurona[nueva]->pesos[0],(int)neurona[nueva]->pesos[1]);

	// Creamos las aristas necesarias para mantener la topologia
	neurona[nueva].AnadirVecina(mas_bmu);
	neurona[nueva].AnadirVecina(mayor_error);
    neurona[mas_bmu].AnadirVecina(nueva);
	neurona[mayor_error].AnadirVecina(nueva);
    neurona[mas_bmu].EliminarVecina(mayor_error);
    neurona[mayor_error].EliminarVecina(mas_bmu);

	// Actualizamos los errores de estas neuronas
	float incr_mas_bmu=-0.5*interpol*neurona[mas_bmu].error;
	float incr_mayor_error=-0.5*interpol*neurona[mayor_error].error;
	neurona[mas_bmu].error+=incr_mas_bmu;                
	neurona[mayor_error].error+=incr_mayor_error;
         neurona[nueva].error=(neurona[mayor_error].error+neurona[mas_bmu].error)/2; //2011
     //  neurona[nueva]->error=-(incr_mas_bmu+incr_mayor_error);
     //  error-=(error*interpol); //2011
}


void Mapa::EliminarNeuronasGNG()
{
	int i;
	VECTOR::iterator vecina;

	for (i=0;i<this->config.MAX_NEURONAS;i++)
	{
		if ((num_neuronas>2) && (neurona[i].utilizada==true) && (neurona[i].error_parcial==0))
		{
			for (vecina=neurona[i].vecinas.begin();vecina!=neurona[i].vecinas.end();vecina++)
				neurona[(*vecina).vecina].EliminarVecina(i);
			neurona[i].utilizada=false;
            neurona[i].vecinas.erase(neurona[i].vecinas.begin(),neurona[i].vecinas.end());
			num_neuronas--;
		}
	}

   // se eliminan las neuronas que queden desconectadas
   if(num_neuronas>10)
   {
      int aux;
      for (i=0;i<this->config.MAX_NEURONAS;i++)
      {
		   if((neurona[i].utilizada==true)&&(neurona[i].vecinas.size()==0))
		   {
				neurona[i].utilizada=false;
				num_neuronas--;
		   }     
      }
   }
}


void Mapa::EliminarAristasGNG()
{
  VECTOR::iterator vecina;
  int i;

  for (i=0;i<this->config.MAX_NEURONAS;i++)
  {
      if (neurona[i].utilizada==true)
      {
          vecina = neurona[i].vecinas.begin();
		  while(vecina != neurona[i].vecinas.end()){
              if ( (*vecina).antiguedad > this->config.MAX_ANTIGUEDAD)
              {
                  neurona[(*vecina).vecina].EliminarVecina(i);
			      vecina = neurona[i].vecinas.erase(vecina);
              }
              else
              {
                  vecina++;
              }
          }//fin del while
      }
   }

  // se eliminan las neuronas que queden desconectadas
   if(num_neuronas>10)
     {
		  for (i=0;i<this->config.MAX_NEURONAS;i++)
		  {
			   if((neurona[i].utilizada==true)&&(neurona[i].vecinas.size()==0))
			   {
				   neurona[i].utilizada=false;
				   num_neuronas--;
			   }
		  }
     }
}


// Eliminar ruido y conexiones incorrrectas en 3D??¿¿¿¿
//
void
Mapa::EliminarAristasIncorrectas()
{
	//VECTOR::iterator vecina;
 //   float porc;
	//int i,xmedia,ymedia,zmedia;
 //   int num_mayores,num_menores;
 //   int eliminadas[1000][3],num_eliminadas=0;
 //   int x1,y1,x2,y2;

	//for (i=0;i<MAX_NEURONAS;i++)
	//{
 //       num_mayores=num_menores=0;
	//	if (neurona[i].utilizada==true)
	//	{
 //           x1=neurona[i].pesos[0];
 //           y1=neurona[i].pesos[1];
	//		z1=neurona[i].pesos[2];
	//		for (vecina=neurona[i].vecinas.begin();vecina!=neurona[i].vecinas.end();vecina++)
	//		{
 //               num_menores=num_mayores=0;
 //               x2=neurona[(*vecina).vecina].pesos[0];
 //               y2=neurona[(*vecina).vecina].pesos[1];
	//			z2=neurona[(*vecina).vecina].pesos[2];
 //           	for(porc=0.1;porc<1;porc=porc+0.1)
 //                  {
	//			    xmedia=x1+porc*(x2-x1);
	//			    ymedia=y1+porc*(y2-y1);
	//				zmedia=z1+porc*(z2-z1);
	//			    if (nube_puntos[ymedia*ANCHO+xmedia]<UMBRAL_DATOS)
 //                       num_menores++;
 //                   else
 //                       num_mayores++;
 //                  }

 //               if(num_menores>=(num_mayores/7))   // >=25% en negro
 //                 {
 //                 eliminadas[num_eliminadas][0]=i;
 //                 eliminadas[num_eliminadas][1]=(*vecina).vecina;
 //                 num_eliminadas++;
 //                 neurona[(*vecina).vecina].EliminarVecina(i);
 //                 neurona[i].vecinas.erase(vecina);
 //                 vecina--;
 //                 }
 //           }
 //       }
	//}

 //  // se eliminan las neuronas que queden desconectadas
 //  if(num_neuronas>10)
 //    {
 //     int aux;
 //     aux=MAX_NEURONAS;
 //     for (i=0;i<MAX_NEURONAS;i++)
 //         {
 //          if((neurona[i].utilizada==true)&&(neurona[i].vecinas.size()==0))
 //              neurona[i].utilizada=false;
 //          if(neurona[i].utilizada==false)
 //             aux--;
 //         }

 //     // se insertan tantas nuevas neuronas como desconectadas se hayan eliminado
 //     for(int i=aux;i<num_neuronas;i++)
 //         InsertarNeuronaGNG();
 //    }

}

// GNG 3D GPU
void Mapa::AprenderGNG_GPU()
{
	int i,iteracion=0,entrada,al;
	float x,y,z;
	int ran;
	float error;
	float distancia;
	VECTOR::iterator vecina;
	int neuronas_fuera;
	bool ultima_vuelta=false;
	int chkIter=50,countChkIter=0,wakeCUDA=0;

	//Variables cálculo tiempos
	LARGE_INTEGER t_ini, t_fin;
	double secs;
	double secs_gpu;

	// se crea el fichero donde se almacenaran los tiempos intermedios
	char nombre_fichero_tiempos[200];
	FILE *fichero_tiempos;
	float tiempo_actual;

	// allocate device memory and data
	Neurona* d_idata = NULL;
	reduceSTRUCT2* d_odata = NULL;

	// init aux matrix
	auxVECINA* d_matVecinas = NULL;
	auxVECINA* h_matVecinas = NULL;

	int numBlocks,numThreads;
	numBlocks=numThreads=0;

	init_mem_cuda_gng(d_idata,d_odata,d_matVecinas,this->config.MAX_NEURONAS);
	h_matVecinas = new auxVECINA[(this->config.MAX_NEURONAS*(100))+this->config.MAX_NEURONAS];
	
	nube_puntos = new float[input_pcl.size()];
	for(int i=0;i<input_pcl.size();i++)
	{
		nube_puntos[i*3]=input_pcl[i].x;
		nube_puntos[i*3+1]=input_pcl[i].y;
		nube_puntos[i*3+2]=input_pcl[i].z;
	}

	do
	{
		iteracion++;

		if (num_neuronas==this->config.MAX_NEURONAS)
			ultima_vuelta=true;

		// Inicializamos los errores
		error=0;
		for (i=0;i<num_neuronas;i++)
			neurona[i].error_parcial=0;

		getNumBlocksAndThreads(num_neuronas,numBlocks,numThreads);
		#ifdef ITERATION_TIME_MESSAGES
			QueryPerformanceCounter(&t_ini);
		#endif
		CU_AjusteGNG(neurona,d_idata,d_odata,h_matVecinas,d_matVecinas,numThreads,numBlocks,num_neuronas,this->config.NUM_ENTRADAS,nube_puntos,this->config.NUM_PUNTOS);
		#ifdef ITERATION_TIME_MESSAGES
			QueryPerformanceCounter(&t_fin);
			secs = performancecounter_diff(&t_fin, &t_ini);
			//fprintf(fi_debug,"%.16g milliseconds with %d neuronas CUDA ENABLED\n", secs * 1000.0,num_neuronas);
		#endif
		
		EliminarAristasGNG();
   		//EliminarNeuronasGNG();

		//if(ELIMINAR_ARISTAS_INCORRECTAS)
		//   EliminarAristasIncorrectas();
																																																																																																																																																																																																										if(ultima_vuelta==false)
		if(num_neuronas<(this->config.MAX_NEURONAS))
		{
			InsertarNeuronaGNG();
   			num_neuronas++;
		}
		else
			ultima_vuelta=true;

		neuronas_fuera=0;
		//    neuronas_fuera=NeuronasFuera(datos);
      

		} while (  ((num_neuronas<this->config.PORC_APRENDIZAJE*this->config.MAX_NEURONAS) || (neuronas_fuera>0) ||
					((ultima_vuelta==false) && (this->config.PORC_APRENDIZAJE==1)))
			) ;

		// DESTROY CUDA VARIABLES 
		delete [] h_matVecinas;
		//free(h_odata);

		destroy_mem_cuda_gng(d_idata,d_odata);  
}


	float Mapa::getError()
	{
		this->error=0;
		for(int i=0;i<num_neuronas;i++)
		{
			float distancia=0;
			float min_distancia=99999;
			for(int j=0;j<this->input_pcl.size();j++)
			{
				distancia=sqrt(neurona[i].Distancia_2(input_pcl[j].x,input_pcl[j].y,input_pcl[j].z));
				if(distancia<min_distancia) min_distancia=distancia;
			}
			this->error=distancia;
		}
		return error;
	}

	float Mapa::getLearningError() const
	{
		return l_error;
	}

// GNGF 3D CPU
//
// GNG 3D normal
void Mapa::AprenderGNG()
{
	int i,iteracion=0,entrada,al;
	float x,y,z;
	int ran;
	float error;
	float distancia;
	float minimo,minimo2;
	minimo=minimo2=FLT_MAX;
	int iminimo=0,iminimo2=1;
	VECTOR::iterator vecina;
	int neuronas_fuera;
	bool ultima_vuelta=false;

	//Variables cálculo tiempos
	LARGE_INTEGER t_ini, t_fin;
	double secs;

	// se crea el fichero donde se almacenaran los tiempos intermedios
	char nombre_fichero_tiempos[200];
	FILE *fichero_tiempos;
	float tiempo_actual;

	do
	{
		iteracion++;

		if (num_neuronas==this->config.MAX_NEURONAS)
			ultima_vuelta=true;

		// Inicializamos los errores
		error=0;
		for (i=0;i<num_neuronas;i++)
			neurona[i].error_parcial=0;

		#ifdef ITERATION_TIME_MESSAGES
			QueryPerformanceCounter(&t_ini);
		#endif
		AjustarGNG();
		#ifdef ITERATION_TIME_MESSAGES
			QueryPerformanceCounter(&t_fin);
			secs = performancecounter_diff(&t_fin, &t_ini);
			fprintf(fi_debug,"%.16g milliseconds with %d neuronas CPU\n", secs * 1000.0,num_neuronas);
		#endif

		//printf("error;%f;neurons;%d\n",this->getLearningError(),num_neuronas);

		EliminarAristasGNG();
   		//DeleteNeuronsGNG();

		//if(ELIMINAR_ARISTAS_INCORRECTAS)
		//   EliminarAristasIncorrectas();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        if(ultima_vuelta==false)
		if(num_neuronas<(this->config.MAX_NEURONAS))
		{
			InsertarNeuronaGNG();
   			num_neuronas++;
 		}
		else
		{
			ultima_vuelta=true;
		}

		neuronas_fuera=0;
		//    neuronas_fuera=NeuronasFuera(datos);
      

	} while (  ((num_neuronas<this->config.PORC_APRENDIZAJE*this->config.MAX_NEURONAS) || (neuronas_fuera>0) ||
					((ultima_vuelta==false) && (this->config.PORC_APRENDIZAJE==1)))
					) ;
}

// GNG 3D normal
void Mapa::AprenderGNG_HYBRID()
{
	int i,iteracion=0,entrada,al;
	float x,y,z;
	int ran;
	float error;
	float distancia;
	VECTOR::iterator vecina;
	int neuronas_fuera;
	bool ultima_vuelta=false;
	int chkIter=500,countChkIter=0,wakeCUDA=0;

	//Variables cálculo tiempos
	LARGE_INTEGER t_ini, t_fin;
	double secs;
	double secs_gpu;

	// se crea el fichero donde se almacenaran los tiempos intermedios
	char nombre_fichero_tiempos[200];
	FILE *fichero_tiempos;
	float tiempo_actual;

	// allocate device memory and data
	Neurona* d_idata = NULL;
	reduceSTRUCT2* d_odata = NULL;

	// init aux matrix
	auxVECINA* d_matVecinas = NULL;
	auxVECINA* h_matVecinas = NULL;

	int numBlocks,numThreads;
	numBlocks=numThreads=0;

	init_mem_cuda_gng(d_idata,d_odata,d_matVecinas,this->config.MAX_NEURONAS);
	h_matVecinas = new auxVECINA[(this->config.MAX_NEURONAS*(100))+this->config.MAX_NEURONAS];

	do
	{
		iteracion++;

		if (num_neuronas==this->config.MAX_NEURONAS)
			ultima_vuelta=true;

		// Inicializamos los errores
		error=0;
		for (i=0;i<num_neuronas;i++)
			neurona[i].error_parcial=0;

		//if(wakeCUDA==0)
		//{
		//	if(countChkIter%chkIter!=0){
		//		#ifdef ITERATION_TIME_MESSAGES
		//			QueryPerformanceCounter(&t_ini);
		//		#endif
		//		this->AjustarGNG();
		//		#ifdef ITERATION_TIME_MESSAGES
		//			QueryPerformanceCounter(&t_fin);
		//			secs = performancecounter_diff(&t_fin, &t_ini);
		//			fprintf(fi_debug,"%.16g milliseconds with %d neuronas CPU\n", secs * 1000.0,num_neuronas);
		//		#endif
		//	}else
		//	{
		//		//check if a cuda algorithm is faster than cpu algorithm
		//		getNumBlocksAndThreads(num_neuronas,numBlocks,numThreads);
		//		QueryPerformanceCounter(&t_ini);
		//		CU_AjusteGNG(neurona,d_idata,d_odata,h_matVecinas,d_matVecinas,numThreads,numBlocks,num_neuronas,NUM_ENTRADAS,nube_puntos,NUM_PUNTOS);
		//		QueryPerformanceCounter(&t_fin);
		//		secs_gpu = performancecounter_diff(&t_fin, &t_ini);
		//		if(secs_gpu < secs) wakeCUDA=1;
		//		#ifdef ITERATION_TIME_MESSAGES
		//			fprintf(fi_debug,"%.16g milliseconds with %d neuronas CUDA ENABLED\n", secs * 1000.0,num_neuronas);
		//		#endif
		//	}
		//}
		//else
		//{
		//	getNumBlocksAndThreads(num_neuronas,numBlocks,numThreads);
		//	#ifdef ITERATION_TIME_MESSAGES
		//		QueryPerformanceCounter(&t_ini);
		//	#endif
		//	CU_AjusteGNG(neurona,d_idata,d_odata,h_matVecinas,d_matVecinas,numThreads,numBlocks,num_neuronas,NUM_ENTRADAS,nube_puntos,NUM_PUNTOS);
		//	#ifdef ITERATION_TIME_MESSAGES
		//		QueryPerformanceCounter(&t_fin);
		//		secs = performancecounter_diff(&t_fin, &t_ini);
		//		fprintf(fi_debug,"%.16g milliseconds with %d neuronas CUDA ENABLED\n", secs * 1000.0,num_neuronas);
		//	#endif
		//	
		//}
		//countChkIter++;
		if(num_neuronas > BOUND_NUM_NEURONS_CUDA)
		{
			getNumBlocksAndThreads(num_neuronas,numBlocks,numThreads);
			CU_AjusteGNG(neurona,d_idata,d_odata,h_matVecinas,d_matVecinas,numThreads,numBlocks,num_neuronas,this->config.NUM_ENTRADAS,nube_puntos,this->config.NUM_PUNTOS);
		}else
		{
			this->AjustarGNG();
		}

		
		EliminarAristasGNG();
   		//EliminarNeuronasGNG();

		//if(ELIMINAR_ARISTAS_INCORRECTAS)
		//   EliminarAristasIncorrectas();
																																																																																																																																																																																																										if(ultima_vuelta==false)
		if(num_neuronas<(this->config.MAX_NEURONAS))
		{
			InsertarNeuronaGNG();
   			num_neuronas++;
		}
		else
		{
			ultima_vuelta=true;
		}

	neuronas_fuera=0;
	//    neuronas_fuera=NeuronasFuera(datos);
      

	} while (  ((num_neuronas<this->config.PORC_APRENDIZAJE*this->config.MAX_NEURONAS) || (neuronas_fuera>0) ||
					((ultima_vuelta==false) && (this->config.PORC_APRENDIZAJE==1)))
			) ;

	// DESTROY CUDA VARIABLES 
	delete [] h_matVecinas;
	//free(h_odata);

	destroy_mem_cuda_gng(d_idata,d_odata);  
}

void Mapa::AjustarGNG()
{
	float x,y,z;
	int al,i;
	int iminimo,iminimo2;
	float distancia;
	float minimo,minimo2;
	//float error=0;

	//reset learning error;
	this->l_error=0;
	
	VECTOR::iterator vecina;
	int big_rand=0;
	cv::Point3f rnd_point;
	
	for (int entrada=0;entrada<this->config.NUM_ENTRADAS;entrada++)
	{		
		//do
		//{
			big_rand = (RAND_MAX)*(int)rand() + rand();
			al=big_rand%(this->config.NUM_PUNTOS);
			rnd_point=input_pcl[al];
		//}while( !isSkinColorRGB(input_color[al][0],input_color[al][1],input_color[al][2]) );

		// Buscamos la neurona mas cercana
		this->BuscarNeuronasGanadoras(iminimo,iminimo2,minimo,minimo2,rnd_point.x,rnd_point.y,rnd_point.z);

		this->l_error+=sqrt(minimo);

		neurona[iminimo].IncrementarEdad();

		// Se incrementan los errores de la neurona ganadora
		neurona[iminimo].error+=minimo;
		neurona[iminimo].error_parcial+=minimo;

		//neurona[iminimo].ModificarPesos(x,y,z,COEF_GANADORA*((float)datos[y*ANCHO+x]/MAX_COLOR));
		neurona[iminimo].ModificarPesos(rnd_point.x,rnd_point.y,rnd_point.z,this->config.COEF_GANADORA);

		// Se modifican los pesos de las neuronas vecinas
		for(vecina=neurona[iminimo].vecinas.begin();vecina!=neurona[iminimo].vecinas.end();vecina++)
		{
  		  	neurona[(*vecina).vecina].ModificarPesos(rnd_point.x,rnd_point.y,rnd_point.z,this->config.COEF_VECINAS);
		}

		neurona[iminimo].CrearArista(iminimo2);
		neurona[iminimo2].CrearArista(iminimo);
	}
	this->l_error=this->l_error/config.NUM_ENTRADAS;
}

void Mapa::BuscarNeuronasGanadoras(int &iminimo,int &iminimo2,float &minimo,float &minimo2,float x,float y,float z)
{
	float distancia;
	minimo=FLT_MAX;
	minimo2=FLT_MAX;
	iminimo=iminimo2=-1;
	for (int i=0;i<num_neuronas;i++)
	{
		if (neurona[i].utilizada==true)
		{
			distancia=neurona[i].Distancia_2(x,y,z);
			if (distancia<minimo)
			{
				minimo2=minimo;
				iminimo2=iminimo;
				minimo=distancia;
				iminimo=i;
			}
			else if (distancia<minimo2)
			{
				minimo2=distancia;
				iminimo2=i;
			}
		}
	} 
}

float Mapa::XMin() const
{
	float xmin = FLT_MAX;
	for(int i=0;i<num_neuronas;i++)
	{
		if( neurona[i].pesos[0] < xmin )
			xmin = neurona[i].pesos[0];
	}
	return xmin;
}

float Mapa::XMax() const
{
	float xmax = -9999;
	for(int i=0;i<num_neuronas;i++)
	{
		if( neurona[i].pesos[0] > xmax )
			xmax = neurona[i].pesos[0];
	}
	return xmax;
}
float Mapa::YMin() const
{
	float ymin = FLT_MAX;
	for(int i=0;i<num_neuronas;i++)
	{
		if( neurona[i].pesos[1] < ymin )
			ymin = neurona[i].pesos[1];
	}
	return ymin;
}
float Mapa::YMax() const
{
	float ymax = -9999;
	for(int i=0;i<num_neuronas;i++)
	{
		if( neurona[i].pesos[1] > ymax )
			ymax = neurona[i].pesos[1];
	}
	return ymax;
}
float Mapa::ZMin() const
{
	float zmin = FLT_MAX;
	for(int i=0;i<num_neuronas;i++)
	{
		if( neurona[i].pesos[2] < zmin )
			zmin = neurona[i].pesos[2];
	}
	return zmin;
}
float Mapa::ZMax() const
{
	float zmax = -9999;
	for(int i=0;i<num_neuronas;i++)
	{
		if( neurona[i].pesos[2] > zmax )
			zmax = neurona[i].pesos[2];
	}
	return zmax;
}

cv::Point3f Mapa::centroid() const
{
	float xmin,ymin,zmin;
	float xmax,ymax,zmax;
	xmin=ymin=zmin=999999;
	xmax=ymax=zmax=-999999;
	for(int i=0;i<num_neuronas;i++)
	{
		if( neurona[i].pesos[0] < xmin )
			xmin = neurona[i].pesos[0];
		if( neurona[i].pesos[0] > xmax )
			xmax = neurona[i].pesos[0];

		if( neurona[i].pesos[1] < ymin )
			ymin = neurona[i].pesos[1];
		if( neurona[i].pesos[1] > ymax )
			ymax = neurona[i].pesos[1];

		if( neurona[i].pesos[2] < zmin )
			zmin = neurona[i].pesos[2];
		if( neurona[i].pesos[2] > zmax )
			zmax = neurona[i].pesos[2];
	}
	cv::Point3f centroid;
	centroid.x = xmin+((xmax-xmin)/2);
	centroid.y = ymin+((ymax-ymin)/2);
	centroid.z = zmin+((zmax-zmin)/2);
	return centroid;
}

void erase(std::vector<cv::Point3f> &pcl, double x, double y, double z)
{
    std::vector<cv::Point3f>::iterator iter = pcl.begin();
    while (iter != pcl.end())
    {
		if ((*iter).x == x && (*iter).y == y && (*iter).z == z)
        {
            iter = pcl.erase(iter);
        }
        else
        {
           ++iter;
        }
    }
}

void Mapa::sort()
{
	// centroid of the learned network
	cv::Point3f centroid = this->centroid();
	centroid.x /= 10;
	centroid.y /= 10;
	centroid.z /= 10;
	
	//initial point
	cv::Point3f p;
	p.x=neurona[0].pesos[0]/10; p.y=neurona[0].pesos[1]/10; p.z=neurona[0].pesos[2]/10;  

	double threshold=0;	
	
	//aux maps
	std::vector<cv::Point3f> mapa;
	std::vector<cv::Point3f> sort;
	for(int i=0;i<this->config.MAX_NEURONAS;i++)
	{
		if(neurona[i].utilizada)
		{
			cv::Point3f aux;
			aux.x = neurona[i].pesos[0]/10; aux.y = neurona[i].pesos[1]/10; aux.z = neurona[i].pesos[2]/10; 
			mapa.push_back(aux);
		}
	}

	bool found=false;
	double max_angle=-3.14/2;
	while(mapa.size() != 0)
	{
		int index=0;
		double min_distance=999999;
		double min_angle=99999;
		for(int i=0; i<mapa.size(); i++)
		{
			cv::Point3f point = mapa[i];
			//double distance = computeManhatanDistance(p,point);
			double aux = atan2(point.y-centroid.y,point.x-centroid.x);
			if( aux>=max_angle && aux<=min_angle)
			{
				index = i;
				min_angle = aux;
				found=true;
			}
		}

		// 4 cuadrante cambio de signo.
		if(!found)
		{
			min_angle=9999;
			for(int i=0; i<mapa.size(); i++)
			{
				cv::Point3f point = mapa[i];
				double aux = atan2(point.y-centroid.y,point.x-centroid.x);
				if( aux<=min_angle)
				{
					index = i;
					min_angle = aux;
				}
			}
		}
		found=false;
		max_angle=min_angle;
		//push back the next point
		p.x=mapa[index].x;
		p.y=mapa[index].y;
		
		sort.push_back(mapa[index]);
		erase(mapa,mapa[index].x,mapa[index].y,mapa[index].z);
	}

	// reorder map
	for(int i=0;i<sort.size();i++)
	{
		neurona[i].pesos[0]=sort[i].x;
		neurona[i].pesos[1]=sort[i].y;
		neurona[i].pesos[2]=sort[i].z;
	}
}