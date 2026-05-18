#ifndef EXPORT_H
#define EXPORT_H

#include "canvas.h"
#include "generator.h"

// ─── Export PNG ───────────────────────────────────────────────
// Sauvegarde le composite final en PNG
void export_png(Canvas *c, const char *path);

// Sauvegarde chaque calque séparément (debug)
void export_layers_png(Canvas *c, const char *prefix);

// ─── Export batch ─────────────────────────────────────────────
// Génère N images avec des seeds différentes
void export_batch(CanvasFormat fmt, SceneType scene,
                  int palette_set, int count,
                  const char *out_dir);

// ─── Export toutes scènes × tous formats ──────────────────────
void export_all(unsigned int seed, const char *out_dir);

#endif