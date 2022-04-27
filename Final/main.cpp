#include <iostream>
#include <sstream>    
#include <string> 
#include <fstream> 
#include <vector>
#include <cmath>
#include <cstdlib>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#define PI 3.1415926

class Animal {
public:
	std::vector<double> verticesVector; // Vector to read in vertex x, y and z values fromt the OBJ file.
	std::vector<int> facesVector; // Vector to read in face vertex indices from the OBJ file.
	std::vector<double> faceNormalsVector; // Vector to read in face normals
	std::vector<double> textureVector; // Vector to read in texture data.
	double *vertices = NULL;  // Vertex array of the object x, y, z values.
	int *faces = NULL; // Face (triangle) vertex indices.
	double *normals = NULL; // Normal array
	double *textures = NULL; // Texture array
	int numIndices; // Number of face vertex indices.
	double position[3]; // Position of the object.

	void loadOBJ(std::string fileName);
	void computeFaceNormals();
	void initialize();
};

static Animal tiger;
static Animal bunny;
static Animal cat;
static Animal dog;
static Animal duck;

static float lightPosition[3] = {0.0, 20.0, 20.0};
static double posX = 0, posY = 0, posZ = 50; // Camera position
static double frontX = 0, frontY = 0, frontZ = 49; // The front direction of the camera
static double sideX = 1, sideY = 0, sideZ = 50; // The right side direction of the camera
static int theta = 180, phi = 0; // Polar ccordinates angles

bool lightSwitch = true;

static unsigned int texture[3];

void coordinatesUpdate() {
	theta %= 360;
	if(phi > 45) phi = 45; // Cap the top vertical panning
	if(phi < -45) phi = -45; // Cap the bottom vertical panning

	double vecX, vecY, vecZ;
	vecX = sin((float)theta*PI/180)*cos((float)phi*PI/180);
	vecZ = cos((float)theta*PI/180)*cos((float)phi*PI/180);
	vecY = sin((float)phi*PI/180);
	frontX = posX + vecX;
	frontY = posY + vecY;
	frontZ = posZ + vecZ;
	vecX = sin((float)((theta-90)%360)*PI/180)*cos((float)phi*PI/180);
	vecZ = cos((float)((theta-90)%360)*PI/180)*cos((float)phi*PI/180);
	vecY = sin((float)phi/180*PI);
	sideX = posX + vecX;
	sideY = posY + vecY;
	sideZ = posZ + vecZ;
}

void Animal::initialize() {
	// Load data from vector to arrays

	normals = new double[faceNormalsVector.size()];
	for(int i = 0; i < faceNormalsVector.size(); i++) normals[i] = faceNormalsVector[i];

	vertices = new double[verticesVector.size()];
	for (int i = 0; i < verticesVector.size(); i++) vertices[i] = verticesVector[i];

	faces = new int[facesVector.size()];
	for (int i = 0; i < facesVector.size(); i++) faces[i] = facesVector[i];
	numIndices = facesVector.size();

	if(textureVector.size() > 0)
	{
		textures = new double[textureVector.size()];
		for (int i = 0; i < textureVector.size(); i++) textures[i] = textureVector[i];
	}
}

void Animal::computeFaceNormals()
{
	// Sizing the vector to be the same as verticesVector
	for(int i = 0; i < verticesVector.size(); i++)
		faceNormalsVector.push_back(0);

	// Each face has 3 vertices, increment by 3 every time
	for(int i = 0; i < facesVector.size(); i += 3)
	{
		double vertexIndex1 = facesVector[i] * 3;
		double vertexIndex2 = facesVector[i+1] * 3;
		double vertexIndex3 = facesVector[i+2] * 3;

		//Vector A
		double Ax = verticesVector[vertexIndex2] - verticesVector[vertexIndex1];
		double Ay = verticesVector[vertexIndex2+1] - verticesVector[vertexIndex1+1];
		double Az = verticesVector[vertexIndex2+2] - verticesVector[vertexIndex1+2];

		// Vector B
		double Bx = verticesVector[vertexIndex3] - verticesVector[vertexIndex1];
		double By = verticesVector[vertexIndex3+1] - verticesVector[vertexIndex1+1];
		double Bz = verticesVector[vertexIndex3+2] - verticesVector[vertexIndex1+2];

		// Normal calculated using cross-product
		double normalX = Ay*Bz - Az*By;
		double normalY = Az*Bx - Ax*Bz;
		double normalZ = Ax*By - Ay*Bx;

		// Placing data into normal vector
		faceNormalsVector[vertexIndex1] = normalX;
		faceNormalsVector[vertexIndex1+1] = normalY;
		faceNormalsVector[vertexIndex1+2] = normalZ;

		faceNormalsVector[vertexIndex2] = normalX;
		faceNormalsVector[vertexIndex2+1] = normalY;
		faceNormalsVector[vertexIndex2+2] = normalZ;
		
		faceNormalsVector[vertexIndex3] = normalX;
		faceNormalsVector[vertexIndex3+1] = normalY;
		faceNormalsVector[vertexIndex3+2] = normalZ;
	}
}

void Animal::loadOBJ(std::string fileName)
{
   std::string line;
   int count, vertexIndex1, vertexIndex2, vertexIndex3;
   double coordinateValue;
   char currentCharacter, previousCharacter;

   // Open the OBJ file.
   std::ifstream inFile(fileName.c_str(), std::ifstream::in);

   // Read successive lines.
   while (getline(inFile, line))
   {
	  // Line has vertex data.
	  if (line.substr(0, 2) == "v ")
	  {
		 // Initialize a string from the character after "v " to the end.
		 std::istringstream currentString(line.substr(2));

		 // Read x, y and z values. The (optional) w value is not read. 
		 for (count = 1; count <= 3; count++)
		 {
			currentString >> coordinateValue;
			verticesVector.push_back(coordinateValue);
		 }
	  }

	  // Line has face data.
	  else if (line.substr(0, 2) == "f ")
	  {
		 // Initialize a string from the character after "f " to the end.
		 std::istringstream currentString(line.substr(2));

		 // Strategy in the following to detect a vertex index within a face line is based on the
		 // fact that vertex indices are exactly those that follow a white space. Texture and
		 // normal indices are ignored.
		 // Moreover, from the third vertex of a face on output one triangle per vertex, that
		 // being the next triangle in a fan triangulation of the face about the first vertex.
		 previousCharacter = ' ';
		 count = 0;
		 while (currentString.get(currentCharacter))
		 {
			// Stop processing line at comment.
			if ((previousCharacter == '#') || (currentCharacter == '#')) break;

			// Current character is the start of a vertex index.
			if ((previousCharacter == ' ') && (currentCharacter != ' '))
			{
			   // Move the string cursor back to just before the vertex index.
			   currentString.unget();

			   // Read the first vertex index, decrement it so that the index range is from 0, increment vertex counter.
			   if (count == 0)
			   {
				  currentString >> vertexIndex1;
				  vertexIndex1--;
				  count++;
			   }

			   // Read the second vertex index, decrement it, increment vertex counter.
			   else if (count == 1)
			   {
				  currentString >> vertexIndex2;
				  vertexIndex2--;
				  count++;
			   }

			   // Read the third vertex index, decrement it, increment vertex counter AND output the first triangle.
			   else if (count == 2)
			   {
				  currentString >> vertexIndex3;
				  vertexIndex3--;
				  count++;
				  facesVector.push_back(vertexIndex1);
				  facesVector.push_back(vertexIndex2);
				  facesVector.push_back(vertexIndex3);
			   }

			   // From the fourth vertex and on output the next triangle of the fan.
			   else
			   {
				  vertexIndex2 = vertexIndex3;
				  currentString >> vertexIndex3;
				  vertexIndex3--;
				  facesVector.push_back(vertexIndex1);
				  facesVector.push_back(vertexIndex2);
				  facesVector.push_back(vertexIndex3);
			   }

			   // Begin the process of detecting the next vertex index just after the vertex index just read.
			   currentString.get(previousCharacter);
			}

			// Current character is not the start of a vertex index. Move ahead one character.
			else previousCharacter = currentCharacter;
		 }
	  }

	// Line has texture coords data
	else if (line.substr(0, 3) == "vt ")
	{
		// Initialize a string from the character after "vt " to the end.
		std::istringstream currentString(line.substr(3));

		// Read the two values
		currentString >> coordinateValue;
		textureVector.push_back(coordinateValue);

		currentString >> coordinateValue;
		textureVector.push_back(coordinateValue);
	}

	  // Nothing other than vertex and face data is processed.
	  else
	  {
	  }
   }

   // Close the OBJ file.
   inFile.close();
}

struct imageFile
{
	int width;
	int height;
	unsigned char *data;
};

imageFile *getBMP(std::string fileName)
{
	int offset, // No. of bytes to start of image data in input BMP file. 
		w, // Width in pixels of input BMP file.
		h; // Height in pixels of input BMP file.

		   // Initialize imageFile objects.
	imageFile *tempStore = new imageFile; // Temporary storage.
	imageFile *outRGB = new imageFile; // RGB output file.
	imageFile *outRGBA = new imageFile; // RGBA output file.

										// Initialize input stream.
	std::ifstream inFile(fileName.c_str(), std::ios::binary);

	// Get start point of image data in input BMP file.
	inFile.seekg(10);
	inFile.read((char *)&offset, 4);

	// Get image width and height.
	inFile.seekg(18);
	inFile.read((char *)&w, 4);
	inFile.read((char *)&h, 4);

	// Determine the length of padding of the pixel rows 
	// (each pixel row of a BMP file is 4-byte aligned by padding with zero bytes).
	int padding = (3 * w) % 4 ? 4 - (3 * w) % 4 : 0;

	// Allocate storage for temporary input file, read in image data from the BMP file, close input stream.
	tempStore->data = new unsigned char[(3 * w + padding) * h];
	inFile.seekg(offset);
	inFile.read((char *)tempStore->data, (3 * w + padding) * h);
	inFile.close();

	// Set image width and height and allocate storage for image in output RGB file.
	outRGB->width = w;
	outRGB->height = h;
	outRGB->data = new unsigned char[3 * w * h];

	// Copy data from temporary input file to output RGB file adjusting for padding and performing BGR to RGB conversion.
	int tempStorePos = 0;
	int outRGBpos = 0;
	for (int j = 0; j < h; j++)
		for (int i = 0; i < 3 * w; i += 3)
		{
			tempStorePos = (3 * w + padding) * j + i;
			outRGBpos = 3 * w * j + i;
			outRGB->data[outRGBpos] = tempStore->data[tempStorePos + 2];
			outRGB->data[outRGBpos + 1] = tempStore->data[tempStorePos + 1];
			outRGB->data[outRGBpos + 2] = tempStore->data[tempStorePos];
		}

	// Set image width and height and allocate storage for image in output RGBA file.
	outRGBA->width = w;
	outRGBA->height = h;
	outRGBA->data = new unsigned char[4 * w * h];

	// Copy image data from output RGB file to output RGBA file, setting all A values to 1.
	for (int j = 0; j < 4 * w * h; j += 4)
	{
		outRGBA->data[j] = outRGB->data[(j / 4) * 3];
		outRGBA->data[j + 1] = outRGB->data[(j / 4) * 3 + 1];
		outRGBA->data[j + 2] = outRGB->data[(j / 4) * 3 + 2];
		outRGBA->data[j + 3] = 0xFF;
	}

	// Release temporary storage and the output RGB file and return the RGBA version.
	delete[] tempStore;
	delete[] outRGB;
	return outRGBA;
}

// Load external textures.
void loadTextures()
{
	// Local storage for bmp image data.
	imageFile *image[3];

	// Load the images.
	image[0] = getBMP("TigerTexture.bmp");
	image[1] = getBMP("grass.bmp");
	image[2] = getBMP("sky.bmp");

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->width, image[2]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void rightMenu(int id)
{

}

void lightMenu(int id)
{
	if (id == 5) 
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		lightSwitch = true;
	};

	if (id == 6)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		lightSwitch = false;
	}
	glutPostRedisplay();
}

void lightColourMenu(int id)
{
	if (id == 1) 
	{
		float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	}
	if (id == 2)
	{
		float lightDifAndSpec[] = { 1.0, 0.5, 0.5, 1.0 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	}
	if (id == 3)
	{
		float lightDifAndSpec[] = { 0.5, 1.0, 0.5, 1.0 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	} 
	if (id == 4)
	{
float lightDifAndSpec[] = { 0.5, 0.5, 1.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	} 
	glutPostRedisplay();
}

void generateMenu() {
	int subMenu = glutCreateMenu(lightMenu);
	glutAddMenuEntry("On", 5);
	glutAddMenuEntry("Off", 6);

	int lightColour;
	lightColour = glutCreateMenu(lightColourMenu);
	glutAddMenuEntry("White", 1);
	glutAddMenuEntry("Red", 2);
	glutAddMenuEntry("Green", 3);
	glutAddMenuEntry("Blue", 4);

	glutCreateMenu(rightMenu);
	glutAddSubMenu("Lighting", subMenu);
	glutAddSubMenu("Colour", lightColour);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void setup(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	glClearColor(1.0, 1.0, 1.0, 0.0);

	float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
	float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightPos[] = { 0.0, 1.5, 3.0, 1.0 };
	float globAmb[] = { 0.2, 0.2, 0.2, 1.0 };

	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	float matAmbAndDif[] = { 0.4, 0.4, 0.4, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float matShine[] = { 50.0 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);

	tiger.loadOBJ("Tiger.obj");
	bunny.loadOBJ("Bunny.obj");
	cat.loadOBJ("Cat.obj");
	dog.loadOBJ("Dog.obj");
	duck.loadOBJ("Duck.obj");

	tiger.computeFaceNormals();
	bunny.computeFaceNormals();
	cat.computeFaceNormals();
	dog.computeFaceNormals();
	duck.computeFaceNormals();

	tiger.initialize();
	bunny.initialize();
	cat.initialize();
	dog.initialize();
	duck.initialize();

	glGenTextures(3, texture);
	loadTextures();
	generateMenu();
}

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(posX, posY, posZ,
			  frontX, frontY, frontZ,
			  0.0, 1.0, 0.0);

	if(lightSwitch) {
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
	}
	else
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glRotatef(180, 0, 0, 1);
	glScalef(0.05, 0.05, 0.05);
	glTranslatef(-380, 0, 0);
	glVertexPointer(3, GL_DOUBLE, 0, tiger.vertices);
	glNormalPointer(GL_DOUBLE, 0, tiger.normals);
	glTexCoordPointer(2, GL_DOUBLE, 0, tiger.textures);
	glDrawElements(GL_TRIANGLES, tiger.numIndices, GL_UNSIGNED_INT, tiger.faces);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glScalef(45, 45, 45);
	glTranslatef(0.2, -0.130, 0);
	glVertexPointer(3, GL_DOUBLE, 0, bunny.vertices);
	glNormalPointer(GL_DOUBLE, 0, bunny.normals);
	glDrawElements(GL_TRIANGLES, bunny.numIndices, GL_UNSIGNED_INT, bunny.faces);
	glPopMatrix();

	glPushMatrix();
	glScalef(0.04, 0.04, 0.04);
	glRotatef(-90, 1, 0, 0);
	glRotatef(90, 0, 0, 1);
	glTranslatef(0, 350, 65);
	glVertexPointer(3, GL_DOUBLE, 0, cat.vertices);
	glNormalPointer(GL_DOUBLE, 0, cat.normals);
	glDrawElements(GL_TRIANGLES, cat.numIndices, GL_UNSIGNED_INT, cat.faces);
	glPopMatrix();

	glPushMatrix();
	glScalef(0.25, 0.25, 0.25);
	glRotatef(-90, 1, 0, 0);
	glTranslatef(-200, -200, 212);
	glVertexPointer(3, GL_DOUBLE, 0, dog.vertices);
	glNormalPointer(GL_DOUBLE, 0, dog.normals);
	glDrawElements(GL_TRIANGLES, dog.numIndices, GL_UNSIGNED_INT, dog.faces);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glTranslatef(-0.5, 0, -1.8);
	glVertexPointer(3, GL_DOUBLE, 0, duck.vertices);
	glNormalPointer(GL_DOUBLE, 0, duck.normals);
	glDrawElements(GL_TRIANGLES, duck.numIndices, GL_UNSIGNED_INT, duck.faces);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, -4.5, 100.0);
	glTexCoord2f(8.0, 0.0); glVertex3f(100.0, -4.5, 100.0);
	glTexCoord2f(8.0, 8.0); glVertex3f(100.0, -4.5, -100.0);
	glTexCoord2f(0.0, 8.0); glVertex3f(-100.0, -4.5, -100.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, -4.5, -70.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(100.0, -4.5, -70.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(100.0, 120.0, -70.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-100.0, 120.0, -70.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(100.0, -4.5, 70.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(100.0, -4.5, -70.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(100.0, 120.0, -70.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(100.0, 120.0, 70.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, -4.5, -70.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(-100.0, -4.5, 70.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(-100.0, 120.0, 70.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-100.0, 120.0, -70.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(100.0, -4.5, 70.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(-100.0, -4.5, 70.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(-100.0, 120.0, 70.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(100.0, 120.0, 70.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-100.0, 120.0, -70.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(100.0, 120.0, -70.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(100.0, 120.0, 70.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-100.0, 120.0, 70.0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)w / (double)h, 1.0, 300.0);
	glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		posX = frontX;
		posZ = frontZ;
		break;
	case 's':
		posX += posX-frontX;
		posZ += posZ-frontZ;
		break;
	case 'd':
		posX = sideX;
		posZ = sideZ;
		break;
	case 'a':
		posX += posX-sideX;
		posZ += posZ-sideZ;
		break;
	case 'q':
		if(posY > -3) posY--;
		break;
	case 'e':
		posY++;
		break;
	default:
		break;
	}
	coordinatesUpdate();
	glutPostRedisplay();
}

void specialKeyInput(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) 
		phi += 2;
	if (key == GLUT_KEY_DOWN)
		phi -= 2;
	if (key == GLUT_KEY_RIGHT) 
		theta -= 2;
	if (key == GLUT_KEY_LEFT)
		theta += 2;
	coordinatesUpdate();
	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press w, a, s, d to move around. Press q, e to move up and down." << std::endl;
	std::cout << "Press arrow keys to pan up, down, left and right." << std::endl;
	std::cout << "Use menu to turn on/off light and change light colour." << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Finals.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}

