#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <math.h>

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_POLYGON);
	glVertex3f(20.0, 25.0, 0.0);
	glVertex3f(80.0, 25.0, 0.0);
	glVertex3f(50.0, 25+sqrt(3)/2*60, 0.0);
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
	
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(500, 200);
	
	glutCreateWindow("Hello from Hau Zheng Yi.cpp");
	
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}