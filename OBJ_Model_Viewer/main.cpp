#include <iostream>
#include <sstream>    
#include <string> 
#include <fstream> 
#include <vector>
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#define PI 3.1415926

// Globals.
static std::vector<float> verticesVector; // Vector to read in vertex x, y and z values fromt the OBJ file.
static std::vector<int> facesVector; // Vector to read in face vertex indices from the OBJ file.
static std::vector<float> faceNormalsVector; // Vector to read in face normals
static std::vector<float> vertexNormalsVector; // Vector to read in vertex normals
static std::vector<int> vertexWeightVector; // Vector to read in the number of faces surrounding a vertex
static float *vertices = NULL;  // Vertex array of the object x, y, z values.
static int *faces = NULL; // Face (triangle) vertex indices.
static float *normals = NULL; // Normal array
static int numIndices; // Number of face vertex indices.
static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate the object.
static float Xlight = 0.0, Ylight = 0.0, Zlight = 0.0; // Angles to rotate light source.
static float Xbuff = 0.0, Ybuff = 0.0, Zbuff = 0.0; // Angles of the previous frame.
static float Xmax, Xmin, Ymax, Ymin, Zmax, Zmin; // min/max values of vertices on each axis
static float diagonal; // diagonal length of the bounding box
static int shadeMode = 0; // 0 for face normals, 1 for vertex normals
static float lightPosition[3] = {0.1, 0.1, 4.1};

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

// function to find the bounding coordinates for the object
void computeBoundingBox()
{
	// temperarily setting the max/min values of each axis to the first vertex as reference
	Xmax = Xmin = verticesVector[0];
	Ymax = Ymin = verticesVector[1];
	Zmax = Zmin = verticesVector[2];

	// compare each vertex's coordinates to the current min/max coordinates and update if there is
	// any new min/max values
	for(int i = 0; i < verticesVector.size(); i++)
	{
		if(i % 3 == 0)
		{
			if(verticesVector[i] < Xmin) Xmin = verticesVector[i];
			if(verticesVector[i] > Xmax) Xmax = verticesVector[i];
		}
		if(i % 3 == 1)
		{
			if(verticesVector[i] < Ymin) Ymin = verticesVector[i];
			if(verticesVector[i] > Ymax) Ymax = verticesVector[i];
		}
		if(i % 3 == 2)
		{
			if(verticesVector[i] < Zmin) Zmin = verticesVector[i];
			if(verticesVector[i] > Zmax) Zmax = verticesVector[i];
		}
	}

	// calculate the diagonal length of the bounding box
	diagonal = sqrt(pow(Xmax - Xmin, 2) + pow(Ymax - Ymin, 2) + pow(Zmax - Zmin, 2));
}

// Each face contains 3 vertices, each vertex contains 3 values.
// We need to find all 3 vertex coordinates, and create 2 vectors A, B, which are just 2 edges of the
// triangle.
// The cross-product A x B is used to find the normal vector
// "Vector" here is referring to the mathematical vector
void computeFaceNormals()
{
	// Sizing the vector to be the same as verticesVector
	for(int i = 0; i < verticesVector.size(); i++)
	{
		faceNormalsVector.push_back(0);
		vertexNormalsVector.push_back(0);
		vertexWeightVector.push_back(0);
	}

	// Each face has 3 vertices, increment by 3 every time
	for(int i = 0; i < facesVector.size(); i += 3)
	{
		float vertexIndex1 = facesVector[i] * 3;
		float vertexIndex2 = facesVector[i+1] * 3;
		float vertexIndex3 = facesVector[i+2] * 3;

		//Vector A
		float Ax = verticesVector[vertexIndex2] - verticesVector[vertexIndex1];
		float Ay = verticesVector[vertexIndex2+1] - verticesVector[vertexIndex1+1];
		float Az = verticesVector[vertexIndex2+2] - verticesVector[vertexIndex1+2];

		// Vector B
		float Bx = verticesVector[vertexIndex3] - verticesVector[vertexIndex1];
		float By = verticesVector[vertexIndex3+1] - verticesVector[vertexIndex1+1];
		float Bz = verticesVector[vertexIndex3+2] - verticesVector[vertexIndex1+2];

		// Normal calculated using cross-product
		float normalX = Ay*Bz - Az*By;
		float normalY = Az*Bx - Ax*Bz;
		float normalZ = Ax*By - Ay*Bx;

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


		// with this method it is more efficient to calculate the vertex normals inside here
		vertexNormalsVector[vertexIndex1] += normalX;
		vertexNormalsVector[vertexIndex1+1] += normalY;
		vertexNormalsVector[vertexIndex1+2] += normalZ;
		vertexWeightVector[vertexIndex1]++;
		vertexWeightVector[vertexIndex1+1]++;
		vertexWeightVector[vertexIndex1+2]++;

		vertexNormalsVector[vertexIndex2] += normalX;
		vertexNormalsVector[vertexIndex2+1] += normalY;
		vertexNormalsVector[vertexIndex2+2] += normalZ;
		vertexWeightVector[vertexIndex2]++;
		vertexWeightVector[vertexIndex2+1]++;
		vertexWeightVector[vertexIndex2+2]++;
		
		vertexNormalsVector[vertexIndex3] += normalX;
		vertexNormalsVector[vertexIndex3+1] += normalY;
		vertexNormalsVector[vertexIndex3+2] += normalZ;
		vertexWeightVector[vertexIndex3]++;
		vertexWeightVector[vertexIndex3+1]++;
		vertexWeightVector[vertexIndex3+2]++;
	}

	for(int i = 0; i < vertexNormalsVector.size(); i++)
	{
		vertexNormalsVector[i] /= vertexWeightVector[i];
	}
}

void loadOBJ(std::string fileName)
{
   std::string line;
   int count, vertexIndex1, vertexIndex2, vertexIndex3;
   float coordinateValue;
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

	  // Nothing other than vertex and face data is processed.
	  else
	  {
	  }
   }

   // Close the OBJ file.
   inFile.close();
}

// independent light positioning
void calculateLightPosition()
{
	float x = lightPosition[0];
	float y = lightPosition[1];
	float z = lightPosition[2];

	// rotate in x axis, update z and y coordinates
	if(Xlight != Xbuff)
	{
		bool clockwise;
		if(Xlight > Xbuff)
		{
			Xlight -= 5.0;
			clockwise = 1;
		}
		else
		{
			Xlight += 5.0;
			clockwise = 0;
		}
		float angle;
		if(z > 0 && y > 0)
			angle = atan(y/z)/PI*180;
		else if(z < 0 && y > 0)
			angle = 180 - atan(abs(y/z))/PI*180;
		else if(z < 0 && y < 0)
			angle = 180 + atan(abs(y/z))/PI*180;
		else
			angle = 360 - atan(abs(y/z))/PI*180;
		if(clockwise)
			Xlight = angle + 5.0;
		else
			Xlight = angle - 5.0;
		if(Xlight > 360.0) Xlight -= 360.0;
		if(Xlight < 0) Xlight += 360.0;
		float length = sqrt(z*z + y*y);
		z = length*cos((Xlight)/180*PI);
		y = length*sin((Xlight)/180*PI);
		Xbuff = Xlight;
	}

	// rotate in y axis, update x and z coordinates
	if(Ylight != Ybuff)
	{
		bool clockwise;
		if(Ylight > Ybuff)
		{
			Ylight -= 5.0;
			clockwise = 1;
		}
		else
		{
			Ylight += 5.0;
			clockwise = 0;
		}
		float angle;
		if(z > 0 && x > 0)
			angle = atan(x/z)/PI*180;
		else if(z < 0 && x > 0)
			angle = 180 - atan(abs(x/z))/PI*180;
		else if(z < 0 && x < 0)
			angle = 180 + atan(abs(x/z))/PI*180;
		else
			angle = 360 - atan(abs(x/z))/PI*180;
		if(clockwise)
			Ylight = angle + 5.0;
		else
			Ylight = angle - 5.0;
		if(Ylight > 360.0) Ylight -= 360.0;
		if(Ylight < 0) Ylight += 360.0;
		float length = sqrt(x*x + z*z);
		z = length*cos((Ylight)/180*PI);
		x = length*sin((Ylight)/180*PI);
		Ybuff = Ylight;
	}

	// rotate in z axis, update x and y coordinates
	if(Zlight != Zbuff)
	{
		bool clockwise;
		if(Zlight > Zbuff)
		{
			Zlight -= 5.0;
			clockwise = 1;
		}
		else
		{
			Zlight += 5.0;
			clockwise = 0;
		}
		float angle;
		if(y > 0 && x > 0)
			angle = atan(x/y)/PI*180;
		else if(y < 0 && x > 0)
			angle = 180 - atan(abs(x/y))/PI*180;
		else if(y < 0 && x < 0)
			angle = 180 + atan(abs(x/y))/PI*180;
		else
			angle = 360 - atan(abs(x/y))/PI*180;
		if(clockwise)
			Zlight = angle + 5.0;
		else
			Zlight = angle - 5.0;
		if(Zlight > 360.0) Zlight -= 360.0;
		if(Zlight < 0) Zlight += 360.0;
		float length = sqrt(x*x + y*y);
		y = length*cos((Zlight)/180*PI);
		x = length*sin((Zlight)/180*PI);
		Zbuff = Zlight;
	}

	// update new position
	lightPosition[0] = x;
	lightPosition[1] = y;
	lightPosition[2] = z;
}

// Initialization routine.
void setup(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glClearColor(1.0, 1.0, 1.0, 0.0);

	// Light property vectors.
	float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
	float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float lightPos[] = { 0.0, 1.5, 3.0, 1.0 };
	float globAmb[] = { 0.2, 0.2, 0.2, 1.0 };

	// Light properties.
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0); // Enable particular light source.
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Enable two-sided lighting.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.

	// Material property vectors.
	float matAmbAndDif[] = { 0.4, 0.0, 0.8, 1.0 };
	float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float matShine[] = { 50.0 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);

	// Read the external OBJ file into the internal vertex and face vectors.
	loadOBJ("gourd.obj");

	computeBoundingBox();
	computeFaceNormals();

	// Size the face normal array and copy into it face normal index values from the face normal vector.
	if(shadeMode)
	{
		normals = new float[vertexNormalsVector.size()];
		for(int i = 0; i < vertexNormalsVector.size(); i++) normals[i] = vertexNormalsVector[i];
	}
	else
	{
		normals = new float[faceNormalsVector.size()];
		for(int i = 0; i < faceNormalsVector.size(); i++) normals[i] = faceNormalsVector[i];
	}

	// Size the vertex array and copy into it x, y, z values from the vertex vector.
	vertices = new float[verticesVector.size()];
	for (int i = 0; i < verticesVector.size(); i++) vertices[i] = verticesVector[i];

	// Size the faces array and copy into it face index values from the face vector.
	faces = new int[facesVector.size()];
	for (int i = 0; i < facesVector.size(); i++) faces[i] = facesVector[i];
	numIndices = facesVector.size();

	// Specify the number of coordinates per vertex, data type of each coordinate, offset between consecutive vertices and the pointer to the vertex array
	glVertexPointer(3, GL_FLOAT, 0, vertices);
}

// Drawing routine.
void drawScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	calculateLightPosition();
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.0, 0.0, 0.0);

	if(shadeMode)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);
	glNormalPointer(GL_FLOAT, 0, normals);

	// Bounding
	glScalef(4.0/diagonal, 4.0/diagonal, 4.0/diagonal);
	glTranslatef(-(Xmax+Xmin)/2, -(Ymax+Ymin)/2, -(Zmax+Zmin)/2);

	// Rotate scene.
	glRotatef(Zangle, 0.0, 0.0, 1.0);
	glRotatef(Yangle, 0.0, 1.0, 0.0);
	glRotatef(Xangle, 1.0, 0.0, 0.0);

	// Draw the object mesh.
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, faces);

	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / (float)h, 1.0, 50.0);
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
	case 'x':
		Xangle += 5.0;
		if (Xangle > 360.0) Xangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'X':
		Xangle -= 5.0;
		if (Xangle < 0.0) Xangle += 360.0;
		glutPostRedisplay();
		break;
	case 'y':
		Yangle += 5.0;
		if (Yangle > 360.0) Yangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'Y':
		Yangle -= 5.0;
		if (Yangle < 0.0) Yangle += 360.0;
		glutPostRedisplay();
		break;
	case 'z':
		Zangle += 5.0;
		if (Zangle > 360.0) Zangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'Z':
		Zangle -= 5.0;
		if (Zangle < 0.0) Zangle += 360.0;
		glutPostRedisplay();
		break;
	case 's':
		shadeMode = !shadeMode;
		glutPostRedisplay();
		break;
	case 'u':
        Xlight += 5.0;
        glutPostRedisplay();
        break;
    case 'U':
        Xlight -= 5.0;
        glutPostRedisplay();
        break;
    case 'v':
        Ylight += 5.0;
        glutPostRedisplay();
        break;
    case 'V':
        Ylight -= 5.0;
        glutPostRedisplay();
        break;
    case 'w':
        Zlight += 5.0;
        glutPostRedisplay();
        break;
    case 'W':
        Zlight -= 5.0;
        glutPostRedisplay();
        break;
	default:
		break;
	}
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press x, X, y, Y, z, Z to turn the object." << std::endl;
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
	glutCreateWindow("OBJmodelViewer.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}

