#include <stdio.h>
#include <math.h>
#define HOD 0.5
#ifndef WHAT
#define WHAT 15
#endif

double fx, fy_za, fy, m, g, poc_vyska, Vo, x, y, sin_uhel, F, ax, ay, th,
    t1, vy, ymax, fg, y1, t, y0, x1, x0, vx, v, x_celek, t_vystupu, t_padu,
    y_max, a, xy_max, v_dopadu, t_zadane, x_aktual, y_aktual,v_aktual;


void volne_pad()
{

    ay = (fy_za / m);
    if ((ay - g) != 0) {
	t1 = sqrt(2 * HOD / fabs(ay - g));
	vy = t1 * fabs(ay - g);
	t_padu = (-vy + sqrt(vy * vy + 2 * g * (y0 - HOD))) / g;
	v_aktual = vy  - 0.5 * g * t_zadane;
    } else {
	vy = 0;
	t_padu = sqrt(2 * y0 / g);
	v_aktual = g*t_zadane;	
    }
    v_dopadu = vy + g * t_padu;
    x_celek = x0;
    t = t_padu + t1;
    y_max = y0;
    xy_max = x0;
    x_aktual = x0;
    y_aktual = y0 - 0.5 * g * t_zadane * t_zadane;
    v_aktual = g * t_zadane;
}

void vypocet()
{
//tihova sila
    fg = m * g;
//vysledna fila ve smeru osy y
    fy = fy_za - fg;
//zrychleni ve smeru os x a y a celkove zrychleni
    ax = fx / m;
    ay = ( fy / m);
    a = sqrt(ax * ax + ay * ay);

// vypocet promenych v 0.5 metru; dale uz to bude volne pad;
// cas pusobeni 0.5 metru
    t1 = sqrt(2*HOD / a);
//souradnice bodu po pusobeni F 0.5 metra
    y1 = (ay * t1 * t1) / 2 + y0;
    x1 = (ax * t1 * t1) / 2 + x0;
// slozky rychlosti v bode [x1,y1]
    vx = ax * t1;
    vy = ay * t1;
// celkova rychlost v bode [x1,y1]
    v = sqrt(vx * vx + vy * vy);
// v tomto case bude maximum
    t_vystupu = vy / g;
    y_max = y1 + (t_vystupu * vy) - (g * t_vystupu * t_vystupu / 2);
    xy_max = t_vystupu * vx + x1;

// pokud vypocitame t ziskame celkove cas letu 
// 0 = y'-  0.5gt*t + v0y*t
// t = t1 + t_vystupu + t_padu;
// 0 = y_max - 0.5*g*t_padu;
    t_padu = sqrt(2 * y_max / g);
//celkovy cas letu
    t = t1 + t_vystupu + t_padu;
//rychlost dopadu je
    v_dopadu = sqrt(vx * vx + (g * t_padu) * (g * t_padu));

printf("\n\nt1 =%lf\n\n", t1);

    //celkova draha je 
    x_celek = vx * t_padu + xy_max;

#if (WHAT&31)
	//aktualni pozice [x,y] v zadanem case t_zadane
    if (t1 >= t_zadane) {
	y_aktual = (ay * t_zadane * t_zadane) / 2 + y0;
	x_aktual = (ax * t_zadane * t_zadane) / 2 + x0;
	v_aktual=sqrt((ay*t_zadane)*(ay*t_zadane)+(ax*t_zadane)*(ax*t_zadane));
    } else {
	x_aktual = vx * t_zadane + x1;
	y_aktual = y1 + vy * t_zadane - 0.5 * g * t_zadane * t_zadane;
  	v_aktual = v - 0.5 *g * t_zadane;
    }

#endif
}


int main(void)
{
//  double fx,fy,m,g,poc_vyska,f;

    printf("*****************xpapiez, 98557*****************\n");
    printf("**********************Vrh***********************\n");
    printf("Zadejte pocatecni souradnice bodu[x,y]\n");
    printf("Souradnice y nesmi byt zaporna!");
    if (scanf("%lf %lf", &x0, &y0) != 2) {
	printf("nejsou zadane pocatecni souradnice bodu, koncim");
	return 99;
    }

    printf
	("Zadejte hmotnost vrhaneho telesa,\n hmotnost musi byt kladna!\n");
    if (scanf("%lf", &m) != 1) {
	printf("neni zadana hmotnost");
	return 80;
    }

    printf
	("Zadejte velikost gravitacniho zrychleni,\n g musi byt kladne!\n");
    if (scanf("%lf", &g) != 1) {
	printf("neni zadana gravitace");
	return 90;
    }

    printf("zadejte slozky sily [fx,fy]\n");
    if (scanf("%lf %lf", &fx, &fy_za) != 2) {
	printf("nekompletni sila");
	return 99;
    }
#if (WHAT&31)
    printf("Zadejte cas ve ktere chcete zjistit polohu telesa");
    if (scanf("%lf", &t_zadane) != 1) {
	printf("\nneni zadan cas\n...program konci...\n");
	return 100;
    }
#endif

    if ((m <= 0) || (g <= 0) || (y0 < 0)) {
	printf("y=%lf m=%lf g=%lf\n", y0, m, g);
	printf("Nespravne zadane data\n ...program KONCI...\n");
	return 99;
    }
#if (WHAT&31)
    if (t_zadane < 0)
	printf("zaporne cas\n...program konci...\n");
#endif
//**************************************************
    fg = m * g;
    fy = fy_za - fg;

    ((fy_za <= fg) && (fx == 0)) ? volne_pad() : vypocet();


  /*******************************************************/

    printf
	("Bod na ktery z pocatecniho mista [%.2lf,%.2lf] pusobila na draze"
	 " 0.5 m sila se slozkami fx = %.2lf N a fy = %.2lf N mel hmotnost"
	 " %.2lf kg pri tihovem zrychleni %.2lf m/s se choval nasledovne:\n",
	 x0, y0, fx, fy_za, m, g);

    // makro
#if (WHAT & 1)
if (ay<=0) xy_max=x0;
    printf("Bod dosahnul maximalni vysku v bode [%.2lf,%.2lf]\n",
	   xy_max, y_max);
#endif

#if (WHAT & 2)
    printf("Bod dopadl v bode [%.2lf,0.00]\n", x_celek);
#endif

#if (WHAT & 4)
    printf("Bod dopadl za cas %.2lf  s\n", t);
#endif

#if (WHAT & 8)
    printf("Rychlost dopadu %.2lf m/s\n", v_dopadu);
#endif


#if (WHAT & 31)
    if (t_zadane<t)
    printf("v case t = %.2lf byl bod v [%.2lf,%.2lf] a rychlost = %.2lf\n", 
  	    t_zadane, x_aktual, y_aktual,v_aktual);
else printf("v case t = %.2lf byl bod uz na zemi\n",t_zadane);

#endif

    return 0;
}
