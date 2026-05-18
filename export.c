#include "export.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

// ─── Export PNG simple ────────────────────────────────────────
void export_png(Canvas *c, const char *path) {
    canvas_save_png(c, path);
}

// ─── Export calques séparés ───────────────────────────────────
void export_layers_png(Canvas *c, const char *prefix) {
    for (int i = 0; i < LAYER_COUNT; i++) {
        char path[512];
        snprintf(path, sizeof(path), "%s_layer_%d_%s.png",
                 prefix, i, LAYER_NAMES[i]);
        cairo_status_t st =
            cairo_surface_write_to_png(c->layers[i], path);
        if (st == CAIRO_STATUS_SUCCESS)
            printf("  📄 Calque exporté : %s\n", path);
        else
            fprintf(stderr, "  ❌ Erreur calque %d : %s\n",
                    i, cairo_status_to_string(st));
    }
}

// ─── Export batch ─────────────────────────────────────────────
void export_batch(CanvasFormat fmt, SceneType scene,
                  int palette_set, int count,
                  const char *out_dir) {
    mkdir(out_dir, 0755);

    static const char *fmt_names[] = {"landscape", "portrait", "square"};
    static const char *scene_names[] = {
        "hero", "fight", "cityscape",
        "speedlines", "speech", "villain", "impact"
    };

    for (int i = 0; i < count; i++) {
        Canvas *c = canvas_create(fmt);

        GeneratorParams p = {
            .scene       = scene,
            .format      = fmt,
            .seed        = (unsigned int)(42 + i * 137),
            .palette_set = palette_set,
        };

        generator_run(c, &p);

        char path[512];
        snprintf(path, sizeof(path),
                 "%s/%s_%s_%03d.png",
                 out_dir,
                 scene_names[scene],
                 fmt_names[fmt],
                 i);

        export_png(c, path);
        canvas_destroy(c);
    }

    printf("✅ Batch terminé : %d images dans '%s'\n", count, out_dir);
}

// ─── Export toutes scènes × tous formats ──────────────────────
void export_all(unsigned int seed, const char *out_dir) {
    mkdir(out_dir, 0755);

    static const char *fmt_names[]   = {"landscape", "portrait", "square"};
    static const char *scene_names[] = {
        "hero", "fight", "cityscape",
        "speedlines", "speech", "villain", "impact"
    };

    CanvasFormat formats[] = {FMT_LANDSCAPE, FMT_PORTRAIT, FMT_SQUARE};

    for (int f = 0; f < 3; f++) {
        for (int s = 0; s < SCENE_COUNT; s++) {
            Canvas *c = canvas_create(formats[f]);

            GeneratorParams p = {
                .scene       = (SceneType)s,
                .format      = formats[f],
                .seed        = seed + (unsigned int)(f * 100 + s * 13),
                .palette_set = 4, // aléatoire
            };

            generator_run(c, &p);

            char path[512];
            snprintf(path, sizeof(path),
                     "%s/%s_%s.png",
                     out_dir,
                     scene_names[s],
                     fmt_names[f]);

            export_png(c, path);
            canvas_destroy(c);

            printf("  ✔ %s\n", path);
        }
    }

    printf("\n🎉 Export complet : %d images dans '%s'\n",
           3 * SCENE_COUNT, out_dir);
}