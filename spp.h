#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int echo;

#define TAG_CALLBACK(x) void x (char *buf, FILE *out)

struct Tag {
	const char *name;
	TAG_CALLBACK((*callback));
};

void spp_run(char *buf, FILE *out);
void spp_eval(char *buf, FILE *out);
void spp_io(FILE *in, FILE *out);
int spp_file(const char *file, FILE *out);

#include "config.h"

static char *tag_pre = TAG_PRE;
static char *tag_post = TAG_POST;
static char *token = TOKEN;
static int tag_begin = TAG_BEGIN;
int echo = ECHO;

#if DEBUG
#define D if (1)
#else
#define D if (0)
#endif

