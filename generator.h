#ifndef GENERATOR_H
#define GENERATOR_H

#include "canvas.h"
#include "shapes.h"
#include "brush.h"
#include "palette.h"

// ─── Types de scènes générables ───────────────────────────────
typedef enum {
    SCENE_HERO_STANDALONE,    // héros seul, pose dynamique
    SCENE_FIGHT,              // combat avec explosion
    SCENE_CITYSCAPE,          // décor urbain + personnage
    SCENE_SPEEDLINES,         // focus speed lines + personnage
    SCENE_SPEECH,             // personnage + bulle de dialogue
    SCENE_VILLAIN,            // antagoniste avec cape
    SCENE_IMPACT,             // impact / onomatopée géante
    SCENE_COUNT
} SceneType;

// ─── Paramètres de génération ─────────────────────────────────
typedef struct {
    SceneType     scene;
    CanvasFormat  format;
    unsigned int  seed;        // graine pour reproductibilité
    int           palette_set; // 0=rouge, 1=bleu, 2=jaune, 3=vert, 4=aléatoire
} GeneratorParams;

// ─── Jeu de couleurs d'un personnage ─────────────────────────
typedef struct {
    ColorID base;
    ColorID shadow;
    ColorID highlight;
    ColorID accent;
} CharacterPalette;

// ─── API principale ───────────────────────────────────────────

// Génère une scène complète sur le canvas
void generator_run(Canvas *c, GeneratorParams *p);

// Scènes individuelles
void gen_scene_hero_standalone(Canvas *c, GeneratorParams *p);
void gen_scene_fight(Canvas *c, GeneratorParams *p);
void gen_scene_cityscape(Canvas *c, GeneratorParams *p);
void gen_scene_speedlines(Canvas *c, GeneratorParams *p);
void gen_scene_speech(Canvas *c, GeneratorParams *p);
void gen_scene_villain(Canvas *c, GeneratorParams *p);
void gen_scene_impact(Canvas *c, GeneratorParams *p);

// Utilitaires internes
CharacterPalette gen_pick_palette(int palette_set, unsigned int seed);
void             gen_background_gradient(Canvas *c, ColorID top, ColorID bot);
void             gen_halftone_overlay(Canvas *c, double density);
void             gen_panel_border(Canvas *c, double thickness);
void             gen_dynamic_lines(Canvas *c, double cx, double cy,
                                   unsigned int seed);

#endif