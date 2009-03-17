#include "spp.h"

char *lbuf = NULL;
int lbuf_s = 1024;
int lbuf_n = 0;
int delta = 0;
int predelta = 0;

void spp_run(char *buf, FILE *out)
{
	int i;
	char *tok = buf;

	if (token) {
		tok = strstr(buf, token);
		if (tok) {
			*tok = '\0';
			tok = tok + 1;
		} else tok = buf;
	}

	fflush(out);
	for(i=0;tags[i].callback;i++) {
		D printf("NAME=(%s)\n", tok);
		if ((tags[i].name==NULL)||(!strcmp(buf, tags[i].name))) {
			tags[i].callback(tok, out);
			break;
		}
	}
}

void spp_eval(char *buf, FILE *out)
{
	char *ptr, *ptr2;

//D printf("-----------(%s)\n", buf);
	/* (pre) tag */
	ptr = strstr(buf, tag_pre);
	if (ptr) {
		D printf("==> 0.0 (%s)\n", ptr);
		if (!tag_begin || (tag_begin && ptr == buf)) {
			*ptr = '\0';
			ptr = ptr + predelta; //strlen(tag_pre);
			D printf("==> 0 (%s) +%d\n", ptr, predelta);
		}
	}

	/* (post) tag */
	if (ptr) ptr2 = strstr(ptr, tag_post);
	else ptr2 = strstr(buf, tag_post);
	if (ptr2) {
		*ptr2 = '\0';
		if (lbuf && lbuf[0]) {
			D printf("==> 1 (%s)\n", lbuf);
			if (ptr) strcat(lbuf, ptr);
			else strcat(lbuf, buf);
			spp_run(lbuf+delta+1, out);
			lbuf[0]='\0';
			lbuf_n = 0;
		} else {
			D printf("==> 2 (%s)\n", ptr);
			E {
				fprintf(out, "%s", buf);
				if (!ptr) fprintf(out, "\n");
			}
			if (ptr) {
				D printf(" ==> 2.1: run(%s)\n", ptr);
				spp_run(ptr, out);
			}
		}
		E fprintf(out, ptr2+delta);
	} else {
		D printf("==> 3\n");
		if (ptr) strcat(lbuf, ptr);
		else {
			if (lbuf == NULL) {
				printf("syntax error?\n");
				exit(1);
			}
			strcat(lbuf, buf);
		}
		//E fprintf(out, buf);
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
	E fprintf(out, lbuf);
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
		"  -v            show version information\n");
	if (proc) {
		printf("Processor specific flags:\n");
		printf("name: %s\n", proc->name);
		for(i=0;args[i].flag;i++) {
			printf(" %s   %s\n", args[i].flag, args[i].desc);
		}
	}
	exit(0);
}

void spp_proc_list()
{
	int i;
	proc = NULL;
	for(i=0;procs[i];i++)
		printf("%s\n", procs[i]->name);
}

void spp_proc_set(char *arg, int fail)
{
	int j;
	proc = NULL;
	for(j=0;procs[j];j++) {
		if (!strcmp(procs[j]->name, arg)) {
			proc = procs[j];
			// TODO: wtf!
			tag_pre = proc->tag_pre;
			tag_post = proc->tag_post;
			token = proc->token;
			tag_begin = proc->tag_begin;
			args = (struct Arg*)proc->args;
			tags = (struct Tag*)proc->tags;
			predelta = strlen(tag_pre);
			delta = strlen(tag_post);
			proc = proc;
			return;
		}
	}
	if (fail) {
		fprintf(stderr, "Invalid preprocessor name '%s'\n", arg);
		exit(1);
	}
}

int main(int argc, char **argv)
{
	int i, j;
	FILE *out = stdout;
	char *arg;

	spp_proc_set(argv[0], 0);

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
				spp_proc_set(arg, 1);
			} else
			if (!strcmp(argv[i],"-v")) {
				printf("spp-%s\n", VERSION);
				exit(1);
			} else
			if (!strcmp(argv[i],"-h")) {
				/* show help */
				spp_help(argv[0]);
			} else
			if (!strcmp(argv[i],"-l")) {
				spp_proc_list();
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
			} else spp_file(argv[i], stdout);
		}
	}
	return 0;
}
