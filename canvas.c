#include "canvas.h"
#include <stdlib.h>
#include <stdio.h>

// ─── Définition unique de LAYER_NAMES ────────────────────────
const char *LAYER_NAMES[LAYER_COUNT] = {
    "Background",
    "Flat_Color",
    "Shadow",
    "Highlight",
    "Effects",
    "Outline",
};

void canvas_get_size(CanvasFormat fmt, int *w, int *h) {
    switch (fmt) {
        case FMT_LANDSCAPE: *w = 1920; *h = 1080; break;
        case FMT_PORTRAIT:  *w = 1080; *h = 1920; break;
        case FMT_SQUARE:    *w = 1000; *h = 1000; break;
    }
}

Canvas *canvas_create(CanvasFormat fmt) {
    Canvas *c = calloc(1, sizeof(Canvas));
    canvas_get_size(fmt, &c->width, &c->height);
    c->format = fmt;

    for (int i = 0; i < LAYER_COUNT; i++) {
        c->layers[i] = cairo_image_surface_create(
            CAIRO_FORMAT_ARGB32, c->width, c->height);
        c->cr[i] = cairo_create(c->layers[i]);

        cairo_save(c->cr[i]);
        cairo_set_operator(c->cr[i], CAIRO_OPERATOR_CLEAR);
        cairo_paint(c->cr[i]);
        cairo_restore(c->cr[i]);
    }

    c->composite    = cairo_image_surface_create(
        CAIRO_FORMAT_RGB24, c->width, c->height);
    c->cr_composite = cairo_create(c->composite);

    return c;
}

void canvas_destroy(Canvas *c) {
    if (!c) return;
    for (int i = 0; i < LAYER_COUNT; i++) {
        cairo_destroy(c->cr[i]);
        cairo_surface_destroy(c->layers[i]);
    }
    cairo_destroy(c->cr_composite);
    cairo_surface_destroy(c->composite);
    free(c);
}

cairo_t *canvas_layer(Canvas *c, LayerID layer) {
    return c->cr[layer];
}

void canvas_clear_layer(Canvas *c, LayerID layer) {
    cairo_save(c->cr[layer]);
    cairo_set_operator(c->cr[layer], CAIRO_OPERATOR_CLEAR);
    cairo_paint(c->cr[layer]);
    cairo_restore(c->cr[layer]);
}

void canvas_composite(Canvas *c) {
    cairo_t *cr = c->cr_composite;

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    for (int i = 0; i < LAYER_COUNT; i++) {
        cairo_set_source_surface(cr, c->layers[i], 0, 0);
        cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
        cairo_paint(cr);
    }
}

void canvas_save_png(Canvas *c, const char *filename) {
    canvas_composite(c);
    cairo_status_t st = cairo_surface_write_to_png(c->composite, filename);
    if (st == CAIRO_STATUS_SUCCESS)
        printf("✅ Sauvegardé : %s\n", filename);
    else
        fprintf(stderr, "❌ Erreur PNG : %s\n", cairo_status_to_string(st));
}