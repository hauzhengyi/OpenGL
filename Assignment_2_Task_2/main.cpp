#include <GL/glew.h>
#include <GL/freeglut.h> 

void drawScene(void)
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

	glLineWidth(2.0);

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