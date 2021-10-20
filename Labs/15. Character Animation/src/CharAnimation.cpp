//  ========================================================================
//  COSC422: Advanced Computer Graphics;  University of Canterbury (2021)
//
//  FILE NAME: CharAnimation.cpp
//
//  See Ex16_CharAnimation.pdf for details
//  ========================================================================

#include <iostream>
#include <fstream>
#include <map>
#include <GL/freeglut.h>
#include <IL/il.h>
using namespace std;

#include <assimp/cimport.h>
#include <assimp/types.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "assimp_extras.h"

//----------Globals----------------------------
const aiScene* scene = NULL;
float camAngle = 0, camDist = 0, camNear = 0, camFov = 60.0;		//Camera's parameters
aiVector3D scene_min, scene_max;
std::map<int, int> texIdMap;
int currTick = 0, tDuration = 0, animIndex = 0; 
float CDR = 3.14159265 / 180.0;   //Conversion from degrees to radians
float timeStep;

//----A basic mesh structure that stores initial values of-----
//----vertex coordinates and normal components-----------------
struct meshInit
{
	int mNumVertices;
	aiVector3D* mVertices;
	aiVector3D* mNormals;
};
meshInit* initData;

//----Loads the character model and stores mesh data------- 
bool loadModel(const char* fileName)
{
	int numVert;
	aiMesh* mesh;
	aiVector3D  scene_center;
	float scene_scale;

	scene = aiImportFile(fileName, aiProcessPreset_TargetRealtime_MaxQuality);
	if(scene == NULL) exit(1);
	//printSceneInfo(scene);
	//printMeshInfo(scene);
	//printTreeInfo(scene->mRootNode);
	//printBoneInfo(scene);
	//for(int i = 0; i < scene->mNumAnimations; i++)
	//   printAnimInfo(scene, i);  //<<<WARNING!  This output could contain many many lines!

	initData = new meshInit[scene->mNumMeshes];

	//Store initial vertex coords, normals.
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		mesh = scene->mMeshes[i];
		numVert = mesh->mNumVertices;
		(initData + i)->mNumVertices = numVert;
		(initData + i)->mVertices = new aiVector3D[numVert];
		(initData + i)->mNormals = new aiVector3D[numVert];

		for (int k = 0; k < numVert; k++)
		{
			(initData + i)->mVertices[k] = mesh->mVertices[k];
			(initData + i)->mNormals[k] = mesh->mNormals[k];
		}
	}

	//Compute camera parameters using model's dimensions
	get_bounding_box(scene, &scene_min, &scene_max);
	scene_center = (scene_min + scene_max) * 0.5f;
	aiVector3D scene_diag = scene_max - scene_center;
	scene_scale = scene_diag.Length();
	camNear = scene_scale / tan(camFov * CDR * 0.5);
	camDist = camNear * 1.5;
	return true;
}

//------Loads textures of the model ---------
void loadGLTextures(const aiScene* scene)
{
	string filename;
	/* initialization of DevIL */
	ilInit();
	if (scene->HasTextures())
	{
		std::cout << "Support for meshes with embedded textures is not implemented" << endl;
		exit(1);
	}

	for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
	{
		aiString path;  // filename

		if (scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			glEnable(GL_TEXTURE_2D);
			ILuint imageId;
			GLuint texId;
			ilGenImages(1, &imageId);
			glGenTextures(1, &texId);
			texIdMap[m] = texId;   //store tex ID against material id in a hash map
			ilBindImage(imageId); /* Binding of DevIL image name */
			ilEnable(IL_ORIGIN_SET);
			ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
			if (ilLoadImage((ILstring)path.data))   //if success
			{
				/* Convert image to RGBA */
				ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

				/* Create and load textures to OpenGL */
				glBindTexture(GL_TEXTURE_2D, texId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
					ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
					ilGetData());
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				cout << "m = " << m << "  Texture:" << filename << " successfully loaded." << endl;
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
				glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
				glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
			}
			else
			{
				cout << "Couldn't load Image: " << path.data << endl;
			}
		}

	}  //loop for material
}


//-----Draws all meshes of the character model by traversing the node tree-----
void render(const aiScene* sc, const aiNode* nd)
{
	aiMatrix4x4 m = nd->mTransformation;
	aiMesh* mesh;
	aiFace* face;
	GLuint texId;
	aiColor4D diffuse;
	aiMaterial* mtl;
	int meshIndex, materialIndex;
	float materialCol[4] = { 0.8, 0.8, 0.8, 1 };

	m.Transpose();   //Convert to column-major order
	glPushMatrix();
	glMultMatrixf((float*)&m);   //Multiply by the transformation matrix for this node

	// Draw all meshes
	for (int n = 0; n < nd->mNumMeshes; n++)
	{
		meshIndex = nd->mMeshes[n];
		mesh = scene->mMeshes[meshIndex];

		materialIndex = mesh->mMaterialIndex;  //Get material index attached to the mesh
		mtl = sc->mMaterials[materialIndex];
		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))  //Get material colour from model
			glColor4f(diffuse.r, diffuse.g, diffuse.b, 1.0);
		else
			glColor4fv(materialCol);   //Default material colour


		if (mesh->HasTextureCoords(0))
		{
			glEnable(GL_TEXTURE_2D);
			texId = texIdMap[materialIndex];    //Use map to get texId from material index
			glBindTexture(GL_TEXTURE_2D, texId);
		}
		else 
			glDisable(GL_TEXTURE_2D);

		
		//Get the polygons from each mesh and draw them
		for (int k = 0; k < mesh->mNumFaces; k++)
		{
			face = &mesh->mFaces[k];
			GLenum face_mode;

			switch(face->mNumIndices)
			{
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			for(int i = 0; i < face->mNumIndices; i++) {
				int vertexIndex = face->mIndices[i]; 

				if(mesh->HasVertexColors(0))
					glColor4fv((GLfloat*)&mesh->mColors[0][vertexIndex]);

				if (mesh->HasTextureCoords(0))
					glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x, mesh->mTextureCoords[0][vertexIndex].y);

				if (mesh->HasNormals())
					glNormal3fv(&mesh->mNormals[vertexIndex].x);

				glVertex3fv(&mesh->mVertices[vertexIndex].x);
			}

			glEnd();
		}

	}

	// Draw all children of the current node
	for (int i = 0; i < nd->mNumChildren; i++)
		render(sc, nd->mChildren[i]);

	glPopMatrix();
}

//------Updates the nodes  with matrices computed using the ---------
//------position and rotation keys obtained from keyframes-----------
void updateNodeMatrices(int tick)
{
	aiAnimation* anim = scene->mAnimations[animIndex];
	aiMatrix4x4 matPos, matRot, matProd;
	aiMatrix3x3 matRot3;
	aiNode* node;
	int nPoskeys, nRotkeys, index;
	aiVector3D posn1, posn2, posn;
	aiQuaternion rotn1, rotn2, rotn;
	float factor, time1, time2;

	for (int i = 0; i < anim->mNumChannels; i++)
	{
		matPos = aiMatrix4x4();   //Identity
		matRot = aiMatrix4x4();
		aiNodeAnim* channel = anim->mChannels[i];
		nPoskeys = channel->mNumPositionKeys;
		nRotkeys = channel->mNumRotationKeys;

        if (tick < (channel->mPositionKeys[nPoskeys - 1]).mTime)
		{
			index = 1;
			while (tick >= (channel->mPositionKeys[index]).mTime) index++;
			posn1 = (channel->mPositionKeys[index - 1]).mValue;
			posn2 = (channel->mPositionKeys[index]).mValue;
			time1 = (channel->mPositionKeys[index - 1]).mTime;
			time2 = (channel->mPositionKeys[index]).mTime;
			factor = (tick - time1) / (time2 - time1);
			posn = (1 - factor)*posn1 + factor * posn2;
		}
		else
			posn = (channel->mPositionKeys[nPoskeys - 1]).mValue;

		if (tick < (channel->mRotationKeys[nRotkeys - 1]).mTime)
		{
			index = 1;
			while (tick >= (channel->mRotationKeys[index]).mTime) index++;
			rotn1 = (channel->mRotationKeys[index - 1]).mValue;
			rotn2 = (channel->mRotationKeys[index]).mValue;
			time1 = (channel->mRotationKeys[index - 1]).mTime;
			time2 = (channel->mRotationKeys[index]).mTime;
			factor = (tick - time1) / (time2 - time1);
			rotn.Interpolate(rotn, rotn1, rotn2, factor);
		}
		else
			rotn = (channel->mRotationKeys[nRotkeys - 1]).mValue;

		matPos.Translation(posn, matPos);
		matRot3 = rotn.GetMatrix();
		matRot = aiMatrix4x4(matRot3);

		matProd = matPos * matRot;
		node = scene->mRootNode->FindNode(channel->mNodeName);
		node->mTransformation = matProd;
	}
}

void special(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT) camAngle -= 5;
	else if (key == GLUT_KEY_RIGHT) camAngle += 5;
	glutPostRedisplay();
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
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50);

	loadModel("dwarf.x");			//Specify input file name here
	loadGLTextures(scene);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camFov, 1, camNear, 5000.0);

	tDuration = scene->mAnimations[animIndex]->mDuration;
	float fps = scene->mAnimations[animIndex]->mTicksPerSecond;
	if (fps < 10) fps = 30;
	timeStep = 1000.0 / fps;     //Animation time step in m.Sec
}

//-----Draws a floor plane------
void drawFloor()
{
	int ix, iz;
	float y = -0.1;  //floor level
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	for (int x = -1000; x < 1000; x += 100)
	{
		ix = x / 100;
		for (int z = -1000; z < 1000; z += 100)
		{
			iz = z / 100;
			if ((ix + iz) % 2 == 0) glColor3f(0.8, 1, 1);
			else glColor3f(0.8, 0.8, 0.8);
			glVertex3f(x, y, z);
			glVertex3f(x+100, y, z);
			glVertex3f(x+100, y, z+100);
			glVertex3f(x, y, z+100);
		}
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
}


//------The main display function---------
void display()
{
	float lightPosn[4] = { -200, 200, 200, 1 }; 

	float ex =  camDist * sin(camAngle*CDR);
	float ez =  camDist * cos(camAngle*CDR);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(ex, camDist*0.5, ez, 0, 0, 0, 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosn);

	glEnable(GL_LIGHTING);
	drawFloor();

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	   glTranslatef(0, -scene_min.y, 0);   //move the character above floor plane
	   render(scene, scene->mRootNode);
	glPopMatrix();

	glutSwapBuffers();
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Character Animation");

	initialise();
	glutDisplayFunc(display);
	glutSpecialFunc(special);
	glutMainLoop();

	aiReleaseImport(scene);
}

