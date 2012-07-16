#ifndef __GEOM_GTS__
#define __GEOM_GTS__

#include <iostream>
#include <vector>
#include "tmapas.h"

using namespace std;

// Class to handle a vertex in 3d space (x,y,z)
//
class Vertex
{
	public:
		// constructor, overload constructor, destructor, copy constructor
		Vertex();
		Vertex(int id,float x,float y,float z);
		~Vertex();

		// getters
		float getX() const;
		float getY() const;
		float getZ() const;
		int getId() const;

		// setters
		void setX(float x);
		void setY(float y);
		void setZ(float z);

	private:
		// x,y,z position of vertex in 3D space
		float x,y,z;
		int id;

};

// class to handle edges mesh, each edge is composed by two vertices
class Edge
{
	private:
		int idV1,idV2;
		int id;
		
	public:
		Edge();
		Edge(int id,int idV1, int idV2);
		~Edge();
		
		// getters
		int getIdV1() const;
		int getIdV2() const;
		int getId() const;

		// setters
		void setIdV1(int idV1);
		void setIdV2(int idV2);
};

// class to handle faces of a 3d mesh, contains a triangle composed by a 
class Face
{
	private:
		int idE1,idE2,idE3;
		int id;
	
	public:
		Face();
		Face(int id,int idE1,int idE2,int idE3);
		~Face();
		
		// getters
		int getIdE1() const;
		int getIdE2() const;
		int getIdE3() const;
		int getId()	  const;


		// setters
		void setIdE1(int idE1);
		void setIdE2(int idE2);
		void setIdE3(int idE3);
};

// class that holds a 3D mesh 
//
class Mesh
{
	private:
		// Edges of faces
		//
		vector<Edge> *edges;

		// Faces of a surface mesh
		//
		vector<Face> *faces;

		// Number of Vertex && vertices mesh
		//
		int numVertex;
		Vertex *vertices;

	public:
		Mesh();
		Mesh(Mapa* mapa);
		~Mesh();
		Mesh(Mapa &m);
		
		bool generarFicheroGTS(char *nombreFichero);
		bool generarFicheroGNG(char *nombreFichero);
		

		int getId() const;
};



#endif