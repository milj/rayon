#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "/usr/include/GL/glut.h"
#include "../libmgf/parser.h"
#include "../radiance/color.h"
#include "../radiance/resolu.h"


#define MIN(a,b) ((a)<(b))?(a):(b)
#define MAX(a,b) ((a)<(b))?(b):(a)
#define SQR(x) (x*x)
#define UNIFORM(a,b) ((double)rand()*((b)-(a))/RAND_MAX+(a))


#define INITX 100
#define INITY 100
#define DEFAULT_SCREENX 600
#define DEFAULT_SCREENY 300
#define DEFAULT_ANGLE_OF_VIEW 40
#define INTERACTIVE_MAP MAP_TM
#define DEFAULT_RAYTRACING_DEPTH 20
#define LIGHT_THR 0.1
#define AMBIENT 0.0
#define CLIP_NEAR 0.1
#define CLIP_FAR 30.0
#define NQCDIVS 5
#define KD_MIN_NUM 3
#define KD_DEPTH 18
#define KD_MAX_DEPTH 50
#define EPS 0.0000001
#define LUMI_R 0.265
#define LUMI_G 0.670
#define LUMI_B 0.065
#define GAMMA 2.6
#define DEFAULT_PATHS_PER_PIXEL 100
#define DEFAULT_LIGHT_SAMPLES 1




typedef enum {FALSE, TRUE}
bool;


typedef enum {SOLID, WIREFRAME, LIGHTS}
view_type;


typedef enum {VIEWER, RAYTRACER, BATCH_RAYTRACER, TONE_MAPPER}
mode_type;


typedef enum {MAP_HD, MAP_EV, MAP_TM, MAP_TW, MAP_TG, MAP_TL}
tone_mapping_type;


typedef enum {REFLECT, TRANSMIT}
ray_type;


typedef enum {OUT_OF_MEM, BAD_USAGE}
error_type;


#include "scene.h"
#include "mgfview.h"
#include "camera.h"
#include "geometry.h"
#include "dodatki.h"
#include "saves.h"
#include "main.h"
#include "raytrace.h"
#include "triangle_moller.h"
#include "kdtree.h"
#include "colours.h"
#include "tone_mapping.h"
