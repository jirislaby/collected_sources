#include "colors.h"

int main()
{
	colors<unsigned char> osmbit1;	// 8bitove barevne schema
	colors<unsigned char> osmbit2;	// 8bitove barevne schema
	colors<unsigned char> osmbit3;	// 8bitove barevne schema
	colors<unsigned short int> high1;// high colors
	colors<unsigned short int> high2;// high colors
	colors<unsigned short int> high3;// high colors
	colors<unsigned long int> true1;// true colors
					// nejvyssích 8 bitu obsahuje nuly
	colors<unsigned long int> true2;// true colors
					// nejvyssích 8 bitu obsahuje nuly
	colors<unsigned long int> true3;// true colors
					// nejvyssích 8 bitu obsahuje nuly

	cout << "==== 8bitova barva: ====\n";

	cout << "Zadej 1. barvu - 3 realna cisla od 0 do 1 (slozky R,G,B): ";
	cin >> osmbit1;
	cout << "Zadej 2. barvu - 3 realna cisla od 0 do 1 (slozky R,G,B): ";
	cin >> osmbit2;
	osmbit3 = osmbit1+osmbit2;

	cout << osmbit1 << "\naditivne smichano s";
	cout << osmbit2 << "\ndava";
	cout << osmbit3;
	cout << osmbit1+osmbit2 << "\n";

	cout << "==== 16bitova barva: ====\n";

	cout << "Zadej 1. barvu - 3 realna cisla od 0 do 1 (slozky R,G,B): ";
	cin >> high1;

	cout << "Zadej 2. barvu - 3 realna cisla od 0 do 1 (slozky R,G,B): ";
	cin >> high2;
	high3 = high1-high2;

	cout << high1 << "\nsubtraktivne smichano s";
	cout << high2 << "\ndava";
	cout << high3;
	cout << high1-high2 << "\n";

	cout << "==== 24bitova barva: ====\n";

	cout << "Zadej 1. barvu - 3 realna cisla od 0 do 1 (slozky R,G,B): ";
	cin >> true1;

	cout << "Zadej 2. barvu - 3 realna cisla od 0 do 1 (slozky R,G,B): ";
	cin >> true2;
	true3 = true1-true2;

	cout << true1 << "\nsubtraktivne smichano s";
	cout << true2 << "\ndava";
	cout << true3 ;
	cout << true1-true2 << "\n";

	return 0;
}
