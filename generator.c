#include "generator.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ─── Palette personnage selon choix ───────────────────────────
CharacterPalette gen_pick_palette(int palette_set, unsigned int seed) {
    srand(seed);
    int choice = (palette_set == 4) ? (rand() % 4) : palette_set;
    switch (choice) {
        case 0: return (CharacterPalette){COL_RED_BASE,    COL_RED_DEEP,    COL_RED_HI,    COL_BLUE_BASE};
        case 1: return (CharacterPalette){COL_BLUE_BASE,   COL_BLUE_DEEP,   COL_BLUE_HI,   COL_RED_BASE};
        case 2: return (CharacterPalette){COL_YELLOW_BASE, COL_YELLOW_DEEP, COL_YELLOW_HI, COL_GREY_DARK};
        case 3: return (CharacterPalette){COL_GREEN_BASE,  COL_GREEN_DEEP,  COL_GREEN_HI,  COL_YELLOW_BASE};
        default: return (CharacterPalette){COL_RED_BASE,   COL_RED_DEEP,    COL_RED_HI,    COL_BLUE_BASE};
    }
}

// ─── Fond dégradé simulé (deux bandes) ───────────────────────
void gen_background_gradient(Canvas *c, ColorID top, ColorID bot) {
    cairo_t *cr = canvas_layer(c, LAYER_BACKGROUND);
    int steps = 16;
    for (int i = 0; i < steps; i++) {
        double t0 = (double)i / steps;
        double t1 = (double)(i + 1) / steps;
        double tm = (t0 + t1) * 0.5;
        Color ct = palette_get(top);
        Color cb = palette_get(bot);
        Color cm = color_lerp(ct, cb, tm);
        cairo_set_source_rgba(cr, cm.r, cm.g, cm.b, 1.0);
        cairo_rectangle(cr, 0, t0 * c->height,
                        c->width, (t1 - t0) * c->height + 1);
        cairo_fill(cr);
    }
}

// ─── Overlay halftone sur toute la surface ────────────────────
void gen_halftone_overlay(Canvas *c, double density) {
    cairo_t *cr = canvas_layer(c, LAYER_EFFECTS);
    Brush bh = brush_halftone(COL_HALFTONE, density * 3.0);
    bh.opacity = 0.18;
    brush_halftone_fill(cr, &bh, 0, 0, c->width, c->height, density * 8.0);
}

// ─── Bordure de panneau ───────────────────────────────────────
void gen_panel_border(Canvas *c, double thickness) {
    cairo_t *cr = canvas_layer(c, LAYER_OUTLINE);
    cairo_new_path(cr);
    cairo_rectangle(cr, thickness * 0.5, thickness * 0.5,
                    c->width  - thickness,
                    c->height - thickness);
    palette_apply(cr, COL_INK);
    cairo_set_line_width(cr, thickness);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
    cairo_stroke(cr);
}

// ─── Lignes dynamiques rayonnantes ────────────────────────────
void gen_dynamic_lines(Canvas *c, double cx, double cy,
                       unsigned int seed) {
    cairo_t *cr = canvas_layer(c, LAYER_EFFECTS);
    Brush bs = brush_speedline();
    double max_r = sqrt((double)(c->width * c->width +
                                 c->height * c->height));
    brush_speedlines(cr, &bs, cx, cy, max_r * 0.08, max_r * 0.95,
                     120, seed);
}

// ═══════════════════════════════════════════════════════════════
// SCÈNE 1 : Héros standalone
// ═══════════════════════════════════════════════════════════════
void gen_scene_hero_standalone(Canvas *c, GeneratorParams *p) {
    srand(p->seed);
    CharacterPalette pal = gen_pick_palette(p->palette_set, p->seed);

    // Fond
    gen_background_gradient(c, COL_BLUE_DEEP, COL_BLACK);

    // Starburst derrière le héros
    shape_starburst(c,
        c->width * 0.5, c->height * 0.42,
        c->height * 0.25, c->height * 0.48,
        16, COL_YELLOW_BASE);

    // Speed lines depuis le centre
    gen_dynamic_lines(c, c->width * 0.5, c->height * 0.42, p->seed);

    // Halftone overlay
    gen_halftone_overlay(c, 6.0);

    // Personnage principal
    ShapeParams sp = {
        .cx          = c->width  * 0.50,
        .cy          = c->height * 0.38,
        .scale       = c->height * 0.75,
        .angle       = 0,
        .color_base   = pal.base,
        .color_shadow = pal.shadow,
        .color_hi     = pal.highlight,
        .seed         = p->seed,
    };
    shape_humanoid(c, &sp);

    // Bordure
    gen_panel_border(c, 12.0);
}

// ═══════════════════════════════════════════════════════════════
// SCÈNE 2 : Combat avec explosion
// ═══════════════════════════════════════════════════════════════
void gen_scene_fight(Canvas *c, GeneratorParams *p) {
    srand(p->seed);
    CharacterPalette pal  = gen_pick_palette(p->palette_set, p->seed);
    CharacterPalette pal2 = gen_pick_palette((p->palette_set + 1) % 4,
                                             p->seed + 99);

    // Fond action
    shape_action_panel(c, COL_RED_BASE);
    gen_halftone_overlay(c, 5.0);

    // Explosion centrale
    shape_explosion(c,
        c->width  * 0.50,
        c->height * 0.45,
        c->height * 0.30,
        COL_YELLOW_BASE,
        p->seed + 1);

    // Explosion secondaire
    shape_explosion(c,
        c->width  * 0.30,
        c->height * 0.55,
        c->height * 0.12,
        COL_YELLOW_HI,
        p->seed + 2);

    // Héros gauche
    ShapeParams sp1 = {
        .cx          = c->width  * 0.25,
        .cy          = c->height * 0.40,
        .scale       = c->height * 0.65,
        .color_base   = pal.base,
        .color_shadow = pal.shadow,
        .color_hi     = pal.highlight,
        .seed         = p->seed,
    };
    shape_humanoid(c, &sp1);

    // Ennemi droite
    ShapeParams sp2 = {
        .cx          = c->width  * 0.75,
        .cy          = c->height * 0.40,
        .scale       = c->height * 0.65,
        .color_base   = pal2.base,
        .color_shadow = pal2.shadow,
        .color_hi     = pal2.highlight,
        .seed         = p->seed + 50,
    };
    shape_humanoid(c, &sp2);

    // Onomatopée
    shape_onomatopoeia_box(c,
        c->width * 0.35, c->height * 0.08,
        c->width * 0.30, c->height * 0.14,
        -0.12,
        COL_YELLOW_BASE);

    gen_panel_border(c, 14.0);
}

// ═══════════════════════════════════════════════════════════════
// SCÈNE 3 : Cityscape
// ═══════════════════════════════════════════════════════════════
void gen_scene_cityscape(Canvas *c, GeneratorParams *p) {
    srand(p->seed);
    CharacterPalette pal = gen_pick_palette(p->palette_set, p->seed);

    // Ciel nocturne
    gen_background_gradient(c, COL_BLUE_DEEP, COL_BLACK);

    // Lune
    Brush bmoon = brush_flat(COL_YELLOW_HI);
    brush_circle(canvas_layer(c, LAYER_FLAT_COLOR), &bmoon,
                 c->width * 0.82, c->height * 0.15,
                 c->height * 0.07);
    // Contour lune
    cairo_t *cr_out = canvas_layer(c, LAYER_OUTLINE);
    cairo_new_path(cr_out);
    cairo_arc(cr_out, c->width * 0.82, c->height * 0.15,
              c->height * 0.07, 0, 2 * M_PI);
    palette_apply(cr_out, COL_INK);
    cairo_set_line_width(cr_out, 3.5);
    cairo_stroke(cr_out);

    // Bâtiments arrière-plan (plus sombres)
    shape_cityscape(c, c->height * 0.85, c->width,
                    COL_GREY_DARK, p->seed + 10);

    // Bâtiments avant-plan
    shape_cityscape(c, c->height * 0.95, c->width,
                    COL_BLACK, p->seed + 20);

    // Halftone ciel
    gen_halftone_overlay(c, 7.0);

    // Héros en silhouette sur les toits
    ShapeParams sp = {
        .cx          = c->width  * 0.50,
        .cy          = c->height * 0.52,
        .scale       = c->height * 0.60,
        .color_base   = pal.base,
        .color_shadow = pal.shadow,
        .color_hi     = pal.highlight,
        .seed         = p->seed,
    };
    shape_humanoid(c, &sp);

    gen_panel_border(c, 12.0);
}

// ═══════════════════════════════════════════════════════════════
// SCÈNE 4 : Speed lines focus
// ═══════════════════════════════════════════════════════════════
void gen_scene_speedlines(Canvas *c, GeneratorParams *p) {
    srand(p->seed);
    CharacterPalette pal = gen_pick_palette(p->palette_set, p->seed);

    // Fond blanc cassé
    cairo_t *cr_bg = canvas_layer(c, LAYER_BACKGROUND);
    palette_apply(cr_bg, COL_PAPER);
    cairo_paint(cr_bg);

    // Speed lines massives
    cairo_t *cr_fx = canvas_layer(c, LAYER_EFFECTS);
    Brush bsl = brush_speedline();
    bsl.size    = 2.5;
    bsl.opacity = 0.85;
    double cx = c->width  * 0.50;
    double cy = c->height * 0.40;
    double max_r = sqrt((double)(c->width * c->width +
                                 c->height * c->height));
    brush_speedlines(cr_fx, &bsl, cx, cy,
                     max_r * 0.05, max_r,
                     200, p->seed);

    // Cercle blanc central (zone de focus)
    Brush bw = brush_flat(COL_WHITE);
    brush_circle(canvas_layer(c, LAYER_FLAT_COLOR), &bw,
                 cx, cy, c->height * 0.28);

    // Personnage au centre
    ShapeParams sp = {
        .cx          = cx,
        .cy          = cy - c->height * 0.05,
        .scale       = c->height * 0.70,
        .color_base   = pal.base,
        .color_shadow = pal.shadow,
        .color_hi     = pal.highlight,
        .seed         = p->seed,
    };
    shape_humanoid(c, &sp);

    gen_panel_border(c, 14.0);
}

// ═══════════════════════════════════════════════════════════════
// SCÈNE 5 : Personnage + bulle de dialogue
// ═══════════════════════════════════════════════════════════════
void gen_scene_speech(Canvas *c, GeneratorParams *p) {
    srand(p->seed);
    CharacterPalette pal = gen_pick_palette(p->palette_set, p->seed);

    // Fond neutre
    gen_background_gradient(c, COL_GREY_LIGHT, COL_GREY_MID);
    gen_halftone_overlay(c, 6.5);

    // Personnage
    ShapeParams sp = {
        .cx          = c->width  * 0.38,
        .cy          = c->height * 0.45,
        .scale       = c->height * 0.72,
        .color_base   = pal.base,
        .color_shadow = pal.shadow,
        .color_hi     = pal.highlight,
        .seed         = p->seed,
    };
    shape_humanoid(c, &sp);

    // Bulle de dialogue
    shape_speech_bubble(c,
        c->width  * 0.72,
        c->height * 0.22,
        c->width  * 0.20,
        c->height * 0.12,
        c->width  * 0.52,
        c->height * 0.28,
        COL_WHITE);

    // Starburst décoratif
    shape_starburst(c,
        c->width  * 0.72,
        c->height * 0.22,
        c->height * 0.06,
        c->height * 0.10,
        8, COL_YELLOW_BASE);

    gen_panel_border(c, 12.0);
}

// ═══════════════════════════════════════════════════════════════
// SCÈNE 6 : Villain avec cape
// ═══════════════════════════════════════════════════════════════
void gen_scene_villain(Canvas *c, GeneratorParams *p) {
    srand(p->seed);

    // Fond sombre dramatique
    gen_background_gradient(c, COL_BLACK, COL_RED_DEEP);
    gen_halftone_overlay(c, 5.5);

    // Cape (derrière le corps)
    ShapeParams cape_p = {
        .cx          = c->width  * 0.50,
        .cy          = c->height * 0.30,
        .scale       = c->height * 0.80,
        .color_base   = COL_RED_BASE,
        .color_shadow = COL_RED_DEEP,
        .color_hi     = COL_RED_HI,
        .seed         = p->seed,
    };
    shape_cape(c, &cape_p);

    // Corps du villain
    ShapeParams sp = {
        .cx          = c->width  * 0.50,
        .cy          = c->height * 0.35,
        .scale       = c->height * 0.72,
        .color_base   = COL_BLACK,
        .color_shadow = COL_GREY_DARK,
        .color_hi     = COL_GREY_MID,
        .seed         = p->seed + 1,
    };
    shape_humanoid(c, &sp);

    // Starburst sinistre
    shape_starburst(c,
        c->width * 0.50, c->height * 0.30,
        c->height * 0.18, c->height * 0.32,
        20, COL_RED_SHADOW);

    gen_panel_border(c, 16.0);
}

// ═══════════════════════════════════════════════════════════════
// SCÈNE 7 : Impact / onomatopée
// ═══════════════════════════════════════════════════════════════
void gen_scene_impact(Canvas *c, GeneratorParams *p) {
    srand(p->seed);
    CharacterPalette pal = gen_pick_palette(p->palette_set, p->seed);

    // Fond jaune vif
    cairo_t *cr_bg = canvas_layer(c, LAYER_BACKGROUND);
    palette_apply(cr_bg, COL_YELLOW_BASE);
    cairo_paint(cr_bg);

    // Speed lines depuis le centre
    gen_dynamic_lines(c, c->width * 0.50, c->height * 0.45, p->seed);

    // Grosses explosions
    shape_explosion(c,
        c->width  * 0.50, c->height * 0.45,
        c->height * 0.38, COL_RED_BASE,
        p->seed);
    shape_explosion(c,
        c->width  * 0.50, c->height * 0.45,
        c->height * 0.25, COL_YELLOW_HI,
        p->seed + 5);

    // Boîte onomatopée centrale (grande)
    shape_onomatopoeia_box(c,
        c->width  * 0.20, c->height * 0.35,
        c->width  * 0.60, c->height * 0.20,
        -0.08,
        COL_WHITE);

    // Petites explosions décoratives
    shape_explosion(c, c->width * 0.15, c->height * 0.20,
                    c->height * 0.08, pal.base, p->seed + 10);
    shape_explosion(c, c->width * 0.82, c->height * 0.70,
                    c->height * 0.07, pal.accent, p->seed + 11);

    gen_halftone_overlay(c, 5.0);
    gen_panel_border(c, 14.0);
}

// ═══════════════════════════════════════════════════════════════
// DISPATCHER PRINCIPAL
// ═══════════════════════════════════════════════════════════════
void generator_run(Canvas *c, GeneratorParams *p) {
    SceneType scene = p->scene;

    // Scène aléatoire si hors plage
    if (scene < 0 || scene >= SCENE_COUNT) {
        srand(p->seed);
        scene = rand() % SCENE_COUNT;
    }

    printf("🎨 Génération scène %d (seed=%u, format=%d)\n",
           scene, p->seed, p->format);

    switch (scene) {
        case SCENE_HERO_STANDALONE: gen_scene_hero_standalone(c, p); break;
        case SCENE_FIGHT:           gen_scene_fight(c, p);           break;
        case SCENE_CITYSCAPE:       gen_scene_cityscape(c, p);       break;
        case SCENE_SPEEDLINES:      gen_scene_speedlines(c, p);      break;
        case SCENE_SPEECH:          gen_scene_speech(c, p);          break;
        case SCENE_VILLAIN:         gen_scene_villain(c, p);         break;
        case SCENE_IMPACT:          gen_scene_impact(c, p);          break;
        default: break;
    }
}