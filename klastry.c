#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned char bajt;

typedef struct klaster_{
    int numer_koloru;
    int rozmiar;
    int wypisany;
    struct klaster_ *nast_klaster;
}Klaster;

typedef struct el_klastra_{
    int wiersz;
    int kolumna;
    struct el_klastra_ *poprz_el;
}El_klastra;

void dodaj_el(El_klastra **ostatni_el, int aktualny_wiersz, int aktualna_kolumna){
    El_klastra *nowy_ostatni_el = malloc(sizeof(El_klastra));
    nowy_ostatni_el->wiersz = aktualny_wiersz;
    nowy_ostatni_el->kolumna = aktualna_kolumna;

    El_klastra *stary_ostatni_el = *ostatni_el;
    nowy_ostatni_el->poprz_el = stary_ostatni_el;
    *ostatni_el = nowy_ostatni_el;
}

void usun_el(El_klastra **pierwszy_el, El_klastra **ostatni_el){
    El_klastra *usuwany_el = *ostatni_el;
    if (*pierwszy_el == *ostatni_el){
        *pierwszy_el = NULL;
        *ostatni_el = NULL;
        free (pierwszy_el);
    }
    else {
        *ostatni_el = (*ostatni_el)->poprz_el;
    }
    free (usuwany_el);
}

void dodaj_klaster(Klaster **pierwszy_klaster, int aktualny_kolor, int licznik_rozmiaru_klastra){
    Klaster *nowy_klaster = malloc(sizeof(Klaster));
    nowy_klaster->numer_koloru = aktualny_kolor;
    nowy_klaster->rozmiar = licznik_rozmiaru_klastra;
    nowy_klaster->wypisany = 0;
    nowy_klaster->nast_klaster = NULL;

    if (*pierwszy_klaster == NULL){
        *pierwszy_klaster = nowy_klaster;
    }
    else{
        Klaster *n = *pierwszy_klaster;

        while (n->nast_klaster != NULL)
            n = n->nast_klaster;

        n->nast_klaster = nowy_klaster;
    }
}

void wypisz_klastry(Klaster *pierwszy_klaster)
{
    bool nie_wszystkie_elementy_wypisane;
    int najmniejszy_kolor = pierwszy_klaster->numer_koloru;
    Klaster *kolejny_klaster;
    Klaster *n;
    printf("Kolor : Rozmiary\n");
    do {
        nie_wszystkie_elementy_wypisane = 0;
        for (n = pierwszy_klaster; n->nast_klaster != NULL; n = n->nast_klaster) {
            if (n->wypisany == 0) {
                for (kolejny_klaster = n->nast_klaster; kolejny_klaster->nast_klaster != NULL; kolejny_klaster = kolejny_klaster->nast_klaster) {
                    if (kolejny_klaster->wypisany == 0) {
                        break;
                    }
                }
                if (kolejny_klaster->numer_koloru < najmniejszy_kolor && kolejny_klaster->wypisany == 0) {
                    najmniejszy_kolor = kolejny_klaster->numer_koloru;
                }
            }
        }

        printf("  %3d : ", najmniejszy_kolor);
        for (n = pierwszy_klaster; n != NULL; n = n->nast_klaster) {
            if (n->numer_koloru == najmniejszy_kolor) {
                printf("%d ", n->rozmiar);
                n->wypisany = 1;
            }
        }
        printf("\n");

        for (n = pierwszy_klaster; n->nast_klaster != NULL; n = n->nast_klaster) {
            if (n->wypisany == 0) {
                najmniejszy_kolor = n->numer_koloru;
                nie_wszystkie_elementy_wypisane = 1;
                break;
            }
        }
    } while (nie_wszystkie_elementy_wypisane);
}

int main(int argc, char **argv) {

    unsigned int format;
    long int m,n;
    int i,j,aktualny_kolor,aktualny_wiersz,aktualna_kolumna,licznik_rozmiaru_klastra;
    bajt znak;
    FILE *fptr;
    fpos_t pozycja;

    if (argc !=2) {
        printf("Zle wywolany program!\n");
        exit(1);
    }

    if ((fptr = fopen(argv[1], "r")) == NULL){
        printf("Blad w odczycie pliku!\n");
        exit(1);
    }

    fseek(fptr, 1, SEEK_SET);
    fscanf(fptr, "%d", &format);

    fseek(fptr, 3, SEEK_SET);
    fgetpos( fptr, &pozycja);

    znak = fgetc(fptr);
    if (znak == '#'){
        while(1){
            znak = fgetc(fptr);
            if (znak == '\n')
                break;
        }
    }
    else {
        fseek(fptr, 3, SEEK_SET);
    }

    fscanf(fptr, "%d", &n);
    fscanf(fptr, "%d\n", &m);

    int **piksele = malloc(m*sizeof(int *));
    for (i = 0; i < m; ++i) {
        piksele[i] = malloc(n*sizeof(int));
    }

    if (format == 4){
        int k;
        long int pierwszy_bit_znaku,ostatni_bit_znaku;
        for(i=0; i<m; i++) {
            pierwszy_bit_znaku = 0;
            ostatni_bit_znaku = 7;
            while(ostatni_bit_znaku < n) {
                znak = fgetc(fptr);
                j = pierwszy_bit_znaku;
                for (k = 7; k >= 0; k--) {
                    piksele[i][j] = znak >> k & 1;
                    j++;
                }
                pierwszy_bit_znaku += 8;
                ostatni_bit_znaku += 8;
            }
        }
    }

    else if (format == 5){
        int najjasniejszy_kolor;
        fscanf(fptr, "%d\n", &najjasniejszy_kolor);
        for (i=0; i<m; i++) {
            for (j=0;j<n;j++) {
                znak = fgetc(fptr);
                piksele[i][j] = znak;
            }
        }
    }

    fclose(fptr);

    Klaster *pierwszy_klaster = NULL;
    El_klastra *pierwszy_el, *ostatni_el;

    for (i = 0; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            if (piksele[i][j] != -1) {
                licznik_rozmiaru_klastra = 1;
                pierwszy_el = NULL;
                ostatni_el = NULL;
                aktualny_kolor = piksele[i][j];
                aktualny_wiersz = i;
                aktualna_kolumna = j;
                piksele[i][j] = -1;
                pierwszy_el = malloc(sizeof(El_klastra));
                pierwszy_el->wiersz = i;
                pierwszy_el->kolumna = j;
                pierwszy_el->poprz_el = NULL;
                ostatni_el = pierwszy_el;

                while(1) {
                    if (aktualna_kolumna + 1 < n && piksele[aktualny_wiersz][aktualna_kolumna + 1] != -1 && piksele[aktualny_wiersz][aktualna_kolumna + 1] == aktualny_kolor) {
                        piksele[aktualny_wiersz][++aktualna_kolumna] = -1;
                        dodaj_el(&ostatni_el, aktualny_wiersz, aktualna_kolumna);
                    } else if (aktualny_wiersz + 1 < m && piksele[aktualny_wiersz + 1][aktualna_kolumna] != -1 && piksele[aktualny_wiersz + 1][aktualna_kolumna] == aktualny_kolor) {
                        piksele[++aktualny_wiersz][aktualna_kolumna] = -1;
                        dodaj_el(&ostatni_el, aktualny_wiersz, aktualna_kolumna);
                    } else if (aktualna_kolumna - 1 >= 0 && piksele[aktualny_wiersz][aktualna_kolumna - 1] != -1 && piksele[aktualny_wiersz][aktualna_kolumna - 1] == aktualny_kolor) {
                        piksele[aktualny_wiersz][--aktualna_kolumna] = -1;
                        dodaj_el(&ostatni_el, aktualny_wiersz, aktualna_kolumna);
                    } else if (aktualny_wiersz - 1 >= 0 && piksele[aktualny_wiersz - 1][aktualna_kolumna] != -1 && piksele[aktualny_wiersz - 1][aktualna_kolumna] == aktualny_kolor) {
                        piksele[--aktualny_wiersz][aktualna_kolumna] = -1;
                        dodaj_el(&ostatni_el, aktualny_wiersz, aktualna_kolumna);
                    } else{
                        if(pierwszy_el == ostatni_el){
                            dodaj_klaster(&pierwszy_klaster,aktualny_kolor,licznik_rozmiaru_klastra);
                            break;
                        } else {
                            usun_el(&pierwszy_el, &ostatni_el);
                            licznik_rozmiaru_klastra++;
                            aktualny_wiersz = ostatni_el->wiersz;
                            aktualna_kolumna = ostatni_el->kolumna;
                        }
                    }
                }
            }
        }
    }

    wypisz_klastry(pierwszy_klaster);

    return 0;
}
