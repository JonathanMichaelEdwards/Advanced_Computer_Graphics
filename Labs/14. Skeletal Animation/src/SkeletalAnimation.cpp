//  ========================================================================
//  COSC422: Advanced Computer Graphics;  University of Canterbury (2021)
//
//  See Ex15_SkeletalAnimation.pdf for details
//
//  Motions - https://sites.google.com/a/cgspeed.com/cgspeed/motion-capture
//  ========================================================================


#include <iostream>
#include <fstream>
#include <GL/freeglut.h>
using namespace std;

#include <assimp/cimport.h>
#include <assimp/types.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "assimp_extras.h"

//----------Globals----------------------------
const aiScene* scene = NULL;
aiVector3D scene_min, scene_max, scene_center;
float scene_scale;
bool modelRotn = true;
int period;       //Animation duration in ticks.
int tick = 0;    //current tick
int time_step = 50;

// ------A recursive function to traverse scene graph and render each mesh----------
void render(const aiScene* sc, const aiNode* nd)
{
	aiMatrix4x4 m = nd->mTransformation;
	aiMesh* mesh;
	aiFace* face;
	float materialCol[4] = { 1, 0, 1, 1 };
	int meshIndex;

	m.Transpose();      //Convert to column-major order
	glPushMatrix();
	glMultMatrixf((float*)&m);   //Multiply by the transformation matrix for this node

	// Draw all meshes assigned to this node
	for (int n = 0; n < nd->mNumMeshes; n++)
	{
		meshIndex = nd->mMeshes[n];          //Get the mesh indices from the current node
		mesh = scene->mMeshes[meshIndex];    //Using mesh index, get the mesh object
		glColor4fv(materialCol);   //Default material colour

		//Get the polygons from each mesh and draw them
		for (int k = 0; k < mesh->mNumFaces; k++)
		{
			face = &mesh->mFaces[k];
			glBegin(GL_TRIANGLES);
			for (int i = 0; i < face->mNumIndices; i++) {
				int vertexIndex = face->mIndices[i];
				if (mesh->HasNormals())
					glNormal3fv(&mesh->mNormals[vertexIndex].x);

				glVertex3fv(&mesh->mVertices[vertexIndex].x);
			}
			glEnd();
		}
	}


	// Recursively draw all children of the current node
	for (int i = 0; i < nd->mNumChildren; i++)
		render(sc, nd->mChildren[i]);

	glPopMatrix();
}


//--------------------OpenGL initialization------------------------
void initialise()
{
	float ambient[4] = { 0.2, 0.2, 0.2, 1.0 };  //Ambient light
	float white[4] = { 1, 1, 1, 1 };		    //Light's colour
	float black[4] = { 0, 0, 0, 1 };
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, black);   //Disable specular light
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, 1, 1.0, 500.0);

	//---- Load the model ------
	scene = aiImportFile("./src/Dance.bvh", aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Debone);
	if (scene == NULL) exit(1);
    printSceneInfo(scene);
	printTreeInfo(scene->mRootNode);
    //printMeshInfo(scene);
    //printBoneInfo(scene);
    //printAnimInfo(scene);  //WARNING:  This may generate a lengthy output if the model has animation data

	period = scene->mAnimations[0]->mDuration;

	// aiNode* node = scene->mRootNode;
	// aiAnimation* anim = scene->mAnimations[0];
	// aiNodeAnim* chan = anim->mChannels[i]

	    // Store initial mesh data
    // initData = new meshInit[scene->mNumMeshes];
    // for (uint m = 0; m < scene->mNumMeshes; m++) {
    //     aiMesh *mesh = scene->mMeshes[m];
    //     uint numVert = mesh->mNumVertices;
    //     (initData + m)->mNumVertices = numVert;
    //     (initData + m)->mVertices = new aiVector3D[numVert];
    //     (initData + m)->mNormals = new aiVector3D[numVert];
    //     for (uint v = 0; v < numVert; v++) {
    //         (initData + m)->mVertices[v] = mesh->mVertices[v];
    //         (initData + m)->mNormals[v] = mesh->mNormals[v];
    //     }
    // }

	// for (uint m = 0; m < scene->mNumMeshes; m++) {
    //     aiMesh *mesh = scene->mMeshes[m];
    //     uint numVert = mesh->mNumVertices;
    //     m.mNumVertices = numVert;
    //     m.mVertices = new aiVector3D[numVert];
    //     m.mNormals = new aiVector3D[numVert];
    //     for (uint v = 0; v < numVert; v++) {
    //         m.mVertices[v] = mesh->mVertices[v];
    //         m.mNormals[v] = mesh->mNormals[v];
    //     }
    // }



	get_bounding_box(scene, &scene_min, &scene_max);
	scene_center = (scene_min + scene_max) * 0.5f;
	aiVector3D scene_diag = scene_max - scene_center;
	scene_scale = 1.0 / scene_diag.Length();
}


//------The main display function---------
void display()
{
	float lightPosn[4] = { -5, 10, 10, 1 };
	float CDR = 3.14159265 / 180.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosn);

	glPushMatrix();
	   glScalef(scene_scale, scene_scale, scene_scale);
	   glTranslatef(-scene_center.x, -scene_center.y, -scene_center.z);
	   render(scene, scene->mRootNode);
	glPopMatrix();

	glutSwapBuffers();
}

// redo
void updateNodeMatrices(int tick)
{
    int index;
    aiAnimation *anim = scene->mAnimations[0];
    aiMatrix4x4 matPos, matRot, matProd;
    aiMatrix3x3 matRot3;
    aiNode *nd;

    for (uint i = 0; i < anim->mNumChannels; i++) {
        matPos = aiMatrix4x4(); //Identity
        matRot = aiMatrix4x4();
        aiNodeAnim *ndAnim = anim->mChannels[i]; //Channel
        if (ndAnim->mNumPositionKeys > 1) index = tick;
        else index = 0;
        aiVector3D posn = (ndAnim->mPositionKeys[index]).mValue;
        matPos.Translation(posn, matPos);
        if (ndAnim->mNumRotationKeys > 1) index = tick;
        else index = 0;
        aiQuaternion rotn = (ndAnim->mRotationKeys[index]).mValue;
        matRot3 = rotn.GetMatrix();
        matRot = aiMatrix4x4(matRot3);
        matProd = matPos * matRot;
        nd = scene->mRootNode->FindNode(ndAnim->mNodeName);
        nd->mTransformation = matProd;
    }
}

//----Timer callback for continuous rotation of the model about y-axis----
void update(int value)
{
	if (tick > period) return;
	updateNodeMatrices(tick);
	glutTimerFunc(time_step, update, 0);
	tick++;
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Skeletal Animation");

	initialise();
	glutDisplayFunc(display);
	glutTimerFunc(time_step, update, 0);

	glutMainLoop();
	aiReleaseImport(scene);
}

