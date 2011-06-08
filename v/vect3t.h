#ifndef VECT3
#define VECT3
#include <iostream>
#include <cmath>
using namespace std;

/* Predbezna deklarace sablony tridy (bez atributu a metod) */

template<typename T> class vect3;

/* Predbezne deklarace sablon spratelenych funkci a operatoru pouzitych v 
   tride (jen prototypy) */

/* cteni vektoru */

template<typename T> istream& operator>> (istream&, vect3<T>&);

/* vypis vektoru */

template<typename T> ostream& operator<< (ostream&, const vect3<T>&);

/* konstanta * vektor */

template<typename T> vect3<T> operator* (T, const vect3<T>);

/* skalarni soucin */

template<typename T> T operator* (const vect3<T>, const vect3<T>); 

/* smiseny soucin */

template<typename T> T m_product (vect3<T>, vect3<T>, vect3<T>);

/* delka vektoru */

template<typename T> T abs(const vect3<T>);

/* Plna deklarace sablony tridy vcetne implementace vlozenych metod a 
   prototypu nevlozenych metod a znovu spratelenych funkci */

template<typename T>

class vect3 {

  private:

  T x,y,z;

  public:

/* inicializacni konstruktor (nahradi i prazdny kontruktor): */

  vect3 (T xx=0, T yy=0, T zz=0):x(xx),y(yy),z(zz){}

/* kopirovaci konstruktor vyhovuje implicitni */

/* destruktor vyhovuje implicitni */

/* spratelene funkce a operatory */

/* cteni vektoru */

  friend istream& operator>> <>(istream& in, vect3<T>&);

/* vypis vektoru */

  friend ostream& operator<< <>(ostream& out, const vect3<T>&);

/* konstanta * vektor */

  friend vect3<T> operator* <>(T, const vect3<T>);

/* skalarni soucin */

  friend T operator* <>(const vect3<T>, const vect3<T>);

/* smiseny soucin */

  friend T m_product <>(vect3<T>,vect3<T>,vect3<T>);

/* delka vektoru */

  friend T abs <>(const vect3<T>);

// nespratelene funkce a operatory

  vect3<T> operator+ (const vect3<T>); // soucet vektoru
  vect3<T> operator- (const vect3<T>); // rozdil vektoru
  vect3<T> operator- (void); // opacny vektor
  vect3<T> operator% (const vect3<T>); // vektorovy soucin
  vect3<T> operator* (T mult); // vektor * konstanta

};

template<typename T> T abs(const vect3<T> a) { // delka vektoru
 return sqrt(a*a);
}

template<typename T> vect3<T> vect3<T>::operator+(const vect3<T> b) {
// soucet vektoru
  vect3<T> result(*this);
  result.x+=b.x;
  result.y+=b.y;
  result.z+=b.z;
  return result;
}

template<typename T> vect3<T> vect3<T>::operator-(const vect3<T> b) {
// rozdil vektoru
  vect3<T> result(*this);
  result.x-=b.x;
  result.y-=b.y;
  result.z-=b.z;
  return result;
}

template<typename T> vect3<T> vect3<T>::operator-(void) { // opacny vektor
  vect3<T> result;
  result.x=-this->x;
  result.y=-this->y;
  result.z=-this->z;
  return result;
}

template<typename T> vect3<T> vect3<T>::operator*(T mult) {
	return mult*(*this);
} // vektor * konstanta

template<typename T> T operator*(const vect3<T> a, const vect3<T> b) {
	return a.x*b.x+a.y*b.y+a.z*b.z;
} // skalarni soucin

template<typename T> vect3<T> vect3<T>::operator%(const vect3<T> b) {
// vektorovy soucin
  vect3<T> result;
  result.x=this->y*b.z-this->z*b.y;
  result.y=this->z*b.x-this->x*b.z;
  result.z=this->x*b.y-this->y*b.x;
  return result;
}

template<typename T> vect3<T> operator*(T mult, const vect3<T> a) {
//konstanta * vektor
  vect3<T> result;
  result.x=mult*a.x;
  result.y=mult*a.y;
  result.z=mult*a.z;
  return result;
}

template<typename T> istream& operator>>(istream& in, vect3<T>&a) {
 // cteni vektoru 
	  in >> a.x >> a.y >> a.z;
	  return in;
  }

template<typename T> ostream& operator<<(ostream& out, const vect3<T>&a){
// vypis vektoru
	  out << a.x << " " << a.y << " " << a.z;
	  return out;
  }

template<typename T> T m_product(vect3<T> a, vect3<T> b,vect3<T> c) {
// smiseny soucin
	return (a%b)*c;
}

#endif
