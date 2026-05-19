#include "shapes.h"
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ═══════════════════════════════════════════════════════════════
// HELPERS
// ═══════════════════════════════════════════════════════════════

// Dessine un chemin fermé sur flat + outline
static void draw_shape(cairo_t *cr_flat, cairo_t *cr_outline,
                       ColorID fill_col, double lw) {
    // Remplissage
    Color cf = palette_get(fill_col);
    cairo_set_source_rgba(cr_flat, cf.r, cf.g, cf.b, 1.0);
    cairo_fill_preserve(cr_flat);
    cairo_new_path(cr_flat);

    // Contour encre
    Color ci = palette_get(COL_INK);
    cairo_set_source_rgba(cr_outline, ci.r, ci.g, ci.b, 1.0);
    cairo_set_line_width(cr_outline, lw);
    cairo_set_line_cap(cr_outline, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr_outline, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke(cr_outline);
}

// Copie le chemin courant de cr_src vers cr_dst
// (on redessine le même path sur les deux contextes)
#define BEGIN_SHAPE(cr_f, cr_o) \
    cairo_new_path(cr_f); \
    cairo_new_path(cr_o);

// Macro : ajoute un point au path des deux contextes
#define MOVE2(cr_f,cr_o,x,y)       cairo_move_to(cr_f,x,y);  cairo_move_to(cr_o,x,y)
#define LINE2(cr_f,cr_o,x,y)       cairo_line_to(cr_f,x,y);  cairo_line_to(cr_o,x,y)
#define CURVE2(cr_f,cr_o,x1,y1,x2,y2,x3,y3) \
    cairo_curve_to(cr_f,x1,y1,x2,y2,x3,y3); \
    cairo_curve_to(cr_o,x1,y1,x2,y2,x3,y3)
#define CLOSE2(cr_f,cr_o) cairo_close_path(cr_f); cairo_close_path(cr_o)

// Applique ombre sur cr_shadow avec couleur donnée
static void shadow_fill(cairo_t *cr, ColorID col, double alpha) {
    Color c = palette_get(col);
    cairo_set_source_rgba(cr, c.r, c.g, c.b, alpha);
    cairo_fill(cr);
}

// ═══════════════════════════════════════════════════════════════
// SHAPE_HUMANOID  —  silhouette héroïque style comics
//
//  Proportions : 8 têtes de haut (standard super-héros)
//  s = hauteur totale du personnage
//  cx,cy = centre de la poitrine (haut du torse)
// ═══════════════════════════════════════════════════════════════
void shape_humanoid(Canvas *c, ShapeParams *p) {
    double s  = p->scale;   // hauteur totale
    double cx = p->cx;
    double cy = p->cy;      // centre poitrine

    cairo_t *cf  = canvas_layer(c, LAYER_FLAT_COLOR);
    cairo_t *csh = canvas_layer(c, LAYER_SHADOW);
    cairo_t *chi = canvas_layer(c, LAYER_HIGHLIGHT);
    cairo_t *co  = canvas_layer(c, LAYER_OUTLINE);

    // ── Unités de base (proportions 8 têtes) ─────────────────
    double u  = s / 8.0;   // 1 unité = 1 tête

    // Points de référence verticaux (depuis cy = haut du torse)
    double y_shoulder  = cy;                // épaules
    double y_chest     = cy + u * 0.9;      // milieu poitrine
    double y_waist     = cy + u * 1.8;      // taille
    double y_hip       = cy + u * 2.2;      // hanches
    double y_knee      = cy + u * 4.2;      // genoux
    double y_ankle     = cy + u * 6.0;      // chevilles
    double y_foot      = cy + u * 6.4;      // pieds
    double y_head_base = cy - u * 0.3;      // base du cou
    double y_head_mid  = cy - u * 1.1;      // milieu tête
    double y_head_top  = cy - u * 2.0;      // sommet tête

    // Largeurs
    double w_shoulder  = u * 1.55;   // demi-largeur épaules
    double w_chest     = u * 1.30;   // demi-largeur poitrine
    double w_waist     = u * 0.80;   // demi-largeur taille
    double w_hip       = u * 1.00;   // demi-largeur hanches
    double w_thigh     = u * 0.42;   // demi-largeur cuisse
    double w_knee_w    = u * 0.32;   // demi-largeur genou
    double w_calf      = u * 0.28;   // demi-largeur mollet
    double w_ankle_w   = u * 0.20;   // demi-largeur cheville
    double w_foot_w    = u * 0.38;   // demi-largeur pied
    double w_neck      = u * 0.28;   // demi-largeur cou
    double w_head      = u * 0.72;   // demi-largeur tête
    double w_jaw       = u * 0.58;   // demi-largeur mâchoire
    double w_arm_top   = u * 0.32;   // demi-largeur haut bras
    double w_arm_bot   = u * 0.24;   // demi-largeur avant-bras
    double w_fist      = u * 0.28;   // demi-largeur poing

    double lw_body = u * 0.13;   // épaisseur contour corps
    double lw_det  = u * 0.07;   // épaisseur contour détails

    // ── 1. JAMBE GAUCHE ──────────────────────────────────────
    {
        double ox = cx - u * 0.45;  // offset X centre jambe gauche
        BEGIN_SHAPE(cf, co)
        MOVE2(cf,co,  ox - w_thigh,   y_hip);
        CURVE2(cf,co, ox - w_thigh - u*0.08, y_hip + u*0.5,
                      ox - w_thigh,          y_knee - u*0.3,
                      ox - w_knee_w,         y_knee);
        CURVE2(cf,co, ox - w_calf,    y_knee + u*0.2,
                      ox - w_ankle_w, y_ankle - u*0.2,
                      ox - w_ankle_w, y_ankle);
        LINE2(cf,co,  ox - w_foot_w,  y_foot);
        LINE2(cf,co,  ox + w_foot_w + u*0.1, y_foot);
        LINE2(cf,co,  ox + w_ankle_w, y_ankle);
        CURVE2(cf,co, ox + w_calf,    y_ankle - u*0.2,
                      ox + w_knee_w,  y_knee + u*0.2,
                      ox + w_knee_w,  y_knee);
        CURVE2(cf,co, ox + w_thigh,   y_knee - u*0.3,
                      ox + w_thigh + u*0.08, y_hip + u*0.5,
                      ox + w_thigh,   y_hip);
        CLOSE2(cf,co);
        draw_shape(cf, co, p->color_base, lw_body);

        // Ombre intérieure jambe gauche
        cairo_new_path(csh);
        cairo_move_to(csh,    ox + w_thigh * 0.1, y_hip);
        cairo_curve_to(csh,   ox + w_thigh * 0.3, y_hip + u*0.8,
                              ox + w_knee_w * 0.5, y_knee - u*0.2,
                              ox + w_knee_w * 0.4, y_knee);
        cairo_line_to(csh,    ox + w_knee_w,  y_knee);
        cairo_curve_to(csh,   ox + w_thigh,   y_knee - u*0.3,
                              ox + w_thigh + u*0.08, y_hip + u*0.5,
                              ox + w_thigh,   y_hip);
        cairo_close_path(csh);
        shadow_fill(csh, p->color_shadow, 0.80);
    }

    // ── 2. JAMBE DROITE ───────────────────────────────────────
    {
        double ox = cx + u * 0.45;
        BEGIN_SHAPE(cf, co)
        MOVE2(cf,co,  ox - w_thigh,   y_hip);
        CURVE2(cf,co, ox - w_thigh - u*0.08, y_hip + u*0.5,
                      ox - w_thigh,          y_knee - u*0.3,
                      ox - w_knee_w,         y_knee);
        CURVE2(cf,co, ox - w_calf,    y_knee + u*0.2,
                      ox - w_ankle_w, y_ankle - u*0.2,
                      ox - w_ankle_w, y_ankle);
        LINE2(cf,co,  ox - w_foot_w,  y_foot);
        LINE2(cf,co,  ox + w_foot_w + u*0.1, y_foot);
        LINE2(cf,co,  ox + w_ankle_w, y_ankle);
        CURVE2(cf,co, ox + w_calf,    y_ankle - u*0.2,
                      ox + w_knee_w,  y_knee + u*0.2,
                      ox + w_knee_w,  y_knee);
        CURVE2(cf,co, ox + w_thigh,   y_knee - u*0.3,
                      ox + w_thigh + u*0.08, y_hip + u*0.5,
                      ox + w_thigh,   y_hip);
        CLOSE2(cf,co);
        draw_shape(cf, co, p->color_base, lw_body);

        // Ombre jambe droite (côté droit)
        cairo_new_path(csh);
        cairo_move_to(csh,    ox + w_thigh * 0.2, y_hip);
        cairo_curve_to(csh,   ox + w_thigh * 0.5, y_hip + u*0.6,
                              ox + w_knee_w,       y_knee - u*0.2,
                              ox + w_knee_w,       y_knee);
        cairo_curve_to(csh,   ox + w_thigh,        y_knee - u*0.3,
                              ox + w_thigh + u*0.08, y_hip + u*0.5,
                              ox + w_thigh,         y_hip);
        cairo_close_path(csh);
        shadow_fill(csh, p->color_shadow, 0.80);
    }

    // ── 3. TORSE ─────────────────────────────────────────────
    {
        BEGIN_SHAPE(cf, co)
        // Épaule gauche → taille gauche → hanche gauche
        MOVE2(cf,co,  cx - w_shoulder, y_shoulder);
        CURVE2(cf,co, cx - w_chest,    y_chest - u*0.1,
                      cx - w_chest,    y_chest + u*0.1,
                      cx - w_waist,    y_waist);
        CURVE2(cf,co, cx - w_hip + u*0.05, y_waist + u*0.2,
                      cx - w_hip,          y_hip - u*0.1,
                      cx - w_hip,          y_hip);
        LINE2(cf,co,  cx + w_hip,      y_hip);
        CURVE2(cf,co, cx + w_hip,      y_hip - u*0.1,
                      cx + w_hip - u*0.05, y_waist + u*0.2,
                      cx + w_waist,    y_waist);
        CURVE2(cf,co, cx + w_chest,    y_chest + u*0.1,
                      cx + w_chest,    y_chest - u*0.1,
                      cx + w_shoulder, y_shoulder);
        CLOSE2(cf,co);
        draw_shape(cf, co, p->color_base, lw_body);

        // Ombre torse droite
        cairo_new_path(csh);
        cairo_move_to(csh,    cx + w_chest * 0.4, y_shoulder);
        cairo_curve_to(csh,   cx + w_chest * 0.8, y_chest,
                              cx + w_waist * 0.8, y_waist,
                              cx + w_hip * 0.7,   y_hip);
        cairo_line_to(csh,    cx + w_hip,         y_hip);
        cairo_curve_to(csh,   cx + w_hip,         y_hip - u*0.1,
                              cx + w_waist + u*0.05, y_waist + u*0.2,
                              cx + w_waist,       y_waist);
        cairo_curve_to(csh,   cx + w_chest,       y_chest + u*0.1,
                              cx + w_chest,       y_chest - u*0.1,
                              cx + w_shoulder,    y_shoulder);
        cairo_close_path(csh);
        shadow_fill(csh, p->color_shadow, 0.75);

        // Highlight torse gauche
        cairo_new_path(chi);
        cairo_move_to(chi,    cx - w_chest * 0.6, y_shoulder + u*0.1);
        cairo_curve_to(chi,   cx - w_chest * 0.8, y_chest - u*0.1,
                              cx - w_chest * 0.7, y_chest + u*0.2,
                              cx - w_waist * 0.5, y_waist - u*0.1);
        cairo_curve_to(chi,   cx - w_chest * 0.3, y_chest + u*0.1,
                              cx - w_chest * 0.4, y_chest - u*0.2,
                              cx - w_chest * 0.3, y_shoulder + u*0.1);
        cairo_close_path(chi);
        Color ch = palette_get(p->color_hi);
        cairo_set_source_rgba(chi, ch.r, ch.g, ch.b, 0.45);
        cairo_fill(chi);

        // Ligne pectoraux
        cairo_new_path(co);
        cairo_move_to(co, cx - w_chest * 0.7, y_chest - u*0.15);
        cairo_curve_to(co, cx - w_chest * 0.3, y_chest + u*0.05,
                           cx - u*0.05,         y_chest + u*0.1,
                           cx,                  y_chest + u*0.05);
        cairo_move_to(co, cx + w_chest * 0.7, y_chest - u*0.15);
        cairo_curve_to(co, cx + w_chest * 0.3, y_chest + u*0.05,
                           cx + u*0.05,         y_chest + u*0.1,
                           cx,                  y_chest + u*0.05);
        Color ci2 = palette_get(COL_INK);
        cairo_set_source_rgba(co, ci2.r, ci2.g, ci2.b, 0.6);
        cairo_set_line_width(co, lw_det);
        cairo_stroke(co);

        // Ceinture
        cairo_new_path(cf);
        cairo_rectangle(cf, cx - w_waist - u*0.05,
                            y_waist - u*0.08,
                            (w_waist + u*0.05) * 2,
                            u * 0.22);
        Color cb2 = palette_get(p->color_shadow);
        cairo_set_source_rgba(cf, cb2.r, cb2.g, cb2.b, 1.0);
        cairo_fill(cf);
        cairo_new_path(co);
        cairo_rectangle(co, cx - w_waist - u*0.05,
                            y_waist - u*0.08,
                            (w_waist + u*0.05) * 2,
                            u * 0.22);
        cairo_set_source_rgba(co, ci2.r, ci2.g, ci2.b, 1.0);
        cairo_set_line_width(co, lw_det);
        cairo_stroke(co);
    }

    // ── 4. BRAS GAUCHE ────────────────────────────────────────
    {
        double y_elbow  = cy + u * 1.6;
        double y_wrist  = cy + u * 3.0;
        double ax       = cx - w_shoulder;

        BEGIN_SHAPE(cf, co)
        // Haut du bras
        MOVE2(cf,co,  ax,                  y_shoulder);
        CURVE2(cf,co, ax - w_arm_top,      y_shoulder + u*0.2,
                      ax - w_arm_top - u*0.1, y_elbow - u*0.3,
                      ax - w_arm_top * 0.8,   y_elbow);
        // Avant-bras
        CURVE2(cf,co, ax - w_arm_bot,      y_elbow + u*0.2,
                      ax - w_arm_bot,      y_wrist - u*0.2,
                      ax - w_fist,         y_wrist);
        // Poing
        LINE2(cf,co,  ax - w_fist,         y_wrist + u*0.35);
        LINE2(cf,co,  ax + w_fist * 0.3,   y_wrist + u*0.35);
        LINE2(cf,co,  ax + w_fist * 0.3,   y_wrist);
        // Retour avant-bras intérieur
        CURVE2(cf,co, ax + w_arm_bot * 0.5, y_wrist - u*0.2,
                      ax + w_arm_bot * 0.3,  y_elbow + u*0.2,
                      ax + w_arm_top * 0.1,  y_elbow);
        // Retour haut bras intérieur
        CURVE2(cf,co, ax - w_arm_top * 0.1, y_elbow - u*0.3,
                      ax,                    y_shoulder + u*0.4,
                      ax,                    y_shoulder);
        CLOSE2(cf,co);
        draw_shape(cf, co, p->color_base, lw_body);

        // Ombre bras gauche
        cairo_new_path(csh);
        cairo_move_to(csh,    ax - w_arm_top * 0.3, y_shoulder + u*0.1);
        cairo_curve_to(csh,   ax - w_arm_top,       y_shoulder + u*0.5,
                              ax - w_arm_top - u*0.1, y_elbow - u*0.2,
                              ax - w_arm_top * 0.8,   y_elbow);
        cairo_curve_to(csh,   ax - w_arm_bot,       y_elbow + u*0.2,
                              ax - w_arm_bot,        y_wrist - u*0.2,
                              ax - w_fist,           y_wrist);
        cairo_line_to(csh,    ax - w_fist,           y_wrist + u*0.35);
        cairo_line_to(csh,    ax - w_fist * 0.3,     y_wrist + u*0.35);
        cairo_line_to(csh,    ax - w_fist * 0.3,     y_wrist);
        cairo_curve_to(csh,   ax - w_arm_bot * 0.2,  y_wrist - u*0.3,
                              ax - w_arm_top * 0.5,   y_elbow,
                              ax - w_arm_top * 0.3,   y_shoulder + u*0.1);
        cairo_close_path(csh);
        shadow_fill(csh, p->color_shadow, 0.75);

        // Ligne coude
        cairo_new_path(co);
        cairo_move_to(co, ax - w_arm_top * 0.5, y_elbow - u*0.05);
        cairo_curve_to(co, ax - u*0.05, y_elbow + u*0.1,
                           ax + u*0.05, y_elbow + u*0.05,
                           ax + w_arm_top * 0.1, y_elbow);
        Color ci3 = palette_get(COL_INK);
        cairo_set_source_rgba(co, ci3.r, ci3.g, ci3.b, 0.5);
        cairo_set_line_width(co, lw_det);
        cairo_stroke(co);
    }

    // ── 5. BRAS DROIT ─────────────────────────────────────────
    {
        double y_elbow  = cy + u * 1.6;
        double y_wrist  = cy + u * 3.0;
        double ax       = cx + w_shoulder;

        BEGIN_SHAPE(cf, co)
        MOVE2(cf,co,  ax,                   y_shoulder);
        CURVE2(cf,co, ax + w_arm_top,       y_shoulder + u*0.2,
                      ax + w_arm_top + u*0.1, y_elbow - u*0.3,
                      ax + w_arm_top * 0.8,   y_elbow);
        CURVE2(cf,co, ax + w_arm_bot,       y_elbow + u*0.2,
                      ax + w_arm_bot,        y_wrist - u*0.2,
                      ax + w_fist,           y_wrist);
        LINE2(cf,co,  ax + w_fist,           y_wrist + u*0.35);
        LINE2(cf,co,  ax - w_fist * 0.3,     y_wrist + u*0.35);
        LINE2(cf,co,  ax - w_fist * 0.3,     y_wrist);
        CURVE2(cf,co, ax - w_arm_bot * 0.5,  y_wrist - u*0.2,
                      ax - w_arm_bot * 0.3,   y_elbow + u*0.2,
                      ax - w_arm_top * 0.1,   y_elbow);
        CURVE2(cf,co, ax + w_arm_top * 0.1,  y_elbow - u*0.3,
                      ax,                     y_shoulder + u*0.4,
                      ax,                     y_shoulder);
        CLOSE2(cf,co);
        draw_shape(cf, co, p->color_base, lw_body);

        // Ombre bras droit
        cairo_new_path(csh);
        cairo_move_to(csh,    ax + w_arm_top * 0.3,  y_shoulder + u*0.1);
        cairo_curve_to(csh,   ax + w_arm_top,         y_shoulder + u*0.5,
                              ax + w_arm_top + u*0.1,  y_elbow - u*0.2,
                              ax + w_arm_top * 0.8,    y_elbow);
        cairo_curve_to(csh,   ax + w_arm_bot,          y_elbow + u*0.2,
                              ax + w_arm_bot,           y_wrist - u*0.2,
                              ax + w_fist,              y_wrist);
        cairo_line_to(csh,    ax + w_fist,              y_wrist + u*0.35);
        cairo_line_to(csh,    ax + w_fist * 0.3,        y_wrist + u*0.35);
        cairo_line_to(csh,    ax + w_fist * 0.3,        y_wrist);
        cairo_curve_to(csh,   ax + w_arm_bot * 0.2,     y_wrist - u*0.3,
                              ax + w_arm_top * 0.5,      y_elbow,
                              ax + w_arm_top * 0.3,      y_shoulder + u*0.1);
        cairo_close_path(csh);
        shadow_fill(csh, p->color_shadow, 0.75);
    }

    // ── 6. COU ────────────────────────────────────────────────
    {
        BEGIN_SHAPE(cf, co)
        MOVE2(cf,co,  cx - w_neck, y_head_base);
        LINE2(cf,co,  cx - w_neck, y_shoulder + u*0.05);
        LINE2(cf,co,  cx + w_neck, y_shoulder + u*0.05);
        LINE2(cf,co,  cx + w_neck, y_head_base);
        CLOSE2(cf,co);
        draw_shape(cf, co, p->color_base, lw_body);

        // Ombre cou
        cairo_new_path(csh);
        cairo_rectangle(csh, cx, y_head_base,
                        w_neck, y_shoulder + u*0.05 - y_head_base);
        shadow_fill(csh, p->color_shadow, 0.60);
    }

    // ── 7. TÊTE ───────────────────────────────────────────────
    {
        // Crâne (ovale)
        BEGIN_SHAPE(cf, co)
        MOVE2(cf,co,  cx,           y_head_top);
        CURVE2(cf,co, cx + w_head,  y_head_top,
                      cx + w_head,  y_head_mid,
                      cx + w_jaw,   y_head_base - u*0.1);
        // Mâchoire
        CURVE2(cf,co, cx + w_jaw * 0.7, y_head_base + u*0.05,
                      cx + u*0.1,       y_head_base + u*0.15,
                      cx,               y_head_base + u*0.15);
        CURVE2(cf,co, cx - u*0.1,       y_head_base + u*0.15,
                      cx - w_jaw * 0.7, y_head_base + u*0.05,
                      cx - w_jaw,       y_head_base - u*0.1);
        CURVE2(cf,co, cx - w_head,  y_head_mid,
                      cx - w_head,  y_head_top,
                      cx,           y_head_top);
        CLOSE2(cf,co);
        draw_shape(cf, co, COL_SKIN_BASE, lw_body);

        // Ombre côté droit tête
        cairo_new_path(csh);
        cairo_move_to(csh,    cx + w_head * 0.3, y_head_top + u*0.1);
        cairo_curve_to(csh,   cx + w_head,        y_head_top,
                              cx + w_head,         y_head_mid,
                              cx + w_jaw,          y_head_base - u*0.1);
        cairo_curve_to(csh,   cx + w_jaw * 0.7,   y_head_base + u*0.05,
                              cx + u*0.1,          y_head_base + u*0.15,
                              cx + w_head * 0.3,   y_head_base);
        cairo_close_path(csh);
        shadow_fill(csh, COL_SKIN_SHADOW, 0.70);

        // Highlight front gauche
        cairo_new_path(chi);
        cairo_move_to(chi,    cx - w_head * 0.5, y_head_top + u*0.15);
        cairo_curve_to(chi,   cx - w_head * 0.2, y_head_top + u*0.05,
                              cx,                 y_head_top + u*0.1,
                              cx + w_head * 0.1,  y_head_top + u*0.25);
        cairo_curve_to(chi,   cx,                 y_head_mid - u*0.2,
                              cx - w_head * 0.3,  y_head_mid - u*0.1,
                              cx - w_head * 0.5,  y_head_top + u*0.15);
        cairo_close_path(chi);
        Color ch2 = palette_get(COL_SKIN_HI);
        cairo_set_source_rgba(chi, ch2.r, ch2.g, ch2.b, 0.55);
        cairo_fill(chi);

        // ── Yeux ──────────────────────────────────────────────
        double y_eye   = y_head_mid - u * 0.05;
        double eye_rx  = u * 0.18;
        double eye_ry  = u * 0.12;
        double eye_sep = u * 0.30;

        for (int side = -1; side <= 1; side += 2) {
            double ex = cx + side * eye_sep;

            // Blanc de l'œil
            cairo_save(cf);
            cairo_translate(cf, ex, y_eye);
            cairo_scale(cf, eye_rx, eye_ry);
            cairo_new_path(cf);
            cairo_arc(cf, 0, 0, 1.0, 0, 2 * M_PI);
            cairo_restore(cf);
            Color cw = palette_get(COL_WHITE);
            cairo_set_source_rgba(cf, cw.r, cw.g, cw.b, 1.0);
            cairo_fill(cf);

            // Iris
            cairo_new_path(cf);
            cairo_arc(cf, ex, y_eye, eye_ry * 0.65, 0, 2 * M_PI);
            Color ci4 = palette_get(COL_BLUE_DEEP);
            cairo_set_source_rgba(cf, ci4.r, ci4.g, ci4.b, 1.0);
            cairo_fill(cf);

            // Pupille
            cairo_new_path(cf);
            cairo_arc(cf, ex, y_eye, eye_ry * 0.30, 0, 2 * M_PI);
            Color cb3 = palette_get(COL_BLACK);
            cairo_set_source_rgba(cf, cb3.r, cb3.g, cb3.b, 1.0);
            cairo_fill(cf);

            // Reflet
            cairo_new_path(chi);
            cairo_arc(chi, ex - eye_rx * 0.2, y_eye - eye_ry * 0.3,
                      eye_ry * 0.18, 0, 2 * M_PI);
            Color cw2 = palette_get(COL_WHITE);
            cairo_set_source_rgba(chi, cw2.r, cw2.g, cw2.b, 0.9);
            cairo_fill(chi);

            // Contour œil
            cairo_save(co);
            cairo_translate(co, ex, y_eye);
            cairo_scale(co, eye_rx, eye_ry);
            cairo_new_path(co);
            cairo_arc(co, 0, 0, 1.0, 0, 2 * M_PI);
            cairo_restore(co);
            Color ci5 = palette_get(COL_INK);
            cairo_set_source_rgba(co, ci5.r, ci5.g, ci5.b, 1.0);
            cairo_set_line_width(co, lw_det * 1.2);
            cairo_stroke(co);

            // Sourcil
            cairo_new_path(co);
            cairo_move_to(co, ex - eye_rx * 0.9,
                              y_eye - eye_ry * 1.6);
            cairo_curve_to(co, ex - eye_rx * 0.2, y_eye - eye_ry * 2.0,
                               ex + eye_rx * 0.4,  y_eye - eye_ry * 1.8,
                               ex + eye_rx * 0.9,  y_eye - eye_ry * 1.5);
            cairo_set_source_rgba(co, ci5.r, ci5.g, ci5.b, 1.0);
            cairo_set_line_width(co, lw_det * 1.8);
            cairo_set_line_cap(co, CAIRO_LINE_CAP_ROUND);
            cairo_stroke(co);
        }

        // ── Nez ───────────────────────────────────────────────
        double y_nose = y_head_mid + u * 0.30;
        cairo_new_path(co);
        cairo_move_to(co, cx - u*0.08, y_nose);
        cairo_curve_to(co, cx - u*0.12, y_nose + u*0.08,
                           cx + u*0.12, y_nose + u*0.08,
                           cx + u*0.08, y_nose);
        Color ci6 = palette_get(COL_INK);
        cairo_set_source_rgba(co, ci6.r, ci6.g, ci6.b, 0.55);
        cairo_set_line_width(co, lw_det);
        cairo_stroke(co);

        // ── Bouche ────────────────────────────────────────────
        double y_mouth = y_head_base - u * 0.22;
        cairo_new_path(co);
        cairo_move_to(co, cx - u*0.22, y_mouth);
        cairo_curve_to(co, cx - u*0.10, y_mouth + u*0.12,
                           cx + u*0.10, y_mouth + u*0.12,
                           cx + u*0.22, y_mouth);
        cairo_set_source_rgba(co, ci6.r, ci6.g, ci6.b, 0.80);
        cairo_set_line_width(co, lw_det * 1.3);
        cairo_stroke(co);

        // ── Cheveux (masse simple) ────────────────────────────
        cairo_new_path(cf);
        cairo_move_to(cf, cx - w_head,       y_head_mid);
        cairo_curve_to(cf, cx - w_head,       y_head_top + u*0.1,
                           cx - w_head * 0.3, y_head_top - u*0.1,
                           cx,                y_head_top - u*0.05);
        cairo_curve_to(cf, cx + w_head * 0.3, y_head_top - u*0.1,
                           cx + w_head,        y_head_top + u*0.1,
                           cx + w_head,        y_head_mid);
        cairo_curve_to(cf, cx + w_head * 0.6, y_head_mid - u*0.3,
                           cx - w_head * 0.6,  y_head_mid - u*0.3,
                           cx - w_head,        y_head_mid);
        cairo_close_path(cf);
        Color chair = palette_get(COL_BLACK);
        cairo_set_source_rgba(cf, chair.r, chair.g, chair.b, 1.0);
        cairo_fill(cf);

        // Contour cheveux
        cairo_new_path(co);
        cairo_move_to(co, cx - w_head,       y_head_mid);
        cairo_curve_to(co, cx - w_head,       y_head_top + u*0.1,
                           cx - w_head * 0.3, y_head_top - u*0.1,
                           cx,                y_head_top - u*0.05);
        cairo_curve_to(co, cx + w_head * 0.3, y_head_top - u*0.1,
                           cx + w_head,        y_head_top + u*0.1,
                           cx + w_head,        y_head_mid);
        cairo_set_source_rgba(co, ci6.r, ci6.g, ci6.b, 1.0);
        cairo_set_line_width(co, lw_body);
        cairo_stroke(co);
    }
}

static void fill_and_outline(Canvas *c, LayerID fill_layer,
                              LayerID outline_layer,
                              ColorID fill_col, ColorID outline_col,
                              double outline_w) {
    Brush bf = brush_flat(fill_col);
    brush_fill(canvas_layer(c, fill_layer), &bf);
    cairo_t *cr_out = canvas_layer(c, outline_layer);
    palette_apply(cr_out, COL_INK);
    cairo_set_line_width(cr_out, outline_w);
    cairo_set_line_cap(cr_out, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join(cr_out, CAIRO_LINE_JOIN_ROUND);
    cairo_stroke(cr_out);
    (void)outline_col;
    (void)bf;
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