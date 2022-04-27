#include <iostream>
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h> 

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glColor3f(0.0, 0.0, 0.0);
	glTranslatef(0.0, 0.0, -15.0);
	glutWireCube(5.0);

	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(0.0, 2.5+sqrt(2), 0.0);
	glRotatef(45, 0.0, 0.0, 1.0);
	glutWireCube(2.0);

	glRotatef(-45, 0.0, 0.0, 1.0);
	glTranslatef(-2.0, -6.5-sqrt(2), 0.0);
	glScalef(1.0/3.0, 1, 1.0/3.0);
	glutWireCube(3);

	glScalef(3.0, 1, 3.0);
	glTranslatef(4.0, 0.0, 0.0);
	glScalef(1.0/3.0, 1, 1.0/3.0);
	glutWireCube(3);

	glFlush();
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
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

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

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("box.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}

