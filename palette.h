#ifndef PALETTE_H
#define PALETTE_H

#include <cairo.h>

// ─── Format couleur RGBA ──────────────────────────────────────
typedef struct {
    double r, g, b, a;
} Color;

typedef enum {
    // ── Noirs & blancs ────────────────────────────────────────
    COL_BLACK = 0,
    COL_INK,              // noir contour (légèrement bleuté)
    COL_WHITE,
    COL_PAPER,            // blanc cassé fond webtoon

    // ── Highlights universels ─────────────────────────────────
    COL_HI_YELLOW,        // highlight chaud (nez, front, cou)
    COL_HI_WHITE,         // highlight froid (yeux, cheveux)
    COL_HI_WARM,          // highlight orangé

    // ── Peaux ─────────────────────────────────────────────────
    COL_SKIN_FAIR_HI,
    COL_SKIN_FAIR_BASE,
    COL_SKIN_FAIR_SHADOW,
    COL_SKIN_FAIR_DEEP,

    COL_SKIN_TAN_HI,
    COL_SKIN_TAN_BASE,
    COL_SKIN_TAN_SHADOW,
    COL_SKIN_TAN_DEEP,

    COL_SKIN_DARK_HI,
    COL_SKIN_DARK_BASE,
    COL_SKIN_DARK_SHADOW,
    COL_SKIN_DARK_DEEP,

    COL_SKIN_GREY_HI,     // peau grise (démons, morts-vivants...)
    COL_SKIN_GREY_BASE,
    COL_SKIN_GREY_SHADOW,
    COL_SKIN_GREY_DEEP,

    COL_SKIN_PURPLE_HI,   // peau violette
    COL_SKIN_PURPLE_BASE,
    COL_SKIN_PURPLE_SHADOW,
    COL_SKIN_PURPLE_DEEP,

    COL_SKIN_BLUE_HI,     // peau bleue
    COL_SKIN_BLUE_BASE,
    COL_SKIN_BLUE_SHADOW,
    COL_SKIN_BLUE_DEEP,

    // ── Cheveux ───────────────────────────────────────────────
    COL_HAIR_WHITE_HI,
    COL_HAIR_WHITE_BASE,
    COL_HAIR_WHITE_SHADOW,

    COL_HAIR_BLACK_HI,
    COL_HAIR_BLACK_BASE,
    COL_HAIR_BLACK_SHADOW,

    COL_HAIR_BROWN_HI,
    COL_HAIR_BROWN_BASE,
    COL_HAIR_BROWN_SHADOW,

    COL_HAIR_BLONDE_HI,
    COL_HAIR_BLONDE_BASE,
    COL_HAIR_BLONDE_SHADOW,

    COL_HAIR_RED_HI,
    COL_HAIR_RED_BASE,
    COL_HAIR_RED_SHADOW,

    COL_HAIR_BLUE_HI,
    COL_HAIR_BLUE_BASE,
    COL_HAIR_BLUE_SHADOW,

    COL_HAIR_PINK_HI,
    COL_HAIR_PINK_BASE,
    COL_HAIR_PINK_SHADOW,

    COL_HAIR_GREEN_HI,
    COL_HAIR_GREEN_BASE,
    COL_HAIR_GREEN_SHADOW,

    COL_HAIR_PURPLE_HI,
    COL_HAIR_PURPLE_BASE,
    COL_HAIR_PURPLE_SHADOW,

    COL_HAIR_GREY_HI,
    COL_HAIR_GREY_BASE,
    COL_HAIR_GREY_SHADOW,

    // ── Yeux ──────────────────────────────────────────────────
    COL_EYE_BROWN,
    COL_EYE_BLUE,
    COL_EYE_GREEN,
    COL_EYE_RED,
    COL_EYE_PURPLE,
    COL_EYE_GOLD,
    COL_EYE_GREY,
    COL_EYE_PINK,
    COL_EYE_WHITE,        // yeux blancs (démons...)
    COL_EYE_BLACK,        // yeux noirs

    // ── Vêtements ─────────────────────────────────────────────
    COL_CLOTH_RED_HI,
    COL_CLOTH_RED_BASE,
    COL_CLOTH_RED_SHADOW,

    COL_CLOTH_BLUE_HI,
    COL_CLOTH_BLUE_BASE,
    COL_CLOTH_BLUE_SHADOW,

    COL_CLOTH_GREEN_HI,
    COL_CLOTH_GREEN_BASE,
    COL_CLOTH_GREEN_SHADOW,

    COL_CLOTH_YELLOW_HI,
    COL_CLOTH_YELLOW_BASE,
    COL_CLOTH_YELLOW_SHADOW,

    COL_CLOTH_PURPLE_HI,
    COL_CLOTH_PURPLE_BASE,
    COL_CLOTH_PURPLE_SHADOW,

    COL_CLOTH_BLACK_HI,
    COL_CLOTH_BLACK_BASE,
    COL_CLOTH_BLACK_SHADOW,

    COL_CLOTH_WHITE_HI,
    COL_CLOTH_WHITE_BASE,
    COL_CLOTH_WHITE_SHADOW,

    COL_CLOTH_BROWN_HI,
    COL_CLOTH_BROWN_BASE,
    COL_CLOTH_BROWN_SHADOW,

    // ── Effets / décors ───────────────────────────────────────
    COL_BLUSH,            // rougeur joues
    COL_BLUSH_DEEP,
    COL_SCAR_RED,         // cicatrices
    COL_MARK_RED,         // marques (croix, etc.)
    COL_MARK_BLACK,
    COL_SPEED_LINE,
    COL_PANEL_BG_1,       // fond case clair
    COL_PANEL_BG_2,       // fond case sombre
    COL_PANEL_BG_3,       // fond case coloré

    PALETTE_SIZE
} ColorID;

// ── Groupes de couleurs peau ──────────────────────────────────
typedef enum {
    SKIN_FAIR = 0,
    SKIN_TAN,
    SKIN_DARK,
    SKIN_GREY,
    SKIN_PURPLE,
    SKIN_BLUE,
    SKIN_COUNT
} SkinTone;

// ── Groupes de couleurs cheveux ───────────────────────────────
typedef enum {
    HAIR_WHITE = 0,
    HAIR_BLACK,
    HAIR_BROWN,
    HAIR_BLONDE,
    HAIR_RED,
    HAIR_BLUE,
    HAIR_PINK,
    HAIR_GREEN,
    HAIR_PURPLE,
    HAIR_GREY,
    HAIR_COUNT
} HairColor;

// ── Groupes vêtements ─────────────────────────────────────────
typedef enum {
    CLOTH_RED = 0,
    CLOTH_BLUE,
    CLOTH_GREEN,
    CLOTH_YELLOW,
    CLOTH_PURPLE,
    CLOTH_BLACK,
    CLOTH_WHITE,
    CLOTH_BROWN,
    CLOTH_COUNT
} ClothColor;

// ── Accesseurs par groupe ─────────────────────────────────────
ColorID palette_skin_hi    (SkinTone s);
ColorID palette_skin_base  (SkinTone s);
ColorID palette_skin_shadow(SkinTone s);
ColorID palette_skin_deep  (SkinTone s);

ColorID palette_hair_hi    (HairColor h);
ColorID palette_hair_base  (HairColor h);
ColorID palette_hair_shadow(HairColor h);

ColorID palette_cloth_hi    (ClothColor c);
ColorID palette_cloth_base  (ClothColor c);
ColorID palette_cloth_shadow(ClothColor c);

// ─── API ──────────────────────────────────────────────────────
Color palette_get      (ColorID id);
void  palette_apply    (cairo_t *cr, ColorID id);
void  palette_apply_alpha(cairo_t *cr, ColorID id, double alpha);

Color color_lerp   (Color a, Color b, double t);
Color color_darken (Color c, double factor);
Color color_lighten(Color c, double factor);

#endif