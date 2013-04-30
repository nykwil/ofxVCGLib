/*
 *  ofxVCGLib.cpp
 *
 *  Created by Makira on 11/03/03.
 *  Copyright 2011 yesMAYBEno. All rights reserved.
 *
 */

#include "ofxVCGLib.h"
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/create/ball_pivoting.h>
#include <vcg/complex/algorithms/intersection.h>
#include <vcg/complex/algorithms/pointcloud_normal.h>

namespace ofxVCG {

ofMesh* intersectMeshes(vector<ofMesh> meshes)
{
	
	//tri::Append<MyMesh,MyMesh>::Mesh(ml,mr);
    return 0;
}

void cleanMesh(ofMesh* mesh)
{
	innerMesh m(mesh);
	
	int dup = tri::Clean<innerMesh>::RemoveDuplicateVertex(m);
	int unref =  tri::Clean<innerMesh>::RemoveUnreferencedVertex(m);

	// updating
	UpdateBounding<innerMesh>::Box(m);
	UpdateNormal<innerMesh>::PerFaceNormalized(m);
	UpdateNormal<innerMesh>::PerVertexAngleWeighted(m);
	UpdateNormal<innerMesh>::NormalizePerVertex(m);
//	UpdateFlags<innerMesh>::FaceProjection(m);
	
	// copy back
	vcgMeshToOf(&m, mesh);
}

// make this read in files too
void smoothMesh(ofMesh* mesh, int steps, int smoothingAmount)
{
	innerMesh m(mesh);
	
	// probably want different meshes for each method
	// but the laplacian is one of the simpler
//	tri::Smooth<innerMesh>::VertexCoordLaplacian(m, steps); 
	
	vcgMeshToOf(&m, mesh);
	
}

void vcgMeshToOf(innerMesh* inner, ofMesh* mesh)
{
	// this desperately needs to be smarter, big meshes are going to
	// be bears to deal with.
	mesh->clear();
	
	innerMesh::VertexIterator vi = inner->vert.begin();
	while(vi != inner->vert.end())
	{
		mesh->addVertex(vi->toOf());
		ofVec3f vec(vi->N().X(), vi->N().Y(), vi->N().Z());
		mesh->addNormal(vec);
		++vi;
	}
	
	vector<int> faceIndices = inner->getFaceIndices();
	vector<int>::iterator fi = faceIndices.begin();
	while(fi != faceIndices.end())
	{
		mesh->addIndex(*fi);
		++fi;
	}
}
							

void refineMesh(ofMesh* mesh, int steps, int type=0)
{
	
	float length = 0;
	innerMesh m(mesh);
	int i;
	for(i=0;i < steps;++i)			
	{
		switch(type)
		{
				// some weird template madness, this doesn't compile :|
/*			case 0:
				vcg::Refine<innerMesh, MidPoint<innerMesh> >(m,MidPoint<innerMesh>(&m),length);
				break;
			case 1:
				tri::RefineOddEven<innerMesh, tri::OddPointLoop<innerMesh>, tri::EvenPointLoop<innerMesh> >(m, tri::OddPointLoop<innerMesh>(), tri::EvenPointLoop<innerMesh>(), length);
				break;
			case 2:
				tri::BitQuadCreation<innerMesh>::MakePureByCatmullClark(m);
				tri::UpdateNormals<innerMesh>::PerBitQuadFaceNormalized(m);
				break;      
			case 3:
				tri::BitQuadCreation<innerMesh>::MakePureByRefine(m);
				tri::UpdateNormals<innerMesh>::PerBitQuadFaceNormalized(m);
				break;
			case 4:
				Refine<innerMesh, MidPointButterfly<innerMesh> >(m,MidPointButterfly<innerMesh>(),length);
				break; */
		}		
	}
	
}

// hmm
void simplifyNode(ofNode* node, int degree)
{
}


ofMesh* differenceMeshes(vector<ofMesh> meshes)
{
    return 0;
}

ofMesh* joinMeshes( ofMesh* toBeParent, vector<ofMesh*> toBeJoined )
{
	innerMesh mP(toBeParent);
	
	vector<ofMesh*>::iterator meshIt = toBeJoined.begin();
	while(meshIt != toBeJoined.end()) {
		innerMesh mC( *meshIt ); // messy
		tri::Append<innerMesh,innerMesh>::Mesh(mP,mC); // join them
		++meshIt;
	}
	
	tri::Clean<innerMesh>::RemoveDuplicateVertex(mP); // now clean it up 
	
    return 0;
}

ofMesh* joinMeshes( ofMesh* toBeParent, ofMesh* toBeChild )
{
	innerMesh mP(toBeParent);
	innerMesh mC(toBeChild);
	
	tri::Append<innerMesh,innerMesh>::Mesh(mP,mC); // join them
	tri::Clean<innerMesh>::RemoveDuplicateVertex(mP); // now clean it up 
	
	ofMesh* mesh = new ofMesh();
	
	vcgMeshToOf(&mP, mesh);
	
	return mesh;
	
}



ofNode* intersectNodes(vector<ofNode> nodes)
{
    return 0;
}


ofNode* differenceNodes(vector<ofNode> nodes)
{
    return 0;
}


ofNode* joinNodes(vector<ofNode> nodes)
{
    return 0;
}



bool meshIntersection(ofMesh* aNode, ofMesh* bNode)
{
    return 0;
}


bool nodeIntersection(ofNode* aNode, ofNode* bNode)
{
    return 0;
}



// smart pointers would be freaking sweet here

ofMesh* createMeshFromPoints(vector<ofVec3f> points, int degreeOfFidelity, meshConstructionAlgo useMeshConstructionAlgo = BALL_PIVOT_CONSTRUCTION)
{
	
	innerMesh m;	
	
	if(useMeshConstructionAlgo == BALL_PIVOT_CONSTRUCTION)
	{
		
		Allocator<innerMesh>::AddVertices(m, points.size());
	
		
		const int sz = points.size();
		innerMesh::VertexIterator vi = m.vert.begin();
		vector<ofVec3f>::iterator pit = points.begin();
		while(pit != points.end())
		{	
			(*vi).update( *pit ); 
			++vi;
			++pit;
		}
		
		
		vcg::tri::UpdateBounding<innerMesh>::Box(m);
		vcg::tri::UpdateNormal<innerMesh>::PerFace(m);
		
		tri::BallPivoting<innerMesh>* pivot = new tri::BallPivoting<innerMesh>(m); 	
		pivot->BuildMesh();
		
//		NormalExtrapolation<vector<ofxVCGVertex> >::ExtrapolateNormals(m.vert.begin(), m.vert.end(), 2);
	}
	else
	{
		/*ofxVolume volume;
		volume.Init(points/3);
		
		vector<float>::iterator pit = points.begin();
		while(pit != points.end())
		{	
			volume.Val(*pit, ++pit, ++pit); 
			++pit;
		}
		
		
		vcg::tri::TrivialWalker<innerMesh,ofxVolume> walker;
		vcg::tri::MarchingCubes<innerMesh, TrivialWalker<innerMesh, ofxVolume> > marchingCubes;
		
		MyMarchingCubes	mc(m, walker);
		walker.BuildMesh<MyMarchingCubes>(m, volume, marchingCubes, 20*20);*/
		
	}
	ofMesh* ofm = new ofMesh();
	
	vcgMeshToOf(&m, ofm);
	
	return ofm;
	
}


ofMesh* createMeshFromPoints(vector<float> points, int degreeOfFidelity, meshConstructionAlgo useMeshConstructionAlgo = BALL_PIVOT_CONSTRUCTION)
{
	
	innerMesh m;
	
	if(useMeshConstructionAlgo == BALL_PIVOT_CONSTRUCTION)
	{
		
		
		Allocator<innerMesh>::AddVertices(m, points.size()/3);
		
		
		const int sz = points.size();
		innerMesh::VertexIterator vi = m.vert.begin();
		vector<float>::iterator pit = points.begin();
		while(pit != points.end())
		{	
			(*vi).P().X() = *pit; 
			(*vi).P().Y() = *(++pit);
			(*vi).P().Z() = *(++pit);
			++vi;
			++pit;
		}
		
		
		vcg::tri::UpdateBounding<innerMesh>::Box(m);
		vcg::tri::UpdateNormal<innerMesh>::PerFace(m);
		
		tri::BallPivoting<innerMesh>* pivot = new tri::BallPivoting<innerMesh>(m); 	
		pivot->BuildMesh();
		
//		NormalExtrapolation<vector<ofxVCGVertex> >::ExtrapolateNormals(m.vert.begin(), m.vert.end(), 2);

	}
	else
	{
		
		/*ofxVolume volume;
		volume.Init(points/3);
		
		vector<float>::iterator pit = points.begin();
		while(pit != points.end())
		{	
			volume.Val(*pit, ++pit, ++pit); 
			++pit;
		}
		
		
		vcg::tri::TrivialWalker<innerMesh,ofxVolume> walker;
		vcg::tri::MarchingCubes<innerMesh, TrivialWalker<innerMesh, ofxVolume> > marchingCubes;
		
		MyMarchingCubes	mc(m, walker);
		walker.BuildMesh<MyMarchingCubes>(m, volume, marchingCubes, 20*20);*/
		
	}
	
	ofMesh* ofm = new ofMesh();
	
	vcgMeshToOf(&m, ofm);
	
	return ofm;
	
}


vcgRayMeshIntersection getFacesForRay(ofxVCGRay ray, ofMesh* mesh)
{
	innerMesh m(mesh);
	float t;
	float maxDist = 1000;
	
	// Create a static grid (for fast indexing) and fill it
	innerMeshGrid grid;
	grid.Set(m.face.begin(), m.face.end());
	
	// convert the ray
	vcg::Ray3f vcg_ray;
	vcg::Point3f beg(ray.begin.x, ray.begin.y, ray.begin.z);
	vcg::Point3f dir(ray.direction.x, ray.direction.y, ray.direction.z);
	vcg_ray.SetOrigin(beg);
	vcg_ray.SetDirection(dir);
	
	// get the baricentric points of the intersection
	// plus the face intersected plus the point where they meet
	innerMesh::FacePointer intersectedFace;
	innerMesh::ScalarType bc1, bc2, bc3;
	Point3f hitPoint;
	
	Line3f line;
	
	IntersectionRayMesh(&m, line, hitPoint, bc1, bc2, bc3, intersectedFace);
	
	// now set the intersection data so we have it on the other side
	vcgRayMeshIntersection intersectionData;
	
	intersectionData.baricentricCenter.x = bc1;
	intersectionData.baricentricCenter.y = bc2;
	intersectionData.baricentricCenter.z = bc3;
	
	ofxMeshFace face(intersectedFace);
	intersectionData.intersectedFace = face;
	
	intersectionData.hitPoint.x = hitPoint.X();
	intersectionData.hitPoint.y = hitPoint.Y();
	intersectionData.hitPoint.z = hitPoint.Z();
	
	return intersectionData;
}

void pointsToPlane(vector<ofVec2f> points)
{
}


// don't really like the c-style-ness of this
void getNeighboringFaces(ofxMeshFace* face, ofMesh* mesh)
{
}

void getFacesFromMesh(vector<ofxMeshFace>* faces, ofMesh* mesh){
}

void constructMeshFromFaces(ofMesh* mesh, vector<ofxMeshFace>* faces){
}

void cleanCloudPoints(ofMesh* in, ofMesh* out, float radius) {

	innerMesh m(in);
		
	vcg::tri::UpdateBounding<innerMesh>::Box(m);
	
	if(m.getFaceIndices().size() == 0) {
		cout << "no faces, rebuilding topology" << endl;
		tri::BallPivoting<innerMesh>* pivot = new tri::BallPivoting<innerMesh>(m); 	
		pivot->BuildMesh();
		vcg::tri::UpdateNormal<innerMesh>::PerFace(m);
	}
	
	// now clean the mesh
	int dupdV = tri::Clean<innerMesh>::RemoveDuplicateVertex(m); // any dup'd vertices
	int mergeClose = tri::Clean<innerMesh>::MergeCloseVertex(m, radius); // merge any close vertices
	int degenV = tri::Clean<innerMesh>::RemoveDegenerateVertex(m);
	
	int degenF = tri::Clean<innerMesh>::RemoveDegenerateFace(m); // lame-faces goodbye
	int unref = tri::Clean<innerMesh>::RemoveUnreferencedVertex(m); // now any vertices
	
	cout << " removed " << dupdV << " duplicate vertices " << endl;
	cout << " merged " << mergeClose << " close vertices " << endl;
	cout << " removed " << degenF << " degenerate faces " << endl;
	cout << " removed " << degenV << " degenerate vertices " << endl;
	cout << " removed " << unref << " unreferenced vertices " << endl;
	
	// Rebuild topology
	tri::UpdateTopology<innerMesh>::FaceFace(m);
	tri::UpdateTopology<innerMesh>::VertexFace(m);
	
	vcgMeshToOf(&m, out);
	
	cout << " mesh has " << m.vn << " vertices and " << m.fn << " faces while the cloud has " << out->getVertices().size() << " vec3f " << endl;
}

/*
void reconstructFaceBunny(float _radius, 
									 float _clustering,
									 float _angle) {
	
	radius = _radius;
	clustering = _clustering;
	angle = _angle;
	
	m.face.clear();
	m.fn = 0;	
	m.Clear();	
	
	for (int i = 0; i < BUNNY_NUM_VERTICES; i++) {
		float x = gVerticesBunny[i*3];
		float y = gVerticesBunny[i*3+1];
		float z = gVerticesBunny[i*3+2];
		CVertex mv;
		mv.P() = Point3f(x,y,z);
		m.vert.push_back(mv);
		m.vn++;
//		cout << ofToString(x)+" "+ofToString(y)+" "+ofToString(z) << endl;
	}
//	cout << "total vertices = " + ofToString(m.vn) << endl;
	
	vcg::tri::UpdateBounding<CMesh>::Box(m);
	vcg::tri::UpdateNormals<CMesh>::PerFace(m);	
	tri::BallPivoting<CMesh> pivot(m, _radius, _clustering, _angle); 	
	pivot.BuildMesh();

}

void reconstructFacePointCloud(vector<float> point_cloud, 
										  float _radius, 
										  float _clustering,
										  float _angle) {
	radius = _radius;
	clustering = _clustering;
	angle = _angle;	
	
	m.face.clear();
	m.fn = 0;	
	m.Clear();
	
	for (int i = 0; i < point_cloud.size()/3; i++) {
		float x = point_cloud[i*3];
		float y = point_cloud[i*3+1];
		float z = point_cloud[i*3+2];
		CVertex mv;
		mv.P() = Point3f(x,y,z);
		m.vert.push_back(mv);
		m.vn++;
		cout << ofToString(x)+" "+ofToString(y)+" "+ofToString(z) << endl;
	}
	cout << "point cloud size = " + ofToString((int)point_cloud.size()) << endl;
	cout << "total vertices = " + ofToString(m.vn) << endl;
	
	vcg::tri::UpdateBounding<CMesh>::Box(m);
	vcg::tri::UpdateNormals<CMesh>::PerFace(m);	
	legacyPivot = NULL;
	if (legacyPivot) delete legacyPivot;
	legacyPivot = new tri::BallPivoting<CMesh>(m, _radius, _clustering, _angle); 	
	legacyPivot->BuildMesh();	
	//NormalExtrapolation<vector<CVertex> >::ExtrapolateNormals(m.vert.begin(), m.vert.end(), 2);	
	
	//pivot = new tri::ofxVCGPivot<CMesh>(m, _radius, _clustering, _angle);
	
	int test = 9;
}

bool addFace() {


	for(int i =0; i < 100; i++) {
//		for (int j = 0; j < m.vert.size(); j++) {
//			m.vert[j].ClearFlags();
//		}		
		//if(!pivot->seed()) cout << "seed fail!" << endl;
		cout << "trying add face : " + ofToString(i) << endl;
        if(1 == pivot->addFace()) return false;
	}
	
//	legacyPivot->last_seed = -1;
//	for (int j = 0; j < m.vert.size(); j++) {
//		m.vert[j].ClearFlags();
//	}	
//	while(1) {
//		for(int i = 0; i < 512; i++) {
//			if(!legacyPivot->front.size() && !legacyPivot->SeedFace()) return false;
//			legacyPivot->AddFace();
//			cout << "add face manually!!!!!!!!" << endl;
//		}
//    }	
//	return true;
}

vector<float> getVertices() {
	
	vector<float> verts;
	for (int i = 0; i < m.vert.size(); i++) {
		verts.push_back(m.vert[i].P()[0]);
		verts.push_back(m.vert[i].P()[1]);
		verts.push_back(m.vert[i].P()[2]);
	}
	return verts;
	
}

vector<int> getFaceIndices() {
	
	vector<int> faceIndices;
	for (int i = 0; i < m.face.size(); i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < m.vert.size(); k++) {
				float xc = m.face[i].V(j)->P()[0];
				float yc = m.face[i].V(j)->P()[1];
				float zc = m.face[i].V(j)->P()[2];				
				ofxVec3f c(xc, yc, zc);
				float xt = m.vert[k].P()[0];
				float yt = m.vert[k].P()[1];
				float zt = m.vert[k].P()[2];
				ofxVec3f t(xt, yt, zt);
				if (c == t) {
					faceIndices.push_back(k);
					break;
				}
			}
		}
	}
	return faceIndices;
	
}

int getFaceNum() {
	
	return m.face.size();
	
}
*/

}