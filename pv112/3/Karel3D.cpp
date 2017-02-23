// PoužitíGlugu.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <fstream>

using namespace std;

static GLfloat cam_rot[2] = { 0.0, 0.0 }, a;

void init(void)
{
	GLfloat Karel3D[9], x, y, z;
	unsigned int j, pocet = 0;
	ifstream soubor;

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	
	glNewList(1, GL_COMPILE);
	glColor3f(1.0f, 1.0f, 0.0f);
	soubor.open("Karel3D.raw", ios::binary);
	while (!soubor.eof()) {
		soubor >> Karel3D[pocet % 9];
		
		if (!(++pocet % 9)) {
			glBegin(GL_LINE_LOOP);	
			for(j = 0; j < 3; j++) {
				x = Karel3D[j*3];
				y = Karel3D[j*3 + 1];
				z = Karel3D[j*3 + 2];
				glVertex3f(x,y,z);
			}
			glEnd();
		}
	}
	soubor.close();
	glEndList();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glLoadIdentity();
/*	gluLookAt(20*cosf(cam_rot[1])*sinf(cam_rot[1]),
			20*sinf(cam_rot[0]),
			20*cosf(cam_rot[0])*sinf(cam_rot[1]), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0);*/
	gluLookAt(20*sinf(cam_rot[1]),
			20*sinf(cam_rot[0])*cosf(cam_rot[1]),
			20*cosf(cam_rot[0])*cosf(cam_rot[1]),
			0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
/*	gluLookAt(20*sinf(cam_rot[0])*cosf(cam_rot[1]),
			20*cosf(cam_rot[0])*cosf(cam_rot[1]),
			20*sinf(cam_rot[1]), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0);*/

	glRotatef(10, a, 0, 0);
	glCallList(1);

	glColor3f(1.0, 1.0, 1.0);
	glFlush();
	glutSwapBuffers();
	
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	        //(Left, Right, Bottom, Top, Near, Far);
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 50.0);
	glMatrixMode(GL_MODELVIEW);
}

void karel_mouse(int button, int state, int x, int y)
{
	static int ox, oy;

	if (state == 0) {
		ox = x;
		oy = y;
	} else {
		cam_rot[1] += (GLfloat)((ox - x))/500.0;
		cam_rot[0] += (GLfloat)((y - oy))/500.0;
		glutPostRedisplay();
	}
}

void karel_keyb(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		cam_rot[0] -= .03;
		break;
	case 'a':
		cam_rot[0] += .03;
		break;
	case 'w':
		cam_rot[1] -= .03;
		break;
	case 'e':
		cam_rot[1] += .03;
		break;
	case 'm':
		a+= 1;
		break;
	case 27:
		exit(0);
	}
	glutPostRedisplay();
}

int main(int argc,char **argv)
{
		
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(karel_mouse);
	glutKeyboardFunc(karel_keyb);
	glutMainLoop();

	return 0;
}

