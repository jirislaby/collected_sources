#include "vect3t.h"

#define TYP double

int main(int argc, char *argv[]) {

	vect3<TYP> r, s, t;

	cout << "\n\nZadej 1. vektor (r): ";

	cin >> r;

	cout << "\nr = " << r << ", jeho delka = " << abs(r) << "\n"
	     << "Opacny vektor = " << -r << ", jeho delka = " << abs(-r)
	     << "\n-r/2 = " << static_cast<TYP>(-0.5)*r
	     << "\nDelka polovicniho opacneho = "
	     << abs(static_cast<TYP>(-0.5)*r)
	     << "\n\nZadej 2. vektor (s): ";

	cin >> s;

	cout << "\ns = " << s << "\nr + s = " << r+s << "\nr - s = "
	     << r-s << "\nr . s = " << r*s << "\nr x s = " << r%s
	     << "\ns . r = " << s*r << "\ns x r = " << s%r
	     << " (opacny k r x s)\n\nZadej 3. vektor (t): ";

	cin >> t;

	cout << "\nt = " << t << "\nSmiseny soucin = " << m_product(r,s,t)
	     << "\n\n";
	
	return 0;
}
