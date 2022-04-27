#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include <GL/glew.h>
#include <GL/freeglut.h> 

const static double PI = 3.14159265358979323846;

using namespace std;

// Globals.
static unsigned int texture[1]; // Array of texture indices.
static float dy = 0.0; // Distance to move in the y direction in gluLookAt()
static float dx = 0.0; // Distance to move in the x direction in gluLookAt()
static float r = 0.0; // Rotation parameter
static bool lightOn = false;

/* Please rename the variables */

// varaibles for the objects
static int nTiger;
static double* vTiger;
static int* fTiger;
static double* vNormTiger;
static double* fNormTiger;
static double* texCorTiger;
static double* bBoxTiger;

static int nBunny;
static double* vBunny;
static int* fBunny;
static double* vNormBunny;
static double* fNormBunny;
static double* texCorBunny;
static double* bBoxBunny;

static int nCat;
static double* vCat;
static int* fCat;
static double* vNormCat;
static double* fNormCat;
static double* texCorCat;
static double* bBoxCat;

/* Please remove this comment */

static int nDog;
static double* vDog;
static int* fDog;
static double* vNormDog;
static double* fNormDog;
static double* texCorDog;
static double* bBoxDog;

static int nDuck;
static double* vDuck;
static int* fDuck;
static double* vNormDuck;
static double* fNormDuck;
static double* texCorDuck;
static double* bBoxDuck;

/* From this line to line 335 (except reading texture coords) - Copied from Project 2 - please replace with your own code after adapting */

static vector<double> verticesVector; // Vector to read in vertex x, y and z values fromt the OBJ file.
static vector<int> facesVector; // Vector to read in face vertex indices from the OBJ file.
static vector<double> facesNormal; // Calculated normal of each face
static vector<double> verticesNormal; // Calculated normal of each vector
static vector<double> verticesTotalArea; // Total area of all the faces connecting each vertex. 
static vector<double> textureCoords; // texture coordinates read from OBJ
static int numIndices; // Number of face vertex indices.
static double Xmin = INT_MAX, Ymin = INT_MAX, Zmin = INT_MAX;
static double Xmax = INT_MIN, Ymax = INT_MIN, Zmax = INT_MIN;
// variables used to calculate the bounding box
static double Xsize = -1, Ysize = -1, Zsize = -1;
static double Xcenter = -1, Ycenter = -1, Zcenter = -1;
static double boundingDiagonal = -1;

// Function to compute the variables of the bounding box. 
void ComputeBoundingBox()
{
    Xsize = Xmax - Xmin;
    Ysize = Ymax - Ymin;
    Zsize = Zmax - Zmin;

    Xcenter = Xmin + (Xmax - Xmin) / 2;
    Ycenter = Ymin + (Ymax - Ymin) / 2;
    Zcenter = Zmin + (Zmax - Zmin) / 2;

    boundingDiagonal = sqrt(pow(Xsize, 2) + pow(Ysize, 2) + pow(Zsize, 2)); 
}

// Function to compute the normal of each face. 
// Result stored in facesNormal vector.
// Also does pre-calculations for vertex normals.
void ComputeFaceNormals()
{        
    for (int i = 0; i < facesVector.size(); i += 3)
    {
        // vertices that formed the face. 
        double v1x = verticesVector[facesVector[i] * 3];
        double v1y = verticesVector[facesVector[i] * 3 + 1];
        double v1z = verticesVector[facesVector[i] * 3 + 2];
        double v2x = verticesVector[facesVector[i+1] * 3];
        double v2y = verticesVector[facesVector[i+1] * 3 + 1];
        double v2z = verticesVector[facesVector[i+1] * 3 + 2];
        double v3x = verticesVector[facesVector[i+2] * 3];
        double v3y = verticesVector[facesVector[i+2] * 3 + 1];
        double v3z = verticesVector[facesVector[i+2] * 3 + 2];

        // vectors that formed the face. 
        double fv1x = v2x - v1x;
        double fv1y = v2y - v1y;
        double fv1z = v2z - v1z;
        double fv2x = v3x - v1x;
        double fv2y = v3y - v1y;
        double fv2z = v3z - v1z;

        // calculating normal vector by computing the cross product of the two vectors
        double fnvx = fv1y * fv2z - fv2y * fv1z;
        double fnvy = fv1x * fv2z - fv2x * fv1z;
        double fnvz = fv1x * fv2y - fv2x * fv1y;

        // calculates the magnitude of the normal
        double l = sqrt(pow(fnvx, 2) + pow(fnvy, 2) + pow(fnvz, 2));

        // updates facesNormal vector
        facesNormal[facesVector[i] * 3] = fnvx;
        facesNormal[facesVector[i] * 3 + 1] = fnvy;
        facesNormal[facesVector[i] * 3 + 2] = fnvz;
        facesNormal[facesVector[i+1] * 3] = fnvx;
        facesNormal[facesVector[i+1] * 3 + 1] = fnvy;
        facesNormal[facesVector[i+1] * 3 + 2] = fnvz;
        facesNormal[facesVector[i+2] * 3] = fnvx;
        facesNormal[facesVector[i+2] * 3 + 1] = fnvy;
        facesNormal[facesVector[i+2] * 3 + 2] = fnvz;

        // multiply the normal vector its magnitude
        fnvx *= l;
        fnvy *= l;
        fnvz *= l;

        // add the multiplied normals to verticesNormal
        // division happens in ComputeVertexNormals()
        verticesNormal[facesVector[i] * 3] += fnvx;
        verticesNormal[facesVector[i] * 3 + 1] += fnvy;
        verticesNormal[facesVector[i] * 3 + 2] += fnvz;
        verticesNormal[facesVector[i+1] * 3] += fnvx;
        verticesNormal[facesVector[i+1] * 3 + 1] += fnvy;
        verticesNormal[facesVector[i+1] * 3 + 2] += fnvz;
        verticesNormal[facesVector[i+2] * 3] += fnvx;
        verticesNormal[facesVector[i+2] * 3 + 1] += fnvy;
        verticesNormal[facesVector[i+2] * 3 + 2] += fnvz;

        // add the magnitude to verticesTotalArea
        verticesTotalArea[facesVector[i]] += l;
        verticesTotalArea[facesVector[i+1]] += l;
        verticesTotalArea[facesVector[i+2]] += l;
    }
}

// Function to compute the normal of each vector. 
// Result stored in verticesNormal vector. 
void ComputeVertexNormals()
{
    int j = 0; // index for verticesTotalArea
    for (int i = 0; i < verticesNormal.size(); i++)
    {
        verticesNormal[i] /= verticesTotalArea[j];
        if ((i + 1) % 3 == 0) j++;
    }
}

// Routine to read a Wavefront OBJ file. 
// Only vertex and face lines are processed. All other lines,including texture, 
// normal, material, etc., are ignored.
// Vertex lines are assumed to have only x, y, z coordinate values and no w value.
// If a w value is present it is ignored.
// Within a face line only vertex indices are read. Texture and normal indices are 
// allowed but ignored. Face lines can have more than three vertices.
//
// OUTPUT: The vertex coordinate values are written to a vector "verticesVector" 
// (this name is hardcoded in the routine so the calling program should take note). 
// The face vertex indices are written to a vector "facesVector" (hardcoded name).
// Faces with more than three vertices are fan triangulated about the first vertex
// and the triangle indices written. In other words, output faces are all triangles.
// All vertex indices are decremented by 1 to make the index range start from 0.
void loadOBJ(string fileName)
{
    string line;
    int count, vertexIndex1, vertexIndex2, vertexIndex3;
    double coordinateValue;
    char currentCharacter, previousCharacter;

    // Open the OBJ file.
    ifstream inFile(fileName.c_str(), ifstream::in);

    // Read successive lines.
    while (getline(inFile, line))
    {
        // Line has vertex data.
        if (line.substr(0, 2) == "v ")
        {
            // Initialize a string from the character after "v " to the end.
            istringstream currentString(line.substr(2));

            // Read x, y and z values. The (optional) w value is not read. 
            // Min and max values of each axis are updated as value is read. 
            currentString >> coordinateValue;
            verticesVector.push_back(coordinateValue);
            if (coordinateValue < Xmin) Xmin = coordinateValue;
            if (coordinateValue > Xmax) Xmax = coordinateValue;

            currentString >> coordinateValue;
            verticesVector.push_back(coordinateValue);
            if (coordinateValue < Ymin) Ymin = coordinateValue;
            if (coordinateValue > Ymax) Ymax = coordinateValue;

            currentString >> coordinateValue;
            verticesVector.push_back(coordinateValue);
            if (coordinateValue < Zmin) Zmin = coordinateValue;
            if (coordinateValue > Zmax) Zmax = coordinateValue;

            // initializes normal vector (both faces and vertex normals) as well 
            // because each vertex needs a normal vector. 
            verticesNormal.push_back(0);
            verticesNormal.push_back(0);
            verticesNormal.push_back(0);
            facesNormal.push_back(0);
            facesNormal.push_back(0);
            facesNormal.push_back(0);

            // each vertex has a total associated area as well. 
            verticesTotalArea.push_back(0);
        }

        // Line has face data.
        else if (line.substr(0, 2) == "f ")
        {
            // Initialize a string from the character after "f " to the end.
            istringstream currentString(line.substr(2));

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
            istringstream currentString(line.substr(3));

            // Read the two values
            currentString >> coordinateValue;
            textureCoords.push_back(coordinateValue);

            currentString >> coordinateValue;
            textureCoords.push_back(coordinateValue);
        }

        // Nothing other than vertex, texture coords and face data is processed.
        else
        {
        }
    }

    // Close the OBJ file.
    inFile.close();
}

/* End of Project 2 copy paste */

void getOBJ(string file, int* objNumIndices, double** objVertices, int** objFaces, 
    double** objVertNorm, double** objFaceNorm, double** objTexCoords, double** objBoundBox)
{
    verticesVector.clear();
    facesVector.clear();
    facesNormal.clear();
    verticesNormal.clear();
    verticesTotalArea.clear();
    textureCoords.clear();
    Xmin = INT_MAX; Ymin = INT_MAX; Zmin = INT_MAX;
    Xmax = INT_MIN; Ymax = INT_MIN; Zmax = INT_MIN;

    // reads the OBJ file and stores its data in global variables
    loadOBJ(file);

    // computes the normals
    ComputeFaceNormals();
    ComputeVertexNormals();

    // Size the vertex array and copy into it x, y, z values from the vertex vector.
    double *v = new double[verticesVector.size()];
    for (int i = 0; i < verticesVector.size(); i++) v[i] = verticesVector[i];
    *objVertices = v;

    // Size the faces array and copy into it face index values from the face vector.
    int *f = new int[facesVector.size()];
    for (int i = 0; i < facesVector.size(); i++) f[i] = facesVector[i];
    *objFaces = f;
    *objNumIndices = facesVector.size();

    // copies the calculated normals from vectors into arrays
    double* vNorm = new double[verticesVector.size()];
    double* fNorm = new double[verticesVector.size()];
    for (int i = 0; i < verticesVector.size(); i++)
    {
        vNorm[i] = verticesNormal[i];
        fNorm[i] = facesNormal[i]; 
    }
    *objVertNorm = vNorm;
    *objFaceNorm = fNorm;

    if (textureCoords.size() > 0)
    {
        double* tCoords = new double[textureCoords.size()];
        for (int i = 0; i < textureCoords.size(); i++) tCoords[i] = textureCoords[i];
        *objTexCoords = tCoords;
    }
    else *objTexCoords = NULL;

    // computes the bounding box
    ComputeBoundingBox();

    // copies the computed values into an array
    double *boundBox = new double[7];
    boundBox[0] = Xcenter;
    boundBox[1] = Ycenter;
    boundBox[2] = Zcenter;
    boundBox[3] = boundingDiagonal;
    boundBox[4] = Xsize;
    boundBox[5] = Ysize;
    boundBox[6] = Zsize;
    *objBoundBox = boundBox;
}

/* From this line to line 504 - copied from fieldAndSky. Kindly tweak loadTextures to your version, and leave the rest as is */

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
	imageFile *image[1];

	// Load the images.
	image[0] = getBMP("TigerTexture.bmp");

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/* End of fieldAndSky copy paste */

/* All models are left as is - do the required transformations yourself. 
Hint: Bunny requires an about 100x scale to have reasonable size. It is invisible without scaling.
Hint 2: Check an object's center coords and sizes if they are missing from view before transforming.
	(Dog has a very weird initial position) 
Hint 3: Yes, cat + very large tiger + tiny duck + invisible rabbit are stacked together initially. 

Also you will be inside the tiger on starting the program without changing transformations. */

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (lightOn)
	{
		// Specify how texture values combine with current surface color values.
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glLoadIdentity();
		GLfloat light0_pos[]={0.0, 1000.0, 0.0, 1.0};
		glLightfv(GL_LIGHT0, GL_POSITION, light0_pos); 
	}
	else 
	{
		// Specify how texture values combine with current surface color values.
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}

	glLoadIdentity();
	glRotatef(r, 0, 1, 0);
	gluLookAt(0.0 + dx, 80.0 + dy, 20.0, 0.0 + dx, 0.0 + dy, 21.0, 0.0, 1.0, 0.0);

	// Stores the current transformation matrix.
	glPushMatrix();
	// Draw the object mesh.
	glColor3f(0.0, 0.0, 0.0);
	/* Insert transformations for tiger here */
	glPolygonMode(GL_FRONT, GL_FILL);
	// Turn on OpenGL texturing.
	glEnable(GL_TEXTURE_2D);
	glVertexPointer(3, GL_DOUBLE, 0, vTiger);
	glNormalPointer(GL_DOUBLE, 0, fNormTiger);
	glTexCoordPointer(2, GL_DOUBLE, 0, texCorTiger);
    glDrawElements(GL_TRIANGLES, nTiger, GL_UNSIGNED_INT, fTiger);
	// The remaining textures does not use textures
	glDisable(GL_TEXTURE_2D);
	// Restores the saved transformation matrix
	glPopMatrix();

	if (lightOn) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glPushMatrix();
	/* Insert transformations for cat here */
	glVertexPointer(3, GL_DOUBLE, 0, vCat);
	glNormalPointer(GL_DOUBLE, 0, fNormCat);
    glDrawElements(GL_TRIANGLES, nCat, GL_UNSIGNED_INT, fCat);
	glPopMatrix();

	glPushMatrix();
	/* Insert transformations for dog here */
	glVertexPointer(3, GL_DOUBLE, 0, vDog);
	glNormalPointer(GL_DOUBLE, 0, fNormDog);
    glDrawElements(GL_TRIANGLES, nDog, GL_UNSIGNED_INT, fDog);
	glPopMatrix();

	glPushMatrix();
	/* Insert transformations for bunny here - read hint above as starting point */
	glVertexPointer(3, GL_DOUBLE, 0, vBunny);
	glNormalPointer(GL_DOUBLE, 0, fNormBunny);
    glDrawElements(GL_TRIANGLES, nBunny, GL_UNSIGNED_INT, fBunny);
	glPopMatrix();

	glPushMatrix();
	/* Insert transformations for duck here */
	glVertexPointer(3, GL_DOUBLE, 0, vDuck);
	glNormalPointer(GL_DOUBLE, 0, fNormDuck);
    glDrawElements(GL_TRIANGLES, nDuck, GL_UNSIGNED_INT, fDuck);
	glPopMatrix();

	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
	/* Hint: Increase the "far" value above to allow far polygons to stay visible. */
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
	default:
		break;
	}
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) 
	{
		dy += -cos(r * (2 * PI)/360);
		dx += -sin(r * (2 * PI)/360);
	}
	if (key == GLUT_KEY_DOWN)
	{
		dy += cos(r * (2 * PI)/360);
		dx += sin(r * (2 * PI)/360);
	}
	if (key == GLUT_KEY_RIGHT) 
	{
		r += 1;
		if (r == 360) r = 0;
	}
	if (key == GLUT_KEY_LEFT)
	{
		r -= 1;
		if (r == -360) r = 0;
	}
	glutPostRedisplay();
}

// The right button menu callback function.
void rightMenu(int id)
{

}

// The sub-menu callback function.
void lighting_menu(int id)
{
	if (id == 3) 
	{
		// enables lighting and light0
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		lightOn = true;
	};
	if (id == 4)
	{
		// disables lighting and light0
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		lightOn = false;
	}
	glutPostRedisplay();
}

// The colour-menu callback function.
void colour_menu(int id)
{
	if (id == 101) 
	{
		GLfloat mat_ambience[] = { 1.0, 0.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_ambience);
	}
	if (id == 102)
	{
		GLfloat mat_ambience[] = { 1.0, 1.0, 0.0, 1.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_ambience);
	} 
	/* Add your colours here. Hint: Reuse colours used in canvas.cpp. */
	glutPostRedisplay();
}

// Function to create menu.
void makeMenu(void)
{
	int sub_menu;
	sub_menu = glutCreateMenu(lighting_menu);
	glutAddMenuEntry("On", 3);
	glutAddMenuEntry("Off", 4);

	int colour_sub_menu;
	colour_sub_menu = glutCreateMenu(colour_menu);
	glutAddMenuEntry("Red", 101);
	glutAddMenuEntry("Yellow", 102);

	glutCreateMenu(rightMenu);
	glutAddSubMenu("Lighting", sub_menu);
	glutAddSubMenu("Colour", colour_sub_menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Initialization routine.
void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);

	// enables vertex, normal and texture coord arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// light source properties
    GLfloat light0_pos[]={0.0, 1000.0, 0.0, 1.0};
    GLfloat ambient0[]={0.0, 0.0, 0.0, 1.0};  
    GLfloat diffuse0[]={1.0, 1.0, 1.0, 1.0};
    GLfloat specular0[]={1.0, 1.0, 1.0, 1.0}; 

    // applies the properties to light0
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos); 
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0); 
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0); 
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

	// object surface properties
    GLfloat mat_ambience[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; 
    GLfloat mat_shine[] = { 100 };

	// applies the object surface properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_ambience);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shine);

	glShadeModel(GL_FLAT);
	
	// depth test to prevent drawing the other side of the object
    glEnable(GL_DEPTH_TEST);
    // automatically normalizes normal vectors
    glEnable(GL_NORMALIZE);

	// Create texture ids.
	glGenTextures(1, texture);

	// Load external textures.
	loadTextures();

	// Specify how texture values combine with current surface color values.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	/* 
	Rewrite the commands below with changed variable names 
	Protip: You can modify the function to omit vertex normals because they are not required. 
	Protip 2: You can further modify the function to omit texture coordinates where not required.
	*/
	
	getOBJ("Tiger.obj", &nTiger, &vTiger, &fTiger, &vNormTiger, &fNormTiger, &texCorTiger, &bBoxTiger);
	getOBJ("Bunny.obj", &nBunny, &vBunny, &fBunny, &vNormBunny, &fNormBunny, &texCorBunny, &bBoxBunny);
	getOBJ("Cat.obj", &nCat, &vCat, &fCat, &vNormCat, &fNormCat, &texCorCat, &bBoxCat);
	getOBJ("Dog.obj", &nDog, &vDog, &fDog, &vNormDog, &fNormDog, &texCorDog, &bBoxDog);
	getOBJ("Duck.obj", &nDuck, &vDuck, &fDuck, &vNormDuck, &fNormDuck, &texCorDuck, &bBoxDuck);

	makeMenu();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	cout << "Interaction:" << endl;
	cout << "Press the up and down arrow keys to move the viewpoint over the field." << endl
		<< "Press the left and right arrow keys to cycle through the filters." << endl;

	/* Write your own interaction instructions. */
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("fieldAndSkyFiltered.cpp"); /* Fix this */
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
