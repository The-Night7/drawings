#include "palette.h"

// ─── Définition de la palette Comics US ──────────────────────
static const Color PALETTE[PALETTE_SIZE] = {
    // Noirs & blancs
    [COL_BLACK]        = {0.05, 0.05, 0.08, 1.0},
    [COL_INK]          = {0.08, 0.08, 0.15, 1.0},
    [COL_WHITE]        = {1.00, 1.00, 1.00, 1.0},
    [COL_PAPER]        = {0.98, 0.96, 0.90, 1.0},

    // Rouges (style Superman/Spider-Man)
    [COL_RED_HI]       = {1.00, 0.75, 0.70, 1.0},
    [COL_RED_BASE]     = {0.90, 0.15, 0.15, 1.0},
    [COL_RED_SHADOW]   = {0.60, 0.05, 0.05, 1.0},
    [COL_RED_DEEP]     = {0.35, 0.02, 0.02, 1.0},

    // Bleus (style Captain America)
    [COL_BLUE_HI]      = {0.70, 0.80, 1.00, 1.0},
    [COL_BLUE_BASE]    = {0.15, 0.35, 0.85, 1.0},
    [COL_BLUE_SHADOW]  = {0.05, 0.15, 0.55, 1.0},
    [COL_BLUE_DEEP]    = {0.02, 0.05, 0.30, 1.0},

    // Jaunes (style Batman/Flash)
    [COL_YELLOW_HI]    = {1.00, 1.00, 0.70, 1.0},
    [COL_YELLOW_BASE]  = {0.95, 0.80, 0.05, 1.0},
    [COL_YELLOW_SHADOW]= {0.70, 0.55, 0.02, 1.0},
    [COL_YELLOW_DEEP]  = {0.45, 0.30, 0.01, 1.0},

    // Verts (style Hulk)
    [COL_GREEN_HI]     = {0.70, 1.00, 0.70, 1.0},
    [COL_GREEN_BASE]   = {0.15, 0.75, 0.20, 1.0},
    [COL_GREEN_SHADOW] = {0.05, 0.45, 0.08, 1.0},
    [COL_GREEN_DEEP]   = {0.02, 0.25, 0.04, 1.0},

    // Chairs
    [COL_SKIN_HI]      = {1.00, 0.92, 0.82, 1.0},
    [COL_SKIN_BASE]    = {0.95, 0.75, 0.55, 1.0},
    [COL_SKIN_SHADOW]  = {0.75, 0.50, 0.35, 1.0},
    [COL_SKIN_DEEP]    = {0.50, 0.30, 0.20, 1.0},

    // Gris
    [COL_GREY_LIGHT]   = {0.85, 0.85, 0.85, 1.0},
    [COL_GREY_MID]     = {0.55, 0.55, 0.55, 1.0},
    [COL_GREY_DARK]    = {0.25, 0.25, 0.25, 1.0},

    // Effets
    [COL_SPEED_LINE]   = {0.92, 0.92, 0.92, 1.0},
    [COL_HALFTONE]     = {0.80, 0.80, 0.80, 1.0},
};

// ─── API ──────────────────────────────────────────────────────
Color palette_get(ColorID id) {
    return PALETTE[id];
}

void palette_apply(cairo_t *cr, ColorID id) {
    Color c = PALETTE[id];
    cairo_set_source_rgba(cr, c.r, c.g, c.b, c.a);
}

void palette_apply_alpha(cairo_t *cr, ColorID id, double alpha) {
    Color c = PALETTE[id];
    cairo_set_source_rgba(cr, c.r, c.g, c.b, alpha);
}

Color color_lerp(Color a, Color b, double t) {
    return (Color){
        a.r + (b.r - a.r) * t,
        a.g + (b.g - a.g) * t,
        a.b + (b.b - a.b) * t,
        a.a + (b.a - a.a) * t,
    };
}

Color color_darken(Color c, double factor) {
    return (Color){c.r * factor, c.g * factor, c.b * factor, c.a};
}

Color color_lighten(Color c, double factor) {
    return (Color){
        c.r + (1.0 - c.r) * factor,
        c.g + (1.0 - c.g) * factor,
        c.b + (1.0 - c.b) * factor,
        c.a
    };
}