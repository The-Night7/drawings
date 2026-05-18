#include "shapes.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ─── Helper : dessine + remplit + contour ─────────────────────
static void fill_and_outline(Canvas *c, LayerID fill_layer,
                              LayerID outline_layer,
                              ColorID fill_col, ColorID outline_col,
                              double outline_w) {
    // Remplissage
    Brush bf = brush_flat(fill_col);
    brush_fill(canvas_layer(c, fill_layer), &bf);

    // Contour encre
    Brush bo = brush_ink(outline_w);
    (void)outline_col;
    cairo_t *cr_out = canvas_layer(c, outline_layer);
    palette_apply(cr_out, COL_INK);
    cairo_set_line_width(cr_out, outline_w);
    cairo_set_line_cap(cr_out, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr_out, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke(cr_out);
    (void)bo;
}

// ─── Silhouette humanoïde ─────────────────────────────────────
void shape_humanoid(Canvas *c, ShapeParams *p) {
    srand(p->seed);
    double s  = p->scale;
    double cx = p->cx;
    double cy = p->cy;

    cairo_t *cr_flat    = canvas_layer(c, LAYER_FLAT_COLOR);
    cairo_t *cr_shadow  = canvas_layer(c, LAYER_SHADOW);
    cairo_t *cr_hi      = canvas_layer(c, LAYER_HIGHLIGHT);
    cairo_t *cr_outline = canvas_layer(c, LAYER_OUTLINE);

    // ── Torse ────────────────────────────────────────────────
    double tw = s * 0.28, th = s * 0.35;
    double tx = cx - tw * 0.5, ty = cy - s * 0.15;

    cairo_new_path(cr_flat);
    cairo_move_to(cr_flat, tx,        ty);
    cairo_line_to(cr_flat, tx + tw,   ty);
    cairo_line_to(cr_flat, tx + tw * 1.1, ty + th);
    cairo_line_to(cr_flat, tx - tw * 0.1, ty + th);
    cairo_close_path(cr_flat);
    Brush bf = brush_flat(p->color_base);
    brush_fill(cr_flat, &bf);

    // Ombre torse (côté droit)
    cairo_new_path(cr_shadow);
    cairo_move_to(cr_shadow, tx + tw * 0.55, ty);
    cairo_line_to(cr_shadow, tx + tw,        ty);
    cairo_line_to(cr_shadow, tx + tw * 1.1,  ty + th);
    cairo_line_to(cr_shadow, tx + tw * 0.6,  ty + th);
    cairo_close_path(cr_shadow);
    Brush bs = brush_shadow(p->color_shadow);
    brush_fill(cr_shadow, &bs);

    // Highlight torse (côté gauche)
    cairo_new_path(cr_hi);
    cairo_move_to(cr_hi, tx,             ty + th * 0.1);
    cairo_line_to(cr_hi, tx + tw * 0.2,  ty + th * 0.1);
    cairo_line_to(cr_hi, tx + tw * 0.15, ty + th * 0.5);
    cairo_line_to(cr_hi, tx - tw * 0.02, ty + th * 0.5);
    cairo_close_path(cr_hi);
    Brush bh = brush_highlight();
    bh.opacity = 0.5;
    brush_fill(cr_hi, &bh);

    // Contour torse
    cairo_new_path(cr_outline);
    cairo_move_to(cr_outline, tx,             ty);
    cairo_line_to(cr_outline, tx + tw,        ty);
    cairo_line_to(cr_outline, tx + tw * 1.1,  ty + th);
    cairo_line_to(cr_outline, tx - tw * 0.1,  ty + th);
    cairo_close_path(cr_outline);
    palette_apply(cr_outline, COL_INK);
    cairo_set_line_width(cr_outline, s * 0.018);
    cairo_set_line_join(cr_outline, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke(cr_outline);

    // ── Tête ─────────────────────────────────────────────────
    double head_r = s * 0.13;
    double hx = cx, hy = cy - s * 0.15 - head_r * 1.1;

    // Base
    cairo_new_path(cr_flat);
    cairo_arc(cr_flat, hx, hy, head_r, 0, 2 * M_PI);
    brush_fill(cr_flat, &bf);

    // Ombre tête
    cairo_new_path(cr_shadow);
    cairo_arc(cr_shadow, hx + head_r * 0.2, hy,
              head_r * 0.75, -M_PI * 0.3, M_PI * 0.7);
    cairo_arc_negative(cr_shadow, hx, hy, head_r,
                       M_PI * 0.7, -M_PI * 0.3);
    cairo_close_path(cr_shadow);
    brush_fill(cr_shadow, &bs);

    // Contour tête
    cairo_new_path(cr_outline);
    cairo_arc(cr_outline, hx, hy, head_r, 0, 2 * M_PI);
    palette_apply(cr_outline, COL_INK);
    cairo_set_line_width(cr_outline, s * 0.018);
    cairo_stroke(cr_outline);

    // ── Jambes ───────────────────────────────────────────────
    double leg_w = s * 0.11, leg_h = s * 0.42;
    double leg_y  = ty + th;

    for (int side = -1; side <= 1; side += 2) {
        double lx = cx + side * tw * 0.22;

        cairo_new_path(cr_flat);
        cairo_move_to(cr_flat, lx - leg_w * 0.5, leg_y);
        cairo_line_to(cr_flat, lx + leg_w * 0.5, leg_y);
        cairo_line_to(cr_flat, lx + leg_w * 0.45, leg_y + leg_h);
        cairo_line_to(cr_flat, lx - leg_w * 0.45, leg_y + leg_h);
        cairo_close_path(cr_flat);
        brush_fill(cr_flat, &bf);

        // Ombre jambe
        if (side == 1) {
            cairo_new_path(cr_shadow);
            cairo_move_to(cr_shadow, lx,              leg_y);
            cairo_line_to(cr_shadow, lx + leg_w * 0.5, leg_y);
            cairo_line_to(cr_shadow, lx + leg_w * 0.45, leg_y + leg_h);
            cairo_line_to(cr_shadow, lx,               leg_y + leg_h);
            cairo_close_path(cr_shadow);
            brush_fill(cr_shadow, &bs);
        }

        cairo_new_path(cr_outline);
        cairo_move_to(cr_outline, lx - leg_w * 0.5,  leg_y);
        cairo_line_to(cr_outline, lx + leg_w * 0.5,  leg_y);
        cairo_line_to(cr_outline, lx + leg_w * 0.45, leg_y + leg_h);
        cairo_line_to(cr_outline, lx - leg_w * 0.45, leg_y + leg_h);
        cairo_close_path(cr_outline);
        palette_apply(cr_outline, COL_INK);
        cairo_set_line_width(cr_outline, s * 0.015);
        cairo_stroke(cr_outline);
    }

    // ── Bras ─────────────────────────────────────────────────
    double arm_w = s * 0.09, arm_h = s * 0.38;

    for (int side = -1; side <= 1; side += 2) {
        double ax = cx + side * (tw * 0.5 + arm_w * 0.3);

        cairo_new_path(cr_flat);
        cairo_move_to(cr_flat, ax - arm_w * 0.5, ty);
        cairo_line_to(cr_flat, ax + arm_w * 0.5, ty);
        cairo_line_to(cr_flat, ax + arm_w * 0.4 + side * arm_w * 0.3,
                      ty + arm_h);
        cairo_line_to(cr_flat, ax - arm_w * 0.4 + side * arm_w * 0.3,
                      ty + arm_h);
        cairo_close_path(cr_flat);
        brush_fill(cr_flat, &bf);

        cairo_new_path(cr_outline);
        cairo_move_to(cr_outline, ax - arm_w * 0.5, ty);
        cairo_line_to(cr_outline, ax + arm_w * 0.5, ty);
        cairo_line_to(cr_outline, ax + arm_w * 0.4 + side * arm_w * 0.3,
                      ty + arm_h);
        cairo_line_to(cr_outline, ax - arm_w * 0.4 + side * arm_w * 0.3,
                      ty + arm_h);
        cairo_close_path(cr_outline);
        palette_apply(cr_outline, COL_INK);
        cairo_set_line_width(cr_outline, s * 0.015);
        cairo_stroke(cr_outline);
    }
}

// ─── Explosion ────────────────────────────────────────────────
void shape_explosion(Canvas *c, double cx, double cy,
                     double radius, ColorID color,
                     unsigned int seed) {
    srand(seed);
    cairo_t *cr_flat    = canvas_layer(c, LAYER_FLAT_COLOR);
    cairo_t *cr_outline = canvas_layer(c, LAYER_OUTLINE);

    int spikes = 12 + rand() % 8;
    double r_inner = radius * 0.45;

    // Remplissage
    cairo_new_path(cr_flat);
    for (int i = 0; i < spikes * 2; i++) {
        double angle = (double)i / (spikes * 2) * 2.0 * M_PI - M_PI * 0.5;
        double r = (i % 2 == 0) ? radius  * (0.85 + 0.15 * ((double)rand()/RAND_MAX))
                                 : r_inner * (0.80 + 0.20 * ((double)rand()/RAND_MAX));
        double x = cx + cos(angle) * r;
        double y = cy + sin(angle) * r;
        if (i == 0) cairo_move_to(cr_flat, x, y);
        else        cairo_line_to(cr_flat, x, y);
    }
    cairo_close_path(cr_flat);
    Brush bf = brush_flat(color);
    brush_fill(cr_flat, &bf);

    // Contour
    srand(seed); // reset pour même forme
    cairo_new_path(cr_outline);
    for (int i = 0; i < spikes * 2; i++) {
        double angle = (double)i / (spikes * 2) * 2.0 * M_PI - M_PI * 0.5;
        double r = (i % 2 == 0) ? radius  * (0.85 + 0.15 * ((double)rand()/RAND_MAX))
                                 : r_inner * (0.80 + 0.20 * ((double)rand()/RAND_MAX));
        double x = cx + cos(angle) * r;
        double y = cy + sin(angle) * r;
        if (i == 0) cairo_move_to(cr_outline, x, y);
        else        cairo_line_to(cr_outline, x, y);
    }
    cairo_close_path(cr_outline);
    palette_apply(cr_outline, COL_INK);
    cairo_set_line_width(cr_outline, radius * 0.04);
    cairo_set_line_join(cr_outline, CAIRO_LINE_JOIN_MITER);
    cairo_stroke(cr_outline);
}

// ─── Bulle de dialogue ────────────────────────────────────────
void shape_speech_bubble(Canvas *c,
                          double cx, double cy,
                          double rx, double ry,
                          double tail_x, double tail_y,
                          ColorID bg_color) {
    cairo_t *cr_flat    = canvas_layer(c, LAYER_FLAT_COLOR);
    cairo_t *cr_outline = canvas_layer(c, LAYER_OUTLINE);

    // Corps elliptique
    cairo_save(cr_flat);
    cairo_translate(cr_flat, cx, cy);
    cairo_scale(cr_flat, rx, ry);
    cairo_new_path(cr_flat);
    cairo_arc(cr_flat, 0, 0, 1.0, 0, 2 * M_PI);
    cairo_restore(cr_flat);
    Brush bf = brush_flat(bg_color);
    brush_fill(cr_flat, &bf);

    // Queue de la bulle
    cairo_new_path(cr_flat);
    cairo_move_to(cr_flat, cx - rx * 0.15, cy + ry * 0.85);
    cairo_line_to(cr_flat, tail_x, tail_y);
    cairo_line_to(cr_flat, cx + rx * 0.15, cy + ry * 0.85);
    cairo_close_path(cr_flat);
    brush_fill(cr_flat, &bf);

    // Contour ellipse
    cairo_save(cr_outline);
    cairo_translate(cr_outline, cx, cy);
    cairo_scale(cr_outline, rx, ry);
    cairo_new_path(cr_outline);
    cairo_arc(cr_outline, 0, 0, 1.0, 0, 2 * M_PI);
    cairo_restore(cr_outline);
    palette_apply(cr_outline, COL_INK);
    cairo_set_line_width(cr_outline, 3.5);
    cairo_stroke(cr_outline);

    // Contour queue
    cairo_new_path(cr_outline);
    cairo_move_to(cr_outline, cx - rx * 0.15, cy + ry * 0.85);
    cairo_line_to(cr_outline, tail_x, tail_y);
    cairo_line_to(cr_outline, cx + rx * 0.15, cy + ry * 0.85);
    cairo_close_path(cr_outline);
    palette_apply(cr_outline, COL_INK);
    cairo_set_line_width(cr_outline, 3.5);
    cairo_stroke(cr_outline);
}

// ─── Cityscape ────────────────────────────────────────────────
void shape_cityscape(Canvas *c,
                     double base_y, double width,
                     ColorID color, unsigned int seed) {
    srand(seed);
    cairo_t *cr_flat    = canvas_layer(c, LAYER_FLAT_COLOR);
    cairo_t *cr_outline = canvas_layer(c, LAYER_OUTLINE);

    double x = 0;
    while (x < width) {
        double bw = 40 + rand() % 80;
        double bh = 80 + rand() % 300;
        double by = base_y - bh;

        // Bâtiment
        cairo_new_path(cr_flat);
        cairo_rectangle(cr_flat, x, by, bw, bh);
        Brush bf = brush_flat(color);
        brush_fill(cr_flat, &bf);

        // Fenêtres (petits rectangles jaunes)
        int win_cols = (int)(bw / 14);
        int win_rows = (int)(bh / 18);
        for (int wr = 0; wr < win_rows; wr++) {
            for (int wc = 0; wc < win_cols; wc++) {
                if (rand() % 3 != 0) { // 2/3 des fenêtres allumées
                    double wx = x + 4 + wc * 14;
                    double wy = by + 6 + wr * 18;
                    cairo_new_path(cr_flat);
                    cairo_rectangle(cr_flat, wx, wy, 8, 10);
                    Brush bfw = brush_flat(COL_YELLOW_HI);
                    bfw.opacity = 0.85;
                    brush_fill(cr_flat, &bfw);
                }
            }
        }

        // Contour bâtiment
        cairo_new_path(cr_outline);
        cairo_rectangle(cr_outline, x, by, bw, bh);
        palette_apply(cr_outline, COL_INK);
        cairo_set_line_width(cr_outline, 2.5);
        cairo_stroke(cr_outline);

        x += bw + 2 + rand() % 10;
    }
}

// ─── Panneau d'action ─────────────────────────────────────────
void shape_action_panel(Canvas *c, ColorID color) {
    int w = c->width, h = c->height;
    cairo_t *cr = canvas_layer(c, LAYER_BACKGROUND);

    // Fond coloré
    Brush bf = brush_flat(color);
    cairo_new_path(cr);
    cairo_rectangle(cr, 0, 0, w, h);
    brush_fill(cr, &bf);

    // Diagonales dynamiques
    cairo_t *cr_fx = canvas_layer(c, LAYER_EFFECTS);
    palette_apply_alpha(cr_fx, COL_BLACK, 0.08);
    cairo_set_line_width(cr_fx, w * 0.015);
    int lines = 18;
    for (int i = 0; i < lines; i++) {
        double x = (double)i / lines * w * 1.5 - w * 0.25;
        cairo_move_to(cr_fx, x,        0);
        cairo_line_to(cr_fx, x + h * 0.3, h);
        cairo_stroke(cr_fx);
    }
}

// ─── Starburst ────────────────────────────────────────────────
void shape_starburst(Canvas *c,
                     double cx, double cy,
                     double r_inner, double r_outer,
                     int points, ColorID color) {
    cairo_t *cr_flat    = canvas_layer(c, LAYER_FLAT_COLOR);
    cairo_t *cr_outline = canvas_layer(c, LAYER_OUTLINE);

    cairo_new_path(cr_flat);
    for (int i = 0; i < points * 2; i++) {
        double angle = (double)i / (points * 2) * 2.0 * M_PI - M_PI * 0.5;
        double r = (i % 2 == 0) ? r_outer : r_inner;
        double x = cx + cos(angle) * r;
        double y = cy + sin(angle) * r;
        if (i == 0) cairo_move_to(cr_flat, x, y);
        else        cairo_line_to(cr_flat, x, y);
    }
    cairo_close_path(cr_flat);
    Brush bf = brush_flat(color);
    brush_fill(cr_flat, &bf);

    cairo_new_path(cr_outline);
    for (int i = 0; i < points * 2; i++) {
        double angle = (double)i / (points * 2) * 2.0 * M_PI - M_PI * 0.5;
        double r = (i % 2 == 0) ? r_outer : r_inner;
        double x = cx + cos(angle) * r;
        double y = cy + sin(angle) * r;
        if (i == 0) cairo_move_to(cr_outline, x, y);
        else        cairo_line_to(cr_outline, x, y);
    }
    cairo_close_path(cr_outline);
    palette_apply(cr_outline, COL_INK);
    cairo_set_line_width(cr_outline, 3.0);
    cairo_stroke(cr_outline);
}

// ─── Cape ondulante ───────────────────────────────────────────
void shape_cape(Canvas *c, ShapeParams *p) {
    double s  = p->scale;
    double cx = p->cx;
    double cy = p->cy;

    cairo_t *cr_flat    = canvas_layer(c, LAYER_FLAT_COLOR);
    cairo_t *cr_shadow  = canvas_layer(c, LAYER_SHADOW);
    cairo_t *cr_outline = canvas_layer(c, LAYER_OUTLINE);

    // Forme principale de la cape
    cairo_new_path(cr_flat);
    cairo_move_to(cr_flat, cx - s * 0.18, cy - s * 0.10);
    cairo_curve_to(cr_flat,
        cx - s * 0.35, cy + s * 0.15,
        cx - s * 0.40, cy + s * 0.35,
        cx - s * 0.15, cy + s * 0.55);
    cairo_curve_to(cr_flat,
        cx,            cy + s * 0.65,
        cx + s * 0.10, cy + s * 0.60,
        cx + s * 0.20, cy + s * 0.55);
    cairo_curve_to(cr_flat,
        cx + s * 0.42, cy + s * 0.30,
        cx + s * 0.38, cy + s * 0.10,
        cx + s * 0.18, cy - s * 0.10);
    cairo_close_path(cr_flat);
    Brush bf = brush_flat(p->color_base);
    brush_fill(cr_flat, &bf);

    // Ombre cape (pli central)
    cairo_new_path(cr_shadow);
    cairo_move_to(cr_shadow, cx,            cy - s * 0.05);
    cairo_curve_to(cr_shadow,
        cx - s * 0.05, cy + s * 0.20,
        cx - s * 0.08, cy + s * 0.40,
        cx,            cy + s * 0.58);
    cairo_curve_to(cr_shadow,
        cx + s * 0.06, cy + s * 0.40,
        cx + s * 0.04, cy + s * 0.20,
        cx + s * 0.05, cy - s * 0.05);
    cairo_close_path(cr_shadow);
    Brush bs = brush_shadow(p->color_shadow);
    brush_fill(cr_shadow, &bs);

    // Contour cape
    cairo_new_path(cr_outline);
    cairo_move_to(cr_outline, cx - s * 0.18, cy - s * 0.10);
    cairo_curve_to(cr_outline,
        cx - s * 0.35, cy + s * 0.15,
        cx - s * 0.40, cy + s * 0.35,
        cx - s * 0.15, cy + s * 0.55);
    cairo_curve_to(cr_outline,
        cx,            cy + s * 0.65,
        cx + s * 0.10, cy + s * 0.60,
        cx + s * 0.20, cy + s * 0.55);
    cairo_curve_to(cr_outline,
        cx + s * 0.42, cy + s * 0.30,
        cx + s * 0.38, cy + s * 0.10,
        cx + s * 0.18, cy - s * 0.10);
    cairo_close_path(cr_outline);
    palette_apply(cr_outline, COL_INK);
    cairo_set_line_width(cr_outline, s * 0.020);
    cairo_set_line_join(cr_outline, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke(cr_outline);
}

// ─── Boîte onomatopée ─────────────────────────────────────────
void shape_onomatopoeia_box(Canvas *c,
                             double x, double y,
                             double w, double h,
                             double angle,
                             ColorID bg_color) {
    cairo_t *cr_flat    = canvas_layer(c, LAYER_EFFECTS);
    cairo_t *cr_outline = canvas_layer(c, LAYER_OUTLINE);

    cairo_save(cr_flat);
    cairo_translate(cr_flat, x + w * 0.5, y + h * 0.5);
    cairo_rotate(cr_flat, angle);
    cairo_new_path(cr_flat);
    cairo_rectangle(cr_flat, -w * 0.5, -h * 0.5, w, h);
    Brush bf = brush_flat(bg_color);
    brush_fill(cr_flat, &bf);
    cairo_restore(cr_flat);

    cairo_save(cr_outline);
    cairo_translate(cr_outline, x + w * 0.5, y + h * 0.5);
    cairo_rotate(cr_outline, angle);
    cairo_new_path(cr_outline);
    cairo_rectangle(cr_outline, -w * 0.5, -h * 0.5, w, h);
    palette_apply(cr_outline, COL_INK);
    cairo_set_line_width(cr_outline, 4.0);
    cairo_stroke(cr_outline);
    cairo_restore(cr_outline);
}