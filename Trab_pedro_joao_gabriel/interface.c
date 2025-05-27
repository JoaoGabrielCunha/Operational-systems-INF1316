#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cairo.h>
#include "aviao.h"

#define N 100 // número máximo de aviões

Aviao* frota;
int shm_id;
cairo_surface_t *aviao_img = NULL;

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    gint width, height;
    gtk_widget_get_size_request(widget, &width, &height);
    if (width == -1 || height == -1) {
        gtk_window_get_size(GTK_WINDOW(gtk_widget_get_toplevel(widget)), &width, &height);
    }

    // Fundo verde floresta
    cairo_set_source_rgb(cr, 0.13, 0.55, 0.13);
    cairo_paint(cr);

    // Pista preta no centro
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_arc(cr, width / 2, height / 2, 5, 0, 2 * G_PI);
    cairo_fill(cr);

    for (int i = 0; i < N; i++) {
        if ((frota[i].status == STATUS_VOANDO) || (frota[i].status == STATUS_PAUSADO) || (frota[i].status == STATUS_FORA_ESPACO_AEREO)  ) {
            float x = frota[i].x;
            float y = frota[i].y;

            if (x < 0.0 || x > 1.0 || y < 0.0 || y > 1.0)
                continue;

            float px = x * width;
            float py = y * height;

            // Rotação baseada na direção
            double rotacao = 0.0;
            if (frota[i].direcao == 'W') {
                rotacao = G_PI; // 180 graus
            } // 'E' mantém rotacao = 0

            cairo_save(cr);
            cairo_translate(cr, px, py);
            cairo_rotate(cr, rotacao);
            cairo_translate(cr, -10, -10);
            cairo_scale(cr,
                20.0 / cairo_image_surface_get_width(aviao_img),
                20.0 / cairo_image_surface_get_height(aviao_img));
            cairo_set_source_surface(cr, aviao_img, 0, 0);
            cairo_paint(cr);
            cairo_restore(cr);
        }
    }

    return FALSE;
}

gboolean redraw(gpointer user_data) {
    GtkWidget *widget = GTK_WIDGET(user_data);
    gtk_widget_queue_draw(widget);
    return TRUE;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <shm_id>\n", argv[0]);
        return 1;
    }

    shm_id = atoi(argv[1]);
    frota = (Aviao*)shmat(shm_id, NULL, 0);
    if (frota == (void*) -1) {
        perror("Erro ao anexar memória");
        return 1;
    }

    gtk_init(&argc, &argv);

    aviao_img = cairo_image_surface_create_from_png("aviao.png");
    if (cairo_surface_status(aviao_img) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Erro ao carregar imagem aviao.png\n");
        return 1;
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Radar Aéreo");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw), NULL);

    g_timeout_add(100, redraw, drawing_area);

    gtk_widget_show_all(window);
    gtk_main();

    cairo_surface_destroy(aviao_img);
    shmdt(frota);
    return 0;
}
