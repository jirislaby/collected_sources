#ifndef WingManF3D_H
#define WingManF3D_H

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

using namespace vrecko;

inline void LL_ForceFeedback::eff(int a)
{
	static double x = 0.0f;
	x += (double)(sliders[0])/2000;
    LONG rglDirection[2] = { sin(x)*1000, cos(x)*1000 };
	 DICONSTANTFORCE cf={5000};

	 DIEFFECT eff;
    ZeroMemory( &eff, sizeof(eff) );
    eff.dwSize                = sizeof(DIEFFECT);
    eff.dwFlags               = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	 eff.cAxes                 = nAxes;
    eff.rglDirection          = rglDirection;
    eff.lpEnvelope            = 0;
    eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
    eff.lpvTypeSpecificParams = &cf;
    eff.dwStartDelay            = 0;

    if (a) constantEffect->SetParameters(&eff, DIEP_DIRECTION |
                                           DIEP_TYPESPECIFICPARAMS |
                                           DIEP_START);
	 else constantEffect->Stop();

	 HDC hdc = GetDC(handle);
	 char buf[64];
	 int len = sprintf(buf, "x: %.2lf, dirx: %d, diry: %d", x, rglDirection[0], rglDirection[1]);
	 TextOut(hdc, 10, 50, buf, len);
	 MoveToEx(hdc, 400 , 400, NULL);
	 LineTo(hdc, 400 + (rglDirection[0] >> 2), 400 + (rglDirection[1] >> 2));
	 ReleaseDC(handle, hdc);
}

inline LL_ForceFeedback::LL_ForceFeedback(const char*):
	Device(), joydi(NULL), joydev(NULL),
	nPOVs(0), nSliders(0),
	pos(0, 0, 0), rot(0, 0, 0), butt(0),
	oldButt(0), constantEffect(NULL),
	nAxes(0), EUnload(-10), EOnCreate(1)
{
//    device_type = DEVICE_WINGMANF3D;

    addOutput("Buttons", INT, "\0");
    addOutput("Position", VEC3, "\0");
	 addOutput("Rotation", VEC3, "\0");

	 addInput("FFData", VEC3, "\0");
	 addInput("FFByName", STRING, "\0");
}

int __stdcall EnumDevicesCallback(const DIDEVICEINSTANCE *lpddi, void *pvRef)
{
	LL_ForceFeedback::dev_di *dd = (LL_ForceFeedback::dev_di*)pvRef;

   if (dd->di->CreateDevice(lpddi->guidInstance, dd->dev, NULL))
		return DIENUM_CONTINUE;

	return DIENUM_STOP;
}

int __stdcall EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE  *lpddoi, void *pvRef)
{
	LL_ForceFeedback::dev_di *dd = (LL_ForceFeedback::dev_di*)pvRef;

   if( lpddoi->dwType & DIDFT_AXIS )
   {
       DIPROPRANGE diprg; 
       diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
       diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
       diprg.diph.dwHow        = DIPH_BYID; 
       diprg.diph.dwObj        = lpddoi->dwType; // Specify the enumerated axis
       diprg.lMin              = -1000; 
       diprg.lMax              = 1000; 
   
       // Set the range for the axis
	    if((*dd->dev)->SetProperty(DIPROP_RANGE, &diprg.diph)) 
           return DIENUM_STOP;
   }

   // Set the UI to reflect what objects the joystick supports
	if (lpddoi->guidType == GUID_Slider) (*dd->a)++;
   if (lpddoi->guidType == GUID_POV) (*dd->b)++;

	if (lpddoi->dwFlags & DIDOI_FFACTUATOR) (*dd->c)++;

   return DIENUM_CONTINUE;
}

int __stdcall EnumEffectsInFile(const DIFILEEFFECT *lpdfe, void *pvRef)
{
	IDirectInputEffect *DIEffect = NULL;
	 LL_ForceFeedback::dev_di *dd = (LL_ForceFeedback::dev_di*)pvRef;

	 (*dd->dev)->CreateEffect(lpdfe->GuidEffect, lpdfe->lpDiEffect, 
                                                &DIEffect, NULL);

	 if (DIEffect)
	 {
		 if (dd->eff) dd->eff->insert(DIEffect);
		 else DIEffect->Release();
	 }
	 else
	 {
		 *dd->a = 1;
		 return DIENUM_STOP;
	 }

	 return DIENUM_CONTINUE;
}

#define RET(x) { releaseAll(); return x; }

inline int LL_ForceFeedback::openDevice(void)
{
	if (DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
								IID_IDirectInput8, (void**)&joydi, NULL))
		return 1;

	dev_di dd = {&joydev, joydi, &nSliders, &nPOVs, &nAxes, NULL};
	if (joydi->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDevicesCallback, (void*)&dd, 
                         DIEDFL_FORCEFEEDBACK | DIEDFL_ATTACHEDONLY)) RET(2);

	if (!joydev) RET(3);

	if (joydev->SetDataFormat(&c_dfDIJoystick2)) RET(4);

	if (joydev->SetCooperativeLevel(handle, DISCL_EXCLUSIVE | DISCL_FOREGROUND)) RET(5);

// Since we will be playing force feedback effects, we should disable the
// auto-centering spring.
	DIPROPDWORD dipdw;

	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = FALSE;

	if (joydev->SetProperty(DIPROP_AUTOCENTER, &dipdw.diph)) RET(6);

	if (joydev->EnumObjects(EnumObjectsCallback, (void*)&dd, DIDFT_ALL)) RET(7);

	DWORD           rgdwAxes[2]     = { DIJOFS_X, DIJOFS_Y };
	LONG            rglDirection[2] = { 0, 0 };
	DICONSTANTFORCE cf              = { 0 };

	DIEFFECT eff;
	ZeroMemory(&eff, sizeof(eff));
	eff.dwSize                  = sizeof(DIEFFECT);
	eff.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	eff.dwDuration              = INFINITE;
	eff.dwGain                  = DI_FFNOMINALMAX;
	eff.dwTriggerButton         = DIEB_NOTRIGGER;
	eff.cAxes                   = nAxes;
	eff.rgdwAxes                = rgdwAxes;
	eff.rglDirection            = rglDirection;
	eff.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
	eff.lpvTypeSpecificParams   = &cf;

	joydev->CreateEffect(GUID_ConstantForce, &eff, &constantEffect, NULL);

	if (!constantEffect) RET(8);

	int a;
	char ch[8]="Slider0";
	for (a=0; a<nSliders; a++)
	{
		addOutput(ch, VEC3, "\0");
		ch[6]++;
	}
	memcpy(ch, "POV0\0", 5);
	for (a=0; a<nPOVs; a++)
	{
		addOutput(ch, VEC3, "\0");
		ch[3]++;
	}

	printf("sliders: %d, POVs: %d\n", nSliders, nPOVs);

	return 0;
}

#undef RET

inline void LL_ForceFeedback::releaseAll()
{
	for (mapIter = effectsMap.begin(); mapIter != effectsMap.end(); mapIter++)
		delete mapIter->second;
	effectsMap.clear();
	if (constantEffect)
	{
		constantEffect->Release();
		constantEffect = NULL;
	}
	if (joydev)
	{
		joydev->Release();
		joydev = NULL;
	}
	if (joydi)
	{
		joydi->Release();
		joydi = NULL;
	}
}

inline int LL_ForceFeedback::closeDevice()
{
	releaseAll();

	return 0;
}

inline void LL_ForceFeedback::processEvent(std::string inputName, void *value)
{
	if (!strcmp(inputName.c_str(), "FFdata"))
	{
		osg::Vec3 *vec3 = (osg::Vec3*)value;

		LONG rglDirection[2] = { (long)(vec3->x()*1000), (long)(vec3->y()*1000) };
		DICONSTANTFORCE cf = { (long)(vec3->z()*1000) };

		DIEFFECT eff;
		ZeroMemory( &eff, sizeof(eff) );
		eff.dwSize                = sizeof(DIEFFECT);
		eff.dwFlags               = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.cAxes                 = nAxes;
		eff.rglDirection          = rglDirection;
		eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams = &cf; 

		constantEffect->SetParameters( &eff, DIEP_DIRECTION | DIEP_START |
												DIEP_TYPESPECIFICPARAMS);
	} else if (!strcmp(inputName.c_str(), "FFByNamePlay"))
	{
		if ((mapIter = effectsMap.find(*(std::string*)value)) != effectsMap.end())
			mapIter->second->play();
	} else if (!strcmp(inputName.c_str(), "FFByNameStop"))
	{
		if ((mapIter = effectsMap.find(*(std::string*)value)) != effectsMap.end())
			mapIter->second->stop();
	}
}

inline void LL_ForceFeedback::update()
{
	if (!joydev) return;

	if (joydev->Poll()<0)
	{
		HRESULT hr;

		do
		{
			hr = joydev->Acquire();
		} while (hr == DIERR_INPUTLOST);

		return; 
	}

	if (EUnload > 0)
		for (mapIter = effectsMap.begin(); mapIter != effectsMap.end(); mapIter++)
			if (mapIter->second->getLastUsed() > EUnload)
				mapIter->second->unload();
   
	DIJOYSTATE2 js;

	if (joydev->GetDeviceState(sizeof(DIJOYSTATE2), &js))
		return dispatch();

	pos.set(js.lX, js.lY, js.lZ);
	rot.set(js.lRx, js.lRy, js.lRz);

	memcpy(sliders, js.rglSlider, sizeof(long)<<1);
	memcpy(POVs, js.rgdwPOV, sizeof(unsigned long)<<2);

	butt = 0;
	for (int a=0; a < sizeof(butt)<<3; a++)
	{
		butt = butt << 1;
		if (js.rgbButtons[31-a] & 0x80) butt |= 1;
	}

	dispatch();
}

inline void LL_ForceFeedback::dispatch()
{
/*	char vystup[32] = "De|0#";
	osg::Vec3 *vec3;
	int *i, a;

	vystup[3] += (char)id;

	memcpy(vystup + 5, "Position", 9);
	vec3 = new osg::Vec3();
	*vec3 = pos;
	event_dispatcher->reportEvent(vystup, BaseClass::VEC3, (void*)vec3);

	memcpy(vystup + 5, "Rotation", 9);
	vec3 = new osg::Vec3();
	*vec3 = rot;
	event_dispatcher->reportEvent(vystup, BaseClass::VEC3, (void*)vec3);

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

	memcpy(vystup + 5, "Button", 7);
	if (butt ^ oldButt)
	{
		unsigned long ul = butt & (butt ^ oldButt), ul1 = oldButt & (butt ^ oldButt);

		for (a=0; a<32; a++) 
		{
			if (ul & 1) // pushed
			{
				char *s = new char[32];
				sprintf(s, "but%d", a);
            event_dispatcher->reportEvent(vystup, BaseClass::STRING, (void*)s);
			}

			if (ul1 & 1) // released
			{
				char *s = new char[32];
				sprintf(s, "but%d.rel", a);
				event_dispatcher->reportEvent(vystup, BaseClass::STRING, (void*)s);
			}

			ul = ul >> 1;
			ul1 = ul1 >> 1;
		}

		oldButt = butt;
	}*/
}

inline int LL_ForceFeedback::addEffect(std::string name, REFGUID guid, DIEFFECT *eff)
{
	IDirectInputEffect *DIEffect = NULL;

   joydev->CreateEffect(guid, eff, &DIEffect, NULL);

	if (DIEffect)
	{
		LL_ForceFeedback::Effect::EffectCreate ec;

		ec.DIDevice = joydev;
		ec.DIEffect = DIEffect;
		ec.type = 1;

		effectsMap.insert(EMPair(name, new Effect(&ec)));
	}

	return !DIEffect;
}

inline int LL_ForceFeedback::addEffect(std::string name, std::string file)
{
	int ret;

   if (effectsMap.find(name) != effectsMap.end()) return 1;

	LL_ForceFeedback::Effect::EffectCreate ec;

	ec.DIDevice = joydev;
	ec.path = file.c_str();
	ec.retValue = &ret;
	ec.onCreate = EOnCreate;
	ec.type = 0;

	Effect *eff = new Effect(&ec);

	if (!ret) effectsMap.insert(EMPair(name, eff));
	else delete eff;

	return 0;
}

//////////////////// class Effect ////////////////////

inline LL_ForceFeedback::Effect::Effect(PEffectCreate ec)
{
	path = new char[strlen(ec->path)+1];
	strcpy(path, ec->path);
	onCreate = ec->onCreate;
	joydev   = ec->DIDevice;
	type     = ec->type;

	if (type)
	{
		isLoaded = 1;
		effects.insert(ec->DIEffect);
	}
	else isLoaded = !(*ec->retValue = load(onCreate)) && onCreate;
}

inline LL_ForceFeedback::Effect::~Effect()
{
	if (type)	// unload won't unload effects created from DIEffect
	{				// (we have no idea about where to reload from)
		(*effects.begin())->Release();
		effects.clear();
	}
	unload();
}

inline int LL_ForceFeedback::Effect::load(int really)
{
	int ret = 0;

	effects.clear();
	dev_di dd = {&joydev, NULL, &ret, NULL, NULL, really ? &effects : NULL};
	joydev->EnumEffectsInFile(path, EnumEffectsInFile, (void*)&dd, DIFEF_MODIFYIFNEEDED);

	return ret;
}

inline void LL_ForceFeedback::Effect::unload()
{
	if (!isLoaded || type) return;
	if (isPlaying())
	{
		update();
		return;
	}
	isLoaded = 0;
	for (setIter = effects.begin(); setIter != effects.end(); setIter++)
		(*setIter)->Release();
	effects.clear();
}

inline void LL_ForceFeedback::Effect::play()
{
	update();

	if (!isLoaded)
	{
		load(1);
		isLoaded=1;
	}

	for (setIter = effects.begin(); setIter != effects.end(); setIter++)
		(*setIter)->Start(1, 0);
}

inline void LL_ForceFeedback::Effect::stop()
{
	update();
	for (setIter = effects.begin(); setIter != effects.end(); setIter++)
		(*setIter)->Stop();
}

inline int LL_ForceFeedback::Effect::isPlaying()
{
	DWORD status;

	update();
	for (setIter = effects.begin(); setIter != effects.end(); setIter++)
	{
		(*setIter)->GetEffectStatus(&status);
		if (status & DIEGES_PLAYING) return 1;
	}

	return 0;
}