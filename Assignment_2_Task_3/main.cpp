#include <cstdlib>
#include <cmath>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#define PI 3.14159265358979324

static float R = 40.0;
static float X = 50.0;
static float Y = 50.0;
static int numVertices = 5;
bool option = TRUE;

void drawScene(void)
{
	if(option)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(0.0, 0.0, 0.0);

		glBegin(GL_TRIANGLE_STRIP);

		glColor3f(0.2, 0.8, 0.8);

		glVertex3f(25.0, 70.0, 0.0);
		glVertex3f(35.0, 70.0, 0.0);
		glVertex3f(25.0, 80.0, 0.0);

		glVertex3f(45.0, 80.0, 0.0);
		glVertex3f(35.0, 70.0, 0.0);

		glVertex3f(35.0, 20.0, 0.0);
		glVertex3f(45.0, 80.0, 0.0);

		glVertex3f(45.0, 30.0, 0.0);
		glVertex3f(35.0, 20.0, 0.0);

		glVertex3f(65.0, 20.0, 0.0);
		glVertex3f(45.0, 30.0, 0.0);

		glVertex3f(55.0, 30.0, 0.0);
		glVertex3f(65.0, 20.0, 0.0);

		glVertex3f(65.0, 70.0, 0.0);
		glVertex3f(55.0, 30.0, 0.0);

		glVertex3f(55.0, 80.0, 0.0);
		glVertex3f(65.0, 70.0, 0.0);

		glVertex3f(75.0, 80.0, 0.0);
		glVertex3f(75.0, 70.0, 0.0);

		glEnd();

		glLineWidth(3.0);

		glBegin(GL_LINE_LOOP);

		glColor3f(0.1, 0.6, 0.6);

		glVertex3f(25.0, 80.0, 0.0);
		glVertex3f(45.0, 80.0, 0.0);
		glVertex3f(45.0, 30.0, 0.0);
		glVertex3f(55.0, 30.0, 0.0);
		glVertex3f(55.0, 80.0, 0.0);
		glVertex3f(75.0, 80.0, 0.0);
		glVertex3f(75.0, 70.0, 0.0);
		glVertex3f(65.0, 70.0, 0.0);
		glVertex3f(65.0, 20.0, 0.0);
		glVertex3f(35.0, 20.0, 0.0);
		glVertex3f(35.0, 70.0, 0.0);
		glVertex3f(25.0, 70.0, 0.0);

		glEnd();

		glFlush();

		glLineWidth(2.0);

		glBegin(GL_LINE_STRIP);

		glColor3f(0.0, 0.0, 0.0);

		glVertex3f(25.0, 80.0, 0.0);
		glVertex3f(35.0, 70.0, 0.0);
		glVertex3f(45.0, 80.0, 0.0);
		glVertex3f(35.0, 20.0, 0.0);
		glVertex3f(45.0, 30.0, 0.0);

		glEnd();

		glFlush();
	}
	else
	{
		float t = 0;
		int i;

		glClear(GL_COLOR_BUFFER_BIT);
		glLineWidth(1.0);

		glBegin(GL_LINE_LOOP);
		for (i = 0; i < numVertices; ++i)
		{
			glColor3f((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
			glVertex3f(X + 16*pow(sin(t), 3), Y + 13*cos(t) - 5*cos(2*t) - cos(4*t), 0.0);
			t += 2 * PI / numVertices;
		}
		glEnd();

		glFlush();
	}
}

void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'c':
		option = TRUE;
		glutPostRedisplay();
		break;
	case 's':
		option = FALSE;
		glutPostRedisplay();
		break;
	case '+':
		if(option) break;
		numVertices++;
		glutPostRedisplay();
		break;
	case '-':
		if(option) break;
		if (numVertices > 3) numVertices--;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	
	glutCreateWindow("main.cpp");
	
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}