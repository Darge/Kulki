//Tomasz Flendrich
//Projekt koncowy na "Wstep do programowania w jezyku C"
//nr indeksu 264596
#include "headers.h"
extern struct gra stan;

int koduj_wspolrzedne(int x, int y)
//te trzy funkcje sluza zakodowaniu wspolrzednych, aby mozna je bylo latwo przesylac jako argumenty funkcji albo jako 1 element do kolejki
{
    return (x<<16)+y;
}
int dekoduj_wspolrzedna_X(int liczba)
{
    return liczba>>16;
}
int dekoduj_wspolrzedna_Y(int liczba)
{
    return liczba-((liczba>>16)<<16);

}
void animacja_przejscia(int dojscie[][RozmiarPlanszy], int kandydat_X, int kandydat_Y)
/* pierwsza z dwoch funkcji sluzacych do animacji, ktora blokuje dzialanie przyciskow
i pokazuje trase */
{

    int aktualna_dlugosc=dojscie[kandydat_X][kandydat_Y];

    int aktualny_X=kandydat_X;
    int aktualny_Y=kandydat_Y;
    int *trasa;
    trasa=malloc(RozmiarPlanszy*RozmiarPlanszy*sizeof(int));
    for (int i=0; i<RozmiarPlanszy*RozmiarPlanszy; i++)
        trasa[i]=0;
    trasa[0]=aktualna_dlugosc;
    while(aktualna_dlugosc>0) //odtwarzanie trasy, ktora przebiegla kulka po ruchu
    {
        if (aktualna_dlugosc!=dojscie[kandydat_X][kandydat_Y])
        {
            gtk_button_set_image(GTK_BUTTON(stan.guzik[aktualny_X][aktualny_Y]), jaki_kolor(100));
            trasa[aktualna_dlugosc]=koduj_wspolrzedne(aktualny_X, aktualny_Y);
        }

        //gora
        if (aktualna_dlugosc==1)
        {
            aktualna_dlugosc--;
            continue;
        }

        if (aktualny_Y-1>=0 && aktualny_Y-1<RozmiarPlanszy && dojscie[aktualny_X][aktualny_Y-1]==aktualna_dlugosc-1)
        {
            aktualna_dlugosc--;
            aktualny_Y--;
            continue;
        }
        //dol
        if (aktualny_Y+1>=0 && aktualny_Y+1<RozmiarPlanszy && dojscie[aktualny_X][aktualny_Y+1]==aktualna_dlugosc-1)
        {
            aktualna_dlugosc--;
            aktualny_Y++;
            continue;
        }

        //lewo
        if (aktualny_X-1>=0 && aktualny_X-1<RozmiarPlanszy && dojscie[aktualny_X-1][aktualny_Y]==aktualna_dlugosc-1)
        {
            aktualna_dlugosc--;
            aktualny_X--;
            continue;
        }

        //prawo
        if (aktualny_X+1>=0 && aktualny_X+1<RozmiarPlanszy && dojscie[aktualny_X+1][aktualny_Y]==aktualna_dlugosc-1)
        {
            aktualna_dlugosc--;
            aktualny_X++;
            continue;
        }

    }

    //blokowanie stanu.guzikow
    for (int i=0; i<RozmiarPlanszy; i++)
        for (int j=0; j<RozmiarPlanszy; j++)
            g_signal_handler_disconnect(stan.guzik[i][j], stan.wiazania[i*RozmiarPlanszy+j]);

    g_signal_handler_disconnect(stan.nowa_gra_przycisk, stan.wiazania[RozmiarPlanszy*RozmiarPlanszy]);
    g_timeout_add(CzasAnimacji,(GSourceFunc) animacja_przejscia_2, trasa);




}

gboolean animacja_przejscia_2(gpointer trasa)
//druga z funkcji sluzacych do animacji, odblokowuje dzialanie przyciskow oraz usuwa slad przejscia kulki
{
    int *tablica=(int*)trasa;
    for (int i=1; i<tablica[0]; i++)
    {
        int x=dekoduj_wspolrzedna_X(tablica[i]);
        int y=dekoduj_wspolrzedna_Y(tablica[i]);
        zmien_kolor(x, y, stan.plansza[x][y]);
    }

    stan.czy_klikniety=false;
    int punkty_teraz=stan.punkty;
    sprawdzenie_kulek();
    if (punkty_teraz==stan.punkty)
        dodaj_kulki();
    free(trasa);
    for (int i=0; i<RozmiarPlanszy; i++)
        for (int j=0; j<RozmiarPlanszy; j++)
        {
            int wspolrzedne=koduj_wspolrzedne(i, j);

            stan.wiazania[i*RozmiarPlanszy+j]=g_signal_connect(stan.guzik[i][j], "clicked", G_CALLBACK(klikniecie_przycisku), (gpointer)wspolrzedne);
        }
    stan.wiazania[RozmiarPlanszy*RozmiarPlanszy]=g_signal_connect(GTK_BUTTON(stan.nowa_gra_przycisk), "clicked", G_CALLBACK(nowa_gra), 0);
    return false;
}

void dodaj_kulki()
//dodanie kulek po kazdym ruchu
{
    for (int i=0; i<KulkiCoRuch; i++)
    {
        stan.ilosc_kulek++;
        int wspolrzedna_X;
        int wspolrzedna_Y;
        while (true)
        {
            wspolrzedna_X=rand()%RozmiarPlanszy;
            wspolrzedna_Y=rand()%RozmiarPlanszy;
            if (stan.plansza[wspolrzedna_X][wspolrzedna_Y]==0)
                break;
        }
        stan.plansza[wspolrzedna_X][wspolrzedna_Y]=stan.nastepne_int[i];
        zmien_kolor(wspolrzedna_X, wspolrzedna_Y, stan.nastepne_int[i]);
        if (stan.ilosc_kulek==RozmiarPlanszy*RozmiarPlanszy) //jesli cala plansza jest wypelniona, to koniec gry
        {
            if (i==RozmiarPlanszy-1)
            {
                int punkty_teraz=stan.punkty;
                sprawdzenie_kulek(); //sprawdzenie, czy dodane teraz kulki nie kwalifikuja sie do znikniecia
                if (punkty_teraz!=stan.punkty)
                    break;
            }
            GtkWidget *koniec_gry_okno=gtk_dialog_new();
            char wiadomosc[50];
            sprintf(wiadomosc, "Gratulacje, uzyskales %d pkt", stan.punkty);
            gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(koniec_gry_okno))),gtk_label_new(wiadomosc), TRUE, TRUE, 0);
            gtk_widget_show_all(koniec_gry_okno);
            GtkWidget *przycisk_nowa_gra=gtk_dialog_add_button(GTK_DIALOG(koniec_gry_okno), "Nowa gra", (gint)NULL);
            g_signal_connect(przycisk_nowa_gra, "clicked", G_CALLBACK(nowa_gra_dialog), koniec_gry_okno);
            g_signal_connect(koniec_gry_okno, "destroy", G_CALLBACK(nowa_gra_dialog), koniec_gry_okno);
            gtk_window_set_resizable(GTK_WINDOW(koniec_gry_okno), FALSE);
            gtk_dialog_run(GTK_DIALOG(koniec_gry_okno));
            return;
        }
    }
    losuj_nastepne();
    sprawdzenie_kulek();
}
void nowa_gra_dialog(GtkWidget *niewazne, GtkWidget *okno)
{
    nowa_gra(niewazne, stan.punkty);
    gtk_widget_destroy (okno);

}
GtkWidget* jaki_kolor(int kolor)
//funkcja zwracajaca widget ze zdjeciem okreslonego koloru
{
    GtkWidget *zdjecie;
    char nazwa_pliku[20];
    sprintf(nazwa_pliku, "%d.jpg", kolor);
    zdjecie=gtk_image_new_from_file(nazwa_pliku);
    return zdjecie;
}
void zmien_kolor(int x, int y, int kolor)
//funkcja zmieniajaca kolor
{
    gtk_button_set_image(GTK_BUTTON(stan.guzik[x][y]), jaki_kolor(kolor));
    stan.plansza[x][y]=kolor;
}
void losuj_nastepne()
//losuje nastepne kulki oraz wyswietla je w GUI
{
    for (int i=0; i<KulkiCoRuch; i++)
    {
        int losowa=((rand()%LiczbaKolorow)+1);
        stan.nastepne_int[i]=losowa;
        char nazwa_pliku[20];
        sprintf(nazwa_pliku, "%d.jpg", losowa);
        gtk_image_set_from_file(GTK_IMAGE(stan.nastepne[i]), nazwa_pliku);
    }
}
void nowa_gra(GtkWidget *niewazne, int niewazne2)
//funkcja zmieniajaca wartosci zmiennych do stanu poczatkowego
{
    stan.punkty=0;
    stan.ilosc_kulek=KulkiNaStart;
    stan.czy_klikniety=false;
    gtk_label_set_text(GTK_LABEL(stan.punkty_label), "Liczba punktow: 0");
    for (int i=0; i<RozmiarPlanszy; i++)
        for (int j=0; j<RozmiarPlanszy; j++)
            stan.plansza[i][j]=0;

    for (int i=0; i<RozmiarPlanszy; i++)
        for (int j=0; j<RozmiarPlanszy; j++)
            gtk_button_set_image(GTK_BUTTON(stan.guzik[i][j]), gtk_image_new_from_file("0.jpg"));

    int licznik=KulkiNaStart;
    while (licznik>0)
    {
        int kolor=(rand()%LiczbaKolorow)+1;
        int x, y;
        while(TRUE)
        {
            x=rand()%RozmiarPlanszy;
            y=rand()%RozmiarPlanszy;
            if (stan.plansza[x][y]==0)
                break;
        }
        stan.plansza[x][y]=kolor;
        zmien_kolor(x, y, kolor);
        licznik--;
    }
    losuj_nastepne();
    sprawdzenie_kulek(); //szansa ok. 1 do 1,4 miliarda
}

void klikniecie_przycisku(GtkWidget *niewazne, int wspolrzedne)
//gdy sie kliknie na jakas kulke lub puste pole
{
    if (stan.czy_klikniety==false) //gdy zadna kulka nie jest aktualnie wybrana
    {
        stan.klikniety_X=dekoduj_wspolrzedna_X(wspolrzedne);
        stan.klikniety_Y=dekoduj_wspolrzedna_Y(wspolrzedne);
        if (stan.plansza[stan.klikniety_X][stan.klikniety_Y]==0)
            return;
        stan.czy_klikniety=true;
        gtk_button_set_image(GTK_BUTTON(stan.guzik[stan.klikniety_X][stan.klikniety_Y]), jaki_kolor(-1*stan.plansza[stan.klikniety_X][stan.klikniety_Y]));
    }
    else
    {
        int kandydat_X=dekoduj_wspolrzedna_X(wspolrzedne);
        int kandydat_Y=dekoduj_wspolrzedna_Y(wspolrzedne);
        if (kandydat_X==stan.klikniety_X && kandydat_Y==stan.klikniety_Y) //odznaczenie aktualnie kliknietego
        {
            stan.czy_klikniety=false;
            gtk_button_set_image(GTK_BUTTON(stan.guzik[stan.klikniety_X][stan.klikniety_Y]), jaki_kolor(stan.plansza[stan.klikniety_X][stan.klikniety_Y]));
            return;
        }
        if (stan.plansza[kandydat_X][kandydat_Y]!=0) //klikniecie na pole, na ktorym jest juz kulka
        {
            zmien_kolor(stan.klikniety_X, stan.klikniety_Y, stan.plansza[stan.klikniety_X][stan.klikniety_Y]);
            stan.klikniety_X=kandydat_X;
            stan.klikniety_Y=kandydat_Y;
            gtk_button_set_image(GTK_BUTTON(stan.guzik[stan.klikniety_X][stan.klikniety_Y]), jaki_kolor(-1*stan.plansza[stan.klikniety_X][stan.klikniety_Y]));
            return;
        }
        //BFS
        int dojscie[RozmiarPlanszy][RozmiarPlanszy]; //aby mozna bylo odtworzyc trase, ktora przeszla kulka
        for (int i=0; i<RozmiarPlanszy; i++)
            for (int j=0; j<RozmiarPlanszy; j++)
                dojscie[i][j]=0;
        dojscie[stan.klikniety_X][stan.klikniety_Y]=1;
        int kolejka[RozmiarPlanszy*RozmiarPlanszy];
        int poczatek=0, koniec=1;
        kolejka[0]=koduj_wspolrzedne(stan.klikniety_X, stan.klikniety_Y);
        while (poczatek!=koniec) //petla ta zostanie przerwana takze jesli istnieje droga od kliknietego do kandydata
        {

            int aktualny_X=dekoduj_wspolrzedna_X(kolejka[poczatek]);
            int aktualny_Y=dekoduj_wspolrzedna_Y(kolejka[poczatek]);
            poczatek++;
            //sprawdzenie, czy ten aktualny to szukany
            if (aktualny_X==kandydat_X && aktualny_Y==kandydat_Y)
            {
                zmien_kolor(kandydat_X, kandydat_Y, stan.plansza[stan.klikniety_X][stan.klikniety_Y]);
                zmien_kolor(stan.klikniety_X, stan.klikniety_Y, 0);
                animacja_przejscia(dojscie, kandydat_X, kandydat_Y);
                return;
            }

            //gora
            if (aktualny_Y-1!=-1 && stan.plansza[aktualny_X][aktualny_Y-1]==0 && dojscie[aktualny_X][aktualny_Y-1]==0)
            {
                dojscie[aktualny_X][aktualny_Y-1]=dojscie[aktualny_X][aktualny_Y]+1;
                kolejka[koniec]=koduj_wspolrzedne(aktualny_X, aktualny_Y-1);
                koniec++;
            }
            //dol
            if (aktualny_Y+1!=RozmiarPlanszy && stan.plansza[aktualny_X][aktualny_Y+1]==0 && dojscie[aktualny_X][aktualny_Y+1]==0)
            {
                dojscie[aktualny_X][aktualny_Y+1]=dojscie[aktualny_X][aktualny_Y]+1;
                kolejka[koniec]=koduj_wspolrzedne(aktualny_X, aktualny_Y+1);
                koniec++;
            }
            //lewo
            if (aktualny_X-1!=-1 && stan.plansza[aktualny_X-1][aktualny_Y]==0 && dojscie[aktualny_X-1][aktualny_Y]==0)
            {
                dojscie[aktualny_X-1][aktualny_Y]=dojscie[aktualny_X][aktualny_Y]+1;
                kolejka[koniec]=koduj_wspolrzedne(aktualny_X-1, aktualny_Y);
                koniec++;
            }
            //prawo
            if (aktualny_X+1!=RozmiarPlanszy && stan.plansza[aktualny_X+1][aktualny_Y]==0 && dojscie[aktualny_X+1][aktualny_Y]==0)
            {
                dojscie[aktualny_X+1][aktualny_Y]=dojscie[aktualny_X][aktualny_Y]+1;
                kolejka[koniec]=koduj_wspolrzedne(aktualny_X+1, aktualny_Y);
                koniec++;
            }
        }
    }
}
void uaktualnij_punkty()
{
    char tekst[20];
    sprintf(tekst, "Liczba punktow: %d", stan.punkty);
    gtk_label_set_text(GTK_LABEL(stan.punkty_label), tekst);
}

void sprawdzenie_kulek()
//sprawdzenie, czy na planszy nie ma kulek kwalifikujacych sie do zniknicia
{
    //pionowo
    for (int i=0; i<RozmiarPlanszy; i++)
    {
        int aktualnie_jednego_koloru=0;
        for (int j=0; j<RozmiarPlanszy; j++)
        {
            if (j==0 && stan.plansza[i][j]!=0)
            {
                aktualnie_jednego_koloru=1;
                continue;
            }
            if (j==0 && stan.plansza[i][j]==0)
                continue;
            if (stan.plansza[i][j]!=stan.plansza[i][j-1])
            {
                if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                {
                    while (aktualnie_jednego_koloru!=0)
                    {
                        zmien_kolor(i, j-aktualnie_jednego_koloru, 0);
                        stan.punkty++;
                        stan.ilosc_kulek--;
                        aktualnie_jednego_koloru--;
                    }
                    uaktualnij_punkty();
                }
                if (stan.plansza[i][j]!=0)
                    aktualnie_jednego_koloru=1;
                continue;
            }
            if (stan.plansza[i][j]==stan.plansza[i][j-1] && stan.plansza[i][j]!=0)
                aktualnie_jednego_koloru++;

            if (j==RozmiarPlanszy-1)
            {
                if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                {
                    while (aktualnie_jednego_koloru!=0)
                    {
                        zmien_kolor(i, j-aktualnie_jednego_koloru+1, 0);
                        stan.punkty++;
                        stan.ilosc_kulek--;
                        aktualnie_jednego_koloru--;
                    }
                    uaktualnij_punkty();
                }
            }
        }
    }
    //poziomo
    for (int i=0; i<RozmiarPlanszy; i++)
    {
        int aktualnie_jednego_koloru=0;
        for (int j=0; j<RozmiarPlanszy; j++)
        {
            if (j==0 && stan.plansza[j][i]!=0)
            {
                aktualnie_jednego_koloru=1;
                continue;
            }
            if (j==0 && stan.plansza[j][i]==0)
                continue;
            if (stan.plansza[j][i]!=stan.plansza[j-1][i])
            {
                if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                {
                    while (aktualnie_jednego_koloru!=0)
                    {
                        zmien_kolor(j-aktualnie_jednego_koloru, i, 0);
                        stan.punkty++;
                        stan.ilosc_kulek--;
                        aktualnie_jednego_koloru--;
                    }
                    uaktualnij_punkty();
                }
                if (stan.plansza[j][i]!=0)
                    aktualnie_jednego_koloru=1;
                continue;
            }
            if (stan.plansza[j][i]==stan.plansza[j-1][i] && stan.plansza[j][i]!=0)
                aktualnie_jednego_koloru++;

            if (j==RozmiarPlanszy-1)
            {
                if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                {
                    while (aktualnie_jednego_koloru!=0)
                    {
                        zmien_kolor(j-aktualnie_jednego_koloru+1, i, 0);
                        stan.punkty++;
                        stan.ilosc_kulek--;
                        aktualnie_jednego_koloru--;
                    }
                    uaktualnij_punkty();
                }
            }

        }
    }

    //ukos idacy z lewego gornego rogu
    //ukosy lecace z osi x
    for (int i=0; i<RozmiarPlanszy; i++)
    {
        int pozycja_X=i;
        int pozycja_Y=0;
        int aktualnie_jednego_koloru=0;
        while(pozycja_X!=RozmiarPlanszy && pozycja_Y!=RozmiarPlanszy)
        {
            if (pozycja_Y==0)
            {
                if (stan.plansza[pozycja_X][pozycja_Y]!=0)
                {
                    aktualnie_jednego_koloru++;
                }
                pozycja_X++;
                pozycja_Y++;
                continue;
            }
            if (stan.plansza[pozycja_X][pozycja_Y]!=stan.plansza[pozycja_X-1][pozycja_Y-1])
            {
                if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                {
                    while (aktualnie_jednego_koloru!=0)
                    {
                        zmien_kolor(pozycja_X-aktualnie_jednego_koloru, pozycja_Y-aktualnie_jednego_koloru, 0);
                        stan.punkty++;
                        stan.ilosc_kulek--;
                        aktualnie_jednego_koloru--;
                    }
                }
                uaktualnij_punkty();
                aktualnie_jednego_koloru=0;
                if (stan.plansza[pozycja_X][pozycja_Y]!=0)
                    aktualnie_jednego_koloru++;

                pozycja_X++;
                pozycja_Y++;
                continue;
            }
            if (stan.plansza[pozycja_X][pozycja_Y]==stan.plansza[pozycja_X-1][pozycja_Y-1] && stan.plansza[pozycja_X][pozycja_Y]!=0)
            {
                aktualnie_jednego_koloru++;
                if (pozycja_X==RozmiarPlanszy-1 || pozycja_Y==RozmiarPlanszy-1)
                {
                    if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                    {
                        while(aktualnie_jednego_koloru!=0)
                        {
                            zmien_kolor(pozycja_X-aktualnie_jednego_koloru+1, pozycja_Y-aktualnie_jednego_koloru+1, 0);
                            stan.punkty++;
                            stan.ilosc_kulek--;
                            aktualnie_jednego_koloru--;
                        }
                        uaktualnij_punkty();
                    }
                }
                pozycja_X++;
                pozycja_Y++;
                continue;
            }

            if (stan.plansza[pozycja_X][pozycja_Y]==0)
            {
                aktualnie_jednego_koloru=0;
                pozycja_X++;
                pozycja_Y++;
            }

        }
    }

    //ukosy lecace z osi Y
    for (int i=0; i<RozmiarPlanszy; i++)
    {
        int pozycja_X=0;
        int pozycja_Y=i;
        int aktualnie_jednego_koloru=0;
        while(pozycja_X!=RozmiarPlanszy && pozycja_Y!=RozmiarPlanszy)
        {
            if (pozycja_X==0)
            {
                if (stan.plansza[pozycja_X][pozycja_Y]!=0)
                {
                    aktualnie_jednego_koloru++;
                }
                pozycja_X++;
                pozycja_Y++;
                continue;
            }
            if (stan.plansza[pozycja_X][pozycja_Y]!=stan.plansza[pozycja_X-1][pozycja_Y-1])
            {
                if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                {
                    while (aktualnie_jednego_koloru!=0)
                    {
                        zmien_kolor(pozycja_X-aktualnie_jednego_koloru, pozycja_Y-aktualnie_jednego_koloru, 0);
                        stan.punkty++;
                        stan.ilosc_kulek--;
                        aktualnie_jednego_koloru--;
                    }
                }
                uaktualnij_punkty();
                aktualnie_jednego_koloru=0;
                if (stan.plansza[pozycja_X][pozycja_Y]!=0)
                    aktualnie_jednego_koloru++;

                pozycja_X++;
                pozycja_Y++;
                continue;
            }
            if (stan.plansza[pozycja_X][pozycja_Y]==stan.plansza[pozycja_X-1][pozycja_Y-1] && stan.plansza[pozycja_X][pozycja_Y]!=0)
            {
                aktualnie_jednego_koloru++;
                if (pozycja_X==RozmiarPlanszy-1 || pozycja_Y==RozmiarPlanszy-1)
                {
                    if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                    {
                        while(aktualnie_jednego_koloru!=0)
                        {
                            zmien_kolor(pozycja_X-aktualnie_jednego_koloru+1, pozycja_Y-aktualnie_jednego_koloru+1, 0);
                            stan.punkty++;
                            stan.ilosc_kulek--;
                            aktualnie_jednego_koloru--;
                        }
                        uaktualnij_punkty();
                    }
                }
                pozycja_X++;
                pozycja_Y++;
                continue;
            }

            if (stan.plansza[pozycja_X][pozycja_Y]==0)
            {
                aktualnie_jednego_koloru=0;
                pozycja_X++;
                pozycja_Y++;
            }

        }
    }


    //ukos idace z gory z prawej

    //ukosy z osi X
    for (int i=0; i<RozmiarPlanszy; i++)
    {
        int pozycja_X=i;
        int pozycja_Y=0;
        int aktualnie_jednego_koloru=0;
        while(pozycja_X!=-1 && pozycja_Y!=RozmiarPlanszy)
        {
            if (pozycja_Y==0)
            {
                if (stan.plansza[pozycja_X][pozycja_Y]!=0)
                {
                    aktualnie_jednego_koloru++;
                }
                pozycja_X--;
                pozycja_Y++;
                continue;
            }
            if (stan.plansza[pozycja_X][pozycja_Y]!=stan.plansza[pozycja_X+1][pozycja_Y-1])
            {
                if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                {
                    while (aktualnie_jednego_koloru!=0)
                    {
                        zmien_kolor(pozycja_X+aktualnie_jednego_koloru, pozycja_Y-aktualnie_jednego_koloru, 0);
                        stan.punkty++;
                        stan.ilosc_kulek--;
                        aktualnie_jednego_koloru--;
                    }
                }
                uaktualnij_punkty();
                aktualnie_jednego_koloru=0;
                if (stan.plansza[pozycja_X][pozycja_Y]!=0)
                    aktualnie_jednego_koloru++;

                pozycja_X--;
                pozycja_Y++;
                continue;
            }
            if (stan.plansza[pozycja_X][pozycja_Y]==stan.plansza[pozycja_X+1][pozycja_Y-1] && stan.plansza[pozycja_X][pozycja_Y]!=0)
            {
                aktualnie_jednego_koloru++;
                if (pozycja_X==0 || pozycja_Y==RozmiarPlanszy-1)
                {
                    if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                    {

                        while(aktualnie_jednego_koloru!=0)
                        {
                            zmien_kolor(pozycja_X+aktualnie_jednego_koloru-1, pozycja_Y-aktualnie_jednego_koloru+1, 0);
                            stan.punkty++;
                            stan.ilosc_kulek--;
                            aktualnie_jednego_koloru--;
                        }
                        uaktualnij_punkty();
                    }
                }
                pozycja_X--;
                pozycja_Y++;
                continue;
            }

            if (stan.plansza[pozycja_X][pozycja_Y]==0)
            {
                aktualnie_jednego_koloru=0;
                pozycja_X--;
                pozycja_Y++;
            }
        }
    }
    //ukosy z osi Y
    for (int i=0; i<RozmiarPlanszy; i++)
    {
        int pozycja_X=RozmiarPlanszy-1;
        int pozycja_Y=i;
        int aktualnie_jednego_koloru=0;
        while(pozycja_X!=-1 && pozycja_Y!=RozmiarPlanszy)
        {
            if (pozycja_X==RozmiarPlanszy-1)
            {
                if (stan.plansza[pozycja_X][pozycja_Y]!=0)
                {
                    aktualnie_jednego_koloru++;
                }
                pozycja_X--;
                pozycja_Y++;
                continue;
            }
            if (stan.plansza[pozycja_X][pozycja_Y]!=stan.plansza[pozycja_X+1][pozycja_Y-1])
            {
                if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                {
                    while (aktualnie_jednego_koloru!=0)
                    {
                        zmien_kolor(pozycja_X+aktualnie_jednego_koloru, pozycja_Y-aktualnie_jednego_koloru, 0);
                        stan.punkty++;
                        stan.ilosc_kulek--;
                        aktualnie_jednego_koloru--;
                    }
                }
                uaktualnij_punkty();
                aktualnie_jednego_koloru=0;
                if (stan.plansza[pozycja_X][pozycja_Y]!=0)
                    aktualnie_jednego_koloru++;

                pozycja_X--;
                pozycja_Y++;
                continue;
            }
            if (stan.plansza[pozycja_X][pozycja_Y]==stan.plansza[pozycja_X+1][pozycja_Y-1] && stan.plansza[pozycja_X][pozycja_Y]!=0)
            {
                aktualnie_jednego_koloru++;
                if (pozycja_X==0 || pozycja_Y==RozmiarPlanszy-1)
                {
                    if (aktualnie_jednego_koloru>=KulekKoloSiebie)
                    {

                        while(aktualnie_jednego_koloru!=0)
                        {
                            zmien_kolor(pozycja_X+aktualnie_jednego_koloru-1, pozycja_Y-aktualnie_jednego_koloru+1, 0);
                            stan.punkty++;
                            stan.ilosc_kulek--;
                            aktualnie_jednego_koloru--;
                        }
                        uaktualnij_punkty();
                    }
                }
                pozycja_X--;
                pozycja_Y++;
                continue;
            }

            if (stan.plansza[pozycja_X][pozycja_Y]==0)
            {
                aktualnie_jednego_koloru=0;
                pozycja_X--;
                pozycja_Y++;
            }
        }
    }

    if (stan.ilosc_kulek==0) //na wypadek gdy ktos wszystkie kulki zuzyje
        dodaj_kulki();
}
