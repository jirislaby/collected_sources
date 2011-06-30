/*
 *  VRECKO force feedback device controller plugin.
 *
 *  Copyright (c) 2004-2006 Jiri Slaby <jirislaby@gmail.com>
 */

#ifndef LINUX_H_FILE
#define LINUX_H_FILE

#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include <vrecko/Device.h>
#include <osg/Vec3>

#include <set>

namespace vrecko
{

class /*VRECKO_EXPORT*/ LL_ForceFeedback : public Device
{
    public:
	class Effect
	{
		int portDes;
		int lastUsed, isLoaded, onCreate; // see below
		char* path;

		struct input_event event;

		Effect() {}
		void update() { lastUsed = (int)time(NULL); }
		int load(const int);
		void processList(char*, int);

		std::set<struct ff_effect *> effects;
		std::set<struct ff_effect *>::iterator setIter;

	    public:
		typedef struct
		{
			int			portDes;
			const char*		path;
			struct ff_effect	*FFEffect;
			int			onCreate, *retValue;
		} EffectCreate, *PEffectCreate;

		Effect(PEffectCreate);
		~Effect();

		void play();
		void stop();

		void unload();

		int getLastUsed() { return ((int)time(NULL) - lastUsed); }
		int isPlaying();
	};

    protected:
	LL_ForceFeedback() {};

	std::map<std::string, Effect*>			effectsMap;
	std::map<std::string, Effect*>::iterator	mapIter;

    private:
	ff_effect	constantEffect;

	char	*port;
	int 	portDes;
	char	EUnload, // sec. to unload the effect
		EOnCreate; // when to load the effect

	osg::Vec3		pos, rot;
	long			sliders[5];
	unsigned long		POVs[4][2];

	int			nPOVs, nSliders, nAxes;
	int cid;

	typedef std::pair<std::string, Effect*>		EMPair;

	void dispatch();
	void releaseAll();

        virtual void putMessage(const unsigned int, const int) {};
        virtual void putMessage(const char*) {};

        void parseAbs(__u16, __s32);
        void parseRel(__u16, __s32);
        void parseKey(__u16, __s32);

    public:
	LL_ForceFeedback(const char*);
	~LL_ForceFeedback();

	int openDevice(void);
	int closeDevice(void);

	void processEvent(std::string, void*);
	void update(void);

	int addEffect(std::string name, std::string file);

	osg::Vec3 *getPos() { return new osg::Vec3(pos); }
	osg::Vec3 *getRot() { return new osg::Vec3(rot); }
	int getSlider(int a) { return sliders[a]; }
	int getPOV(int a, int b) { return POVs[a][b]; }
	void eff(int);
};

}

#endif
