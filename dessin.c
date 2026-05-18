#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <stdlib.h>

// ─── État du dessin ───────────────────────────────────────────
typedef struct {
    cairo_surface_t *surface;
    gboolean drawing;
    double last_x, last_y;
    double pressure;       // simulée via vitesse
    double hue;            // couleur courante (HSV)
} DrawState;

DrawState state = {0};

// ─── Conversion HSV → RGB ─────────────────────────────────────
void hsv_to_rgb(double h, double s, double v,
                double *r, double *g, double *b) {
    int i = (int)(h * 6);
    double f = h * 6 - i;
    double p = v * (1 - s);
    double q = v * (1 - f * s);
    double t = v * (1 - (1 - f) * s);
    switch (i % 6) {
        case 0: *r=v; *g=t; *b=p; break;
        case 1: *r=q; *g=v; *b=p; break;
        case 2: *r=p; *g=v; *b=t; break;
        case 3: *r=p; *g=q; *b=v; break;
        case 4: *r=t; *g=p; *b=v; break;
        case 5: *r=v; *g=p; *b=q; break;
    }
}

// ─── Initialise la surface blanche ────────────────────────────
static void init_surface(GtkWidget *widget) {
    if (state.surface)
        cairo_surface_destroy(state.surface);

    state.surface = gdk_window_create_similar_surface(
        gtk_widget_get_window(widget),
        CAIRO_CONTENT_COLOR,
        gtk_widget_get_allocated_width(widget),
        gtk_widget_get_allocated_height(widget));

    cairo_t *cr = cairo_create(state.surface);
    cairo_set_source_rgb(cr, 1, 1, 1); // fond blanc
    cairo_paint(cr);
    cairo_destroy(cr);
}

// ─── Tracé d'un segment naturel ───────────────────────────────
static void draw_brush_stroke(double x1, double y1,
                               double x2, double y2,
                               double width) {
    cairo_t *cr = cairo_create(state.surface);

    // Couleur avec légère transparence (effet encre)
    double r, g, b;
    hsv_to_rgb(state.hue, 0.85, 0.25, &r, &g, &b);
    cairo_set_source_rgba(cr, r, g, b, 0.75);

    // Caps arrondis = trait naturel
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_width(cr, width);

    // Courbe de Bézier quadratique pour adoucir le trait
    double mx = (x1 + x2) / 2.0;
    double my = (y1 + y2) / 2.0;

    cairo_move_to(cr, x1, y1);
    cairo_curve_to(cr, x1, y1, mx, my, x2, y2);
    cairo_stroke(cr);

    cairo_destroy(cr);
}

// ─── Rendu à l'écran ──────────────────────────────────────────
static gboolean on_draw(GtkWidget *widget, cairo_t *cr,
                         gpointer data) {
    if (!state.surface)
        init_surface(widget);
    cairo_set_source_surface(cr, state.surface, 0, 0);
    cairo_paint(cr);
    return FALSE;
}

// ─── Début du tracé ───────────────────────────────────────────
static gboolean on_button_press(GtkWidget *widget,
                                 GdkEventButton *event,
                                 gpointer data) {
    if (!state.surface)
        init_surface(widget);

    if (event->button == GDK_BUTTON_PRIMARY) {
        state.drawing = TRUE;
        state.last_x  = event->x;
        state.last_y  = event->y;
        // Nouvelle couleur à chaque tracé
        state.hue = (double)rand() / RAND_MAX;
    }
    return TRUE;
}

// ─── Fin du tracé ─────────────────────────────────────────────
static gboolean on_button_release(GtkWidget *widget,
                                   GdkEventButton *event,
                                   gpointer data) {
    state.drawing = FALSE;
    return TRUE;
}

// ─── Mouvement de la souris / stylet ──────────────────────────
static gboolean on_motion(GtkWidget *widget,
                           GdkEventMotion *event,
                           gpointer data) {
    if (!state.drawing) return TRUE;

    double dx = event->x - state.last_x;
    double dy = event->y - state.last_y;
    double speed = sqrt(dx*dx + dy*dy);

    // Plus on va vite → trait plus fin (comme un vrai stylo)
    double width = CLAMP(8.0 - speed * 0.15, 1.5, 8.0);

    draw_brush_stroke(state.last_x, state.last_y,
                      event->x,    event->y,
                      width);

    state.last_x = event->x;
    state.last_y = event->y;

    gtk_widget_queue_draw(widget);
    return TRUE;
}

// ─── Touche clavier ───────────────────────────────────────────
static gboolean on_key_press(GtkWidget *widget,
                              GdkEventKey *event,
                              gpointer data) {
    // C = effacer le canvas
    if (event->keyval == GDK_KEY_c || event->keyval == GDK_KEY_C) {
        cairo_t *cr = cairo_create(state.surface);
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_paint(cr);
        cairo_destroy(cr);
        gtk_widget_queue_draw(widget);
    }
    // S = sauvegarder en PNG
    if (event->keyval == GDK_KEY_s || event->keyval == GDK_KEY_S) {
        cairo_surface_write_to_png(state.surface, "dessin.png");
        g_print("✅ Sauvegardé dans dessin.png\n");
    }
    return FALSE;
}

// ─── Main ─────────────────────────────────────────────────────
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Dessin naturel en C");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 650);

    GtkWidget *canvas = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), canvas);

    // Activer les événements nécessaires
    gtk_widget_add_events(canvas,
        GDK_BUTTON_PRESS_MASK   |
        GDK_BUTTON_RELEASE_MASK |
        GDK_POINTER_MOTION_MASK);

    g_signal_connect(canvas, "draw",                G_CALLBACK(on_draw),           NULL);
    g_signal_connect(canvas, "button-press-event",  G_CALLBACK(on_button_press),   NULL);
    g_signal_connect(canvas, "button-release-event",G_CALLBACK(on_button_release), NULL);
    g_signal_connect(canvas, "motion-notify-event", G_CALLBACK(on_motion),         NULL);
    g_signal_connect(window, "key-press-event",     G_CALLBACK(on_key_press),      NULL);
    g_signal_connect(window, "destroy",             G_CALLBACK(gtk_main_quit),     NULL);

    gtk_widget_show_all(window);
    gtk_main();

    if (state.surface)
        cairo_surface_destroy(state.surface);

    return 0;
}