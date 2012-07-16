#include "geom_gts.h"

// Constructors, destructores, copy constructors, methods 
// Vertex Class

Vertex::Vertex()
{
	x=y=z=0;
}

Vertex::Vertex(int id,float x, float y, float z)
{
	this->id = id;
	this->x = x;
	this->y = y;
	this->z = z;
}

Vertex::~Vertex(){}

float Vertex::getX() const{ return this->x; }
float Vertex::getY() const{ return this->y; }
float Vertex::getZ() const{ return this->z; }

void Vertex::setX(float x){ this->x = x; }
void Vertex::setY(float y){ this->y = y; }
void Vertex::setZ(float z){ this->z = z; }

int Vertex::getId() const{ return this->id; }

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

// Constructors, destructors, copy constructors, methods 
// Edge Class

Edge::Edge(){}

Edge::Edge(int id,int v1,int v2)
{
	this->id = id;
	this->idV1 = v1;
	this->idV2 = v2;
}

Edge::~Edge()
{
}

int Edge::getIdV1() const{ return idV1; }
int Edge::getIdV2() const{ return idV2; }

void Edge::setIdV1(int v){ this->idV1 = v; }
void Edge::setIdV2(int v){ this->idV2 = v; }

int Edge::getId() const{ return this->id; }

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

// Constructors, destructors, copy constructors, methods 
// Face Class

Face::Face(){}

Face::Face(int id,int e1,int e2,int e3)
{
	this->id = id;
	this->idE1 = e1;
	this->idE2 = e2;
	this->idE3 = e3;
}

Face::~Face()
{
}

int Face::getIdE1() const { return idE1; }
int Face::getIdE2() const { return idE2; }
int Face::getIdE3() const { return idE3; }

void Face::setIdE1(int e){ this->idE1 = e; }
void Face::setIdE2(int e){ this->idE2 = e; }
void Face::setIdE3(int e){ this->idE3 = e; }

int Face::getId() const{ return this->id; }

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

// Constructors, destructors, copy constructors, methods 
// Mesh Class


Mesh::Mesh()
{
}

Mesh::Mesh(Mapa* mapa)
{
	int it=0;
	int count=0;
	bool exit=false;

	this->numVertex = mapa->num_neuronas;

	// Counters for edges, faces
	int countEdges=0,countFaces=0;
	
	// malloc memory for vertices
	vertices = new Vertex[numVertex];
	if(!vertices)
	{	
		cout<<"ERROR: couldn't malloc memory for vertices."<<endl;
	}else
	{
		// fill vertices array with neurons data
		for(it;it<numVertex;it++)
		{
			if(mapa->neurona[it].utilizada)
			{
				Vertex aux(it+1,mapa->neurona[it].pesos[0],mapa->neurona[it].pesos[1],mapa->neurona[it].pesos[2]);
				vertices[it]=aux;
			}
		}

		// create edges
		edges = new vector<Edge>();
		for(it=0;it<numVertex;it++)
		{
			
			VECTOR::iterator vecina;
			if(mapa->neurona[it].utilizada)
			{
				// for every neuron, visit neighbours and create edges
				for (vecina=mapa->neurona[it].vecinas.begin();vecina!=mapa->neurona[it].vecinas.end();vecina++)
				{
					if(it < (*vecina).vecina+1)
					{
						countEdges++;
						Edge e(countEdges,it+1,(*vecina).vecina+1);
						edges->push_back(e);
					}
				}
			}
		}

		faces = new vector<Face>();

		std::vector<Edge>::iterator eit1,eit2,eit3;
		for(eit1=edges->begin();eit1!=edges->end();eit1++)
		{
			bool encontrado=false;
			
			int v1 = (*eit1).getIdV1();
			int v2 = (*eit1).getIdV2();
			
			for(eit2=edges->begin();eit2!=edges->end() && !encontrado;eit2++)
			{
				if( (*eit2).getIdV1() == v2 && eit1!=eit2 )
				{
					for(eit3=edges->begin();eit3!=edges->end() && !encontrado;eit3++)
					{
						if( ((*eit3).getIdV1() == v1 && (*eit3).getIdV2() == (*eit2).getIdV2()) || ((*eit3).getIdV2() == v1 && (*eit3).getIdV1() == (*eit2).getIdV2()) )
						{							
							encontrado=true;
							//increment number of faces
							countFaces++;
							
							// obtain indexes of edges
							int idE1 = (*eit1).getId();
							int idE2 = (*eit2).getId();
							int idE3 = (*eit3).getId();
							
							// push back face
							Face f(countFaces,idE1,idE2,idE3);
							faces->push_back(f);
						}
					}					
				}else if( (*eit2).getIdV2() == v2 && eit1!=eit2 )
				{
					bool encontrado=false;
					for(eit3=edges->begin();eit3!=edges->end() && !encontrado;eit3++)
					{
						if( ((*eit3).getIdV1() == v1 && (*eit3).getIdV2() == (*eit2).getIdV1()) || ((*eit3).getIdV2() == v1 && (*eit3).getIdV1() == (*eit2).getIdV1()) )
						{
							encontrado=true;
							//increment number of faces
							countFaces++;
							
							// obtain indexes of edges
							int idE1 = (*eit1).getId();
							int idE2 = (*eit2).getId();
							int idE3 = (*eit3).getId();
							
							// push back face
							Face f(countFaces,idE1,idE2,idE3);
							faces->push_back(f);
						}
					}	
				}
			}
		}
		
		// finish mesh construction
	}
}

Mesh::~Mesh(){
}
		
bool Mesh::generarFicheroGTS(char *nombreFichero)
{
	FILE *fich;

	fich=fopen(nombreFichero,"w");
	if (fich!=NULL){
		fprintf(fich,"%i %i %i\n",numVertex,edges->size(),faces->size());
	   
	   //out points
	   for (int i=0;i<numVertex;i++)
	   {
		  fprintf(fich,"%f %f %f \n",this->vertices[i].getX(),this->vertices[i].getY(),this->vertices[i].getZ());
	   }

	   //out edges
	   for (int i=0;i<edges->size();i++)
	   {
		   Edge ed=this->edges->at(i);
		   fprintf(fich,"%i %i\n",ed.getIdV1(),ed.getIdV2());
	   }

	   //out faces
	   for (int i=0;i<this->faces->size();i++)
	   {
		  Face fa=this->faces->at(i);
		  fprintf(fich,"%i %i %i \n",fa.getIdE1(),fa.getIdE2(),fa.getIdE3());
	   }
	}else
		return false;

	if(fich!=NULL) fclose(fich);
	else return false;

	return true;
}

bool Mesh::generarFicheroGNG(char *nombreFichero)
{
	FILE *fich;

	fich=fopen(nombreFichero,"w");
	if (fich!=NULL){
		fprintf(fich,"%i %i\n",numVertex,edges->size());
	   
	   //out points
	   for (int i=0;i<numVertex;i++)
	   {
		  fprintf(fich,"%f %f %f \n",this->vertices[i].getX(),this->vertices[i].getY(),this->vertices[i].getZ());
	   }

	   //out edges
	   for (int i=0;i<edges->size();i++)
	   {
		   Edge ed=this->edges->at(i);
		   fprintf(fich,"%i %i\n",ed.getIdV1(),ed.getIdV2());
	   }
	}else
		return false;

	if(fich!=NULL) fclose(fich);
	else return false;

	return true;
}

