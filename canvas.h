#ifndef CANVAS_H
#define CANVAS_H

#include <cairo.h>
#include "palette.h"

// ─── Formats de sortie ────────────────────────────────────────
typedef enum {
    FMT_LANDSCAPE,   // 1920×1080
    FMT_PORTRAIT,    // 1080×1920
    FMT_SQUARE,      // 1000×1000
} CanvasFormat;

// ─── Calques (ordre de rendu bas → haut) ──────────────────────
typedef enum {
    LAYER_BACKGROUND = 0,
    LAYER_FLAT_COLOR,
    LAYER_SHADOW,
    LAYER_HIGHLIGHT,
    LAYER_EFFECTS,
    LAYER_OUTLINE,
    LAYER_COUNT
} LayerID;

// ─── Déclaration externe (définie dans canvas.c) ──────────────
extern const char *LAYER_NAMES[LAYER_COUNT];

// ─── Canvas principal ─────────────────────────────────────────
typedef struct {
    int width, height;
    CanvasFormat format;

    cairo_surface_t *layers[LAYER_COUNT];
    cairo_t         *cr[LAYER_COUNT];

    cairo_surface_t *composite;
    cairo_t         *cr_composite;
} Canvas;

// ─── API ──────────────────────────────────────────────────────
Canvas  *canvas_create(CanvasFormat fmt);
void     canvas_destroy(Canvas *c);
cairo_t *canvas_layer(Canvas *c, LayerID layer);
void     canvas_clear_layer(Canvas *c, LayerID layer);
void     canvas_composite(Canvas *c);
void     canvas_save_png(Canvas *c, const char *filename);
void     canvas_get_size(CanvasFormat fmt, int *w, int *h);

#endif