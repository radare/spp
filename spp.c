/* MIT (C) pancake (at) nopcode (dot) org - 2009-2017 */

#include "spp.h"
#include "config.h"

// TODO: avoid globals

static char *lbuf = NULL;
static int lbuf_s = 1024;
static int lbuf_n = 0;
//static char *tag_pre, *tag_post, *token = NULL;

//int echo[MAXIFL];
//int ifl = 0; /* conditional nest level */

int spp_run(char *buf, Output *out) {
	int i, ret = 0;
	char *tok;

	D fprintf (stderr, "SPP_RUN(%s)\n", buf);
	if (proc->chop) {
		for (; IS_SPACE (*buf); buf++);
		int buflen = strlen (buf);
		for (tok = buf + (buflen? buflen - 1: 0); IS_SPACE (*tok); tok--) {
			*tok = '\0';
		}
	}

	if (proc->token) {
		tok = strstr (buf, proc->token);
		if (tok) {
			*tok = '\0';
			tok = tok + 1;
		} else {
			tok = buf;
		}
	} else {
		tok = buf;
	}
	for (i = 0; tags[i].callback; i++) {
		D fprintf (stderr, "NAME=(%s)\n", tok);
		if ((!tags[i].name) || (!strcmp (buf, tags[i].name))) {
			if (out->fout) {
				fflush (out->fout);
			}
			ret = tags[i].callback (&proc->state, tok, out);
			proc->state.ifl += ret;
			if (ret == -1) {
				break;
			}
			if (ret) {

				if (proc->state.ifl < 0 || proc->state.ifl >= MAXIFL) {
					fprintf (stderr, "Nested conditionals parsing error.\n");
					break;
				}
			}
			break;
		}
	}
	return ret;
}

static char *spp_run_str(char *buf, int *rv) {
	char *b;
	Output tmp;
	tmp.fout = NULL;
	tmp.cout = r_strbuf_new ("");
	int rc = spp_run (buf, &tmp);
	b = strdup (r_strbuf_get (tmp.cout));
	r_strbuf_free (tmp.cout);
	if (rv) {
		*rv = rc;
	}
	return b;
}

void lbuf_strcat(char *dst, char *src) {
	int len = strlen (src);
	if (!lbuf || (len + lbuf_n) > lbuf_s) {
		lbuf = realloc (lbuf, lbuf_s << 1);
	}
	memcpy (lbuf + lbuf_n, src, len + 1);
	lbuf_n += len;
}

void do_printf(Output *out, char *str, ...) {
	va_list ap;
	va_start (ap, str);
	if (out->fout) {
		vfprintf (out->fout, str, ap);
	} else {
		char tmp[4096];
		vsnprintf (tmp, sizeof (tmp), str, ap);
		r_strbuf_append (out->cout, tmp);
	}
	va_end (ap);
}

int do_fputs(Output *out, char *str) {
	int i;
	int printed = 0;
	for (i = 0; i <= proc->state.ifl; i++) {
		if (!proc->state.echo[i]) {
			return printed;
		}
	}
	if (str[0]) {
		printed = 1;
	}
	if (proc->fputs) {
		proc->fputs (out, str);
	} else {
		if (out->fout) {
			fprintf (out->fout, "%s", str);
		}
	}
	return printed;
}

void spp_eval(char *buf, Output *out) {
	char *ptr, *ptr2;
	char *ptrr = NULL;
	int delta;

	int printed = 0;
retry:
	/* per word */
	if (!proc->tag_pre && proc->token) {
		do {
			ptr = strstr (buf, proc->token);
			if (ptr) {
				*ptr = '\0';
			}
			delta = strlen (buf) - 1;
			if (buf[delta] == '\n') {
				buf[delta] = '\0';
			}
			if (*buf) {
				spp_run (buf, out);
			}
			buf = ptr + 1;
		} while (ptr);
		return;
	}

	if (!proc->tag_post) {
		/* handle per line here ? */
		return;
	}

	// TODO: do it in scope!
	delta = strlen (proc->tag_post);

	/* (pre) tag */
	ptr = proc->tag_pre? strstr (buf, proc->tag_pre): NULL;
	if (ptr) {
		D printf ("==> 0.0 (%s)\n", ptr);
		if (!proc->tag_begin || (proc->tag_begin && ptr == buf)) {
			*ptr = '\0';
			ptr = ptr + strlen (proc->tag_pre);
			if (do_fputs (out, buf)) {
				printed = 1;
			}
			D printf ("==> 0 (%s)\n", ptr);
		}
		ptrr = strstr (ptr + strlen (proc->tag_pre), proc->tag_pre);
	}

	/* (post) tag */
	if (!ptr) {
		if (do_fputs (out, buf)) {
			printed = 1;
		}
		return;
	}
	ptr2 = strstr (ptr, proc->tag_post);
	if (ptr2) {
		*ptr2 = '\0';
		if (ptrr) {
			if (ptrr < ptr2) {
				char *p = strdup (ptr2 + 2);
				char *s = spp_run_str (ptrr + strlen (proc->tag_pre), NULL);
				D fprintf (stderr, "strcpy(%s)(%s)\n", ptrr, s);
				strcpy (ptrr, s);
				free (s);
				ptr[-2] = proc->tag_pre[0]; // XXX -2 check underflow?

				D fprintf (stderr, "strcat(%s)(%s)\n", ptrr, p);
				strcat (ptrr, p);
				buf = ptr - 2;
				D fprintf (stderr, "CONTINUE (%s)\n", buf);
				free (p);
				ptrr = NULL;
				goto retry;
			}
		}
		if (lbuf && lbuf[0]) {
			D printf("==> 1 (%s)\n", lbuf);
			if (ptr) {
				lbuf_strcat (lbuf, buf);
				if (do_fputs (out, buf)) {
					printed = 1;
				}
				spp_run (ptr, out);
			} else {
				lbuf_strcat (lbuf, buf);
				D printf ("=(1)=> spp_run(%s)\n", lbuf);
				spp_run (lbuf+delta, out);
				D printf ("=(1)=> spp_run(%s)\n", lbuf);
			}
			lbuf[0]='\0';
			lbuf_n = 0;
		} else {
			D printf ("==> 2 (%s)\n", ptr);
			if (ptr) {
				D printf (" ==> 2.1: run(%s)\n", ptr);
				spp_run (ptr, out);
				buf = ptr2 + delta;
				if (buf[0] == '\n' && printed) {
					buf++;
				}
				D printf (" ==> 2.1: continue(%s)\n", buf);
				goto retry;
			} else {
				if (do_fputs (out, buf)) {
					printed = 1;
				}
			}
		}
		if (do_fputs (out, buf)) {
			printed = 1;
		}
	} else {
		D printf ("==> 3\n");
		lbuf_strcat (lbuf, ptr);
	}
}

/* TODO: detect nesting */
void spp_io(FILE *in, Output *out) {
	char buf[4096];
	int lines;
	if (!lbuf) {
		lbuf = calloc (1, 4096);
	}
	if (!lbuf) {
		fprintf (stderr, "Out of memory.\n");
		return;
	}
	lbuf[0] = '\0';
	while (!feof (in)) {
		buf[0] = '\0'; // ???
		fgets (buf, 1023, in);
		if (feof (in)) break;
		lines = 1;
		if (!memcmp (buf, "#!", 2)) {
			fgets (buf, 1023, in);
			if (feof (in)) break;
			lines++;
		}
		if (proc->multiline) {
			while (1) {
				char *eol = buf + strlen (buf) - strlen (proc->multiline);
				if (!strcmp (eol, proc->multiline)) {
					D fprintf (stderr, "Multiline detected!\n");
					fgets (eol, 1023, in);
					if (feof (in)) {
						break;
					}
					lines++;
				} else {
					break;
				}
			}
		}
		spp_eval (buf, out);
		proc->state.lineno += lines;
	}
	do_fputs (out, lbuf);
}

int spp_file(const char *file, Output *out) {
	FILE *in = fopen (file, "r");
	D fprintf (stderr, "SPP-FILE(%s)\n", file);
	if (in) {
		spp_io (in, out);
		fclose (in);
		return 1;
	}
	fprintf (stderr, "Cannot find '%s'\n", file);
	return 0;
}

void spp_proc_list_kw() {
	int i;
	for (i = 0; tags[i].name; i++) {
		printf ("%s\n", tags[i].name);
	}
}

void spp_proc_list() {
	int i;
	for (i=0; procs[i]; i++) {
		printf ("%s\n", procs[i]->name);
	}
}

void spp_proc_set(struct Proc *p, char *arg, int fail) {
	int i, j;
	if (arg)
	for (j = 0; procs[j]; j++) {
		if (!strcmp (procs[j]->name, arg)) {
			proc = procs[j];
			D printf ("SET PROC:(%s)(%s)\n", arg, proc->name);
			break;
		}
	}
	if (arg && *arg && !procs[j] && fail) {
		fprintf (stderr, "Invalid preprocessor name '%s'\n", arg);
		return;
	}
	if (!proc) {
		proc = p;
	}
	if (proc) {
		proc->state.lineno = 1;
		proc->state.ifl = 0;
		for (i = 0; i < MAXIFL; i++) {
			proc->state.echo[i] = proc->default_echo;
		}
		//args = (struct Arg*)proc->args;
		tags = (struct Tag*)proc->tags;
	}
}
