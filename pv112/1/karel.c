// PV112_2.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include <stdio.h>
#include <glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

GLuint wx = 0, wy = 0, karel = 1; // Velikost okna
GLuint window, world;		// Okna
GLuint Bod_x = 0, Bod_y = 0;	// Poloha bodu po kliknuti misi
GLboolean bod = GL_FALSE, stisknuto = GL_FALSE;	// Pro bod generovany mysi

void init(void) 
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}
void display(void) 
{
	float noha = 0, ruka = 1, wxf = (float)wx, wyf = (float)wy;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	glVertex2f(wxf / 3, wyf / 4); /* telo */
	glVertex2f(wxf / 2, wyf / 3 * 2);
	glVertex2f(wxf / 3 * 2, wyf / 4);
	glVertex2f(wxf / 3, 0); /* hlava */
	glVertex2f(wxf / 2, wyf / 4 + 5);
	glVertex2f(wxf / 3 * 2, 0);

	if (karel % 2) {
		noha = 30;
		ruka = 2;
	}

	glVertex2f(wxf / 9 * 4 - 10, wyf / 3 * 2 - 20); /* lnoha */
	glVertex2f(wxf / 9 * 4 - noha, wyf);
	glVertex2f(wxf / 9 * 4 + 10, wyf / 3 * 2 - 20);
	glVertex2f(wxf / 9 * 5 - 10, wyf / 3 * 2 - 20); /* rnoha */
	glVertex2f(wxf / 9 * 5 + noha, wyf);
	glVertex2f(wxf / 9 * 5 + 10, wyf / 3 * 2 - 20);
	glVertex2f(0, wyf / 3 * ruka); /* lruka */
	glVertex2f(wxf / 9 * 4, wyf / 3 + 12);
	glVertex2f(wxf / 9 * 4, wyf / 3 - 12);
	glVertex2f(wxf, wyf / 3 * ruka); /* rruka */
	glVertex2f(wxf / 9 * 5, wyf / 3 - 12);
	glVertex2f(wxf / 9 * 5, wyf / 3 + 12);
	glEnd();
	glFlush();
	glutSwapBuffers();
}
void reshape(int width, int height) 
{
	wx = width;
	wy = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouse(int button, int state, int x, int y) 
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN
			&& stisknuto == GL_FALSE) {
		stisknuto = GL_TRUE;
		bod = !bod;
		wx = x;
		wy = y;
		glutPostRedisplay();
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		stisknuto = GL_FALSE;
}
void keyboard(unsigned char key, int x, int y) 
{
	switch (key) {
	case 'p':
		karel = 1;
		// Funkce spoustene klavesou 'p'
		glutPostRedisplay();
		break;
	case 'r':
		karel = 0;
		// Funkce spoustene klavesou 'r'
		glutPostRedisplay();
		break;
	case 'q':
	case 27:
		exit(0);
	}
}
void menu(int value) 
{
	switch (value) {
	case '1':
		
		    // Funkce prvni volby
		    glutPostRedisplay();
		break;
	case '2':
		// Funkce druhe volby
		glutPostRedisplay();
		break;
	}
}

void karel_timer(int value)
{
	karel++;
	glutPostRedisplay();
	glutTimerFunc(500, karel_timer, 0);
}

int main(int argc, char **argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(256, 256);
	glutInitWindowPosition(100, 100);
	
	// hlavni okno se jmenuje 'window'
	window = glutCreateWindow(*argv);
	init();
	glutTimerFunc(500, karel_timer, 0);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutCreateMenu(menu);
	glutAddMenuEntry("Hlavni menu", 0);
	glutAddMenuEntry("", 0);
	glutAddMenuEntry("Volba 1", '1');
	glutAddMenuEntry("Volba 2", '2');
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();

	return 0;
}
