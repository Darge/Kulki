//Tomasz Flendrich
//Projekt koncowy na "Wstep do programowania w jezyku C"
//nr indeksu 264596
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <glib.h>
#include "headers.h"


int main(int argc,char *argv[])
{
    srand(time(NULL));
    gtk_init(&argc, &argv);
    GtkWidget *okno_glowne=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(okno_glowne),"Kulki");
    g_signal_connect(G_OBJECT(okno_glowne),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    GtkWidget *glowny_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(okno_glowne), glowny_box);
    GtkWidget *poprzeczny = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    stan.punkty_label=gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(poprzeczny), stan.punkty_label,TRUE,TRUE,10);
    gtk_box_pack_start(GTK_BOX(glowny_box), GTK_WIDGET(poprzeczny), TRUE, TRUE, 0);
    stan.nowa_gra_przycisk=gtk_button_new_with_label("Nowa gra");
    gtk_box_pack_start(GTK_BOX(poprzeczny), stan.nowa_gra_przycisk, TRUE, TRUE, 10);
    stan.wiazania[RozmiarPlanszy*RozmiarPlanszy]=g_signal_connect(stan.nowa_gra_przycisk, "clicked", G_CALLBACK(nowa_gra), 0);
    GtkWidget *siatka = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(glowny_box), siatka, TRUE, TRUE, 0);
    gtk_grid_set_row_homogeneous(GTK_GRID(siatka), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(siatka), TRUE);

    GdkColor color;
    color.red = 0xEDED;
    color.green = 0xEDED;
    color.blue = 0xEDED;
    gtk_widget_modify_bg(okno_glowne, GTK_STATE_NORMAL, &color);

    for (int i=0; i<RozmiarPlanszy; i++)
        for (int j=0; j<RozmiarPlanszy; j++)
        {
            stan.guzik[i][j]=gtk_button_new();
            gtk_grid_attach(GTK_GRID(siatka), stan.guzik[i][j], i, j, 1, 1);
        }
    for (int i=0; i<RozmiarPlanszy; i++)
        for (int j=0; j<RozmiarPlanszy; j++)
        {
            int wspolrzedne=koduj_wspolrzedne(i, j);
            stan.wiazania[i*RozmiarPlanszy+j]=g_signal_connect(stan.guzik[i][j], "clicked", G_CALLBACK(klikniecie_przycisku), (gpointer)wspolrzedne);
            gtk_widget_modify_bg(stan.guzik[i][j], GTK_STATE_NORMAL, &color);
        }
    gtk_box_pack_start(GTK_BOX(poprzeczny), gtk_label_new("Nastepne kolory:"), TRUE, TRUE, 0);
    for (int i=0; i<KulkiCoRuch; i++)
    {
        stan.nastepne[i]=gtk_image_new_from_file("0.jpg");
        gtk_box_pack_start(GTK_BOX(poprzeczny), stan.nastepne[i], TRUE, TRUE, 4);
    }
    nowa_gra(stan.nowa_gra_przycisk, 0);
    gtk_widget_show_all(okno_glowne);
    gtk_main();
    return 0;
}
