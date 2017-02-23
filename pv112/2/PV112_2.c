// PoužitíGlugu.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "karel.h"

GLuint poprve= 1;

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}

void display(void)
{
	unsigned int a;

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glLoadIdentity();
	gluLookAt(0.0, 2.0, 15.0, 0.0, 2.0, 0.0, 0.0, 1.0, 0.0);
	glBegin(GL_TRIANGLE_STRIP);
	for (a = 0; a < karel_len; a++)
		glVertex3f(karel[a].y, karel[a].z, karel[a].x);
	glEnd();

/*	glutWireCube(1.0); */
/*	glutWireTeapot(1.0); */
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//(Left, Right, Bottom, Top, Near, Far);
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 30.0);
	glMatrixMode(GL_MODELVIEW);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(*argv);

	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}

