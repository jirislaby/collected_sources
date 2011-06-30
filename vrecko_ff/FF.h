/**
  *  VRECKO force feedback device controller plugin.
  *
  *  Copyright (c) 2004-2006 Jiri Slaby <jirislaby@gmail.com>
  */

#ifndef FF_H_FILE
#define FF_H_FILE

#include <set>
#include <map>

#include <osg/Vec3>

#include "platform.h"

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

#endif
