#include "brush.h"
#include <math.h>
#include <stdlib.h>

// ─── Portabilité M_PI ─────────────────────────────────────────
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ─── Constructeurs ────────────────────────────────────────────
Brush brush_ink(double size) {
    return (Brush){
        .type     = BRUSH_INK,
        .size     = size,
        .opacity  = 1.0,
        .color    = COL_INK,
        .hardness = 1.0,
        .jitter   = 0.0,
        .taper    = 1,
    };
}

Brush brush_flat(ColorID color) {
    return (Brush){
        .type     = BRUSH_FLAT,
        .size     = 1.0,
        .opacity  = 1.0,
        .color    = color,
        .hardness = 1.0,
        .jitter   = 0.0,
        .taper    = 0,
    };
}

Brush brush_shadow(ColorID shadow_color) {
    return (Brush){
        .type     = BRUSH_SHADOW,
        .size     = 1.0,
        .opacity  = 0.85,
        .color    = shadow_color,
        .hardness = 1.0,
        .jitter   = 0.0,
        .taper    = 0,
    };
}

Brush brush_highlight(void) {
    return (Brush){
        .type     = BRUSH_HIGHLIGHT,
        .size     = 1.0,
        .opacity  = 0.90,
        .color    = COL_WHITE,
        .hardness = 1.0,
        .jitter   = 0.0,
        .taper    = 0,
    };
}

Brush brush_speedline(void) {
    return (Brush){
        .type     = BRUSH_SPEEDLINE,
        .size     = 1.5,
        .opacity  = 0.7,
        .color    = COL_INK,
        .hardness = 1.0,
        .jitter   = 0.15,
        .taper    = 1,
    };
}

Brush brush_halftone(ColorID color, double dot_size) {
    return (Brush){
        .type     = BRUSH_HALFTONE,
        .size     = dot_size,
        .opacity  = 0.6,
        .color    = color,
        .hardness = 1.0,
        .jitter   = 0.0,
        .taper    = 0,
    };
}

// ─── Helpers internes ─────────────────────────────────────────
static void apply_brush_color(cairo_t *cr, Brush *b) {
    Color c = palette_get(b->color);
    cairo_set_source_rgba(cr, c.r, c.g, c.b, b->opacity);
}

static double jitter_val(double jitter, int seed) {
    srand(seed);
    return ((double)rand() / RAND_MAX - 0.5) * 2.0 * jitter;
}

// ─── Trait simple ─────────────────────────────────────────────
void brush_stroke(cairo_t *cr, Brush *b,
                  double x1, double y1,
                  double x2, double y2) {
    apply_brush_color(cr, b);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

    if (b->taper) {
        int steps = 12;
        double dx = x2 - x1, dy = y2 - y1;
        for (int i = 0; i < steps; i++) {
            double t0 = (double)i / steps;
            double t1 = (double)(i + 1) / steps;
            double w  = b->size * sin(M_PI * (t0 + t1) * 0.5);
            if (w < 0.5) w = 0.5;
            cairo_set_line_width(cr, w);
            double jx = jitter_val(b->jitter, i * 7 + 3);
            double jy = jitter_val(b->jitter, i * 13 + 7);
            cairo_move_to(cr, x1 + dx * t0 + jx, y1 + dy * t0 + jy);
            cairo_line_to(cr, x1 + dx * t1 + jx, y1 + dy * t1 + jy);
            cairo_stroke(cr);
        }
    } else {
        cairo_set_line_width(cr, b->size);
        cairo_move_to(cr, x1, y1);
        cairo_line_to(cr, x2, y2);
        cairo_stroke(cr);
    }
}

// ─── Courbe de Bézier cubique ─────────────────────────────────
void brush_bezier(cairo_t *cr, Brush *b,
                  double x1, double y1,
                  double cx1, double cy1,
                  double cx2, double cy2,
                  double x2, double y2) {
    apply_brush_color(cr, b);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_width(cr, b->size);
    cairo_move_to(cr, x1, y1);
    cairo_curve_to(cr, cx1, cy1, cx2, cy2, x2, y2);
    cairo_stroke(cr);
}

// ─── Remplissage ──────────────────────────────────────────────
void brush_fill(cairo_t *cr, Brush *b) {
    apply_brush_color(cr, b);
    cairo_fill_preserve(cr);
}

// ─── Contour d'un chemin ──────────────────────────────────────
void brush_stroke_path(cairo_t *cr, Brush *b) {
    apply_brush_color(cr, b);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_width(cr, b->size);
    cairo_stroke(cr);
}

// ─── Cercle ───────────────────────────────────────────────────
void brush_circle(cairo_t *cr, Brush *b,
                  double cx, double cy, double radius) {
    cairo_new_path(cr);
    cairo_arc(cr, cx, cy, radius, 0, 2 * M_PI);
    brush_fill(cr, b);
    cairo_new_path(cr);
}

// ─── Ellipse ──────────────────────────────────────────────────
void brush_ellipse(cairo_t *cr, Brush *b,
                   double cx, double cy,
                   double rx, double ry, double angle) {
    cairo_save(cr);
    cairo_translate(cr, cx, cy);
    cairo_rotate(cr, angle);
    cairo_scale(cr, rx, ry);
    cairo_new_path(cr);
    cairo_arc(cr, 0, 0, 1.0, 0, 2 * M_PI);
    cairo_restore(cr);
    brush_fill(cr, b);
    cairo_new_path(cr);
}

// ─── Speed lines ──────────────────────────────────────────────
void brush_speedlines(cairo_t *cr, Brush *b,
                      double cx, double cy,
                      double inner_r, double outer_r,
                      int count, unsigned int seed) {
    apply_brush_color(cr, b);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);

    srand(seed);
    for (int i = 0; i < count; i++) {
        double angle = ((double)i / count) * 2.0 * M_PI;
        angle += ((double)rand() / RAND_MAX - 0.5)
                 * (2.0 * M_PI / count) * 0.6;

        double w = b->size * (0.5 + (double)rand() / RAND_MAX * 1.5);
        cairo_set_line_width(cr, w);

        double r_out = inner_r + (outer_r - inner_r)
                       * (0.6 + 0.4 * (double)rand() / RAND_MAX);

        double x1 = cx + cos(angle) * inner_r;
        double y1 = cy + sin(angle) * inner_r;
        double x2 = cx + cos(angle) * r_out;
        double y2 = cy + sin(angle) * r_out;

        cairo_move_to(cr, x1, y1);
        cairo_line_to(cr, x2, y2);
        cairo_stroke(cr);
    }
}

// ─── Halftone (trame de points) ───────────────────────────────
void brush_halftone_fill(cairo_t *cr, Brush *b,
                         double x, double y,
                         double w, double h,
                         double spacing) {
    apply_brush_color(cr, b);
    double dot_r = b->size * 0.5;
    int cols = (int)(w / spacing) + 1;
    int rows = (int)(h / spacing) + 1;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            double px = x + col * spacing
                        + (row % 2 == 0 ? 0 : spacing * 0.5);
            double py = y + row * spacing;
            cairo_new_path(cr);
            cairo_arc(cr, px, py, dot_r, 0, 2 * M_PI);
            cairo_fill(cr);
        }
    }
}