//////////////////////////////////////////////////////////////////////////////////
// OBJmodelViewer.cpp
//
// An object defined in an external Wavefront OBJ file is loaded and displayed. 
// Only vertex and face lines are read. All other lines are ignored. Vertex lines 
// are assumed to have only x, y and z coordinate values. The (optional) w value 
// is ignored if present. Within a face line only vertex indices are read. Texture 
// and normal indices are allowed but ignored. Face lines can have more than three 
// vertices. If a face line has more than three vertices the output is a fan 
// triangulation. Therefore, the mesh generated consists of only triangles.
//
// Interaction:
// Press x, X, y, Y, z, Z to turn the object.
//
// Sumanta Guha.
//////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>     
#include <string> 
#include <fstream> 
#include <vector>
#include <climits>
#include <cmath>

#include <GL/glew.h>
#include <GL/freeglut.h> 

// Globals.
static std::vector<double> verticesVector; // Vector to read in vertex x, y and z values fromt the OBJ file.
static std::vector<int> facesVector; // Vector to read in face vertex indices from the OBJ file.
static std::vector<double> facesNormal; // Calculated normal of each face
static std::vector<double> verticesNormal; // Calculated normal of each vector
static std::vector<double> verticesTotalArea; // Total area of all the faces connecting each vertex. 
static double *vertices = NULL;  // Vertex array of the object x, y, z values.
static int *faces = NULL; // Face (triangle) vertex indices.
static int numIndices; // Number of face vertex indices.
static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate the object.
static double Xmin = INT_MAX, Ymin = INT_MAX, Zmin = INT_MAX;
static double Xmax = INT_MIN, Ymax = INT_MIN, Zmax = INT_MIN;

static double Xsize = -1, Ysize = -1, Zsize = -1;
static double Xcenter = -1, Ycenter = -1, Zcenter = -1;
static double boundingDiagonal = -1;
static bool smoothShading = false;
static float lightXangle = 0.0;
static float lightYangle = 0.0;
static float lightZangle = 0.0;

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

// Function to place the model at the center of the viewing frustum. 
void CenterModel()
{
    glScalef(4.0/boundingDiagonal, 4.0/boundingDiagonal, 4.0/boundingDiagonal);
    glTranslatef(-Xcenter, -Ycenter, -Zcenter);
}

// Function to compute the normal of each face. 
// Result stored in facesNormal vector.
void ComputeFaceNormals()
{
    verticesNormal.clear();
    facesNormal.clear();

    // initializing verticesNormal and facesNormal to the size of verticesVector
    for (int i = 0; i < verticesVector.size(); i++)
    {
        verticesNormal.push_back(0);
        facesNormal.push_back(0);
    }
        
    // initializing verticesTotalArea to verticesVector / 3
    for (int i = 0; i < verticesVector.size() / 3; i++)
        verticesTotalArea.push_back(0);

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
void loadOBJ(std::string fileName)
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

// Initialization routine.
void setup(void)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    GLfloat light0_pos[]={0.0, 0.0, 4.0, 1.0};
    GLfloat ambient0[]={0.0, 0.0, 0.0, 1.0};  
    GLfloat diffuse0[]={1.0, 1.0, 1.0, 1.0};
    GLfloat specular0[]={1.0, 1.0, 1.0, 1.0}; 
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos); 
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0); 
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0); 
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

    GLfloat mat_ambience[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; 
    GLfloat mat_shine[] = { 100 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_ambience);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shine);

    glClearColor(1.0, 1.0, 1.0, 0.0);

    // Read the external OBJ file into the internal vertex and face vectors.
    loadOBJ("gourd.obj");

    // Size the vertex array and copy into it x, y, z values from the vertex vector.
    vertices = new double[verticesVector.size()];
    for (int i = 0; i < verticesVector.size(); i++) vertices[i] = verticesVector[i];

    // Size the faces array and copy into it face index values from the face vector.
    faces = new int[facesVector.size()];
    for (int i = 0; i < facesVector.size(); i++) faces[i] = facesVector[i];
    numIndices = facesVector.size();

    glVertexPointer(3, GL_DOUBLE, 0, vertices);
}

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Rotates light source.
    glRotatef(lightZangle, 0.0, 0.0, 1.0);
    glRotatef(lightYangle, 0.0, 1.0, 0.0);
    glRotatef(lightXangle, 1.0, 0.0, 0.0);

    GLfloat light0_pos[]={0.0, 0.0, 4.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos); 

    // rotates the scene back.
    glRotatef(-lightXangle, 1.0, 0.0, 0.0);
    glRotatef(-lightYangle, 0.0, 1.0, 0.0);
    glRotatef(-lightZangle, 0.0, 0.0, 1.0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(0.0, 0.0, 0.0);

    // Rotate scene.
    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);

    ComputeBoundingBox();
    CenterModel();

    ComputeFaceNormals();
    ComputeVertexNormals();

    // allocates the array for normals. 
    // each vertex has a normal, hence the size of the normal array should be the same as the vertices vector.
    double* normalPtr = new double[verticesVector.size()];
    if (smoothShading)
    {
        glShadeModel(GL_SMOOTH);
        for (int i = 0; i < verticesVector.size(); i++) 
            normalPtr[i] = verticesNormal[i];
    }
    else
    {
        glShadeModel(GL_FLAT);
        for (int i = 0; i < verticesVector.size(); i++) 
            normalPtr[i] = facesNormal[i];
    }

    glNormalPointer(GL_DOUBLE, 0, normalPtr);

    // Draw the object mesh.
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, faces);

    delete normalPtr;

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
        smoothShading = !smoothShading;
        glutPostRedisplay();
        break;
    case 'u':
        lightXangle += 5.0;
        if (lightXangle > 360.0) lightXangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'U':
        lightXangle -= 5.0;
        if (lightXangle < 360.0) lightXangle += 360.0;
        glutPostRedisplay();
        break;
    case 'v':
        lightYangle += 5.0;
        if (lightYangle > 360.0) lightYangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'V':
        lightYangle -= 5.0;
        if (lightYangle < 360.0) lightYangle += 360.0;
        glutPostRedisplay();
        break;
    case 'w':
        lightZangle += 5.0;
        if (lightZangle > 360.0) lightZangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'W':
        lightZangle -= 5.0;
        if (lightZangle < 360.0) lightZangle += 360.0;
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
    std::cout << "Press x, X, y, Y, z, Z to turn the object, press u, U, v, V, w, W to turn the light source." << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
    printInteraction();
    glutInit(&argc, argv);

    glutInitContextVersion(1, 2);
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

