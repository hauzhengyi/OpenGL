#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>

int numVert = 3;
float pi = 3.1415926;
float rad = 30;
float xCenter = 50;
float yCenter = 50;

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);

	glBegin(GL_LINE_LOOP);
	for(float t = 0; t < 2*pi; t += 2*pi/numVert) {
		float x = xCenter + rad*cos(t);
		float y = yCenter + rad*sin(t);
		glVertex3f(x, y, 0.0);
	}
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
	case '+':
		numVert++;
		glutPostRedisplay();
		break;
	case '-':
		numVert > 3? numVert-- : numVert;
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
	
	glutInitWindowSize(700, 700);
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