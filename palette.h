#ifndef PALETTE_H
#define PALETTE_H

#include <cairo.h>

// ─── Format couleur RGBA ──────────────────────────────────────
typedef struct {
    double r, g, b, a;
} Color;

// ─── Palette Comics US ────────────────────────────────────────
// Chaque teinte a 4 niveaux : highlight, base, shadow, deep shadow

typedef enum {
    // Noirs & blancs
    COL_BLACK = 0,
    COL_INK,           // noir légèrement bleuté (contours)
    COL_WHITE,
    COL_PAPER,         // blanc cassé

    // Rouges
    COL_RED_HI,
    COL_RED_BASE,
    COL_RED_SHADOW,
    COL_RED_DEEP,

    // Bleus
    COL_BLUE_HI,
    COL_BLUE_BASE,
    COL_BLUE_SHADOW,
    COL_BLUE_DEEP,

    // Jaunes
    COL_YELLOW_HI,
    COL_YELLOW_BASE,
    COL_YELLOW_SHADOW,
    COL_YELLOW_DEEP,

    // Verts
    COL_GREEN_HI,
    COL_GREEN_BASE,
    COL_GREEN_SHADOW,
    COL_GREEN_DEEP,

    // Chairs (personnages)
    COL_SKIN_HI,
    COL_SKIN_BASE,
    COL_SKIN_SHADOW,
    COL_SKIN_DEEP,

    // Gris (décors)
    COL_GREY_LIGHT,
    COL_GREY_MID,
    COL_GREY_DARK,

    // Effets
    COL_SPEED_LINE,    // gris très clair pour speed lines
    COL_HALFTONE,      // pour les trames

    PALETTE_SIZE
} ColorID;

// ─── API ──────────────────────────────────────────────────────
Color       palette_get(ColorID id);
void        palette_apply(cairo_t *cr, ColorID id);
void        palette_apply_alpha(cairo_t *cr, ColorID id, double alpha);

// Utilitaires
Color       color_lerp(Color a, Color b, double t);
Color       color_darken(Color c, double factor);
Color       color_lighten(Color c, double factor);

#endif