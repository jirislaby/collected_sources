#include <stdio.h>
#include "FF.h"

using namespace vrecko;

ForceFeedback *wm;

void paint()
{
    wm->eff(0);
    wm->update();
    osg::Vec3 *v = wm->getPos();
    osg::Vec3 *w = wm->getRot();
    wm->processEvent("FFdata", (void*));
    printf("pos(%.2f, %.2f), rot(%.2f), POVs(%d, %d), slid(%d)\n",
	    v->x(), v->y(), w->z(),
	    wm->getPOV(0, 0), wm->getPOV(0, 1),
	    wm->getSlider(0)
	    );
    delete v;
    delete w;
}

int main()
{
    wm = new ForceFeedback("/dev/input/event5");

    int ret=wm->openDevice();
    if (ret) printf("error %d\n", ret);

    printf("addeffect: %d\n", wm->addEffect("ble", "Infinite.ffe"));
    printf("addeffect: %d\n", wm->addEffect("bla", "viewpoin_scr.ffe"));
//    return 50;
    while (3)
    {
	paint();
	usleep(100000);
    }

    wm->closeDevice();
    delete wm;

    puts("OK");

    return 0;
}
