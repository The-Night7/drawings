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
    LAYER_BACKGROUND = 0,  // fond papier / décor
    LAYER_FLAT_COLOR,      // aplats de couleur de base
    LAYER_SHADOW,          // ombres dures
    LAYER_HIGHLIGHT,       // highlights
    LAYER_EFFECTS,         // speed lines, halftones, onomatopées
    LAYER_OUTLINE,         // contours encre noire (toujours au-dessus)
    LAYER_COUNT
} LayerID;

static const char *LAYER_NAMES[LAYER_COUNT] = {
    "Background",
    "Flat Color",
    "Shadow",
    "Highlight",
    "Effects",
    "Outline",
};

// ─── Canvas principal ─────────────────────────────────────────
typedef struct {
    int width, height;
    CanvasFormat format;

    cairo_surface_t *layers[LAYER_COUNT];
    cairo_t         *cr[LAYER_COUNT];

    // Surface de composition finale
    cairo_surface_t *composite;
    cairo_t         *cr_composite;
} Canvas;

// ─── API ──────────────────────────────────────────────────────
Canvas *canvas_create(CanvasFormat fmt);
void    canvas_destroy(Canvas *c);

// Accès au contexte Cairo d'un calque
cairo_t *canvas_layer(Canvas *c, LayerID layer);

// Effacer un calque (transparent)
void canvas_clear_layer(Canvas *c, LayerID layer);

// Fusionner tous les calques → composite
void canvas_composite(Canvas *c);

// Export
void canvas_save_png(Canvas *c, const char *filename);

// Dimensions selon format
void canvas_get_size(CanvasFormat fmt, int *w, int *h);

#endif