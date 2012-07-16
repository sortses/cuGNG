/********************************************************************
*  cuGNG3D.cu
*  CUDA implementation of Growing Neural Gas Networkd for 3D input Data
*********************************************************************/

#define debug_msg 1

#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <cutil_inline.h>
#include <float.h>

#include "sharedmem.cuh"
#include "reduction.h"
#include "Neurona.h"
#include "cuGNG3D.h"

using namespace std;

enum ReduceType
{
    REDUCE_INT,
    REDUCE_FLOAT,
    REDUCE_DOUBLE
};

/* retorna "a - b" en segundos */
double performancecounter_diff(LARGE_INTEGER *a, LARGE_INTEGER *b)
{
  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);
  return (double)(a->QuadPart - b->QuadPart) / (double)freq.QuadPart;
}

void checkCUDAError(const char* msg);

/************************************************************************/
/* Init CUDA                                                            */
/************************************************************************/
bool init_cuda_device(int device_number)
{
	int count = 0;
	int i = 0;

	cudaGetDeviceCount(&count);
	if(count == 0) {
		fprintf(stderr, "There is no device.\n");
		return false;
	}

	cudaDeviceProp prop;
	for(i = 0; i < count; i++) {
		if(cudaGetDeviceProperties(&prop, i) == cudaSuccess) {
			if(prop.major >= 1) {
				break;
			}
		}
	}
	if(i == count) {
		fprintf(stderr, "There is no device supporting CUDA.\n");
		return false;
	}
	cudaSetDevice(device_number);

	cudaGetDeviceProperties(&prop, 1);
	printf("CUDA device: %s initialized.\n",prop.name);

	return true;
}

bool isPow2(unsigned int x)
{
    return ((x&(x-1))==0);
}

unsigned int nextPow2( unsigned int x ) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

////////////////////////////////////////////////////////////////////////////////
// Compute the number of threads and blocks to use for the given reduction kernel
// For the kernels >= 3, we set threads / block to the minimum of maxThreads and
// n/2. For kernels < 3, we set to the minimum of maxThreads and n.  For kernel 
// 6, we observe the maximum specified number of blocks, because each thread in 
// that kernel can process a variable number of elements.
////////////////////////////////////////////////////////////////////////////////
void getNumBlocksAndThreads(int n,int &blocks, int &threads)
{     
	int maxBlocks=64;
	int maxThreads=256;

    threads = (n < maxThreads*2) ? nextPow2((n + 1)/ 2) : maxThreads;
	blocks = (n + (threads * 2 - 1)) / (threads * 2);
}

// Constantes
__constant__ float  COEF_VECINAS,COEF_GANADORA;
__constant__ int  size;

//FUNCION REDUCE MIN NEURONA 3
__global__ void
reduceMinNeurona3_Min2(Neurona *g_idata, reduceSTRUCT2 *g_odata,float x,float y,float z)
{
    SharedMemory<reduceSTRUCT2> smem;
    reduceSTRUCT2 *sdata = smem.getPointer();

    // perform first level of reduction,
    // reading from global memory, writing to shared memory
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*(blockDim.x*2) + threadIdx.x;

	if( i<size && g_idata[i].utilizada )
	{
		sdata[tid].distancia1 = (g_idata[i].pesos[0]-x)*(g_idata[i].pesos[0]-x)+(g_idata[i].pesos[1]-y)*(g_idata[i].pesos[1]-y)+(g_idata[i].pesos[2]-z)*(g_idata[i].pesos[2]-z);
		sdata[tid].indice1 = i;
		sdata[tid].distancia2 = 99999999;
		sdata[tid].indice2 = -1;
	}
	else
	{
		sdata[tid].distancia1 = 99999999;
		sdata[tid].indice1 = -1;
		sdata[tid].distancia2 = 99999999;
		sdata[tid].indice2 = -1;
	}

	if (i + blockDim.x < size && g_idata[i+blockDim.x].utilizada )
	{
		float distancia = (g_idata[i+blockDim.x].pesos[0]-x)*(g_idata[i+blockDim.x].pesos[0]-x)+(g_idata[i+blockDim.x].pesos[1]-y)*(g_idata[i+blockDim.x].pesos[1]-y)+(g_idata[i+blockDim.x].pesos[2]-z)*(g_idata[i+blockDim.x].pesos[2]-z);
	
		if(sdata[tid].distancia1 > distancia) 
		{
			sdata[tid].distancia2 = sdata[tid].distancia1; 
			sdata[tid].indice2 = sdata[tid].indice1;
			sdata[tid].distancia1 = distancia; 
			sdata[tid].indice1 = i+blockDim.x;
		}
		else
		{
			sdata[tid].distancia2 = distancia;
			sdata[tid].indice2 = i+blockDim.x;
		}
	}

    __syncthreads();

    // do reduction in shared mem
    for(unsigned int s=blockDim.x/2; s>0; s>>=1) 
    {
        if (tid < s) 
        {
			if(sdata[tid].distancia1 > sdata[tid + s].distancia1)
			{
				if( sdata[tid].distancia1 < sdata[tid + s].distancia2 )
				{
					sdata[tid].distancia2 = sdata[tid].distancia1; 
					sdata[tid].indice2 = sdata[tid].indice1;
				}
				else
				{
					sdata[tid].distancia2 = sdata[tid + s].distancia2; 
					sdata[tid].indice2 = sdata[tid + s].indice2;
				}
				sdata[tid].distancia1 = sdata[tid + s].distancia1;
				sdata[tid].indice1 = sdata[tid + s].indice1;
			}
			else
			{
				if(sdata[tid].distancia2 > sdata[tid + s].distancia1)
				{
					sdata[tid].distancia2 = sdata[tid + s].distancia1;
					sdata[tid].indice2 = sdata[tid + s].indice1;
				}
			}
        }
        __syncthreads();
    }

    // write result for this block to global mem 
    if (tid == 0)
	{	
		g_odata[blockIdx.x].distancia1 = sdata[0].distancia1;
		g_odata[blockIdx.x].indice1 = sdata[0].indice1;
		g_odata[blockIdx.x].distancia2 = sdata[0].distancia2;
		g_odata[blockIdx.x].indice2 = sdata[0].indice2;
	}
}

__global__ void
adjustWeightsWinningNeurons(auxVECINA *mat,Neurona *g_idata, reduceSTRUCT2 *g_odata, unsigned int numBlocks,float x,float y,float z)
{

	float minimo,minimo2;
	int iminimo,iminimo2;

	minimo=g_odata[0].distancia1;
	iminimo=g_odata[0].indice1;
	minimo2=g_odata[0].distancia2;
	iminimo2=g_odata[0].indice2;

	//printf("minimo1 %f iminimo1 %d",minimo,iminimo);
	//printf("minimo2 %f iminimo2 %d",minimo2,iminimo2);

	//Obtenemos los mínimos finales en gpu
	for(int i=1;i<numBlocks;i++)
	{
		if( g_odata[i].distancia1 < minimo )
		{
			if( g_odata[i].distancia2 < minimo )
			{
				minimo2 = g_odata[i].distancia2;
				iminimo2 = g_odata[i].indice2;
			}
			else
			{
				minimo2 = minimo;
				iminimo2 = iminimo;
			}
			minimo=g_odata[i].distancia1;
			iminimo=g_odata[i].indice1;
		}
		else
		{
			if( g_odata[i].distancia1 < minimo2 )
			{
				minimo2 = g_odata[i].distancia1;
				iminimo2 = g_odata[i].indice1;
			}
		}
	}

	for(int i=1;i<=mat[iminimo*(100+1)].indice_vecina;i++)
	{
		mat[iminimo*(100+1)+i].antiguedad = mat[iminimo*(100+1)+i].antiguedad + 1;
	}

	// Se incrementan los errores de la neurona ganadora
	g_idata[iminimo].error = g_idata[iminimo].error + minimo;
	g_idata[iminimo].error_parcial = g_idata[iminimo].error_parcial + minimo;

	g_idata[iminimo].pesos[0] += COEF_GANADORA*((float)x-g_idata[iminimo].pesos[0]);
	g_idata[iminimo].pesos[1] += COEF_GANADORA*((float)y-g_idata[iminimo].pesos[1]);
	g_idata[iminimo].pesos[2] += COEF_GANADORA*((float)z-g_idata[iminimo].pesos[2]);

	// Se modifican los pesos de las neuronas vecinas
	for(int i=1;i<=mat[iminimo*(100+1)].indice_vecina;i++)
	{
		g_idata[mat[iminimo*(100+1)+i].indice_vecina].pesos[0]+=COEF_VECINAS*((float)x-g_idata[mat[iminimo*(100+1)+i].indice_vecina].pesos[0]);
		g_idata[mat[iminimo*(100+1)+i].indice_vecina].pesos[1]+=COEF_VECINAS*((float)y-g_idata[mat[iminimo*(100+1)+i].indice_vecina].pesos[1]);
		g_idata[mat[iminimo*(100+1)+i].indice_vecina].pesos[2]+=COEF_VECINAS*((float)z-g_idata[mat[iminimo*(100+1)+i].indice_vecina].pesos[2]);
	}

	bool encontrado=false;

	for(int i=1;i<=mat[iminimo*(100+1)].indice_vecina && encontrado == false;i++)
	{
		if( mat[iminimo*(100+1)+i].indice_vecina == iminimo2)
		{
			encontrado=true;
			mat[iminimo*(100+1)+i].antiguedad = 0;
		}
	}

	if(encontrado==false)
	{
		int num = mat[iminimo*(100+1)].indice_vecina;
		mat[iminimo*(100+1)+num+1].antiguedad = 0;
		mat[iminimo*(100+1)+num+1].indice_vecina = iminimo2;
		mat[iminimo*(100+1)].indice_vecina = mat[iminimo*(100+1)].indice_vecina + 1;
	}

	encontrado=false;
	for(int i=1;i<=mat[iminimo2*(100+1)].indice_vecina && encontrado == false;i++)
	{
		if( mat[iminimo2*(100+1)+i].indice_vecina == iminimo )
		{
			encontrado=true;
			mat[iminimo2*(100+1)+i].antiguedad = 0;
		}
	}

	if(encontrado==false)
	{
		int num = mat[iminimo2*(100+1)].indice_vecina;
		mat[iminimo2*(100+1)+num+1].antiguedad = 0;
		mat[iminimo2*(100+1)+num+1].indice_vecina = iminimo;
		mat[iminimo2*(100+1)].indice_vecina = mat[iminimo2*(100+1)].indice_vecina + 1; 
	}
}


int init_mem_cuda_gng(Neurona* &d_idata, reduceSTRUCT2* &d_odata,auxVECINA* &d_matVecinas,int max_neurons)
{
	int maxThreads = 256;  // number of threads per block
	int maxBlocks = 64;

	// allocate mem for the result on host side
	//reduceSTRUCT2* h_odata = (reduceSTRUCT2*) malloc(numBlocks*sizeof(reduceSTRUCT2));

	// size of bytes that gng need to store neurons
	unsigned int bytes = max_neurons * sizeof(Neurona);

	cutilSafeCallNoSync( cudaMalloc((void**) &d_idata, bytes) );
	cutilSafeCallNoSync( cudaMalloc((void**) &d_odata, maxBlocks*sizeof(reduceSTRUCT2)) );
	cutilSafeCallNoSync( cudaMalloc((void**) &d_matVecinas, ((max_neurons*100)+max_neurons)*sizeof(auxVECINA)) );

	//printf(" bytes d_idata = %i \n",bytes);
	//printf(" bytes d_odata = %i \n",maxBlocks*sizeof(reduceSTRUCT2));
	//printf(" d_matVecinas = %i \n",((max_neurons*100)+max_neurons)*sizeof(auxVECINA));

	return 0;
}

int destroy_mem_cuda_gng(Neurona* d_idata, reduceSTRUCT2* d_odata )
{
  cutilSafeCallNoSync(cudaFree(d_idata));
  cutilSafeCallNoSync(cudaFree(d_odata));
  return 0;
}

/************************************************************************/
/* CU_AjusteGNG                                                         */
/************************************************************************/
int CU_AjusteGNG(Neurona *h_idata,Neurona *d_idata,reduceSTRUCT2* d_odata,auxVECINA* h_matVecinas,auxVECINA* d_matVecinas,int numThreads,int numBlocks,int size,int NUM_ENTRADAS,float *nube_puntos,int NUM_PUNTOS)
{
	int entrada;
	float minimo,minimo2;
	int iminimo=0,iminimo2=1;
	float error;
	
	float COEF_GANADORA=(float)0.1;
    float COEF_VECINAS=(float)0.01;
	int UMBRAL_DATOS=127;

    dim3 dimBlock(numThreads, 1, 1);
    dim3 dimGrid(numBlocks, 1, 1);
    int smemSize = numThreads * sizeof(Neurona);

	int al;
	float x,y,z;

	error=0;
	minimo=minimo2=FLT_MAX;

	////Reservamos memoria para el vector neuronas que vamos a copiar a la memoria global GPU
    unsigned int bytes = size * sizeof(Neurona);

	/*cudaEvent_t start,stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	cudaEventRecord(start,0);*/
    cudaMemcpy(d_idata, h_idata, bytes, cudaMemcpyHostToDevice);
	/*cudaEventRecord(stop,0);
	cudaEventSynchronize(stop);

	float elapsedTime;
	cudaEventElapsedTime(&elapsedTime,start,stop);
	printf("Time to transfer host -> device: %3.5f ms -- bytes= %d\n",elapsedTime,bytes);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);*/

	/****************************************************************************************/
	//Iteramos para las N_ENTRADAS modificando los pesos de la neurona ganadora y sus vecinas
	/****************************************************************************************/

	cudaMemcpyToSymbol("COEF_GANADORA",    &COEF_GANADORA,    sizeof(float));
	cudaMemcpyToSymbol("COEF_VECINAS",    &COEF_VECINAS,    sizeof(float));

	int aux;
	if( isPow2(size) )
	{
		cudaMemcpyToSymbol("size",    &size,    sizeof(size));
	}else
	{
		aux = nextPow2(size);
		cudaMemcpyToSymbol("size",    &aux,    sizeof(aux));	
	}

	// **************************************************************************************************
	// *************************** COPIAMOS EL std::vector Y REALIZAMOS UNA COPIA ***********************
	// **************************************************************************************************
	
	for(int i=0;i<size;i++)
	{
		if( h_idata[i].utilizada == false )
		{
			h_matVecinas[i*(100+1)].indice_vecina = -1;
			h_matVecinas[i*(100+1)].antiguedad = -1;
		}
		else
		{	
			h_matVecinas[i*(100+1)].indice_vecina = h_idata[i].vecinas.size();

			for(int j=0;j<h_idata[i].vecinas.size();j++)
			{
				h_matVecinas[i*(100+1)+j+1].indice_vecina = h_idata[i].vecinas[j].vecina;
				h_matVecinas[i*(100+1)+j+1].antiguedad = h_idata[i].vecinas[j].antiguedad;
			}
		}
	}

	cudaMemcpy(d_matVecinas, h_matVecinas,((size*100)+size)*sizeof(auxVECINA), cudaMemcpyHostToDevice);

	// **************************************************************************************************
	// **************************************************************************************************
	// **************************************************************************************************

	for (entrada=0;entrada<NUM_ENTRADAS;entrada++)
	{
		al=rand()%(NUM_PUNTOS);

		x=nube_puntos[al*3];
		y=nube_puntos[al*3+1];
		z=nube_puntos[al*3+2];

		reduceMinNeurona3_Min2<<< dimGrid, dimBlock, smemSize >>>(d_idata, d_odata, x, y, z);

		adjustWeightsWinningNeurons<<<1,1>>>(d_matVecinas, d_idata, d_odata, numBlocks,x,y, z);
	}

	//Copiamos la matriz después de las lambda entradas y actualizamos el vector de neuronas host
	cudaMemcpy(h_matVecinas, d_matVecinas, sizeof(auxVECINA)*((size*100)+size), cudaMemcpyDeviceToHost);
	
	//Copiamos los cambios realizados en el vector de neuronas device
	cutilSafeCall( cudaMemcpy(h_idata, d_idata, bytes, cudaMemcpyDeviceToHost) );

	for(int i=0;i<size;i++)
	{
		int numVecinas = h_matVecinas[i*(100+1)].indice_vecina;
		
		//Borramos las vecinas de esa neurona y actualizamos
		if(numVecinas > 0)	h_idata[i].vecinas.clear();

		for(int j=1;j<=numVecinas;j++)
		{
			VECINAS vec;
			vec.vecina = h_matVecinas[i*(100+1)+j].indice_vecina;
			vec.antiguedad = h_matVecinas[i*(100+1)+j].antiguedad;
			h_idata[i].vecinas.push_back(vec);
		}
	}
	
	return 0;
}

void checkCUDAError(const char *msg)
{
    cudaError_t err = cudaGetLastError();
    if( cudaSuccess != err) 
    {
        fprintf(stderr, "Cuda error: %s: %s.\n", msg, 
                                  cudaGetErrorString( err) );
        exit(EXIT_FAILURE);
    }                         
}
