// Svetlo.cpp : Defines the entry point for the console application.
//


#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

static GLfloat n[3] = { 1.0f, 1.0f, 0.0f };

void init(void) 
{
	GLfloat LightAmbient[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
/*	GLfloat LightAmbient[]= { 0.0f, 0.0f, 1.0f, 1.0f };
	GLfloat LightDiffuse[]= { 0.0f, 1.0f, 1.0f, 1.0f };*/
	GLfloat LightPosition[]= { 0.0f, 2.0f, 2.0f, 1.0f };
	GLfloat Mata[]= { 0.24725f, 0.1995f, 0.0745f, 1.0f };
	GLfloat Matd[]= { 0.75164f, 0.60648f, 0.22648f, 1.0 };
	GLfloat Mats[]= { 51.2f };

	glShadeModel(GL_SMOOTH);
	glClearColor (0.0, 0.0, 0.0, 0.0);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Mata);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Matd);
	glMaterialfv(GL_FRONT, GL_SHININESS, Mats);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
}

void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//   glutSolidSphere(1.0, 50, 50);
   glBegin(GL_TRIANGLES);
   glNormal3f(n[0], n[1], n[2]);
   glVertex3f(0.0f, 0.5f, 1.0f);
   glVertex3f(1.0f, 0.5f, 1.0f);
   glVertex3f(0.0f, 0.0f, 1.0f);
   glEnd();
   glBegin(GL_LINES);
   glVertex3f(n[0], n[1], n[2]);
   glVertex3f(0.0f, 0.0f, 0.0f);
   glEnd();
   glFlush();
   glutSwapBuffers();
}

void reshape(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      glOrtho (-1.5, 1.5, -1.5*(GLfloat)h/(GLfloat)w,
         1.5*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
   else
      glOrtho (-1.5*(GLfloat)w/(GLfloat)h,
         1.5*(GLfloat)w/(GLfloat)h, -1.5, 1.5, -10.0, 10.0);
   glRotatef(1.3f, 1.0f, 0.0f, 0.0f);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

static void pv112_4_timer(int value)
{
	static float a;

	n[0] = sinf(a);
	n[2] = cosf(a);
	a += 0.1;
	glutPostRedisplay();
	glutTimerFunc(100, pv112_4_timer, 0);
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutTimerFunc(100, pv112_4_timer, 0);
   glutMainLoop();
   return 0;
}
