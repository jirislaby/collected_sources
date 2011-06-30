/**
 *  VRECKO force feedback device controller plugin.
 *
 *  Copyright (c) 2004-2006 Jiri Slaby <jirislaby@gmail.com>
 */

#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#include <vrecko/Device.h>
#include <osg/Vec3>

#include "linux.h"

using namespace vrecko;

void LL_ForceFeedback::eff(int a)
{
    struct ff_effect eff;
    struct input_event event;
    static double x = 0.0;

    x += (double)(sliders[0] << 6);
    if (x>0xffff) x = 0.0;

    memset(&eff, 0, sizeof(eff));
    eff.type         = FF_CONSTANT;
    eff.id=cid;
    eff.direction    = (__u16)x;
    eff.replay.length=0xffff;
    eff.u.constant.level=32767;

    if (ioctl(portDes, EVIOCSFF, &eff) < 0) perror("Uploading effect");

    memset(&event,0,sizeof(event));
    event.type=EV_FF;
    event.code=cid;
    event.value=1;
    if (write(portDes, &event, sizeof(event)) != sizeof(event)) perror("Starting effect");
}

LL_ForceFeedback::LL_ForceFeedback(const char* portName):
    Device(portName),
    portDes(-1), EUnload(-10), EOnCreate(1),
    pos(0, 0, 0), rot(0, 0, 0), nPOVs(0),
    nSliders(0), nAxes(0)
{
    port = new char[strlen(portName)+1];
    strcpy(port, portName);
}

LL_ForceFeedback::~LL_ForceFeedback()
{
    delete[] port;
}

#define RET(x) { close(portDes); return (x); }

int LL_ForceFeedback::openDevice()
{
    struct input_event event;
    struct ff_effect eff;
    unsigned char bitMask[EV_MAX/8 + 1];

    if ((portDes = open(port, O_NDELAY | O_RDWR)) < 0) return errno;

    // read supported event types
    if (ioctl(portDes, EVIOCGBIT(0, EV_MAX), bitMask) < 0) RET(errno);

/*    for (int a = 0; a < (int)sizeof(bitMask); a++)
    {
	for (int b = 1; b < 128; b = b << 1)
	    printf("%c", !!(bitMask[a] & b) + '0');
	putchar(' ');
    }
    puts("");*/

    // ff is not available and we are a ff driver, so error
    if (!(bitMask[2] & 32)) RET(1);

    // switch off autocentering
    memset(&event, 0, sizeof(event));
    event.type  = EV_FF;
    event.code  = FF_AUTOCENTER;
    event.value = 0;
    // this is dirty and it waits for a better solution (kernel need NOT to
    // write the whole buffer at once)
    if (write(portDes, &event, sizeof(event)) != sizeof(event)) RET(errno);

    memset(&eff, 0, sizeof(eff));
    eff.type         = FF_CONSTANT;
    eff.id=-1;
    eff.replay.length=0xffff;
    if (ioctl(portDes, EVIOCSFF, &eff) < 0) RET(2);
    cid=eff.id;

    memset(&constantEffect, 0, sizeof(eff));
    constantEffect.type          = FF_CONSTANT;
    constantEffect.id            = -1;
    constantEffect.replay.length = 0xffff;

    if (ioctl(portDes, EVIOCSFF, &constantEffect) < 0) RET(3);

    return 0;
}

#undef RET

int LL_ForceFeedback::closeDevice()
{
    if (portDes >= 0) close(portDes);

    return 0;
}

void LL_ForceFeedback::update()
{
    struct input_event buf[32];
    unsigned int a, readBytes;

    if (portDes < 0) return;

    readBytes = read(portDes, buf, sizeof(buf));
    if (readBytes < 0) return;

    if ((readBytes < 0) || (readBytes % sizeof(struct input_event))) return;

    readBytes /= sizeof(struct input_event);
    for (a = 0; a < readBytes; a++)
        switch (buf[a].type)
        {
            case EV_ABS:
                parseAbs(buf[a].code, buf[a].value);
                break;
            case EV_KEY:
                parseKey(buf[a].code, buf[a].value);
                break;
            case EV_FF_STATUS:
		fprintf(stderr, "effect %d %s\n", buf[a].code,
			(buf[a].value == FF_STATUS_STOPPED) ? "stopped" :
			"playing");
		break;
			
        }

    dispatch();
}

void LL_ForceFeedback::parseAbs(__u16 code, __s32 value)
{
    switch (code)
    {
        case ABS_X:
        case ABS_Y:
        case ABS_Z:
            pos._v[(int)code - ABS_X] = (osg::Vec3::value_type)value;
            break;
        case ABS_RX:
        case ABS_RY:
        case ABS_RZ:
            rot._v[(int)code - ABS_RX] = (osg::Vec3::value_type)value;
            break;
        case ABS_HAT0X:
        case ABS_HAT1X:
        case ABS_HAT2X:
        case ABS_HAT3X:
            POVs[(int)code - ABS_HAT0X][0] = (int)value;
            break;
        case ABS_HAT0Y:
        case ABS_HAT1Y:
        case ABS_HAT2Y:
        case ABS_HAT3Y:
            POVs[(int)code - ABS_HAT0Y][1] = (int)value;
            break;
	case ABS_THROTTLE:
	case ABS_RUDDER:
	case ABS_WHEEL:
	case ABS_GAS:
	case ABS_BRAKE:
	    sliders[(int)code - ABS_THROTTLE] = (int)value;
	    break;
    }
}

void LL_ForceFeedback::parseKey(__u16 code, __s32 value)
{
    if ((code >= BTN_JOYSTICK) && (code < BTN_DEAD))
    {
        putMessage(code - BTN_JOYSTICK, value);
        return;
    }
}

void LL_ForceFeedback::dispatch()
{
/*	char vystup[32] = "De|0#";
	osg::Vec3 *vec3;
	int *i, a;

	vystup[3] += (char)id;

	memcpy(vystup + 5, "Slider0", 7);
	for (a=0; a<nSliders; a++)
	{
		i = new int(sliders[a]);
		event_dispatcher->reportEvent(vystup, BaseClass::INT, (void*)i);
		vystup[11]++;
	}

	memcpy(vystup + 5, "POV0", 5);
	for (a=0; a<nPOVs; a++)
	{
		i = new int(POVs[a]);
		event_dispatcher->reportEvent(vystup, BaseClass::INT, (void*)i);
      vystup[8]++;
	}
*/
}

void LL_ForceFeedback::processEvent(std::string inputName, void *value)
{
    if (!strcmp(inputName.c_str(), "FFdata"))
    {
	osg::Vec3 *vec3 = (osg::Vec3*)value;

	constantEffect.direction = __u16(0xffff *
		atanf(vec3->y()/vec3->x()) /2/M_PI);
	constantEffect.u.constant.level = __s16(vec3->z());

	ioctl(portDes, EVIOCSFF, &constantEffect);
    } else if (!strcmp(inputName.c_str(), "FFByNamePlay"))
    {
	mapIter = effectsMap.find(*(std::string*)value);
	if(mapIter != effectsMap.end()) mapIter->second->play();
    } else if (!strcmp(inputName.c_str(), "FFByNameStop"))
    {
	mapIter = effectsMap.find(*(std::string*)value);
	if (mapIter != effectsMap.end()) mapIter->second->stop();
    }
}

int LL_ForceFeedback::addEffect(std::string name, std::string file)
{
    int ret = 0;

    if (effectsMap.find(name) != effectsMap.end()) return 1;

    LL_ForceFeedback::Effect::EffectCreate ec;

    ec.portDes = portDes;
    ec.path = file.c_str();
    ec.retValue = &ret;
    ec.onCreate = EOnCreate;

    Effect *eff = new Effect(&ec);

    if (!ret) effectsMap.insert(EMPair(name, eff));
    else delete eff;

    return ret;
}

//////////////////// class Effect ////////////////////

LL_ForceFeedback::Effect::Effect(PEffectCreate ec)
{
    path = new char[strlen(ec->path)+1];
    strcpy(path, ec->path);
    onCreate = ec->onCreate;
    portDes  = ec->portDes;

    isLoaded = !(*ec->retValue = load(onCreate)) && onCreate;
}

LL_ForceFeedback::Effect::~Effect()
{
    unload();
}

#define RIFFFORC "RIFF\0\0\0\0FORC\x5c\x77\x7e\x19\xba\x34\xd3\x11\xab\xd5\0\xc0\x4f\x8e\xc6\x27"
#define RET(x) { munmap(file, len); close(fd); return x; }

int LL_ForceFeedback::Effect::load(const int really)
{
    union size_u
    {
	char  c[4];
	__u32 i;
    } *size;

    effects.clear();

    if (really)
    {
	int fd = open(path, O_RDONLY);
	if (fd < 0) return errno;

	int len = lseek(fd, 0, SEEK_END);
	if (len < 9) return 1;

	char *file = (char*)mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
	if (file == MAP_FAILED) { close(fd); return 2; }

	char *tmp = file;

	if (memcmp(tmp, RIFFFORC, 28)) RET(5);
	tmp += 28;

	while (tmp < file+len)
	{
	    printf("%s\n", tmp);
	    if (memcmp(tmp, "LIST", 4)) RET(6);
	    tmp += 4;
	    size = (size_u*)tmp;
	    processList(tmp, size->i);
	    tmp += size->i + 4;
	}

	munmap(file, len);
	close(fd);
    }

    return 0;
}

#undef RET
#undef RIFFFORC

void LL_ForceFeedback::Effect::processList(char *ptr, int len)
{
    while (*ptr++) len--;

    if ((len >> 1) << 1 != len) ptr++, len--;

    while (len > 0)
    {

	len--;
    }
}

void LL_ForceFeedback::Effect::unload()
{
    if (!isLoaded) return;

    if (isPlaying())
    {
	update();
	return;
    }

    isLoaded = 0;

    for (setIter = effects.begin(); setIter != effects.end(); setIter++)
	delete (*setIter);

    effects.clear();
}

void LL_ForceFeedback::Effect::play()
{
    update();

    if (!isLoaded)
    {
	load(1);
	isLoaded=1;
    }

    for (setIter = effects.begin(); setIter != effects.end(); setIter++)
    {
	event.type = EV_FF;
	event.code = (*setIter)->id;
	event.value = 1;
	write(portDes, &event, sizeof(event));
    }
}

void LL_ForceFeedback::Effect::stop()
{
    update();
    for (setIter = effects.begin(); setIter != effects.end(); setIter++)
    {
	event.type = EV_FF;
	event.code = (*setIter)->id;
	event.value = 0;
	write(portDes, &event, sizeof(event));
    }
}

int LL_ForceFeedback::Effect::isPlaying()
{
    update();

    for (setIter = effects.begin(); setIter != effects.end(); setIter++)
    {
//	(*setIter)->;
/*	if (status & DIEGES_PLAYING) return 1;*/
    }

    return 0;
}
