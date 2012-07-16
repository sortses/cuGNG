//#include <stdio.h>
//#include <vector>
//#include <stdlib.h>
////#include <windows.h>
//#include "tmapas.h"
//#include "cuGNG3D.h"
//#include "geom_gts.h"
//#include <opencv/cv.h>
//
//// leer fichero datos con nube de puntos
//int ObtenerPatronesEntrada(char *nombre, int tipo, std::vector<cv::Point3f> &pcl, std::vector<cv::Vec3b> &color) //0 laser 3D   1 ToF camera 2 fichero gts
//{
//	float aux,lax,lay,laz;
//	unsigned int r,g,b;
//	char auxchar[100];
//	int uno,dos,lineas,tonogris;
//	patron mipatron;
//	int numpatrones=0;
//	FILE *fich;
//
//   int eltipo,num_vecinas,cantidad;
//
//   //if (script==false)
//   int factor=1;  ////cuidado con el factor si es exterior mejor 10 o 100
//
//   fich=fopen(nombre,"r");
//   if (fich!=NULL){
//
//      if( tipo == 0 || tipo ==  1 || tipo == 3)
//		fscanf(fich,"%s\n",auxchar);
//	  else if( tipo == 4) //PLY
//	  {
//		  fscanf(fich,"%s\n",auxchar);
//		  fscanf(fich,"%s %s %s\n",auxchar,auxchar,auxchar);
//	  }
//
//      switch(tipo)
//	  {
//		  case 0:
//			  fscanf(fich,"%d %d %d\n",&lineas,&uno,&dos);   //laser3D
//			  break;
//		  case 1:
//			  fscanf(fich,"%d %d %d %d\n",&lineas,&uno,&dos,&dos);   //camara tiempo vuelo
//			  break;
//		  case 2:
//			  fscanf(fich,"%d %d %d\n",&lineas,&uno,&dos);   //fichero gts
//			  break;
//		  case 3:
//			  fscanf(fich,"%d %d %d\n",&lineas,&uno,&dos); //fichero off
//			  break;
//		  case 4: //PLY
//			  fscanf(fich,"%s %s %d\n",&auxchar,&auxchar,&lineas); //fichero off
//		  default:
//			  break;
//	  }    
//      
//	  if( tipo == 0 || tipo == 1)
//		fscanf(fich,"%s\n",auxchar);
//	  else if(tipo == 4)
//	  {
//		  // read the rest of the header
//		  fscanf(fich,"%s %s %s\n",auxchar); fscanf(fich,"%s %s %s\n",auxchar); fscanf(fich,"%s %s %s\n",auxchar);
//		  fscanf(fich,"%s %s %s\n",auxchar); fscanf(fich,"%s %s %s\n",auxchar); fscanf(fich,"%s %s %s\n",auxchar);
//		  fscanf(fich,"%s\n",auxchar);
//	  }
//
//	  //*nube_puntos = new float[lineas*3];
//
//	  int cont=0;
//      while (!feof(fich)){
//		 switch(tipo)
//		 {
//			  case 0:
//				  fscanf(fich,"%f %f %f %d %d %d\n",&lax,&lay,&laz,&tonogris,&tonogris,&tonogris);  //laser3D
//				  break;
//			  case 1:
//				  fscanf(fich,"%f %f %f %d %d %d %d %d\n",&lax,&lay,&laz,&tonogris,&tonogris,&tonogris,&uno,&dos); //camara tiempo vuelo
//				  break;
//			  case 2:
//				  fscanf(fich,"%f %f %f\n",&lax,&lay,&laz);  //fichero gts
//				  break;
//			  case 3:
//				  fscanf(fich,"%f %f %f %d %d %d\n",&lax,&lay,&laz,&r,&g,&b); //COFF
//			  case 4:
//				  fscanf(fich,"%f %f %f %d %d %d\n",&lax,&lay,&laz,&r,&g,&b); //COFF
//			  default:
//				  break;
//		 }
//
//	    
//		//(*nube_puntos)[cont]=lax*factor; //desplazo para evitar negativos
//	    //(*nube_puntos)[cont+1]=lay*factor;
//	    //(*nube_puntos)[cont+2]=laz*factor; // ojo que en los ficheros del scanner 3d se descarta la x que nosotros llamamos z
//	    cv::Point3f point(lax,lay,laz);
//		
//		// Normalizar RGB
//		/*if( tipo == 4)
//		{
//			float norm_r=r/255.0;
//			float norm_g=g/255.0;
//			float norm_b=b/255.0;
//			ColorRGB c(norm_r,norm_g,norm_b);
//			color.push_back(c);
//		}else*/
//		{
//			cv::Vec3b pix_color;
//			pix_color[0]=r;
//			pix_color[1]=g;
//			pix_color[2]=b;
//			color.push_back(pix_color);
//		}
//		
//		pcl.push_back(point);
//      }
//   }
//
//   if (fich!=NULL)
//	fclose(fich);
//   return pcl.size();
//}
//
//void GuardarFicheroGTS(Mapa *mapa, char *nombre)
//{
//   FILE *fich;
//
//   fich=fopen(nombre,"w");
//   if (fich!=NULL){
//	   fprintf(fich,"%i 0 0\n",mapa->num_neuronas);
//	   for (int i=0;i<mapa->num_neuronas;i++)
//	   {
//		if (mapa->neurona[i].utilizada==TRUE)
//		{
//		  fprintf(fich,"%f %f %f \n",mapa->neurona[i].pesos[0],mapa->neurona[i].pesos[1],mapa->neurona[i].pesos[2]);
//		}	
//	   }
//	   fclose(fich);
//	}
//}
//
//void exportPLY(vector<cv::Point3f> pcl, vector<cv::Vec3b> pcl_color, char *nombre)
//{
//   FILE *fich;
//
//   fich=fopen(nombre,"w");
//   if (fich!=NULL){
//		fprintf(fich,"ply\n");
//		fprintf(fich,"format ascii 1.0\n");
//		fprintf(fich,"element vertex %d\n", pcl.size());
//		fprintf(fich,"property float x\n");
//		fprintf(fich,"property float y\n");
//		fprintf(fich,"property float z\n");
//		fprintf(fich,"property uchar red\n");
//		fprintf(fich,"property uchar green\n");
//		fprintf(fich,"property uchar blue\n");
//		fprintf(fich,"end_header\n");
//
//	   for (int i=0;i<pcl.size();i++)
//	   {
//		  fprintf(fich,"%f %f %f %d %d %d\n",pcl[i].x, pcl[i].y, pcl[i].z, pcl_color[i][0],pcl_color[i][1],pcl_color[i][2]);	
//	   }
//	   fclose(fich);
//	}
//}
//
//hsv_color rgb_to_hsv(unsigned char r, unsigned char g, unsigned char b)
//{
//	hsv_color hsv;
//	unsigned char rgb_min, rgb_max;
//	rgb_min = MIN3(r,g,b);
//	rgb_max = MAX3(r,g,b);
//
//	hsv.val = rgb_max;
//	if (hsv.val == 0) {
//		hsv.hue = hsv.sat = 0;
//		return hsv;
//	}
//
//	hsv.sat = 255*long(rgb_max - rgb_min)/hsv.val;
//	if (hsv.sat == 0) {
//		hsv.hue = 0;
//		return hsv;
//	}
//
//	/* Compute hue */
//	if (rgb_max == r) {
//		hsv.hue = 0 + 43*(g-b)/(rgb_max - rgb_min);
//	} else if (rgb_max == g) {
//		hsv.hue = 85 + 43*(b - r)/(rgb_max - rgb_min);
//	} else /* rgb_max == rgb.b */ {
//		hsv.hue = 171 + 43*(r - g)/(rgb_max - rgb_min);
//	}
//	return hsv;
//}
//
//bool isSkinColorRGB(unsigned char r,unsigned char g,unsigned char b)
//{
//	// reference h s values for skin color regions
//	unsigned char h = 25;
//	unsigned char s = 75;
//
//	int threshold_h = 25;
//	int threshold_s = 20;
//
//	hsv_color hsv = rgb_to_hsv(r,g,b);
//	//cout << (int)hsv.hue << ";" << (int)hsv.sat << endl;
//
//	/*if( (r>(b_r-threshold) && r<(b_r+threshold)) && (g>(b_g-threshold) && g<(b_g+threshold)) 
//		&& (b>(b_b-threshold) && b<(b_b+threshold)) )*/
//	if( (hsv.hue>=0 && hsv.hue<=60) && (hsv.sat>=60 && hsv.sat<=110) )
//		return true;
//	else
//		return false;
//}
//
//void segmentarPatronesEntradaSCR(std::vector<cv::Point3f> &pcl, std::vector<cv::Vec3b> &color)
//{
//	vector<cv::Point3f>::iterator it = pcl.begin();
//	vector<cv::Vec3b>::iterator it_color = color.begin();
//
//    for (unsigned int i=0;i<pcl.size();i++) {
//        if( !isSkinColorRGB((unsigned char)(*it_color)[0],(unsigned char)(*it_color)[1],(unsigned char)(*it_color)[2]) )
//		{
//			it=pcl.erase(it);
//			it_color=color.erase(it_color);
//		}else
//		{
//			it++;
//			it_color++;
//		}
//	}
//
//	float avg=0.0;
//	for (unsigned int i=0;i<pcl.size();i++) {
//        avg+=pcl[i].z;
//	}
//	avg=avg/pcl.size();
//
//	for (unsigned int i=0;i<pcl.size();i++) {
//        if( avg-pcl[i].z > 0.3  )
//			cout << "point.x = " << pcl[i].x << " point.y = " << pcl[i].y << " point.z = " << pcl[i].z << endl;
//	}
//}
//
//int main(int argc, char* argv[])
//{
//	/*printf("77777777777  7777777777 777777 77777777 777777777777777777777777777777777");
//	printf("7777777777777777777777777777777777777777777777777777777777777777777777777");
//	printf("7777777777777777777777777777777777777777777777777777777777777777777777777");
//	printf("77777777777  77777777777777 777777777777777777777777777777777777777777777");
//	printf("77777777777777777777......777...7777.,7777......=77......777......+777777");
//	printf("77777777777  77 77,.7777777 7.,.,777.,77+.7777777777777I.777..7777..77777");
//	printf("77..7?.77.~  7..77.7777777777..7.~77.,77.+7777777777777..777..77777.I7777");
//	printf("7,.777777.~777.,77. 77....:77..77.?7.,77.777,...,777....7777..77777.:7777");
//	printf("7..777777.~777.,77.I77777.:77..777.7.,77.=77777., 777777.:77..77777.777 7");
//	printf("7+.777777..777.,77,.77777.:77..777=..,77+.77 77.,777777 .:77..7777..7777 ");
//	printf("77,....777.....,7777......777..7777:.:7777......?77.....,777......777777 ");
//	printf("7777777777777777777777777777777777777777777777777777777777777777777777777");
//	printf("777777 7777777777777777777777777777777777777 777777777777777 777777777777");
//	printf("777777 7777777777777777777777777777777777777 777777777777777 777777777777");
//	printf("777777 7777777777777777777777777777777777777 777777777777777 777777777777");*/
//	printf("777777 7777777777777777777777777777777777777 777777777777777 777777777777");
//
//	/*if( argc < 7 )
//	{
//		printf("\n");
//		printf("cuGNG3D.exe ficheroPuntos tipoFichero ficheroSalida numNeuronas numPatrones modoComputacion");
//		printf("\n");
//		printf("ficheroPuntos       : fichero en formato gts,xyz,p3d que contiene la nube de puntos.\n");
//		printf("tipoFichero         : Tipo de fichero de nube de puntos indicado anteriormente:\n");
//		printf("\t\t\t 0: Fichero entrada Laser3D.\n");
//		printf("\t\t\t 1: Fichero entrada ToF.\n");
//		printf("\t\t\t 2: Fichero entrada GTS (solo vértices).\n");
//		printf("\t\t\t 4: Fichero entrada PLY (obtenidos kinect RGBDemo).\n");
//		printf("ficheroSalida       : fichero de salida donde se almacena la topología de la red neuronal generada\n");
//		printf("numNeuronas       : Número de neuronas máximo de la red neuronal\n");
//		printf("numPatrones       : Número de patrones para ajustar la red neuronal\n");
//		printf("modoComputacion   : Modo de computacion: \n");
//		printf("\t\t\t 0: cpu\n");
//		printf("\t\t\t 1: gpu\n");
//		printf("\t\t\t 2: hibrido\n");
//		exit(-1);
//	}*/
//
//	// parametros de aprendizaje
//    float PORC_APRENDIZAJE=1;
//	bool ELIMINAR_ARISTAS_INCORRECTAS=true;
//	
//	// Num entradas
//	//int NUM_ENTRADAS=atoi(argv[5]);
//	int NUM_ENTRADAS=1500;
//    float COEF_GANADORA=(float)0.1;
//    float COEF_VECINAS=(float)0.01;
//    float ALFA=0.0;
//    float BETA=0.0;
//
//    // parametros configuracion NG
//	//int MAX_NEURONAS=atoi(argv[4]);
//	int MAX_NEURONAS=300;
//
//	int MAX_ITERACION=MAX_NEURONAS;
//    float LAMBDA_I=(float) MAX_NEURONAS;
//    float LAMBDA_F=0.01;
//    float EPSILON_I=0.5;
//    float EPSILON_F=0.0;
//    float EDAD_I=20;
//    float EDAD_F=200;
//
//    float COEF_TODAS=(float)0;
//    int UMBRAL_DATOS=127;
//    //--------------------
//
//    int LIM_TIEMPO_NG;
//    int ANCHO_MAPA=7;//10;
//    int ALTO_MAPA=7;//10;
//    int LIMITE=100;
//	int NUM_PUNTOS=0;
//	int MAX_ANTIGUEDAD=88;
//    int UMBRAL=80;
//
//	// tipo computacion 0:cpu 1:gpu 2:hibrido
//	//int tipoComputacion = atoi(argv[6]);
//	int tipoComputacion = 0;
//	int deviceNumber = 0;
//
//	bool cudaEnabled = (tipoComputacion==0? false:true);
//	bool hibrido = (tipoComputacion==2? true:false);
//
//	//PARÁMETROS DEL MAPA
//	ConfiguracionMapa config_map(MAX_NEURONAS,cudaEnabled,hibrido,deviceNumber,tipoComputacion,NUM_PUNTOS,PORC_APRENDIZAJE,
//			NUM_ENTRADAS,MAX_ANTIGUEDAD,COEF_GANADORA,COEF_VECINAS,ALFA,BETA,COEF_TODAS,UMBRAL_DATOS);
//
//	// Variables cálculo tiempos
//	LARGE_INTEGER t_ini, t_fin;
//	double secs;
//
//	// Lectura fichero patrones entrada.
//	int i=0;
//	float *nube_puntos = NULL;
//
//	char entrada[100]="hand.ply";
//
//	std::vector<cv::Point3f> pcl;
//	std::vector<cv::Vec3b> color;
//
//	//NUM_PUNTOS = ObtenerPatronesEntrada(argv[1],atoi(argv[2]),&nube_puntos);
//	NUM_PUNTOS = ObtenerPatronesEntrada(entrada,4,pcl,color);
//
//	segmentarPatronesEntradaSCR(pcl,color);
//	exportPLY(pcl,color,"segmented_hand.ply");
//
//    //---------------------------------------------------------------------------
//	clock_t inicio=clock();
//   
//	Mapa *mapa = NULL;
//	if(tipoComputacion==0)
//		mapa = new Mapa(MAX_NEURONAS,false,false,NUM_PUNTOS,deviceNumber);
//	else if(tipoComputacion == 1)
//		mapa = new Mapa(MAX_NEURONAS,true,false,NUM_PUNTOS,deviceNumber);
//	else
//		mapa = new Mapa(MAX_NEURONAS,true,true,NUM_PUNTOS,deviceNumber);
//	
//	//fprintf(mapa->fi_debug,"cuGNG3D.exe ficheroPuntos tipoFichero=%s ficheroSalida numNeuronas=%s numPatrones=%s modoComputacion=%s",argv[2],argv[4],argv[5],argv[6]);
//
//    // se asignan los nuevos parametros de aprendizaje
//	mapa->AsignarParametrosAprendizaje(pcl,color,NUM_ENTRADAS,ELIMINAR_ARISTAS_INCORRECTAS,PORC_APRENDIZAJE,UMBRAL_DATOS,pcl.size());
//
//    // Realizo el aprendizaje del mapa
//    mapa->AsignarParametrosGNG(MAX_ANTIGUEDAD,COEF_GANADORA,COEF_VECINAS,ALFA,BETA);
//    mapa->Inicializar();
//	
//	QueryPerformanceCounter(&t_ini);
//	
//	if(tipoComputacion==2){	
//		QueryPerformanceCounter(&t_ini);
//		mapa->AprenderGNG_HYBRID();
//		QueryPerformanceCounter(&t_fin);
//	}else if(tipoComputacion==1){
//		QueryPerformanceCounter(&t_ini);
//		mapa->AprenderGNG_GPU();
//		QueryPerformanceCounter(&t_fin);
//	}else if(tipoComputacion==0){
//		QueryPerformanceCounter(&t_ini);
//		mapa->AprenderGNG();
//		QueryPerformanceCounter(&t_fin);
//	}else{
//		printf("Error, tipo de computación no disponible\n");
//		exit(-2);
//	}
//
//	secs = performancecounter_diff(&t_fin, &t_ini);
//	//printf(" %.16g milliseconds with %d neuronas \n", secs * 1000.0,MAX_NEURONAS);
//
//	//printf("sizeof(neuralnetwork) = %i",sizeof(mapa));
//	//printf("Error of learned map %f \n",mapa->getError());
//
//	// Obtenemos una nueva entrada
//	NUM_PUNTOS = ObtenerPatronesEntrada(entrada,4,pcl,color);
//	segmentarPatronesEntradaSCR(pcl,color);
//	QueryPerformanceCounter(&t_ini);
//		mapa->AjustarGNG();
//	secs = performancecounter_diff(&t_fin, &t_ini);
//	printf(" %.16g milliseconds with %d neuronas \n", secs * 1000.0,MAX_NEURONAS);
//
//
//	char *res= new char[50];
//	sprintf(res,"T: %i seg",((clock()-inicio)/CLK_TCK));
//
//	//sprintf(fichero,argv[3]);	
//
//	// Utilizando la clase mesh para exportar el mapa a fichero de nube de puntos
//	Mesh mesh(mapa);
//	mesh.generarFicheroGNG("hand.gng");
//
//	mapa->Guardar("hand.map");
//	return 0;
//}