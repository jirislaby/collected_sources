#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <GL/glew.h>
#include <GL/glut.h>

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutCreateWindow("GLEW Test");
	GLenum ret = glewInit();
	if (GLEW_OK != ret)
		  errx(1, "Error: %s\n", glewGetErrorString(ret));

	printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	sleep(3);

	return 0;
}

