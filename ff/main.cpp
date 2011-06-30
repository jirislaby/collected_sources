#include <windows.h>
#include <stdio.h>
#include "FF.h"

#define CSN "mojatrida"

using namespace vrecko;

ForceFeedback *wm;
int xxx = 1, stop = 0;

void WINAPI paint(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
   char buf[256];
	HDC hdc=GetDC(hwnd);

	RECT r={0,0,1200,800};
	FillRect(hdc, &r, NULL);
	wm->update();
	osg::Vec3 *v = &wm->getPos();
	int len = sprintf(buf, "x: %f, y: %f, z: %f, ", v->x(), v->y(), v->z());
	v = &wm->getRot();
	len += sprintf(buf+len, "rx: %f, ry: %f, rz: %f, ", v->x(), v->y(), v->z());
	len += sprintf(buf+len, "slider: %d, POV: %d, butt: %lu", wm->getSlider(0), wm->getPOV(0), wm->getButt());
	TextOut(hdc, 10, 10, buf, len);
	ReleaseDC(hwnd, hdc);
	wm->eff(xxx);      
}

LRESULT WINAPI wproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
   {
      case WM_CREATE:
			SetTimer(hwnd, 0, 50, paint);
         break;
		case WM_KEYUP:
			switch (wParam)
			{
				case VK_ESCAPE:
		         stop = 1;
               break;
				case 'A':
					wm->addEffect("blee", "c:\\f.ffe");
					wm->processEvent("FFByNamePlay", new std::string("blee"));
					break;
				case 'Q':
					wm->processEvent("FFByNameStop", new std::string("blee"));
					break;
				case 'S':
					xxx = !xxx;
					break;
			}
			break;
      case WM_DESTROY:
			KillTimer(hwnd, 0);
			PostQuitMessage(0);
         break;
      default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
   }
	return 0;
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow)
{
	HWND handle;
   WNDCLASS trida;

	trida.style=CS_HREDRAW | CS_VREDRAW;
	trida.lpfnWndProc=(WNDPROC)wproc;
	trida.cbClsExtra=0;
	trida.cbWndExtra=0;
	trida.hInstance=hinstance;
	trida.hIcon=LoadIcon(NULL, IDI_APPLICATION);
	trida.hCursor=LoadCursor(NULL, IDC_ARROW);
	trida.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	trida.lpszMenuName=NULL;
	trida.lpszClassName=CSN;

	if (!RegisterClass(&trida)) { puts("regclass"); return 1; }

	handle=CreateWindow(CSN, "aaa", WS_OVERLAPPEDWINDOW|WS_MAXIMIZE, 0, 0, 1200, 900, 0, 0, hinstance, 0);
	if (!handle) {printf("cw: %ld\n", GetLastError()); return 1;}

	ShowWindow(handle, SW_SHOW);
	UpdateWindow(handle);

	wm = new ForceFeedback("");

	wm->setHandle(handle);

	int ret=wm->openDevice();
	if (ret) printf("error %d\n", ret);

	MSG msg;

   while (!stop && GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	} 

	wm->closeDevice();
	delete wm;

	DestroyWindow(handle);
	UnregisterClass(CSN, hinstance);
	puts("OK");

	return 0;
}