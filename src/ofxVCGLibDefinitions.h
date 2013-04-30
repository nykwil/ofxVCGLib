/*
 *  ofxVCGLibDefinitions.h
 *  ofxVCGEx
 *
 */


#pragma once


#include "ofMain.h"

#include<complex\complex.h>
#include<vcg\space\index\grid_static_ptr.h>

#include "ofxVCGCloud.h"
//#include "ofxVCGVolume.h"

#define __FLT_EVAL_METHOD__ 0

using namespace vcg;
using namespace tri;

// base types
class ofxVCGVertex;

class innerMeshFace; // composition -> ofxMeshFace
class innerMesh; // converted to ofMesh
class innerEdge;

struct ofxVCGTypes:public UsedTypes<Use<ofxVCGVertex>::AsVertexType, Use<innerEdge>::AsEdgeType, Use<innerMeshFace>::AsFaceType>{};

class ofxVCGVertex: 
public Vertex<ofxVCGTypes, vertex::Coord3f, vertex::BitFlags, vertex::Normal3f, vertex::Mark, vertex::Color4b, vertex::VFAdj>  
{

public:
	
	ofxVCGVertex() {
	
		InitIMark(); // do this so that it keeps track of its index
	}
	
	ofxVCGVertex(ofVec3f sourceVec3f)
	{
		
		InitIMark(); // do this so that it keeps track of its index
		
		P().X() = sourceVec3f.x;
		P().Y() = sourceVec3f.y;
		P().Z() = sourceVec3f.z;
		
	}
	
	void update(ofVec3f sourceVec3f)
	{
		P().X() = sourceVec3f.x;
		P().Y() = sourceVec3f.y;
		P().Z() = sourceVec3f.z;
	}
	
	ofVec3f& toOf()
	{
		ofVec.x = P().X();
		ofVec.y = P().Y();
		ofVec.z = P().Z();
		return ofVec;
	}
	
	ofVec3f& normalToOf()
	{
		ofVec.x = N().X();
		ofVec.y = N().Y();
		ofVec.z = N().Z();
		return ofVec;
	}
	
private:
	
	ofVec3f ofVec;
	
};

// used to store face info
// only used in vcg->ofxMeshFace conversion
class innerMeshFace:public Face<ofxVCGTypes, face::FFAdj, face::Mark, face::VertexRef, face::BitFlags, face::Normal3f> {
	
};

// used to store edge info
class innerEdge:public Edge<ofxVCGTypes>{
	
};

class innerMesh:public vcg::tri::TriMesh<vector<ofxVCGVertex>, vector<innerMeshFace> > {
	
	public:
	
	vector<int> mesh_indices;
	
	innerMesh() {}
	
	innerMesh(ofMesh* mesh);
	
	vector<float> getVertices() {
		
		vector<float> verts;
		for (int i = 0; i < vert.size(); i++) {
			verts.push_back(vert[i].P()[0]);
			verts.push_back(vert[i].P()[1]);
			verts.push_back(vert[i].P()[2]);
		}
		return verts;
		
	}
	
	vector<int> getFaceIndices();
	

};

class ofxVCGRay {
	
public:
	ofVec3f begin, direction;
	
};
/*
// to be implemented
class ofxVCGMesh : public vrt::VertexMesh< vector<ofxVCGVertex> > {
};
*/
// other typedefs

//typedef SimpleVolume<SimpleVoxel> ofxVCGVolume;
//typedef ofxVCGVolume<SimpleVoxel> ofxVolume;

typedef vcg::GridStaticPtr<innerMesh::FaceType, innerMesh::ScalarType> innerMeshGrid;

