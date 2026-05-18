#ifndef SHAPES_H
#define SHAPES_H

#include "canvas.h"
#include "brush.h"

// ─── Paramètres d'une silhouette ──────────────────────────────
typedef struct {
    double cx, cy;      // centre
    double scale;       // échelle globale
    double angle;       // rotation
    ColorID color_base;
    ColorID color_shadow;
    ColorID color_hi;
    unsigned int seed;  // pour la variation aléatoire
} ShapeParams;

// ─── Formes de base ───────────────────────────────────────────

// Corps humain stylisé (silhouette)
void shape_humanoid(Canvas *c, ShapeParams *p);

// Visage stylisé (vue de face)
void shape_face(Canvas *c, ShapeParams *p);

// Explosion / impact
void shape_explosion(Canvas *c, double cx, double cy,
                     double radius, ColorID color,
                     unsigned int seed);

// Onomatopée géométrique (cadre + fond)
void shape_onomatopoeia_box(Canvas *c,
                             double x, double y,
                             double w, double h,
                             double angle,
                             ColorID bg_color);

// Bulle de dialogue
void shape_speech_bubble(Canvas *c,
                          double cx, double cy,
                          double rx, double ry,
                          double tail_x, double tail_y,
                          ColorID bg_color);

// Décor urbain (buildings en silhouette)
void shape_cityscape(Canvas *c,
                     double base_y, double width,
                     ColorID color, unsigned int seed);

// Panneau d'action (fond coloré avec diagonales)
void shape_action_panel(Canvas *c, ColorID color);

// Étoiles / éclats décoratifs
void shape_starburst(Canvas *c,
                     double cx, double cy,
                     double r_inner, double r_outer,
                     int points, ColorID color);

// Cape / forme fluide ondulante
void shape_cape(Canvas *c, ShapeParams *p);

#endif