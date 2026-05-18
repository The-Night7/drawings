#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canvas.h"
#include "generator.h"
#include "export.h"

// ─── Aide ─────────────────────────────────────────────────────
static void print_usage(const char *prog) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║       CEL SHADING COMIC GENERATOR                ║\n");
    printf("╚══════════════════════════════════════════════════╝\n\n");
    printf("Usage: %s [OPTIONS]\n\n", prog);
    printf("OPTIONS:\n");
    printf("  -s, --scene    <0-%d>   Type de scène (défaut: aléatoire)\n",
           SCENE_COUNT - 1);
    printf("  -f, --format   <0-2>    Format: 0=paysage 1=portrait 2=carré\n");
    printf("  -p, --palette  <0-4>    Palette: 0=rouge 1=bleu 2=jaune 3=vert 4=aléatoire\n");
    printf("  -r, --seed     <uint>   Graine aléatoire (défaut: 42)\n");
    printf("  -o, --output   <path>   Fichier de sortie PNG (défaut: output.png)\n");
    printf("  -b, --batch    <n>      Génère N images en batch\n");
    printf("  -a, --all               Génère toutes scènes × tous formats\n");
    printf("  -l, --layers            Exporte aussi les calques séparément\n");
    printf("  -h, --help              Affiche cette aide\n\n");
    printf("SCÈNES:\n");
    printf("  0 = Hero standalone    4 = Speech bubble\n");
    printf("  1 = Fight              5 = Villain avec cape\n");
    printf("  2 = Cityscape          6 = Impact / onomatopée\n");
    printf("  3 = Speed lines\n\n");
    printf("EXEMPLES:\n");
    printf("  %s -s 0 -f 1 -p 1 -r 1337 -o hero.png\n", prog);
    printf("  %s -b 10 -s 1 -f 0 -o ./output\n", prog);
    printf("  %s --all -o ./gallery\n\n", prog);
}

// ─── Main ─────────────────────────────────────────────────────
int main(int argc, char *argv[]) {
    // Valeurs par défaut
    int          scene       = -1;        // -1 = aléatoire
    int          format      = FMT_LANDSCAPE;
    int          palette_set = 4;         // aléatoire
    unsigned int seed        = 42;
    char         output[512] = "output.png";
    int          batch       = 0;
    int          export_all_flag = 0;
    int          export_layers   = 0;

    // ── Parsing des arguments ─────────────────────────────────
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            print_usage(argv[0]);
            return 0;
        }
        else if ((!strcmp(argv[i], "-s") || !strcmp(argv[i], "--scene"))
                 && i + 1 < argc) {
            scene = atoi(argv[++i]);
        }
        else if ((!strcmp(argv[i], "-f") || !strcmp(argv[i], "--format"))
                 && i + 1 < argc) {
            format = atoi(argv[++i]);
        }
        else if ((!strcmp(argv[i], "-p") || !strcmp(argv[i], "--palette"))
                 && i + 1 < argc) {
            palette_set = atoi(argv[++i]);
        }
        else if ((!strcmp(argv[i], "-r") || !strcmp(argv[i], "--seed"))
                 && i + 1 < argc) {
            seed = (unsigned int)atoi(argv[++i]);
        }
        else if ((!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output"))
                 && i + 1 < argc) {
            strncpy(output, argv[++i], sizeof(output) - 1);
        }
        else if ((!strcmp(argv[i], "-b") || !strcmp(argv[i], "--batch"))
                 && i + 1 < argc) {
            batch = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--all")) {
            export_all_flag = 1;
        }
        else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--layers")) {
            export_layers = 1;
        }
        else {
            fprintf(stderr, "⚠️  Option inconnue : %s\n", argv[i]);
        }
    }

    // ── Mode : export toutes scènes ───────────────────────────
    if (export_all_flag) {
        printf("🚀 Export complet (toutes scènes × tous formats)...\n");
        export_all(seed, output[0] ? output : "./gallery");
        return 0;
    }

    // ── Mode : batch ──────────────────────────────────────────
    if (batch > 0) {
        if (scene < 0) scene = 0;
        printf("🚀 Batch : %d images, scène %d, format %d\n",
               batch, scene, format);
        export_batch((CanvasFormat)format, (SceneType)scene,
                     palette_set, batch, output);
        return 0;
    }

    // ── Mode : image unique ───────────────────────────────────
    Canvas *c = canvas_create((CanvasFormat)format);

    GeneratorParams p = {
        .scene       = (SceneType)scene,
        .format      = (CanvasFormat)format,
        .seed        = seed,
        .palette_set = palette_set,
    };

    generator_run(c, &p);

    export_png(c, output);

    if (export_layers) {
        char prefix[512];
        snprintf(prefix, sizeof(prefix), "%s_debug", output);
        export_layers_png(c, prefix);
    }

    canvas_destroy(c);
    return 0;
}