#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

struct t_lidi {
	char jmeno[16];
	char prijmeni[21];
	int rok_narozeni;
	struct t_lidi *next;
} *lidi;

int soucet = 0, pocet = 0, prumer = 0;

int napln(char *filename, int rok);

int main(int argc, char **argv)
{
	time_t cas;
	int aktual_rok;
	cas = time(NULL);
	aktual_rok = localtime(&cas)->tm_year;

	lidi = (struct t_lidi *)malloc(sizeof(struct t_lidi));
	lidi->next = NULL;

	char *filename;

	if (argc != 2) {
		puts("Spousteni prumvek soubor\n");
		return 1;
	}
	filename = argv[1];

	napln(filename, aktual_rok);

	printf("ok");

	return 0;
}

int napln(char *filename, int rok)
{
	FILE *fr;

	char radek[101], *meno, *surname, *cislo, *zbytek;

	if (!(fr = fopen(filename, "r"))) {
		puts("soubor nelze otevrit");
		return 2;
	}
	struct t_lidi *tmp = lidi;
	char delims[] = " \t";

	int vek;
	while (fgets(radek, 101, fr) != NULL) {
		//   fgets (radek, 101, fr);
		meno = strtok(radek, delims);
		if (meno == NULL)
			continue;
		surname = strtok(NULL, delims);
		if (surname == NULL)
			continue;
		cislo = strtok(NULL, delims);
		if (cislo == NULL)
//      continue;
			//podminky
			if (strlen(meno) > 15)
				meno[15] = 0;
		if (strlen(surname) > 20)
			surname[20] = 0;
		if ((cislo[strlen(cislo) - 2] == ' ')
		    || (cislo[strlen(cislo) - 1] == '\n'))
			cislo[strlen(cislo) - 1] = 0;

//      printf("%s ",meno);
//      printf(" %s ",surname);
		strtol(cislo, &zbytek, 10);
//    printf("rok = %s", cislo);
//    printf(" velikost=%d, obs=%c\n",strlen(cislo),cislo[strlen(cislo)-2]);
		//vytvoreni struktury
//naplneni 
		if (!zbytek[0]) {

			//tmp->next = lidi;
			strcpy(tmp->jmeno, meno);
			printf("v tmp->jmeno je: %s \n", tmp->jmeno);
			strcpy(tmp->prijmeni, surname);
			printf("v tmp->prijmeni je: %s \n", tmp->prijmeni);
			tmp->rok_narozeni = atoi(cislo);
			printf("v tmp->rok_narozeni je: %d \n",
			       tmp->rok_narozeni);
			vek = (1900 + rok) - tmp->rok_narozeni;
			printf(" vek = %d ", vek);
			soucet += vek;
			printf(" soucet = %d ", soucet);
			pocet++;
			printf(" pocet = %d \n", pocet);
//tmp->next=lidi;
			tmp = (struct t_lidi *)malloc(sizeof(struct t_lidi));
			tmp->next = lidi;
			lidi = tmp;
		} else
			continue;
	}

//   lidi=tmp;
	free(tmp);

	fclose(fr);
    /***********************************************************/
	prumer = soucet / pocet;
	printf(" prumer = %d ", prumer);
	//vypis
	int i;
	for (i = 0; i < pocet; i++) {
		vek = (rok + 1900) - lidi->rok_narozeni;
		printf("vek=%d, prumer=%d \n", vek, prumer);
//zkus zakomentovat if a pude to
		if ((prumer == vek)) {
			printf("lidi: %s, %s, %d, vek=%d \n", lidi->jmeno,
			       lidi->prijmeni, lidi->rok_narozeni,
			       2005 - (lidi->rok_narozeni));
			lidi = lidi->next;
		}
	}
	return 0;
}
