#ifndef BRUSH_H
#define BRUSH_H

#include <cairo.h>
#include "palette.h"
#include "canvas.h"

// ─── Types de pinceaux ────────────────────────────────────────
typedef enum {
    BRUSH_INK,        // contour dur, noir, épaisseur variable
    BRUSH_FLAT,       // aplat de couleur franc, sans anti-alias
    BRUSH_SHADOW,     // ombre dure (offset + couleur sombre)
    BRUSH_HIGHLIGHT,  // highlight dur (blanc/clair)
    BRUSH_SPEEDLINE,  // lignes de vitesse rayonnantes
    BRUSH_HALFTONE,   // trame de points style comics
    BRUSH_COUNT
} BrushType;

// ─── Paramètres d'un pinceau ──────────────────────────────────
typedef struct {
    BrushType type;
    double    size;         // épaisseur de base
    double    opacity;      // 0.0 → 1.0
    ColorID   color;        // couleur principale
    double    hardness;     // 0=doux 1=dur (cel shading = 1.0)
    double    jitter;       // irrégularité du trait (0=parfait)
    int       taper;        // 1 = effilement aux extrémités
} Brush;

// ─── Constructeurs prédéfinis ─────────────────────────────────
Brush brush_ink(double size);
Brush brush_flat(ColorID color);
Brush brush_shadow(ColorID shadow_color);
Brush brush_highlight(void);
Brush brush_speedline(void);
Brush brush_halftone(ColorID color, double dot_size);

// ─── Primitives de tracé ──────────────────────────────────────

// Trait entre deux points (avec effilement si taper=1)
void brush_stroke(cairo_t *cr, Brush *b,
                  double x1, double y1,
                  double x2, double y2);

// Courbe de Bézier cubique
void brush_bezier(cairo_t *cr, Brush *b,
                  double x1, double y1,
                  double cx1, double cy1,
                  double cx2, double cy2,
                  double x2, double y2);

// Remplissage d'un chemin déjà défini
void brush_fill(cairo_t *cr, Brush *b);

// Contour d'un chemin déjà défini
void brush_stroke_path(cairo_t *cr, Brush *b);

// Cercle plein
void brush_circle(cairo_t *cr, Brush *b,
                  double cx, double cy, double radius);

// Ellipse pleine
void brush_ellipse(cairo_t *cr, Brush *b,
                   double cx, double cy,
                   double rx, double ry, double angle);

// Speed lines rayonnant depuis un centre
void brush_speedlines(cairo_t *cr, Brush *b,
                      double cx, double cy,
                      double inner_r, double outer_r,
                      int count, unsigned int seed);

// Trame de demi-teintes (halftone)
void brush_halftone_fill(cairo_t *cr, Brush *b,
                         double x, double y,
                         double w, double h,
                         double spacing);

#endif