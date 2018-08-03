#ifndef _INCLUDE_SPP_H_
#define _INCLUDE_SPP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "s_api.h"

#ifdef __WINDOWS__
#include <io.h>
#define popen    _popen
#define pclose   _pclose
#define srandom  srand
#define snprintf _snprintf
#endif

#define VERSION "1.0"

#define MAXIFL 128

extern int ifl;
extern int echo[MAXIFL];
extern int lineno;

#ifndef HAVE_FORK
#define HAVE_FORK 1
#endif

#ifndef HAVE_FORK
#define HAVE_FORK 1
#endif

#ifndef DLL_LOCAL
#ifdef _MSC_VER
#define DLL_LOCAL
#else
#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#endif
#endif

#define GET_ARG(x,y,i) if (y[i][2]) x = y[i] + 2; else x = y[++i]

#define DEFAULT_PROC(x) \
struct Tag *tags = (struct Tag *)&x##_tags; \
struct Arg *args = (struct Arg *)&x##_args; \
struct Proc *proc = &x##_proc;

typedef struct {
	SStrBuf *cout;
	FILE *fout;
	int size;
} Output;

typedef struct SppState {
	int lineno;
	int echo[MAXIFL];
	int ifl;
} SppState;

typedef struct SppBuf {
    char *lbuf;
    int lbuf_s;
    int lbuf_n;
} SppBuf;

#define ARG_CALLBACK(x) int x (char *arg)
/* XXX swap arguments ?? */
#define TAG_CALLBACK(x) int x (SppState *state, Output *out, char *buf)
#define PUT_CALLBACK(x) int x (Output *out, char *buf)
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
	SppState state;
	SppBuf buf;
};

R_API int spp_file(const char *file, Output *out);
R_API int spp_run(char *buf, Output *out);
R_API void spp_eval(char *buf, Output *out);
R_API void spp_io(FILE *in, Output *out);
#ifdef _MSC_VER
void do_printf(Output *out, char *str, ...);
#else
void do_printf(Output *out, char *str, ...) __attribute__ ((format (printf, 2, 3)));
#endif

R_API void spp_proc_list(void);
R_API void spp_proc_list_kw(void);
R_API void spp_proc_set(struct Proc *p, char *arg, int fail);

#if DEBUG
#define D if (1)
#else
#define D if (0)
#endif

#endif
