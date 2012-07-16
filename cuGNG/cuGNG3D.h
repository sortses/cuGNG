/********************************************************************
*  sample_cu.h
*  This is a example of the CUDA program.
*********************************************************************/
#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include "Neurona.h"
#include <windows.h>

/************************************************************************/
/* HelloCUDA                                                            */
/************************************************************************/

int CU_AjusteGNG(Neurona *h_idata,Neurona *d_idata,reduceSTRUCT2* d_odata,auxVECINA* h_matVecinas,auxVECINA* d_matVecinas,int numThreads,int numBlocks,int size,int NUM_ENTRADAS,float *nube_puntos,int NUM_PUNTOS);

int init_mem_cuda_gng(Neurona* &d_idata, reduceSTRUCT2* &d_odata,auxVECINA* &d_matVecinas,int max_neurons);

int destroy_mem_cuda_gng(Neurona* d_idata, reduceSTRUCT2* d_odata );

void getNumBlocksAndThreads(int n,int &blocks, int &threads);

bool init_cuda_device(int num_device);

bool isPow2(unsigned int x);

unsigned int nextPow2( unsigned int x );

double performancecounter_diff(LARGE_INTEGER *a, LARGE_INTEGER *b);


#endif // __SAMPLE_H__