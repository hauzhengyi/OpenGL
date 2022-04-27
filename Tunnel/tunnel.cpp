#include <cstdlib>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

#include "getBMP.h"

// Globals.
static unsigned int texture[24]; // Array of texture indices.
static float d = 0.0; // Distance parameter in gluLookAt().
static int filter = 0; // Filter id.
static long font = (long)GLUT_BITMAP_8_BY_13; // Font selection.

// Load external textures.
void loadTextures()
{
	// Local storage for bmp image data.
	imageFile *image[4];

	// Load the images.
	image[0] = getBMP("floor.bmp");
	image[1] = getBMP("wall.bmp");
	image[2] = getBMP("ceiling.bmp");
	image[3] = getBMP("closeend.bmp");

	// Bind floor image to texture[0] with specified mag and min filters. 
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind floor image to texture[1] with specified mag and min filters. 
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
	        	 GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind floor image to texture[2] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind floor image to texture[3] with specified mag and min filters.
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind floor image to texture[4] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind floor image to texture[5] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->width, image[0]->height, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind wall image to texture[6] with specified mag and min filters.
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind wall image to texture[7] with specified mag and min filters. 
	glBindTexture(GL_TEXTURE_2D, texture[7]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind wall image to texture[8] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[8]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind wall image to texture[9] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[9]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind wall image to texture[10] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[10]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind wall image to texture[11] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[11]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->width, image[1]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind ceiling image to texture[12] with specified mag and min filters.
	glBindTexture(GL_TEXTURE_2D, texture[12]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->width, image[2]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind ceiling image to texture[13] with specified mag and min filters.
	glBindTexture(GL_TEXTURE_2D, texture[13]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->width, image[2]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind ceiling image to texture[14] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[14]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->width, image[2]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind wall image to texture[15] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[15]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->width, image[2]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind wall image to texture[16] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[16]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->width, image[2]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind wall image to texture[17] with specified mag and min filters. 
	// Use mipmapping.
	glBindTexture(GL_TEXTURE_2D, texture[17]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->width, image[2]->height, 0,
		         GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind tunnel end image to texture[18] with specified mag and min filters.
	glBindTexture(GL_TEXTURE_2D, texture[18]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[3]->width, image[3]->height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, image[3]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Bind tunnel end image to texture[19] with specified mag and min filters.
	glBindTexture(GL_TEXTURE_2D, texture[19]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[3]->width, image[3]->height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, image[3]->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind tunnel end image to texture[20] with specified mag and min filters.
	glBindTexture(GL_TEXTURE_2D, texture[20]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[3]->width, image[3]->height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, image[3]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind tunnel end image to texture[21] with specified mag and min filters.
	glBindTexture(GL_TEXTURE_2D, texture[21]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[3]->width, image[3]->height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, image[3]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind tunnel end image to texture[22] with specified mag and min filters.
	glBindTexture(GL_TEXTURE_2D, texture[22]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[3]->width, image[3]->height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, image[3]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind tunnel end image to texture[23] with specified mag and min filters.
	glBindTexture(GL_TEXTURE_2D, texture[23]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[3]->width, image[3]->height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, image[3]->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
	char *c;

	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Write messages.
void writeData(void)
{
	switch (filter)
	{
	case 0:
		glRasterPos3f(-4.9, 4.8, -5.1); writeBitmapString((void*)font, "Mag filter: GL_NEAREST");
		glRasterPos3f(-4.9, 4.5, -5.1); writeBitmapString((void*)font, "Min filter: GL_NEAREST");
		break;
	case 1:
		glRasterPos3f(-4.9, 4.8, -5.1); writeBitmapString((void*)font, "Mag filter: GL_LINEAR");
		glRasterPos3f(-4.9, 4.5, -5.1); writeBitmapString((void*)font, "Min filter: GL_LINEAR");
		break;
	case 2:
		glRasterPos3f(-4.9, 4.8, -5.1); writeBitmapString((void*)font, "Mag filter: GL_LINEAR");
		glRasterPos3f(-4.9, 4.5, -5.1); writeBitmapString((void*)font, "Min filter: GL_NEAREST_MIPMAP_NEAREST");
		break;
	case 3:
		glRasterPos3f(-4.9, 4.8, -5.1); writeBitmapString((void*)font, "Mag filter: GL_LINEAR");
		glRasterPos3f(-4.9, 4.5, -5.1); writeBitmapString((void*)font, "Min filter: GL_LINEAR_MIPMAP_NEAREST");
		break;
	case 4:
		glRasterPos3f(-4.9, 4.8, -5.1); writeBitmapString((void*)font, "Mag filter: GL_LINEAR");
		glRasterPos3f(-4.9, 4.5, -5.1); writeBitmapString((void*)font, "Min filter: GL_NEAREST_MIPMAP_LINEAR");
		break;
	case 5:
		glRasterPos3f(-4.9, 4.8, -5.1); writeBitmapString((void*)font, "Mag filter: GL_LINEAR");
		glRasterPos3f(-4.9, 4.5, -5.1); writeBitmapString((void*)font, "Min filter: GL_LINEAR_MIPMAP_LINEAR");
		break;
	default:
		break;
	}
}

// Initialization routine.
void setup(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	// Create texture ids.
	glGenTextures(24, texture);

	// Load external textures.
	loadTextures();

	// Specify how texture values combine with current surface color values.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Turn on OpenGL texturing.
	glEnable(GL_TEXTURE_2D);
}

// Drawing routine.
void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
	gluLookAt(0.0, 10.0, 15.0 + d, 0.0, 10.0, d, 0.0, 1.0, 0.0);

	// Map the floor texture onto a rectangle along the xz-plane.
	glBindTexture(GL_TEXTURE_2D, texture[filter]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, 0.0, 100.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(10.0, 0.0, 100.0);
	glTexCoord2f(1.0, 10.0); glVertex3f(10.0, 0.0, -100.0);
	glTexCoord2f(0.0, 10.0); glVertex3f(-10.0, 0.0, -100.0);
	glEnd();

	// Map the left wall texture onto a rectangle along the yz-plane at x = -10.0.
	glBindTexture(GL_TEXTURE_2D, texture[filter+6]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, 0.0, 100.0);
	glTexCoord2f(10.0, 0.0); glVertex3f(-10.0, 0.0, -100.0);
	glTexCoord2f(10.0, 1.0); glVertex3f(-10.0, 20.0, -100.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-10.0, 20.0, 100.0);
	glEnd();

	// Map the right wall texture onto a rectangle along the yz-plane at x = 10.0.
	glBindTexture(GL_TEXTURE_2D, texture[filter+6]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(10.0, 0.0, 100.0);
	glTexCoord2f(10.0, 0.0); glVertex3f(10.0, 0.0, -100.0);
	glTexCoord2f(10.0, 1.0); glVertex3f(10.0, 20.0, -100.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(10.0, 20.0, 100.0);
	glEnd();

	// Map the ceiling texture onto a rectangle along the xy-plane at z = 20.0.
	glBindTexture(GL_TEXTURE_2D, texture[filter+12]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, 20.0, 100.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(10.0, 20.0, 100.0);
	glTexCoord2f(1.0, 10.0); glVertex3f(10.0, 20.0, -100.0);
	glTexCoord2f(0.0, 10.0); glVertex3f(-10.0, 20.0, -100.0);
	glEnd();

	// Map the tunnel end texture onto a rectangle parallel to the xy-plane at the far side.
	glBindTexture(GL_TEXTURE_2D, texture[filter+18]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-10.0, 0.0, -70.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(10.0, 0.0, -70.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(10.0, 20.0, -70.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-10.0, 20.0, -70.0);
	glEnd();

	// Write data.
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();
	glColor3f(0.0, 0.0, 0.0);
	writeData();

	glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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
	if (key == GLUT_KEY_UP) d -= 0.1;
	if (key == GLUT_KEY_DOWN) d += 0.1;
	if (key == GLUT_KEY_RIGHT)
	{
		filter++;
		if (filter == 6) filter = 0;
	}
	if (key == GLUT_KEY_LEFT)
	{
		filter--;
		if (filter == -1) filter = 5;
	}
	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press the up and down arrow keys to move the viewpoint over the field." << std::endl
		<< "Press the left and right arrow keys to cycle through the filters." << std::endl;
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("tunnel.cpp");
	glewInit();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
