#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 /*zahlavi */
typedef struct sOsoba {
	char jmeno[30];
	char prijmeni[30];
	int rok_narozeni;
	struct sOsoba *next;
} Osoba;

/*prototyp funkce*/
Osoba *napln(char *fname, char *lname, int rok, Osoba * uk);

/*Deklarace funkce ktera je typu ukazatel na strukturu Osoba.
Funkce alokuje pamet pro dalsi polozku "zasobniku",
vyplni hodnoty nove polozky dle parametru
a pripoji ji jakoby na zacatek "zasobniku".
Funkce vraci ukazatel na posledni pripojeny prvek.*/
Osoba *napln(char *fname, char *lname, int rok, Osoba * uk)
{
	Osoba *P;
	P = (Osoba *) malloc(sizeof(Osoba));	/*Alokace pameti */
	if (P == NULL) {
		printf("Malo pameti\n");
		exit(1);
	}
	P->next = uk;		/*nastavi ukazatel na nasledujici polozku */

	/*vyplneni pridavane polozky daty */
	P->rok_narozeni = rok;
	strncpy(P->jmeno, fname, 16);
	strncpy(P->prijmeni, lname, 20);
	P->jmeno[15] = '\0';
	P->prijmeni[20] = '\0';
	return P;
}
int main(int argc, char *argv[])
{

	/*deklarace promennych */
	FILE *fr;
	Osoba *os1, *os2;
	char *ret1, *ret2, *ret3, *radek, **Tret;
	int rok, arok, prok = 0, Trok = 0, t = 0;
	time_t *cas;

	/*alokovani pameti */
	radek = (char *)malloc(100 * sizeof(char));
	cas = (time_t *) malloc(sizeof(time_t));

	/*zjisteni aktualniho casu */
	if (time(cas) == -1) {
		printf("Nepodaril se zjistit aktualni cas.");
		return 1;
	}
	strftime(radek, 4, "%Y", localtime(cas));
	arok = strtod(radek, Tret);

	/*otevreni souboru */
	if (argc == 2) {
		if ((fr = fopen(argv[1], "r")) == NULL) {
			printf("Neplatne jmeno souboru.\n");
			return 1;
		}
	}

	else {
		printf("Pouziti: prumvek nazev_souboru\n");
		return 1;
	}			/*osetreni spravnosti vstupnich parametru */
	os1 = NULL;
	while (!feof(fr)) {	/*cyklus vytvori zasobnik a kontroluje vstupni data */

		do {
			ret1 = strtok(fgets(radek, 100, fr), " \t\n");
		} while ((ret1 == NULL) && (!feof(fr)));
		if ((feof(fr)) && (ret1 != NULL))
			t = 1;
		if ((feof(fr)) && (ret1 == NULL))
			break;
		if (t != 1)
			if ((ret2 = strtok(NULL, " \t\n")) == NULL)
				t = 1;
		if (t != 1)
			if ((ret3 = strtok(NULL, " \t\n")) == NULL)
				t = 1;
		if (t == 1) {
			printf("Spatny format souboru!\n");
			return t;
		}
		rok = strtod(ret3, Tret);
		if ((rok <= 0) || (rok > arok)) {
			printf("Nesmyslne datum narozeni: %s %s %d\n", ret1,
			       ret2, rok);
		}

		else {		/*kdyz data splnuji vsechny podminky ulozi je do zasobniku */
			os2 = napln(ret1, ret2, rok, os1);
			t = 2;	/*indikuje ze zasobnik obsahuje alespon 1 zaznam */
			os1 = os2;
		}
	}
	fclose(fr);		/*uzavreni souboru */
	if (t == 2) {		/*kdy ulozil alespon jednu strukturu zpracuje vysledek */
		while (1) {	/*projde zasobbnik a ulozi si potrebna data pro pocitani prumeru */
			prok += (arok - os2->rok_narozeni);
			Trok++;
			if (os2->next != NULL)
				os2 = os2->next;

			else
				break;
		}
		prok = (int)prok / Trok;	/*prumer */
		printf("Prumerny vek osob v souboru %s \nv roce %d je: %d \n",
		       argv[1], arok, prok);
		printf("V tomto veku jsou:\n");
		os2 = os1;
		while (1) {	/*prochazi zasobnik a kdyz se prumer rovna veku vypise osobu */
			if ((arok - os2->rok_narozeni) == prok)
				printf("%s %s \n", os2->jmeno, os2->prijmeni);
			if (os2->next != NULL)
				os2 = os2->next;

			else
				break;
		}
	}
	return 0;
}
