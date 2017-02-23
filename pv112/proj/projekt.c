/*
 * project.c: main project file
 * Copyright (C) 2006 Jiri Slaby
 * Author(s): Jiri Slaby <jirislaby@gmail.com>
 *
 * LICENSE TERMS
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Company nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'', AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COMPANY OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "data.h"

#define DB_JITTER	0.05
#define DB_EQ(a, b, c)	(fabs((a) - (b)) < (c) - DB_JITTER)
#define DB_LT(a, b)	((a) < (b) + DB_JITTER)
#define DB_GT(a, b)	((a) > (b) - DB_JITTER)

#define KAR_XSTEP		0.4
#define KAR_ZSTEP		0.1
#define KAR_ZMAX		1.0
#define KAR_LIMB_STEP		5	/* degrees */

#define PROJ_DIR_1ST		1
#define PROJ_DIR_2ND		2
#define PROJ_DIR_BOTH		(PROJ_DIR_1ST|PROJ_DIR_2ND)
#define PROJ_DIR_DIR		4
#define PROJ_DIR_DONE		8

static char myname[] = "jiri slaby";
static char noname[] = "bily jasir";

struct letinfo {
	char ch;
	unsigned int pos, dpos;

	GLdouble x, z;
};

static struct {
	struct letinfo *letinfo;
	unsigned int mv_now, mv_dir, animate;

	char *mvname;

	GLint rota, rotb, rotlimb;
	GLuint text;
	GLdouble zoom, mvlr, kar_x, kar_z, blend;
} proj_sc;

static void proj_timer(int value);

static inline void
compute_normal(const struct triangle *t)
{
	struct pos v1, v2;

	memcpy(&v1, &t->a, sizeof(v1));
	memcpy(&v2, &t->a, sizeof(v1));

	v1.x -= t->b.x;
	v1.y -= t->b.y;
	v1.z -= t->b.z;
	v2.x -= t->c.x;
	v2.y -= t->c.y;
	v2.z -= t->c.z;

	glNormal3f(v2.y * v1.z - v1.y * v2.z, v2.z * v1.x - v1.z * v2.x,
			v2.x * v1.y - v1.x * v2.y);
}

static inline unsigned int
proj_chartolist(char c)
{
	unsigned int a;

	for (a = FSTLIST; a <= LSTLIST; a++)
		if (lets[a].ch == toupper(c))
			break;

	return a > LSTLIST ? ~0 : lets[a].id;
}

static void
proj_drawface(void)
{
	glBegin(GL_QUADS);
	glNormal3f(0.0,  0.0, -1.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0,  0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0,  0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0,  0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  0.0);
	glEnd();
}

static inline void
proj_draw2faces(void)
{
	glPushMatrix();
	glTranslatef(0.0, 0.0, -1.0);
	proj_drawface();
	glTranslatef(0.0, 0.0, 2.0);
	glRotatef(180, 0.0, 1.0, 0.0);
	proj_drawface();
	glPopMatrix();
}

static void
proj_drawcube(void)
{
	glPushMatrix();
	glutSolidCube(2.0);
/*	proj_draw2faces();
	glRotatef(90, 0.0, 1.0, 0.0);
	proj_draw2faces();
	glRotatef(90, 1.0, 0.0, 0.0);
	proj_draw2faces();*/
	glPopMatrix();
}

static void
robot_drawhands(void)
{
	unsigned int a;

	for (a = 0; a < 2; a++) {
		glPushMatrix();
		glTranslatef(a ? -1.1 : 1.1, 1.0, 0.0);
		glRotatef(a ? -10 : 30, 0.0, 0.0, 1.0);
		if (!a)
			glRotatef(proj_sc.rotlimb, 1.0, 0.0, 0.0);
		glTranslatef(0.0, -0.7, 0.0);

		glPushMatrix();
		glScalef(0.2, 0.6, 0.2);
		proj_drawcube();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, -0.7, 0.0);
		glRotatef(30, 1.0, 0.0, 0.0);
		glTranslatef(0.0, -0.4, 0.0);
		glScalef(0.2, 0.5, 0.2);
		proj_drawcube();
		glPopMatrix();

		glPopMatrix();
	}
}

static void
robot_drawlegs(void)
{
	glPushMatrix();
	glTranslatef(0.0, -1.7, 0.0);
	glRotatef(20, 1.0, 0.0, 0.0);
	glScalef(0.3, 0.7, 0.3);
	glTranslatef(-1.5, 0.0, 0.0);
	proj_drawcube();
	glTranslatef(3.0, 0.0, 0.0);
	proj_drawcube();
	glPopMatrix();
}

static void
robot_drawchest(void)
{
	glPushMatrix();
	glScalef(1.0, 1.3, 0.6);
	proj_drawcube();
	glPopMatrix();
}

static void
robot_drawhead(void)
{
	glPushMatrix();
	glTranslatef(0.0, 1.7, 0.0);
	glRotatef(45, 0.0, 0.0, 1.0);
	glScalef(0.4, 0.4, 0.3);
	proj_drawcube();
	glPopMatrix();
}

static void
proj_display(void)
{
	struct letinfo *li = proj_sc.letinfo;
	unsigned int a;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, -13.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glTranslatef(proj_sc.mvlr - 1.0, -1.5, -proj_sc.zoom);
	
	glRotatef(proj_sc.rota + 12, 0.0, 1.0, 0.0);
	glRotatef(proj_sc.rotb - 10, 1.0, 0.0, 0.0);

	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPushMatrix(); /* name */
	glColor3f(0.3, 0.5, 0.9);
	glTranslatef(-LET_WID * sizeof(myname) / 2, 0.0, 0.0);
	for (a = 0; a < sizeof(myname); a++, li++)
		if (li->ch != ' ') {
			glPushMatrix(); /* name */
			glTranslatef(li->x, 0.0, li->z);
		
			glCallList(proj_chartolist(li->ch));
			glPopMatrix();
		}
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT2);
	glBindTexture(GL_TEXTURE_2D, proj_sc.text);

	glPushMatrix(); /* robot */
	glTranslatef(-LET_WID * sizeof(myname) / 2 + proj_sc.kar_x, 0.0,
			2.4 + proj_sc.kar_z);
	robot_drawhands();
	robot_drawlegs();
	robot_drawchest();
	robot_drawhead();
	glPopMatrix();

	glEnable(GL_LIGHT2);
	glTranslatef(-1.0, -1.6, 0.6);
	glScalef(LET_WID * sizeof(myname) / 2, 1.0, 1.5);
	glPushMatrix();
	glTranslatef(0.0, -1.0, 1.0);
	proj_drawface();
	glTranslatef(0.0, 0.0, -2.0);
	proj_drawface();
	glPopMatrix();
	glRotatef(90, 1.0, 0.0, 0.0);
	proj_drawface();
	glTranslatef(0.0, 2.0, 2.0);
	proj_drawface();

	glFlush();
	glutSwapBuffers();
}

static void
proj_reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glFrustum(1.8, -1.8, -1.8 * h / w, 1.8 * h / w, 2.0, 30.0);
	else
		glFrustum(1.8 * w / h, -1.8 * w / h, -1.8, 1.8, 2.0, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void
proj_keyhack(unsigned char key, int x, int y)
{
	switch (key) {
	case 'j':
		proj_sc.rota -= 5;
		break;
	case 'k':
		proj_sc.rota += 5;
		break;
	case 'i':
		proj_sc.rotb -= 5;
		break;
	case ',':
		proj_sc.rotb += 5;
		break;
	case '-':
		proj_sc.zoom -= 0.3;
		break;
	case '+':
		proj_sc.zoom += 0.3;
		break;
	case 'b':
		proj_sc.mvlr -= 0.8;
		break;
	case 'v':
		proj_sc.mvlr += 0.8;
		break;
	case 'x':
		proj_sc.blend -= 0.1;
		break;
	case 'c':
		proj_sc.blend += 0.1;
		printf("%f\n", proj_sc.blend);
		break;
	case 'f':
		proj_sc.animate = 0;
		break;
	case 'g':
		if (!proj_sc.animate) {
			glutTimerFunc(50, proj_timer, 0);
			proj_sc.animate++;
		}
		break;
	case 'q':
	case 27:
		proj_sc.animate = 0;
		free(proj_sc.letinfo);
		free(proj_sc.mvname);
		
		glutDestroyWindow(glutGetWindow());

		exit(0);
	default:
		printf("unknown key with code: %d\n", key);
	}
	glutPostRedisplay();
}

static int
proj_1st_diff(void)
{
	char *c = proj_sc.mvname, *d = proj_sc.mv_dir & PROJ_DIR_DIR ? myname :
		noname;

	while (*c)
		if (*c++ != *d++)
		       break;

	return (*c ? (c - proj_sc.mvname - 1) : -1);
}

static unsigned int
proj_dest(const unsigned int from)
{
	char *c = proj_sc.mvname, *d = proj_sc.mv_dir & PROJ_DIR_DIR ? myname :
		noname;

	while (*c) {
		if (*c == d[from] && d[c - proj_sc.mvname] != *c)
			break;
		c++;
	}

	return (c - proj_sc.mvname);
}

static unsigned int
proj_postolet(const unsigned int pos)
{
	unsigned int a;

	for (a = 0; a < sizeof(myname); a++)
		if (proj_sc.letinfo[a].pos == pos)
			break;

	return (a);
}

static int
proj_kar_handup(void)
{
	if (proj_sc.rotlimb >= 70)
		return (1); /* fast path */

	proj_sc.rotlimb += KAR_LIMB_STEP;

	return (0);
}

static int
proj_kar_handdown(void)
{
	if (proj_sc.rotlimb == 0)
		return (1); /* fast path */

	proj_sc.rotlimb -= KAR_LIMB_STEP;

	return (0);
}

static void
proj_decide_let(struct letinfo *li)
{
	int diff;

	if (proj_sc.mv_dir & PROJ_DIR_2ND) {
		if (!proj_kar_handdown())
			return; /* no work for this time */
		proj_sc.mvname[li->dpos] = li->ch;
		diff = proj_1st_diff();
		if (diff == -1)
			proj_sc.mv_dir &= ~PROJ_DIR_BOTH;
		else {
			proj_sc.mv_now = proj_postolet(diff);
			proj_sc.letinfo[proj_sc.mv_now].dpos = proj_dest(diff);
			proj_sc.mv_dir ^= PROJ_DIR_BOTH;
		}
		li->pos = li->dpos;
	} else {
		proj_sc.mvname[li->dpos] = li->ch;
		proj_sc.mv_now= proj_postolet(li->dpos);
		proj_sc.letinfo[proj_sc.mv_now].dpos = li->pos;
		li->pos = li->dpos;
		proj_sc.mv_dir ^= PROJ_DIR_BOTH;
	}
}

static int
proj_kar_move(const double where, const double liz)
{
	if (liz > DB_JITTER || (proj_sc.kar_z < DB_JITTER &&
			DB_EQ(proj_sc.kar_x, where, KAR_XSTEP)))
		return (proj_kar_handup()); /* fast path */

	if (DB_EQ(proj_sc.kar_x, where, KAR_XSTEP)) {
		proj_sc.kar_z -= KAR_ZSTEP;
		return (0);
	}

	if (proj_sc.kar_z < KAR_ZMAX - DB_JITTER) {
		proj_sc.kar_z += KAR_ZSTEP;
		return (0);
	}
	proj_sc.kar_x += proj_sc.kar_x < where - DB_JITTER ? KAR_XSTEP :
		-KAR_XSTEP;

	return (0);
}

static void
proj_timer(int value)
{
	struct letinfo *li;
	int diff;

	if (!proj_sc.mv_dir) /* init */
		proj_sc.mv_dir = PROJ_DIR_DONE | PROJ_DIR_1ST;
	else if (proj_sc.mv_dir & PROJ_DIR_BOTH) {
		if (proj_sc.mv_now == ~0) {
			proj_sc.mv_now = proj_1st_diff();
			proj_sc.letinfo[proj_sc.mv_now].dpos =
				proj_dest(proj_sc.mv_now);
		}
		li = &proj_sc.letinfo[proj_sc.mv_now];
		if ((DB_GT(li->x, LET_WID * li->dpos) && li->pos < li->dpos) ||
				(DB_LT(li->x, LET_WID * li->dpos) &&
				li->pos > li->dpos)) {
			if (li->z > 0.0 + DB_JITTER) /* move back to the raw */
				proj_sc.kar_z = li->z -= KAR_ZSTEP;
			else /* previous letter has been done */
				proj_decide_let(li);
		} else if (DB_GT(li->z, KAR_ZMAX)) /* move horizontally */
			proj_sc.kar_x =	li->x += li->pos < li->dpos ?
					KAR_XSTEP : -KAR_XSTEP;
		else /* move from the raw */
			if (proj_kar_move(li->x, li->z))
				proj_sc.kar_z = li->z += KAR_ZSTEP;
	} else if (proj_sc.mv_dir & PROJ_DIR_DONE) {
		proj_sc.mv_dir ^= PROJ_DIR_DIR;
		proj_sc.mv_dir |= PROJ_DIR_1ST;
		diff = proj_1st_diff();
		proj_sc.mv_now = proj_postolet(diff);
		proj_sc.letinfo[proj_sc.mv_now].dpos = proj_dest(diff);
		glutTimerFunc(2000, proj_timer, 0);
		return;
	}

	glutPostRedisplay();
	if (proj_sc.animate)
		glutTimerFunc(50, proj_timer, 0);
}

static void
proj_create_list(unsigned int idx)
{
	struct triangle *t;

	glNewList(idx, GL_COMPILE);
	glScalef(0.01, 0.01, 0.006);
	glBegin(GL_TRIANGLES);
	for (t = lets[idx].t; t < lets[idx].t + lets[idx].s / sizeof(*t); t++) {
		compute_normal(t);
		glVertex3f(t->a.x, t->a.y, t->a.z);
		glVertex3f(t->b.x, t->b.y, t->b.z);
		glVertex3f(t->c.x, t->c.y, t->c.z);
	};
	glEnd();
	glEndList();
}

static void
proj_init_lights(void)
{
	GLfloat amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat amb_dsk[] = { 0.4f, 0.7f, 0.9f, 1.0f };
	GLfloat dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat dif_dsk[] = { 0.0f, 0.0f, 0.1f, 1.0f };
	GLfloat pos[] = { 0.0f, 5.0f, 5.0f, 5.0f };
	GLfloat mata[] = { 0.25f, 0.25f, 0.25f, 1.0f }; /* chrome */
	GLfloat matd[] = { 0.4f, 0.4f, 0.4f, 0.1f };
	GLfloat mats[] = { 76.8f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mata);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matd);
	glMaterialfv(GL_FRONT, GL_SHININESS, mats);

	glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT2, GL_AMBIENT, amb_dsk);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, dif_dsk);
	glLightfv(GL_LIGHT1, GL_POSITION, pos);
	pos[1] = 0.0f;
	glLightfv(GL_LIGHT2, GL_POSITION, pos);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
}

static void
proj_init_texts(void)
{
	glGenTextures(1, &proj_sc.text);
	glBindTexture(GL_TEXTURE_2D, proj_sc.text);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE,
			proj_text_data);
	glEnable(GL_TEXTURE_2D);
}

static void
proj_init(void) 
{
	unsigned int a;

	proj_init_lights();
	proj_init_texts();

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);

	for (a = FSTLIST; a <= LSTLIST; a++)
		proj_create_list(a);
}

int
main(int argc, char **argv)
{
	unsigned int a;

	do { /* names must have same length */
		enum { assertion__ = 1 / (sizeof(myname) == sizeof(noname)) };
	} while (0);

	proj_sc.mv_now = ~0;
	proj_sc.kar_z = KAR_ZMAX;
	proj_sc.kar_x = LET_WID * sizeof(myname) / 2;
	proj_sc.mvname = strdup(myname);
	if (!proj_sc.mvname) {
		perror("strdup");
		goto fail_0;
	}

	proj_sc.letinfo = malloc(strlen(myname) * sizeof(*proj_sc.letinfo));
	if (!proj_sc.letinfo) {
		perror("malloc letters info");
		goto fail_1;
	}
	memset(proj_sc.letinfo, 0, strlen(myname) * sizeof(*proj_sc.letinfo));

	for (a = 0; a < strlen(myname); a++) {
		proj_sc.letinfo[a].ch = myname[a];
		proj_sc.letinfo[a].pos = a;
		proj_sc.letinfo[a].x = LET_WID * a;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600); 
	glutInitWindowPosition(100, 100);
	glutCreateWindow(*argv);
	proj_init();
	glutDisplayFunc(proj_display); 
	glutReshapeFunc(proj_reshape);
	glutKeyboardFunc(proj_keyhack);
	glutTimerFunc(100, proj_timer, 0);
	proj_sc.animate++;
	glutMainLoop();

	return (0);
fail_1:
	free(proj_sc.mvname);	
fail_0:
	return (1);
}
