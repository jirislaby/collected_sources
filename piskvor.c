#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ERROR {puts("Pouziti programu: piskvor pocet_sloupcu pocet_radku (kdo zacina)\n"); return 2;}
char zacinajici = 'x';
int a_sl;
int a_ra;

void PrintArea(const int coll, const int row, char area[coll][row]);
void xoRead(int coll, int row, char area[coll][row]);
int isWinner(const int coll, const int row, char area[coll][row]);

int main(int argc, char *argv[])
{

    int konec, row, coll;
    if ((argc == 1) || ((argc - 1) > 3)) {
        printf("chybny pocet parametru - %d\n", argc - 1);
        ERROR;
    }

    if (((atoi(argv[1])) < 5) || (atoi(argv[1])) > 26) {
        printf(" Chybny pocet sloupcu.");
        printf(" Musi but z intervalu <5.26>\n");
        ERROR;
    }

    if (((atoi(argv[2])) < 5) || (atoi(argv[2])) > 22) {
        printf(" Chybny pocet radku");
        printf(" Musi byt z intervalu <5,22>\n");
	ERROR;
    }

    coll = atoi(argv[1]);
    row = atoi(argv[2]);

    //vytvoreni dynamickeho 2-rozmerneho pole

    char p_area[coll][row];

    printf(" row=%d, coll=%d, zacinajici=%c\n", row, coll, zacinajici);

    if (argc == 3) {
        zacinajici = 'x';
    } else if (argc == 4) {
        if ((argv[3][0] == 'x') || (argv[3][0] == 'o')){
            zacinajici =  argv[3][0];
	}
        else {
            printf("Chybna zacinajici hrac - %c Musi byt 'x' nebo 'o'\n",
                   argv[3][0]);
	    return 2;
	}
    }

    //nulovani pole pred pouzitim
    memset(p_area, ' ', row * coll * sizeof(char));

    konec = 0;
    printf("------------xpapiez, 98557----------\n");
printf(" Hra piskvorky, zadavejte souradnice v poradi sloupec(pismeno)"
       " radek(cislo)\n vstup ukoncite hvezdickou '*' na pozici sloupce nebo"
       " vyhrou jednoho hrace\n");

    //hlavni cyklus
    while (!konec) {
        PrintArea(coll, row, p_area);
        xoRead(coll, row, p_area);
        konec = isWinner(coll, row, p_area);
    }

    PrintArea(coll, row, p_area);
    // v fci vitez se prohazuje 'x' a 'o'
    if (zacinajici=='x') printf("vyhrava kolecko\n");
    else printf("vyhrava krizek\n");
    return 0;
}


void xoRead(const int coll, const int row, char area[row][coll])
{
    int radek, sl, nacteno = 0;
    char sloupec;

    radek = row;
    sl = coll;


    while (!nacteno) {
        if (zacinajici == 'o')
            printf("kolecko: ");
        else
            printf("krizek: ");
//vynulovani souradnic
	sloupec = ' ';
        radek = ' ';
        
	scanf(" %c", &sloupec);
        
	if (sloupec == '*'){
	  if (zacinajici=='o') printf("Kolecko ukoncil vstup\n");
	  else printf("krizek ukoncil vstup\n");
	  exit(0);
	} 
	//ostreni vstupu souradnic
        if (!(isalpha(sloupec))) {
             puts("Souradnice musi byt pismeno(sloupec) a cislo(radek)");
	     continue;
	}
	
	scanf(" %d", &radek);  
        //vykreslovani bodu
	radek--;
	sl = toupper(sloupec) - 'A';
        a_sl = sl;              //posledni zadana souradnice
        a_ra = radek;           //posledni zadana souradnice
//	printf ("row = %d; radek = %d", row, radek);
//	printf ("coll = %d; sloupec = %d", coll, sl);
        if ((row <= radek) || (coll <= sl)) {
            puts("mimo hranice tabulky\n");
            continue;              //skoncit  ???
        }
	if ((sl<0)||(radek<0)){
	  puts("mimo hranice tabulky\n");
	  continue;
	}
        if ((area[radek][sl] != 'x') && (area[radek][sl] != 'o')) {
            area[radek][sl] = zacinajici;
            nacteno = 1;
        } else
            printf("souradnice je obsazena\n");
    }
}

#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

int isWinner(const int coll, const int row, char area[row][coll])
{

    int i = 0, win = 0, k=0;

    //mozna lepsi o zpusob vysetrovani  petice
    char kdo;
    kdo = zacinajici;

    int min_h, max_h, cnt = 1;
    min_h = max(a_sl-4, 0);
    max_h = min(a_sl+4, coll);
    //vodorovny smer
    for (i=min_h+1; i<=max_h; i++) {
	if (area[a_ra][i-1] == area[a_ra][i]) cnt++; else cnt=1;

	if (cnt >= 5) {
	    for (; cnt; cnt--)
		area[a_ra][i-cnt+1] -= 32;
	    win++;
	    break;
	}
    }

/*    if (a_sl < 5) i = -a_sl;
    else  i = -4;
    for (; i < 5; i++) {
      printf("i = %d\na_sl = %d", i, a_sl);
      printf("maximalni hodnota sloupce a_sl+i+4 = %d", (a_sl+i+4));
      if ((a_sl+i+4)>a_sl) { printf("mimoo sloupec!\n"); continue;}
      if ((area[a_ra][a_sl + i + 0] == kdo) &&
          (area[a_ra][a_sl + i + 1] == kdo) &&
          (area[a_ra][a_sl + i + 2] == kdo) &&
          (area[a_ra][a_sl + i + 3] == kdo) &&
          (area[a_ra][a_sl + i + 4] == kdo)) 
	{
                area[a_ra][a_sl + i ] -= 32;
                area[a_ra][a_sl + i + 1] -= 32;
                area[a_ra][a_sl + i + 2] -= 32;
                area[a_ra][a_sl + i + 3] -= 32;
		area[a_ra][a_sl + i + 4] -= 32;
            win++;
	    break;
        }
    }*/
//svivly smer
    cnt = 1;
    min_h = max(a_ra-4, 0);
    max_h = min(a_ra+4, row);

    for (i=min_h+1; i<=max_h; i++) {
	if (area[i-1][a_sl] == area[i][a_sl]) cnt++; else cnt=1;

	if (cnt >= 5) {
	    for (; cnt; cnt--)
		area[i-cnt+1][a_sl] -= 32;
	    win++;
	    break;
	}
    }
/*     if (a_ra < 5)
        i = -a_ra;
    else
        i = -4;
    for (; i < 5; i++) {
        if ((area[a_ra +i +0][a_sl] == kdo) &&
            (area[a_ra+i+1][a_sl] == kdo) &&
            (area[a_ra+i+2][a_sl] == kdo) &&
            (area[a_ra+i+3][a_sl] == kdo) &&
            (area[a_ra+i+4][a_sl] == kdo)) {
                 area[a_ra+i][a_sl] -= 32;
		 area[a_ra+i+1][a_sl] -= 32;
		 area[a_ra+i+2][a_sl] -= 32;
		 area[a_ra+i+3][a_sl] -= 32;
		 area[a_ra+i+4][a_sl] -= 32;
            win++;
	    break;
        }
    }*/

//uhlopricka 1
    if (a_ra < 5) i = -a_ra;
    else i = -4;
    if (a_sl < 5) k = -a_sl;
    else k=-4;
    for (; ((i < 5)&&(k<5)); i++, k++) {
        if ((area[a_ra +i +0][a_sl+i+0] == kdo) &&
            (area[a_ra+i+1][a_sl+i+1] == kdo) &&
            (area[a_ra+i+2][a_sl+i+2] == kdo) &&
            (area[a_ra+i+3][a_sl+i+3] == kdo) &&
            (area[a_ra+i+4][a_sl+i+4] == kdo)) {
                area[a_ra+i+0][a_sl+i+0] -= 32;
                area[a_ra+i+1][a_sl+i+1] -= 32;
                area[a_ra+i+2][a_sl+i+2] -= 32;
                area[a_ra+i+3][a_sl+i+3] -= 32;
	        area[a_ra+i+4][a_sl+i+4] -= 32;
    		win++;
		break;
        }
    }

    
//uhlopricka 2
    if (a_ra < 5) i = -a_ra;
    else i = -4;
    if (a_sl < 5) k = -a_sl;
    else k=-4;
    for (; ((i < 5)&&(k<5)); i++, k++) {
        if ((area[a_ra +i +0][a_sl-i+0] == kdo) &&
            (area[a_ra+i+1][a_sl-i-1] == kdo) &&
            (area[a_ra+i+2][a_sl-i-2] == kdo) &&
            (area[a_ra+i+3][a_sl-i-3] == kdo) &&
            (area[a_ra+i+4][a_sl-i-4] == kdo)) {
                area[a_ra+i+0][a_sl-i+0] -=32;
                area[a_ra+i+1][a_sl-i-1] -=32;
                area[a_ra+i+2][a_sl-i-2] -=32;
                area[a_ra+i+3][a_sl-i-3] -=32;
                area[a_ra+i+4][a_sl-i-4] -=32;
		win++;
		break;
        }
    }

//prohozeni hracu
    if (zacinajici == 'x')
        zacinajici = 'o';
    else
        zacinajici = 'x';
    return win;
}

void PrintArea(const int coll, const int row, char area[row][coll])
{

    int i, j;
    printf("\n  ");
    for (i = 0; i < coll; i++) {
        printf(" %c", 'A' + i);
    }

    for (i = 0; i < row; i++) {
	printf("\n%2d", (i + 1));
        for (j = 0; j < coll; j++) {
            printf(" %c", area[i][j]);
        }
    }
    puts("");
}
