/**
  *  VRECKO force feedback device controller plugin.
  *
  *  Copyright (c) 2004-2006 Jiri Slaby <jirislaby@gmail.com>
  */

#include <set>
#include <map>

#include <osg/Vec3>

#include "platform.h"
#include "FF.h"

using namespace vrecko;

void ForceFeedback::dispatch()
{
/*    char vystup[32];
    osg::Vec3 *vec3;
    int *i, a;

    int len = sprintf(vystup, "De|%d#", id);

    memcpy(vystup + len, "Position", 9);
    vec3 = new osg::Vec3();
    *vec3 = pos;
    event_dispatcher->reportEvent(vystup, BaseClass::VEC3, (void*)vec3);

    memcpy(vystup + len, "Rotation", 9);
    vec3 = new osg::Vec3();
    *vec3 = rot;
    event_dispatcher->reportEvent(vystup, BaseClass::VEC3, (void*)vec3);

    memcpy(vystup + len, "Slider0", 7);
    for (a=0; a<nSliders; a++)
    {
	i = new int(sliders[a]);
	event_dispatcher->reportEvent(vystup, BaseClass::INT, (void*)i);
	vystup[11]++;
    }

    memcpy(vystup + len, "POV0", 5);
    for (a=0; a<nPOVs; a++)
    {
	i = new int(POVs[a]);
	event_dispatcher->reportEvent(vystup, BaseClass::INT, (void*)i);
	vystup[8]++;
    }

*/
}

void ForceFeedback::putMessage(const unsigned int but, const int val)
{
/*
    char vystup[32];

    int len = sprintf(vystup, "De|%d#", id);
*/
    fprintf(stderr, "Button%02d - %d\n", but, val); /*
    sprintf(vystup + len, "Button%02d", but);
    int *i = new int(val);
    event_dispatcher->reportEvent(vystup, BaseClass::BOOL, (void*)i);
*/
}

void ForceFeedback::putMessage(const char* msg)
{
/*    char vystup[32];

    int len = sprintf(vystup, "De|%d#", id);*/

    fprintf(stderr, "%s\n", msg);
/*    memcpy(vystup + len, "Device", 7);
    std::string *s = new std::string(msg);
    event_dispatcher->reportEvent(vystup, BaseClass::STRING, (void*)s);*/
}
