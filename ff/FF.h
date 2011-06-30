/**
  *  VRECKO force feedback device controller plugin.
  *
  *  Copyright (c) 2004,2005 Jiri Slaby <xslaby@fi.muni.cz>
  */

#ifndef FF_H
#define FF_H

#include <set>
#include <map>

#include <osg/Vec3>

#ifdef WIN32
# include "ff/windows.h"
#else
# include "ff/linux.h"
#endif

namespace vrecko
{

class /*VRECKO_EXPORT*/ ForceFeedback : public LL_ForceFeedback
{
    private:
	ForceFeedback() {};

	void dispatch();
        virtual void putMessage(const unsigned int, const int);
        virtual void putMessage(const char*);

    public:
	ForceFeedback(const char*);

	void processEvent(std::string, void*);
	void update(void) { LL_ForceFeedback::update(); dispatch(); };
};

}

using namespace vrecko;

inline ForceFeedback::ForceFeedback(const char* portName):
	LL_ForceFeedback(portName)
{
//    device_type = DEVICE_FORCEFEEDBACK;

//    addOutput("Button", INT, "\0");
    addOutput("Position", VEC3, "\0");
    addOutput("Rotation", VEC3, "\0");

    addInput("FFData", VEC3, "\0");
    addInput("FFByNamePlay", STRING, "\0");
    addInput("FFByNameStop", STRING, "\0");
}

inline void ForceFeedback::processEvent(std::string inputName, void *value)
{
    if (!strcmp(inputName.c_str(), "FFByNamePlay"))
    {
	mapIter = effectsMap.find(*(std::string*)value);
	if (mapIter != effectsMap.end()) mapIter->second->play();
    } else if (!strcmp(inputName.c_str(), "FFByNameStop"))
    {
	mapIter = effectsMap.find(*(std::string*)value);
	if (mapIter != effectsMap.end()) mapIter->second->stop();
    } else LL_ForceFeedback::processEvent(inputName, value);
}

inline void ForceFeedback::dispatch()
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

inline void ForceFeedback::putMessage(const unsigned int but, const int val)
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

inline void ForceFeedback::putMessage(const char* msg)
{
/*    char vystup[32];

    int len = sprintf(vystup, "De|%d#", id);*/

    fprintf(stderr, "%s\n", msg);
/*    memcpy(vystup + len, "Device", 7);
    std::string *s = new std::string(msg);
    event_dispatcher->reportEvent(vystup, BaseClass::STRING, (void*)s);*/
}

#endif
