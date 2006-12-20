#ifndef WINDOWS_H_FILE
#define WINDOWS_H_FILE

#include <set>
#include <map>

#include <vrecko/EventDispatcher.h>
#include <vrecko/Device.h>
#include <osg/Vec3>

#include <stdio.h>

#define DIRECTINPUT_VERSION 0x800
#include <dinput.h>

namespace vrecko
{

class /*VRECKO_EXPORT*/ LL_ForceFeedback : public Device
{
	public:
		class Effect
		{
				IDirectInputDevice8	*joydev;
				int						lastUsed,
											isLoaded,
											type,
											onCreate; // see below
				char*						path;

				Effect() {}
				void update() { lastUsed = GetTickCount(); }
				int load(int);

				std::set<IDirectInputEffect*> effects;
				std::set<IDirectInputEffect*>::iterator setIter;

			public:
				typedef struct
				{
					IDirectInputDevice8	*DIDevice;
					const char*				path;
					IDirectInputEffect	*DIEffect;
               int						onCreate,
												*retValue,
												type;
				} EffectCreate, *PEffectCreate;

				Effect(PEffectCreate);
				~Effect();

				void addEffect(IDirectInputEffect* eff) { effects.insert(eff); update(); }

				void play();
				void stop();

				void unload();

				int getLastUsed() { return (GetTickCount() - lastUsed) / 1000; }
				int isPlaying();
		};

		typedef struct
		{
			IDirectInputDevice8	**dev;
  			IDirectInput8			*di;
			int						*a, *b, *c;
			std::set<IDirectInputEffect*> *eff;
		} dev_di;

	private:
		HWND						handle;

		char						EUnload, // sec. to unload the effect
									EOnCreate; // when to load the effect

		IDirectInput8			*joydi;
		IDirectInputDevice8	*joydev;
		IDirectInputEffect	*constantEffect;
		
		osg::Vec3				pos, rot;
		long						sliders[2];
		unsigned long			POVs[4], butt, oldButt;

		int						nPOVs, nSliders, nAxes;

		void dispatch();
		void releaseAll();

	protected:
		LL_ForceFeedback();

		typedef std::pair<std::string, Effect*>	EMPair;
		std::map<std::string, Effect*>				effectsMap;
		std::map<std::string, Effect*>::iterator	mapIter;

	public:
		LL_ForceFeedback(const char*);

		void setHandle(HWND handle) { LL_ForceFeedback::handle = handle; };

		int openDevice(void);
		int closeDevice(void);

		void processEvent(std::string, void*);
		void update(void);

		int addEffect(std::string name, REFGUID guid, DIEFFECT* effect);
		int addEffect(std::string name, std::string file);

		osg::Vec3 getPos() { return pos; }
		osg::Vec3 getRot() { return rot; }
		unsigned long getButt() { return butt; }
		int getSlider(int a) { return sliders[a]; }
		int getPOV(int a) { return POVs[a]; }
		void eff(int);
};

}

#endif
