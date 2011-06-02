/*
 * ffcfstress.c
 *
 * Force Feedback: Constant Force Stress Test
 *
 * Copyright (C) 2001 Oliver Hamann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/input.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>


/* Helper for testing large bit masks */
#define TEST_BIT(bit,bits) (((bits[bit>>5]>>(bit&0x1f))&1)!=0)


/* Default values for the options */
#define DEFAULT_DEVICE_NAME "/dev/input/event3"
#define DEFAULT_UPDATE_RATE      25.0
#define DEFAULT_MOTION_FREQUENCY  0.1
#define DEFAULT_MOTION_AMPLITUDE  1.0
#define DEFAULT_SPRING_STRENGTH   1.0


/* Options */
const char * device_name = DEFAULT_DEVICE_NAME;
double update_rate       = DEFAULT_UPDATE_RATE;
double motion_frequency  = DEFAULT_MOTION_FREQUENCY;
double motion_amplitude  = DEFAULT_MOTION_AMPLITUDE;
double spring_strength   = DEFAULT_SPRING_STRENGTH;
int stop_and_play = 0;  /* Stop-upload-play effects instead of updating */


/* Global variables about the initialized device */
int device_handle;
int axis_code, axis_min, axis_max;
struct ff_effect effect;

/* Initialize device, create constant force effect */
void init_device()
{
	unsigned long key_bits[32],abs_bits[32],ff_bits[32];
	struct input_event event;
	int valbuf[16];

	/* Open event device with write permission */
	device_handle = open(device_name,O_RDWR|O_NONBLOCK);
	if (device_handle<0) {
		fprintf(stderr,"ERROR: can not open %s (%s) [%s:%d]\n",
		        device_name,strerror(errno),__FILE__,__LINE__);
		exit(1);
	}

	/* Which axis is the x-axis? */
	axis_code=ABS_X;

	/* get axis value range */
	if (ioctl(device_handle,EVIOCGABS(axis_code),valbuf)<0) {
		fprintf(stderr,"ERROR: can not get axis value range (%s) [%s:%d]\n",
		        strerror(errno),__FILE__,__LINE__);
		exit(1);
	}
	axis_min=valbuf[1];
	axis_max=valbuf[2];
	if (axis_min>=axis_max) {
		fprintf(stderr,"ERROR: bad axis value range (%d,%d) [%s:%d]\n",
		        axis_min,axis_max,__FILE__,__LINE__);
		exit(1);
	}

	/* Switch off auto centering */
	memset(&event,0,sizeof(event));
	event.type=EV_FF;
	event.code=FF_AUTOCENTER;
	event.value=0;
	if (write(device_handle,&event,sizeof(event))!=sizeof(event)) {
		fprintf(stderr,"ERROR: failed to disable auto centering (%s) [%s:%d]\n",
		        strerror(errno),__FILE__,__LINE__);
		exit(1);
	}

	/* Initialize constant force effect */
	memset(&effect,0,sizeof(effect));
	effect.type=FF_CONSTANT;
	effect.id=-1;
	effect.trigger.button=0;
	effect.trigger.interval=0;
	effect.replay.length=0xffff;
	effect.replay.delay=0;
	effect.u.constant.level=0;
	effect.direction=0xC000;
	effect.u.constant.envelope.attack_length=0;
	effect.u.constant.envelope.attack_level=0;
	effect.u.constant.envelope.fade_length=0;
	effect.u.constant.envelope.fade_level=0;

	/* Upload effect */
	if (ioctl(device_handle,EVIOCSFF,&effect)==-1) {
		fprintf(stderr,"ERROR: uploading effect failed (%s) [%s:%d]\n",
		        strerror(errno),__FILE__,__LINE__);
		exit(1);
	}

	/* Start effect */
	memset(&event,0,sizeof(event));
	event.type=EV_FF;
	event.code=effect.id;
	event.value=1;
	if (write(device_handle,&event,sizeof(event))!=sizeof(event)) {
		fprintf(stderr,"ERROR: starting effect failed (%s) [%s:%d]\n",
		        strerror(errno),__FILE__,__LINE__);
		exit(1);
	}
}


/* update the device: set force and query joystick position */
void update_device(double force, double * position)
{
	struct input_event event;

	/* Delete effect */
/*	if (stop_and_play && effect.id!=-1) {
		if (ioctl(device_handle,EVIOCRMFF,effect.id)==-1) {
			fprintf(stderr,"ERROR: removing effect failed (%s) [%s:%d]\n",
			        strerror(errno),__FILE__,__LINE__);
			exit(1);
		}
		effect.id=-1;
	}*/

	/* Set force */
	if (force>1.0) force=1.0;
	if (force<-1.0) force=1.0;
	effect.u.constant.level=(short)(force*32767.0); /* only to be safe */
	effect.direction=0xC000;
	effect.u.constant.envelope.attack_level=(short)(force*32767.0); /* this one counts! */
	effect.u.constant.envelope.fade_level=(short)(force*32767.0); /* only to be safe */

	/* Upload effect */
	if (ioctl(device_handle,EVIOCSFF,&effect)==-1) {
		perror("upload effect");
		/* We do not exit here. Indeed, too frequent updates may be
		 * refused, but that is not a fatal error */
	}

	/* Start effect */
	if (stop_and_play && effect.id!=-1) {
		memset(&event,0,sizeof(event));
		event.type=EV_FF;
		event.code=effect.id;
		event.value=1;
		if (write(device_handle,&event,sizeof(event))!=sizeof(event)) {
			fprintf(stderr,"ERROR: re-starting effect failed (%s) [%s:%d]\n",
			        strerror(errno),__FILE__,__LINE__);
			exit(1);
		}
	}

	/* Get events */
	while (read(device_handle,&event,sizeof(event))==sizeof(event)) {
		if (event.type==EV_ABS && event.code==axis_code) {
			*position=((double)(((short)event.value)-axis_min))*2.0/(axis_max-axis_min)-1.0;
			if (*position>1.0) *position=1.0;
			else if (*position<-1.0) *position=-1.0;
		}
	}
}


/* little helper to print a graph bar from a value */
void fprint_bar(FILE * file, double value, int radius)
{
	int i,c;

	for (i=0; i<radius*2+1; i++) {
		if (i==radius) c='|';
		else if ((i<radius && value*radius<i-radius+0.25) ||
		         (i>radius && value*radius>i-radius-0.25)) c='*';
		else if ((i<radius && value*radius<i-radius+0.75) ||
		         (i>radius && value*radius>i-radius-0.75)) c='+';
		else if (i==0) c='<';
		else if (i==radius*2) c='>';
		else c='-';
		fputc(c,file);
	}
}


/* main: perform the spring simulation */
int main(int argc, char * argv[])
{
	double time,position,center,force;

	/* Initialize device, create constant force effect */
	init_device();

	/* Print header */
	printf("\n        position                   center                     force\n");

	/* For ever */
	for (position=0, time=0;; time+=1.0/update_rate) {

		/* Spring center oscillates */
		center = sin( time * 2 * M_PI * motion_frequency ) * motion_amplitude;

		/* Calculate spring force */
		force = ( center - position ) * spring_strength;
		if (force >  1.0) force =  1.0;
		if (force < -1.0) force = -1.0;

		/* Print graph bars */
		printf("\r");
		fprint_bar(stdout,position,12);
		printf(" ");
		fprint_bar(stdout,center,12);
		printf(" ");
		fprint_bar(stdout,force,12);
		fflush(stdout);

		/* Set force and ask for joystick position */
		update_device(force,&position);

		/* Next time... */
		usleep((unsigned long)(1000000.0/update_rate));

	}
}
