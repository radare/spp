#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // only for unlink

#define VERSION "0.1"

extern int echo;

#define GET_ARG(x,y,i) if (y[i][2]) x = y[i]+2; else x = y[++i]

#define DEFAULT_PROC(x) \
struct Tag *tags = (struct Tag *)&x##_tags; \
struct Arg *args = (struct Arg *)&x##_args; \
struct Proc *proc = &x##_proc;

#define ARG_CALLBACK(x) int x (char *arg)
/* XXX swap arguments ?? */
#define TAG_CALLBACK(x) int x (char *buf, FILE *out)
#define PUT_CALLBACK(x) int x (FILE *out, char *buf)
#define IS_SPACE(x) ((x==' ')||(x=='\t')||(x=='\r')||(x=='\n'))

struct Tag {
	const char *name;
	TAG_CALLBACK((*callback));
};

struct Arg {
	const char *flag;
	const char *desc;
	int has_arg;
	ARG_CALLBACK((*callback));
};

struct Proc {
	const char *name;
	struct Tag **tags;
	struct Arg **args;
	TAG_CALLBACK ((*eof));
	PUT_CALLBACK ((*fputs));
	char *tag_pre;
	char *tag_post;
	char *token;
	char *multiline;
	int chop;
	int tag_begin;
	int default_echo;
};

int spp_file(const char *file, FILE *out);
void spp_run(char *buf, FILE *out);
void spp_eval(char *buf, FILE *out);
void spp_io(FILE *in, FILE *out);

#include "config.h"

char *tag_pre, *tag_post, *token;
int tag_begin, echo;

#if DEBUG
#define D if (1)
#else
#define D if (0)
#endif
