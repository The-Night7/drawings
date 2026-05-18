#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ─── Paramètres vidéo ─────────────────────────────────────────
#define MAX_FRAMES     8000
#define FRAME_INTERVAL 30   // capture 1 frame toutes les N ms

// ─── État du dessin ───────────────────────────────────────────
typedef struct {
    cairo_surface_t *surface;
    gboolean         drawing;
    double           last_x, last_y;
    double           hue;

    // Enregistrement vidéo
    cairo_surface_t *frames[MAX_FRAMES];
    int              frame_count;
    gboolean         recording;
    guint            timer_id;
} DrawState;

DrawState state = {0};

// ─── Conversion HSV → RGB ─────────────────────────────────────
static void hsv_to_rgb(double h, double s, double v,
                        double *r, double *g, double *b) {
    int    i = (int)(h * 6);
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
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);
    cairo_destroy(cr);
}

// ─── Copie une surface Cairo ──────────────────────────────────
static cairo_surface_t *copy_surface(cairo_surface_t *src) {
    int w = cairo_image_surface_get_width(src);
    int h = cairo_image_surface_get_height(src);

    // On convertit en image surface pour pouvoir la stocker
    cairo_surface_t *img = cairo_image_surface_create(
        CAIRO_FORMAT_RGB24, w, h);
    cairo_t *cr = cairo_create(img);
    cairo_set_source_surface(cr, src, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
    return img;
}

// ─── Timer : capture une frame toutes les FRAME_INTERVAL ms ──
static gboolean capture_frame(gpointer data) {
    (void)data;
    if (!state.recording || !state.surface) return G_SOURCE_CONTINUE;
    if (state.frame_count >= MAX_FRAMES)    return G_SOURCE_CONTINUE;

    state.frames[state.frame_count++] = copy_surface(state.surface);
    return G_SOURCE_CONTINUE;
}

// ─── Tracé d'un segment naturel ───────────────────────────────
static void draw_brush_stroke(double x1, double y1,
                               double x2, double y2,
                               double width) {
    cairo_t *cr = cairo_create(state.surface);

    double r, g, b;
    hsv_to_rgb(state.hue, 0.85, 0.25, &r, &g, &b);
    cairo_set_source_rgba(cr, r, g, b, 0.75);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_width(cr, width);

    double mx = (x1 + x2) / 2.0;
    double my = (y1 + y2) / 2.0;
    cairo_move_to(cr, x1, y1);
    cairo_curve_to(cr, x1, y1, mx, my, x2, y2);
    cairo_stroke(cr);
    cairo_destroy(cr);
}

// ─── Export vidéo via ffmpeg ──────────────────────────────────
static void export_video(void) {
    if (state.frame_count == 0) {
        g_print("⚠️  Aucune frame enregistrée.\n");
        return;
    }

    g_print("🎬 Export vidéo : %d frames...\n", state.frame_count);

    // Dimensions depuis la première frame
    int w = cairo_image_surface_get_width(state.frames[0]);
    int h = cairo_image_surface_get_height(state.frames[0]);

    // Commande ffmpeg : lit du raw RGB sur stdin
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "ffmpeg -y -f rawvideo -pixel_format rgb24 "
        "-video_size %dx%d -framerate %d "
        "-i pipe:0 "
        "-vf format=yuv420p "
        "-c:v libx264 -crf 18 -preset fast "
        "progression.mp4 2>/dev/null",
        w, h, 1000 / FRAME_INTERVAL);

    FILE *pipe = popen(cmd, "w");
    if (!pipe) {
        g_printerr("❌ Impossible de lancer ffmpeg.\n");
        return;
    }

    for (int i = 0; i < state.frame_count; i++) {
        cairo_surface_t *s = state.frames[i];
        int stride = cairo_image_surface_get_stride(s);
        unsigned char *data = cairo_image_surface_get_data(s);

        // Cairo stocke en BGRA/BGRX → on convertit en RGB24 pour ffmpeg
        for (int y = 0; y < h; y++) {
            unsigned char *row = data + y * stride;
            for (int x = 0; x < w; x++) {
                unsigned char b_ch = row[x*4 + 0];
                unsigned char g_ch = row[x*4 + 1];
                unsigned char r_ch = row[x*4 + 2];
                fputc(r_ch, pipe);
                fputc(g_ch, pipe);
                fputc(b_ch, pipe);
            }
        }
    }

    pclose(pipe);
    g_print("✅ Vidéo sauvegardée : progression.mp4\n");
}

// ─── Libère toutes les frames ─────────────────────────────────
static void free_frames(void) {
    for (int i = 0; i < state.frame_count; i++) {
        cairo_surface_destroy(state.frames[i]);
        state.frames[i] = NULL;
    }
    state.frame_count = 0;
}

// ─── Callbacks GTK ────────────────────────────────────────────
static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
    (void)data;
    if (!state.surface) init_surface(widget);
    cairo_set_source_surface(cr, state.surface, 0, 0);
    cairo_paint(cr);
    return FALSE;
}

static gboolean on_button_press(GtkWidget *widget,
                                 GdkEventButton *event,
                                 gpointer data) {
    (void)data;
    if (!state.surface) init_surface(widget);
    if (event->button == GDK_BUTTON_PRIMARY) {
        state.drawing = TRUE;
        state.last_x  = event->x;
        state.last_y  = event->y;
        state.hue     = (double)rand() / RAND_MAX;
    }
    return TRUE;
}

static gboolean on_button_release(GtkWidget *widget,
                                   GdkEventButton *event,
                                   gpointer data) {
    (void)widget; (void)event; (void)data;
    state.drawing = FALSE;
    return TRUE;
}

static gboolean on_motion(GtkWidget *widget,
                           GdkEventMotion *event,
                           gpointer data) {
    (void)data;
    if (!state.drawing) return TRUE;

    double dx    = event->x - state.last_x;
    double dy    = event->y - state.last_y;
    double speed = sqrt(dx*dx + dy*dy);
    double width = CLAMP(8.0 - speed * 0.15, 1.5, 8.0);

    draw_brush_stroke(state.last_x, state.last_y,
                      event->x, event->y, width);
    state.last_x = event->x;
    state.last_y = event->y;

    gtk_widget_queue_draw(widget);
    return TRUE;
}

static gboolean on_key_press(GtkWidget *widget,
                              GdkEventKey *event,
                              gpointer data) {
    (void)data;

    switch (event->keyval) {

        // ── S : sauvegarder PNG ───────────────────────────────
        case GDK_KEY_s:
        case GDK_KEY_S:
            if (state.surface) {
                // Convertir en image surface avant d'écrire
                cairo_surface_t *img = copy_surface(state.surface);
                cairo_surface_write_to_png(img, "dessin.png");
                cairo_surface_destroy(img);
                g_print("✅ PNG sauvegardé : dessin.png\n");
            }
            break;

        // ── R : démarrer / arrêter l'enregistrement ───────────
        case GDK_KEY_r:
        case GDK_KEY_R:
            if (!state.recording) {
                free_frames();
                state.recording = TRUE;
                state.timer_id  = g_timeout_add(FRAME_INTERVAL,
                                                 capture_frame, NULL);
                g_print("🔴 Enregistrement démarré\n");
            } else {
                state.recording = FALSE;
                g_source_remove(state.timer_id);
                g_print("⏹️  Enregistrement arrêté (%d frames)\n",
                        state.frame_count);
            }
            break;

        // ── V : exporter la vidéo MP4 ─────────────────────────
        case GDK_KEY_v:
        case GDK_KEY_V:
            if (state.recording) {
                state.recording = FALSE;
                g_source_remove(state.timer_id);
            }
            export_video();
            break;

        // ── C : effacer le canvas ─────────────────────────────
        case GDK_KEY_c:
        case GDK_KEY_C: {
            cairo_t *cr = cairo_create(state.surface);
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_paint(cr);
            cairo_destroy(cr);
            gtk_widget_queue_draw(widget);
            g_print("🗑️  Canvas effacé\n");
            break;
        }

        default: break;
    }
    return FALSE;
}

// ─── Main ─────────────────────────────────────────────────────
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),
        "Dessin  |  S=PNG  R=Rec  V=Vidéo  C=Effacer");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 650);

    GtkWidget *canvas = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), canvas);

    gtk_widget_add_events(canvas,
        GDK_BUTTON_PRESS_MASK   |
        GDK_BUTTON_RELEASE_MASK |
        GDK_POINTER_MOTION_MASK);

    g_signal_connect(canvas, "draw",
                     G_CALLBACK(on_draw), NULL);
    g_signal_connect(canvas, "button-press-event",
                     G_CALLBACK(on_button_press), NULL);
    g_signal_connect(canvas, "button-release-event",
                     G_CALLBACK(on_button_release), NULL);
    g_signal_connect(canvas, "motion-notify-event",
                     G_CALLBACK(on_motion), NULL);
    g_signal_connect(window, "key-press-event",
                     G_CALLBACK(on_key_press), NULL);
    g_signal_connect(window, "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    // Nettoyage
    free_frames();
    if (state.surface)
        cairo_surface_destroy(state.surface);

    return 0;
}