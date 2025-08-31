#ifndef COLORS
#define COLORS
#include <iostream>
#include <cmath>
using namespace std;

struct RGB { float R; float G; float B;};

/* Predbezna deklarace sablony tridy (bez atributu a metod) */

template<typename T> class colors;

/* Predbezne deklarace sablon spratelenych funkci a operatoru pouzitych v 
   tride (jen prototypy) */

/* operator vstupu barvy */

template<typename T> istream& operator>> (istream&, colors<T>&);

/* operator vystupu barvy */

template<typename T> ostream& operator<< (ostream&, const colors<T>&);

/* Plna deklarace sablony tridy vcetne implementace vlozenych metod a 
   prototypu nevlozenych metod a znovu spratelenych funkci */

template<typename T>

class colors {

  private:

  T color;

  int pocet_R, pocet_G, pocet_B, max_R, max_G, max_B;

  public:

/* inicializacni konstruktor */

  colors (float, float, float);

/* prazdny konstruktor */

  colors ();

/* destruktor vyhovuje implicitni */

/* spratelene funkce a operatory */

/* operator vstupu barvy */

  friend istream& operator>> <>(istream& in, colors<T>&);

/* operator vystupu barvy */

  friend ostream& operator<< <>(ostream& out, const colors<T>&);

// nespratelene funkce a operatory

  colors<T> operator+ (const colors<T>);// aditivní míchání barev
  colors<T> operator- (const colors<T>);// subtraktivní míchání barev
  void pocet_bitu (int, int, int, int, int, int);// nastavení počtu bitů 
						 //pro jednotlivé barvy
  T RGBtoT (float, float, float);// převod barvy z vyjádření 3 čísly float 
			//na celé číslo typu daného parametrem sablony
  RGB TtoRGB () const;// převod barvy z vyjádření z celého čísla typu 
			//daného parametrem sablony na 3 čísla float
  int zaokrouhli(float a);
};

/* inicializacni konstruktor */

template<typename T> colors<T>::colors (float rr, float gg, float bb) { 
 if (typeid(color)==typeid(unsigned char))
  pocet_bitu(3,3,2,7,7,3);
 else
  if (typeid(color)==typeid(unsigned short int)) 
   pocet_bitu(5,6,5,31,63,31); 
  else
   if (typeid(color)==typeid(unsigned long int)) 
    pocet_bitu(8,8,8,255,255,255);
 color = RGBtoT(rr,gg,bb);
}

/* prazdny konstruktor */

template<typename T> colors<T>::colors () {
 if (typeid(color)==typeid(unsigned char))
  pocet_bitu(3,3,2,7,7,3);
 else
  if (typeid(color)==typeid(unsigned short int)) 
   pocet_bitu(5,6,5,31,63,31); 
  else
   if (typeid(color)==typeid(unsigned long int)) 
    pocet_bitu(8,8,8,255,255,255);
 color = RGBtoT(0,0,0);
}

/* nastavení počtu bitů pro jednotlivé barvy */

template <typename T> void colors<T>::pocet_bitu (int rr, int gg , int bb, 
int m_rr, int m_gg, int m_bb) {
  pocet_R=rr;
  pocet_G=gg;
  pocet_B=bb;
  max_R=m_rr;
  max_G=m_gg;
  max_B=m_bb;
}

/* zaokrouhleni */

template <typename T> int colors<T>::zaokrouhli(float a) {
	if (a >= 0)
		return a - floor(a) <  0.5  ? static_cast<int>(floor(a))
					    : static_cast<int>( ceil(a));
	else
		return a - ceil(a)  <= -0.5 ? static_cast<int>(floor(a))
					    : static_cast<int>( ceil(a));
}

/* převod barvy z vyjádření 3 čísly float na celé číslo typu daného 
						parametrem sablony */

template <typename T> T colors<T>::RGBtoT (float rr, float gg, float bb) {
	T pom = 0;
	pom = zaokrouhli(max_R*rr);
	pom = pom << pocet_G;
	pom = pom | (zaokrouhli(max_G*gg));
	pom = pom << pocet_B;
	pom = pom | (zaokrouhli(max_B*bb));
	return pom;
}

/* převod barvy z vyjádření z celého čísla typu daného parametrem sablony 
na 3 čísla float */

template <typename T> RGB colors<T>::TtoRGB () const {
	RGB result;
	int pom = color;
	result.B = pom & max_B;
	pom = pom >> pocet_B;
	result.G = pom & max_G;
	pom = pom >> pocet_G;
	result.R = pom & max_R;
	return result;
}

template<typename T> colors<T> colors<T>::operator+(const colors<T> b) {
// aditivní míchání barev
	RGB pom1 = TtoRGB();
	RGB pom2 = b.TtoRGB();
	colors<T> result(*this);
	float rrr, ggg, bbb, max=0.0;
	bool over = false;
	rrr = (pom1.R/max_R) + (pom2.R/b.max_R);
	ggg = (pom1.G/max_G) + (pom2.G/b.max_G);
	bbb = (pom1.B/max_B) + (pom2.B/b.max_B);	
	if (rrr > 1) {	max = rrr; over = true; };
	if ((ggg > 1) && (ggg > max)) { max = ggg; over = true; };
	if ((bbb > 1) && (bbb > max)) { max = bbb; over = true; };
	if (over) { rrr=rrr/max; ggg=ggg/max; bbb = bbb/max;};
	result.color = RGBtoT(rrr,ggg,bbb);
	return result;
}

template<typename T> colors<T> colors<T>::operator-(const colors<T> b) {
// subtraktivní míchání barev
	RGB pom1 = TtoRGB();
	RGB pom2 = b.TtoRGB();
	colors<T> result;
	float ccc, mmm, yyy;
	float max=0.0;
	bool over = false;
	ccc = (1-(pom1.R/max_R)) + (1-(pom2.R/b.max_R));
	mmm = (1-(pom1.G/max_G)) + (1-(pom2.G/b.max_G));
	yyy = (1-(pom1.B/max_B)) + (1-(pom2.B/b.max_B));	
	if (ccc > 1) {	max = ccc; over = true; };
	if ((mmm > 1) && (mmm > max)) { max = mmm; over = true; };
	if ((yyy > 1) && (yyy > max)) { max = yyy; over = true; };
	if (over) { ccc=ccc/max; mmm=mmm/max; yyy = yyy/max;};
	result.color = RGBtoT(1-ccc,1-mmm,1-yyy);
	return result;
}

template<typename T> istream& operator>>(istream& in, colors<T>&a) {
// operator vstupu barvy
	  float R, G, B;
	  in >> R >> G >> B;
	  a.color = a.RGBtoT(R,G,B);
	  return in;
}

template<typename T> ostream& operator<<(ostream& out, const colors<T>&a){
// operator vystupu barvy
	RGB pom = a.TtoRGB();
	out << "\n(" << pom.R << "/" << a.max_R << " = "
	    << pom.R/a.max_R << " R, " 
	    << pom.G << "/"
	    << a.max_G << " = " << pom.G/a.max_G << " G, "
	    << pom.B << "/" << a.max_B << " = "
	    << pom.B/a.max_B << " B)";
	return out;
  }
#endif
