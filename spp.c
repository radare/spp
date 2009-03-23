/* Copyleft (r) pancake (at) nopcode (dot) org */

#include "spp.h"

char *lbuf = NULL;
int lbuf_s = 1024;
int lbuf_n = 0;

#define IS_SPACE(x) ((x==' ')||(x=='\t')||(x=='\r')||(x=='\n'))

void spp_run(char *buf, FILE *out)
{
	int i;
	char *tok;

D fprintf(stderr, "SPP_RUN(%s)\n", buf);
	if (proc->chop) {
		for(;IS_SPACE(*buf);buf=buf+1);
		for(tok = buf+strlen(buf)-1; IS_SPACE(*tok);tok=tok-1)*tok='\0';
	}

	if (token) {
		tok = strstr(buf, token);
		if (tok) {
			*tok = '\0';
			tok = tok + 1;
		} else tok = buf;
	} else tok = buf;

	for(i=0;tags[i].callback;i++) {
		D fprintf(stderr, "NAME=(%s)\n", tok);
		if ((tags[i].name==NULL)||(!strcmp(buf, tags[i].name))) {
			fflush(out);
			tags[i].callback(tok, out);
			break;
		}
	}
}

char *spp_run_str(char *buf)
{
	char b[1024];
	FILE *fd = fopen(".out", "w");
	spp_run(buf, fd);
	fclose(fd);
	fd = fopen(".out", "r");
	fseek(fd, 0, SEEK_SET);
	memset(b,'\0', 1024);
	fread(b, 1, 1023, fd);
	fclose(fd);
	unlink(".out");
	return strdup(b);
}

void lbuf_strcat(char *dst, char *src)
{
	int len = strlen(src);
	if ((len+lbuf_n) > lbuf_s)
		lbuf = realloc(lbuf, lbuf_s<<1);
	memcpy(lbuf+lbuf_n, src, len+1);
	lbuf_n += len;
}

void spp_fputs(FILE *out, char *str)
{
	E {
		if (proc->fputs)
			proc->fputs(out, str);
		else fprintf(out, "%s", str);
	}
}

int inside_command = 0;

void spp_eval(char *buf, FILE *out)
{
	char *ptr, *ptr2;
	char *ptrr = NULL;
	int delta;

retry:
	/* per word */
	if (tag_pre == NULL) {
		do {
			ptr = strstr(buf, token);
			if (ptr) *ptr='\0';
			delta = strlen(buf)-1;
			if (buf[delta]=='\n')
				buf[delta]='\0';
			if (*buf) spp_run(buf, out);
			buf = ptr+1;
		} while(ptr);
		return;
	}

	if (tag_post == NULL) {
		/* handle per line here ? */
		return;
	}

	delta = strlen(tag_post);

	//D printf("-----------(%s)\n", buf);
	/* (pre) tag */
	ptr = strstr(buf, tag_pre);
	if (ptr) {
		D printf("==> 0.0 (%s)\n", ptr);
inside_command = 1;
		if (!tag_begin || (tag_begin && ptr == buf)) {
			*ptr = '\0';
			ptr = ptr + strlen(tag_pre);;
			spp_fputs(out, buf);
			D printf("==> 0 (%s)\n", ptr);
		}
		ptrr = strstr(ptr+strlen(tag_pre), tag_pre);
	}

	/* (post) tag */
	//if (ptr) ptr2 = strstr(ptr, tag_post);
	//else ptr2 = strstr(buf, tag_post);
	if (ptr) ptr2 = strstr(ptr, tag_post);
	else {
		spp_fputs(out, buf);
		return;
	}
	if (ptr2) {
		*ptr2 = '\0';

	if (ptrr) {
		if (ptrr<ptr2) {
			char *p = strdup(ptr2+2);
			char *s = spp_run_str(ptrr+strlen(tag_pre));
			D fprintf(stderr, "NESTED (%s)\n", ptrr+strlen(tag_pre));
			D fprintf(stderr, "RESULT (%s)\n", s);
			D fprintf(stderr, "strcpy(%s)(%s)\n",ptrr, s);
			strcpy(ptrr, s);
			free(s);
			ptr2[0]='}';
			ptr[-2]='{';

			D fprintf(stderr, "strcat(%s)(%s)\n",ptrr, p);
			strcat(ptrr, p);
			buf = ptr-2;
			D fprintf(stderr, "CONTINUE (%s)\n", buf);
			ptrr=NULL;
			goto retry;
		}
	}
inside_command = 0;
		if (lbuf && lbuf[0]) {
			D printf("==> 1 (%s)\n", lbuf);
			if (ptr) {
				lbuf_strcat(lbuf, buf);
				spp_fputs(out, lbuf);
				spp_run(ptr, out);
			} else {
				lbuf_strcat(lbuf, buf);
				D printf("=(1)=> spp_run(%s)\n", lbuf);
				spp_run(lbuf+delta, out);
				//spp_run(lbuf+delta+1, out);
			}
			D printf("==>XXX spp_run(%s)\n", lbuf+delta+1);
			lbuf[0]='\0';
			lbuf_n = 0;
		} else {
			D printf("==> 2 (%s)\n", ptr);
			if (ptr) {
				D printf(" ==> 2.1: run(%s)\n", ptr);
				spp_run(ptr, out);
			} else spp_fputs(out, "\n");
		}
		spp_fputs(out, ptr2+delta);
	} else {
		D printf("==> 3\n");
		if (ptr) {
			lbuf_strcat(lbuf, ptr);
		} else {
			if (lbuf == NULL) {
				// XXX should never happen
				fprintf(stderr, "syntax error?\n");
				exit(1);
			}
			if (buf[0]) {
				if (inside_command) {
					lbuf_strcat(lbuf, buf);
				} else spp_fputs(out, buf);
			} else spp_fputs(out, buf);
		}
	}
}

/* TODO: detect nesting */
void spp_io(FILE *in, FILE *out)
{
	char buf[1024];

	if (lbuf==NULL)
		lbuf = malloc(1024);
	lbuf[0]='\0';

	while(!feof(in)) {
		buf[0]='\0'; // ???
		fgets(buf, 1023, in);
		if (feof(in)) break;
		spp_eval(buf, out);
	}
	spp_fputs(out, lbuf);
}

int spp_file(const char *file, FILE *out)
{
	FILE *in = fopen(file, "r");
	D printf("SPP-FILE(%s)\n", file);
	if (in) {
		spp_io (in, stdout);
		fclose(in);
		return 1;
	}
	return 0;
}

void spp_help(char *argv0)
{
	int i;
	printf("Usage: %s [-othesv] [file] [...]\n", argv0);
	printf(	"  -o [file]     set output file (stdout)\n"
		"  -t [type]     define processor type (cpp)\n"
		"  -e [str]      evaluate this string with the selected proc\n"
		"  -s [str]      show this string before anything\n"
		"  -l            list all built-in preprocessors\n"
		"  -L            list keywords registered by the processor\n"
		"  -n            do not read from stdin\n"
		"  -v            show version information\n");
	if (proc) {
		printf("%s specific flags:\n", proc->name);
		for(i=0;args[i].flag;i++)
			printf(" %s   %s\n", args[i].flag, args[i].desc);
	}
	exit(0);
}

void spp_proc_list_kw()
{
	int i;
	for(i=0;tags[i].name;i++)
		printf("%s\n", tags[i].name);
}

void spp_proc_list()
{
	int i;
	for(i=0;procs[i];i++)
		printf("%s\n", procs[i]->name);
}

void spp_proc_set(struct Proc *p, char *arg, int fail)
{
	int j;
	//proc = NULL;
	if (arg)
	for(j=0;procs[j];j++) {
		if (!strcmp(procs[j]->name, arg)) {
			proc = procs[j];
			D printf("SET PROC:(%s)(%s)\n", arg, proc->name);
			break;
		}
	}
	if (arg&&*arg&&!procs[j]&&fail) {
		fprintf(stderr, "Invalid preprocessor name '%s'\n", arg);
		exit(1);
	}
	if (proc == NULL)
		proc = p;

	if (proc != NULL) {
		// TODO: wtf!
		tag_pre = proc->tag_pre;
		tag_post = proc->tag_post;
		echo = proc->default_echo;
		token = proc->token;
		tag_begin = proc->tag_begin;
		args = (struct Arg*)proc->args;
		tags = (struct Tag*)proc->tags;
	}
}

int main(int argc, char **argv)
{
	int dostdin = 1;
	int i, j;
	FILE *out = stdout;
	char *arg;

	spp_proc_set(proc, argv[0], 0);

	if (argc<2)
		spp_io (stdin, stdout);
	else {
		for(i=1;i<argc;i++) {
			/* check preprocessor args */
			if (args)
			for(j=0;args[j].flag;j++) {
				if (!memcmp(argv[i], args[j].flag, 2)) {
					if (args[j].has_arg) {
						GET_ARG(arg, argv, i);
						args[j].callback(arg);
					} else args[j].callback(NULL);
					continue;
				}
			}

			/* TODO: Add these flags in Arg[] */
			if (!memcmp(argv[i], "-o", 2)) {
				GET_ARG(arg, argv, i);
				if (arg!=NULL)
					out = fopen(arg, "w");
				if (arg == NULL || out == NULL) {
					fprintf(stderr, "Cannot open output file\n");
					exit(1);
				}
			} else
			if (!memcmp(argv[i],"-t", 2)) {
				GET_ARG(arg, argv, i);
				spp_proc_set(proc, arg, 1);
			} else
			if (!strcmp(argv[i],"-v")) {
				printf("spp-%s\n", VERSION);
				exit(1);
			} else
			if (!strcmp(argv[i],"-h")) {
				/* show help */
				spp_help(argv[0]);
			} else
			if (!strcmp(argv[i],"-n")) {
				dostdin = 0;
			} else
			if (!strcmp(argv[i],"-l")) {
				spp_proc_list();
				exit(0);
			} else
			if (!strcmp(argv[i],"-L")) {
				spp_proc_list_kw();
				exit(0);
			} else
			if (!strcmp(argv[i],"-s")) {
				GET_ARG(arg, argv, i);
				if (arg==NULL) arg = "";
				fprintf(out, "%s\n", arg);
			} else
			if (!strcmp(argv[i],"-e")) {
				GET_ARG(arg, argv, i);
				if (arg==NULL) arg = "";
				spp_eval(arg, out);
			} else {
				if (i == argc)
					fprintf(stderr, "No file specified.\n");
				else {
					spp_file(argv[i], stdout);
					dostdin = 0;
				}
			}
		}
		if (dostdin) spp_io(stdin, out);
	}

	if (proc->eof)
		proc->eof("", out);

	return 0;
}
