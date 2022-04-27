#include <cstdlib>
#include <vector>
#include <iostream>
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#define INACTIVE 0
#define POINT 1
#define LINE 2
#define RECTANGLE 3
#define POLY 4
#define CIRCLE 5
#define HEXAGON 6
#define NUMBERPRIMITIVES 6
#define PI 3.1415926

static int width, height;
static float pointSize = 3.0;
static int primitive = INACTIVE;
static int pointCount = 0;
static int tempX, tempY;
static int isGrid = 1;
static int circleRes = 100;
static float red = 0.0;
static float green = 0.0;
static float blue = 0.0;
static int fill = 0;

class Point
{
public:
	Point(float xVal, float yVal, float sizeVal)
	{
		x = xVal; y = yVal; size = sizeVal;
		r = red;
		g = green;
		b = blue;
	}
	Point() {};
	void drawPoint(void);
private:
	float x, y;
	float size;
	float r, g, b;
};

void Point::drawPoint()
{
	glColor3f(r, g, b);
	glPointSize(size);
	glBegin(GL_POINTS);
	glVertex3f(x, y, 0.0);
	glEnd();
}

std::vector<Point> points;

void drawPoints(void)
{
	for (auto point : points) { point.drawPoint(); }
}

class Line
{
public:
	Line(float x1Val, float y1Val, float x2Val, float y2Val)
	{
		x1 = x1Val; y1 = y1Val; x2 = x2Val; y2 = y2Val;
		r = red;
		g = green;
		b = blue;
	}
	Line() {};
	void drawLine();
private:
    float x1, y1, x2, y2;
	float r, g, b;
};

void Line::drawLine()
{
	glColor3f(r, g, b);
	glBegin(GL_LINES);
	glVertex3f(x1, y1, 0.0);
	glVertex3f(x2, y2, 0.0);
	glEnd();
}

std::vector<Line> lines;

void drawLines(void)
{
	for (auto line : lines) { line.drawLine(); }
}

class Rect
{
public:
	Rect(float x1Val, float y1Val, float x2Val, float y2Val)
	{
		x1 = x1Val; y1 = y1Val; x2 = x2Val; y2 = y2Val;
		r = red;
		g = green;
		b = blue;
		f = fill;
	}
	Rect() {};
	void drawRectangle();
private:
    float x1, y1, x2, y2;
	float r, g, b;
	float f;
};

void Rect::drawRectangle()
{
	glColor3f(r, g, b);
	if (f == TRUE) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRectf(x1, y1, x2, y2);
}

std::vector<Rect> rectangles;

void drawRectangles(void)
{
	for (auto rectangle : rectangles) { rectangle.drawRectangle(); }
}

class Poly
{
public:
	Poly(float xVal, float yVal)
	{
		polys.push_back(std::pair<float, float>(xVal, yVal));
		r = red;
		g = green;
		b = blue;
	}
	Poly() {};
	void addPoly(float xVal, float yVal);
	void drawPoly();
private:
	std::vector<std::pair<float, float>> polys;
	float r = 0.0, g = 0.0, b = 0.0;
};

void Poly::addPoly(float xVal, float yVal)
{
	polys.push_back(std::pair<float, float>(xVal, yVal));
}

void Poly::drawPoly()
{
	glColor3f(r, g, b);
	glBegin(GL_LINE_STRIP);
	for (auto poly : polys)
	{
		glVertex3f(poly.first, poly.second, 0.0);
	}
	glEnd();
}

std::vector<Poly> polylines;

void drawPolylines(void)
{
	for (auto polyline : polylines) { polyline.drawPoly(); }
}

class Circle
{
public:
	Circle(float x1Val, float y1Val, float x2Val, float y2Val)
	{
		cx = x1Val; cy = y1Val;
		rad = sqrt(powf(x2Val - x1Val, 2) + powf(y2Val - y1Val, 2));
		r = red;
		g = green;
		b = blue;
		f = fill;
	}
	Circle() {};
	void drawCircle();
private:
    float cx, cy, rad;
	float r, g, b;
	float f;
};

void Circle::drawCircle()
{
	if (f == TRUE) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(r, g, b);
	glBegin(GL_POLYGON);
	for (float t = 0; t < 2*PI; t += 2*PI/circleRes)
	{
		float x = cx + rad * cos(t);
		float y = cy + rad * sin(t);
		glVertex3f(x, y, 0.0);
	}
	glEnd();
}

std::vector<Circle> circles;

void drawCircles(void)
{
	for (auto circle : circles) { circle.drawCircle(); }
}

class Hexagon
{
public:
	Hexagon(float x1Val, float y1Val, float x2Val, float y2Val)
	{
		cx = x1Val; cy = y1Val;
		rad = sqrt(powf(x2Val - x1Val, 2) + powf(y2Val - y1Val, 2));
		angle = atan((y2Val - y1Val)/(x2Val - x1Val));
		r = red;
		g = green;
		b = blue;
		f = fill;
	}
	Hexagon() {};
	void drawHexagon();
private:
    float cx, cy, rad, angle;
	float r, g, b;
	float f;
};

void Hexagon::drawHexagon()
{
	if (f == TRUE) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(r, g, b);
	glBegin(GL_POLYGON);
	for (float t = angle; t < angle + 2*PI; t += PI/3)
	{
		float x = cx + rad * cos(t);
		float y = cy + rad * sin(t);
		glVertex3f(x, y, 0.0);
	}
	glEnd();
}

std::vector<Hexagon> hexagons;

void drawHexagons()
{
	for (auto hexagon : hexagons) { hexagon.drawHexagon(); }
}

void drawPointSelectionBox(void)
{
	if (primitive == POINT) glColor3f(1.0, 1.0, 1.0);
	else glColor3f(0.8, 0.8, 0.8);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glRectf(0.0, 0.9*height, 0.1*width, height);

	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRectf(0.0, 0.9*height, 0.1*width, height);

	glPointSize(pointSize);
	glColor3f(red, green, blue);
	glBegin(GL_POINTS);
	glVertex3f(0.05*width, 0.95*height, 0.0);
	glEnd();
}

void drawLineSelectionBox(void)
{
	if (primitive == LINE) glColor3f(1.0, 1.0, 1.0);
	else glColor3f(0.8, 0.8, 0.8);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glRectf(0.0, 0.8*height, 0.1*width, 0.9*height);

	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRectf(0.0, 0.8*height, 0.1*width, 0.9*height);

	glColor3f(red, green, blue);
	glBegin(GL_LINES);
	glVertex3f(0.025*width, 0.825*height, 0.0);
	glVertex3f(0.075*width, 0.875*height, 0.0);
	glEnd();
}

void drawRectangleSelectionBox(void)
{
	if (primitive == RECTANGLE) glColor3f(1.0, 1.0, 1.0);
	else glColor3f(0.8, 0.8, 0.8);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glRectf(0.0, 0.7*height, 0.1*width, 0.8*height);

	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRectf(0.0, 0.7*height, 0.1*width, 0.8*height);

	glColor3f(red, green, blue);
	if (fill == TRUE) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRectf(0.025*width, 0.735*height, 0.075*width, 0.765*height);
	glEnd();
}

void drawPolySelectionBox(void)
{
	if (primitive == POLY) glColor3f(1.0, 1.0, 1.0);
	else glColor3f(0.8, 0.8, 0.8);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glRectf(0.0, 0.6*height, 0.1*width, 0.7*height);

	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRectf(0.0, 0.6*height, 0.1*width, 0.7*height);

	glColor3f(red, green, blue);
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.050*width, 0.675*height, 0.0);
	glVertex3f(0.025*width, 0.650*height, 0.0);
	glVertex3f(0.075*width, 0.650*height, 0.0);
	glVertex3f(0.050*width, 0.625*height, 0.0);
	glEnd();
}

void drawCircleSelectionBox(void)
{
	if (primitive == CIRCLE) glColor3f(1.0, 1.0, 1.0);
	else glColor3f(0.8, 0.8, 0.8);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glRectf(0.0, 0.5*height, 0.1*width, 0.6*height);

	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRectf(0.0, 0.5*height, 0.1*width, 0.6*height);

	glColor3f(red, green, blue);
	if (fill == TRUE) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_POLYGON);
	for (float t = 0; t < 2*PI; t += 2*PI/50)
	{
		float x = 0.050 * width + 0.025 * width * cos(t);
		float y = 0.550 * height + 0.025 * height * sin(t);
		glVertex3f(x, y, 0.0);
	}
	glEnd();
}

void drawHexagonSelectionBox(void)
{
	if (primitive == HEXAGON) glColor3f(1.0, 1.0, 1.0);
	else glColor3f(0.8, 0.8, 0.8);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glRectf(0.0, 0.4*height, 0.1*width, 0.5*height);

	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRectf(0.0, 0.4*height, 0.1*width, 0.5*height);

	glColor3f(red, green, blue);
	if (fill == TRUE) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_POLYGON);
	for (float t = 0; t < 2*PI; t += PI/3)
	{
		float x = 0.050 * width + 0.030 * width * cos(t);
		float y = 0.450 * height + 0.030 * height * sin(t);
		glVertex3f(x, y, 0.0);
	}
	glEnd();
}

void drawInactiveArea(void)
{
	glColor3f(0.6, 0.6, 0.6);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glRectf(0.0, 0.0, 0.1*width, (1 - NUMBERPRIMITIVES*0.1)*height);

	glColor3f(0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRectf(0.0, 0.0, 0.1*width, (1 - NUMBERPRIMITIVES*0.1)*height);
}

void drawTempPoint(void)
{
	glColor3f(1.0, 0.0, 0.0);
	glPointSize(pointSize);
	glBegin(GL_POINTS);
	glVertex3f(tempX, tempY, 0.0);
	glEnd();
}

void drawGrid(void)
{
	int i;

	glEnable(GL_LINE_STIPPLE);
	glLineStipple(1, 0x5555);
	glColor3f(0.75, 0.75, 0.75);

	glBegin(GL_LINES);
	for (i = 2; i <= 9; i++)
	{
		glVertex3f(i*0.1*width, 0.0, 0.0);
		glVertex3f(i*0.1*width, height, 0.0);
	}
	for (i = 1; i <= 9; i++)
	{
		glVertex3f(0.1*width, i*0.1*height, 0.0);
		glVertex3f(width, i*0.1*height, 0.0);
	}
	glEnd();
	glDisable(GL_LINE_STIPPLE);
}

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);

	drawPoints();
	drawLines();
	drawRectangles();
	drawPolylines();
	drawCircles();
	drawHexagons();

	drawPointSelectionBox();
	drawLineSelectionBox();
	drawRectangleSelectionBox();
	drawPolySelectionBox();
	drawCircleSelectionBox();
	drawHexagonSelectionBox();
	drawInactiveArea();

	//if (((primitive == LINE) || (primitive == RECTANGLE)) || (primitive == POLY) && (pointCount == 1)) drawTempPoint();
	if (primitive != POINT && pointCount == 1) drawTempPoint();
	if (isGrid) drawGrid();

	glutSwapBuffers();
}

void pickPrimitive(int y)
{
	if (y < (1 - NUMBERPRIMITIVES*0.1)*height) primitive = INACTIVE;
	else if (y < (1 - 5 * 0.1)*height) primitive = HEXAGON;
	else if (y < (1 - 4 * 0.1)*height) primitive = CIRCLE;
	else if (y < (1 - 3 * 0.1)*height) primitive = POLY;
	else if (y < (1 - 2 * 0.1)*height) primitive = RECTANGLE;
	else if (y < (1 - 1 * 0.1)*height) primitive = LINE;
	else primitive = POINT;
}

void mouseControl(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		y = height - y;
					
		if (x < 0 || x > width || y < 0 || y > height);
		else if (x < 0.1*width)
		{
			pickPrimitive(y);
			pointCount = 0;
		}
		else
		{
			if (primitive == POINT) points.push_back(Point(x, y, pointSize));
			else if (primitive == LINE)
			{
				if (pointCount == 0)
				{
					tempX = x; tempY = y;
					pointCount++;
				}
				else
				{
					lines.push_back(Line(tempX, tempY, x, y));
					pointCount = 0;
				}
			}
			else if (primitive == RECTANGLE)
			{
				if (pointCount == 0)
				{
					tempX = x; tempY = y;
					pointCount++;
				}
				else
				{
					rectangles.push_back(Rect(tempX, tempY, x, y));
					pointCount = 0;
				}
			}
			else if (primitive == POLY)
			{
				if (pointCount == 0)
				{
					tempX = x; tempY = y;
					polylines.push_back(Poly(tempX, tempY));
					pointCount++;
				}
				else
				{
					(polylines.end()-1)->addPoly(x, y);
					pointCount++;
				}
			}
			else if (primitive == CIRCLE)
			{
				if (pointCount == 0)
				{
					tempX = x; tempY = y;
					pointCount++;
				}
				else
				{
					circles.push_back(Circle(tempX, tempY, x, y));
					pointCount = 0;
				}
				
			}
			else if (primitive == HEXAGON)
			{
				if (pointCount == 0)
				{
					tempX = x; tempY = y;
					pointCount++;
				}
				else
				{
					hexagons.push_back(Hexagon(tempX, tempY, x, y));
					pointCount = 0;
				}
			}
		}
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN && primitive == POLY)
	{
		if(pointCount == 1) polylines.pop_back();
		pointCount = 0;
	}
	glutPostRedisplay();
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, (float)w, 0.0, (float)h, -1.0, 1.0);

	width = w;
	height = h;

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

void clearAll(void)
{
	points.clear();
	lines.clear();
	rectangles.clear();
	polylines.clear();
	circles.clear();
	hexagons.clear();
	primitive = INACTIVE;
	pointCount = 0;
}

void rightMenu(int id)
{
	if (id == 1)
	{
		clearAll();
		glutPostRedisplay();
	}
	if (id == 2) exit(0);
}

void grid_menu(int id)
{
	if (id == 3) isGrid = 1;
	if (id == 4) isGrid = 0;
	glutPostRedisplay();
}

void color_menu(int id)
{
	if (id == 5) { red = 1.0; green = 0.0; blue = 0.0; }
	if (id == 6) { red = 0.0; green = 1.0; blue = 0.0; }
	if (id == 7) { red = 0.0; green = 0.0; blue = 1.0; }
	if (id == 8) { red = 0.0; green = 0.0; blue = 0.0; }
	glutPostRedisplay();
}

void fill_menu(int id)
{
	if (id == 9) { fill = 1; }
	if (id == 10) { fill = 0; }
	glutPostRedisplay();
}

void makeMenu(void)
{
	int sub_menu_1;
	sub_menu_1 = glutCreateMenu(grid_menu);
	glutAddMenuEntry("On", 3);
	glutAddMenuEntry("Off", 4);

	int sub_menu_2;
	sub_menu_2 = glutCreateMenu(color_menu);
	glutAddMenuEntry("Red", 5);
	glutAddMenuEntry("Green", 6);
	glutAddMenuEntry("Blue", 7);
	glutAddMenuEntry("Black", 8);

	int sub_menu_3;
	sub_menu_3 = glutCreateMenu(fill_menu);
	glutAddMenuEntry("Fill", 9);
	glutAddMenuEntry("Outline", 10);

	glutCreateMenu(rightMenu);
	glutAddSubMenu("Grid", sub_menu_1);
	glutAddSubMenu("Color", sub_menu_2);
	glutAddSubMenu("Fill", sub_menu_3);
	glutAddMenuEntry("Clear", 1);
	glutAddMenuEntry("Quit", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);

	makeMenu();
}

void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Left click on a box on the left to select a primitive." << std::endl
		<< "Then left click on the drawing area: once for point, twice for line or rectangle." << std::endl
		<< "Right click for menu options." << std::endl;
}

int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("canvas.cpp");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutMouseFunc(mouseControl); 

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}