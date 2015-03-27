//Tomasz Flendrich
//Projekt koncowy na "Wstep do programowania w jezyku C"
//nr indeksu 264596
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>

#define RozmiarPlanszy 9
#define LiczbaKolorow 7
#define KulkiNaStart 5
#define KulekKoloSiebie 5 //ile kulek musi byc kolo siebie aby uzyskac punkty
#define KulkiCoRuch 3//ilosc kulek dodanych co ruch
#define CzasAnimacji 200 //czas trwania animacji w milisekundach

struct gra
{
    GtkWidget *guzik[RozmiarPlanszy][RozmiarPlanszy];
    GtkWidget *punkty_label;
    GtkWidget *nastepne[KulkiCoRuch];
    GtkWidget *nowa_gra_przycisk;
    int plansza[RozmiarPlanszy][RozmiarPlanszy];
    int nastepne_int[KulkiCoRuch];
    int wiazania[RozmiarPlanszy*RozmiarPlanszy+1]; //id_handlery sluzace blokowaniu i odblokowywaniu wszystkich przyciskow na czas animacji
    int ilosc_kulek;
    int klikniety_X, klikniety_Y;
    int punkty;
    bool czy_klikniety; //false jesli teraz gracz wybiera ktora kulke przesunac, true jesli gracz wybiera gdzie ja przesunac
};

struct gra stan;

int koduj_wspolrzedne(int x, int y);
int dekoduj_wspolrzedna_X(int liczba);
int dekoduj_wspolrzedna_Y(int liczba);
void dodaj_kulki();
void nowa_gra_dialog(GtkWidget *niewazne, GtkWidget *okno);
GtkWidget* jaki_kolor(int kolor);
void zmien_kolor(int x, int y, int kolor);
void losuj_nastepne();
void nowa_gra(GtkWidget *niewazne, int niewazne2);
void klikniecie_przycisku(GtkWidget *niewazne, int wspolrzedne);
void uaktualnij_punkty();
void sprawdzenie_kulek();
void animacja_przejscia(int dojscie[][RozmiarPlanszy], int kandydat_X, int kandydat_Y);
gboolean animacja_przejscia_2(void* trasa);
