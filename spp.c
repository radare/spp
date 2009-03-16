#include "spp.h"

char *lbuf = NULL;
int lbuf_s = 1024;
int lbuf_n = 0;
int delta = 0;

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
		if (!tag_begin || (tag_begin && ptr == buf)) {
			*ptr = '\0';
			ptr = ptr + strlen(tag_pre);
		}
	}

	/* (post) tag */
	if (ptr) ptr2 = strstr(ptr, tag_post);
	else ptr2 = strstr(buf, tag_post);
	if (ptr2) {
		*ptr2 = '\0';
		delta = strlen(tag_post); // TODO: cache
		if (lbuf && lbuf[0]) {
D printf("==> 1 (%s)\n", lbuf);
			if (ptr) strcat(lbuf, ptr);
			else strcat(lbuf, buf);
			spp_run(lbuf, out);
			lbuf[0]='\0';
			lbuf_n = 0;
		} else {
D printf("==> 2 (%s)\n", ptr);
			if (echo) fprintf(out, "%s%c", buf, ptr?' ':'\n');
			if (ptr) spp_run(ptr, out);
		}
		if (echo) fprintf(out, ptr2+delta);
	} else {
D printf("==> 3 \n");
		if (ptr) strcat(lbuf, ptr);
		else strcat(lbuf, buf);
		//if (echo)
		//	fprintf(out, buf);
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
		fgets(buf, 1023, in);
		if (feof(in)) break;
		spp_eval(buf, out);
	}
	if (echo)
		fprintf(out, lbuf);
}

int spp_file(const char *file, FILE *out)
{
	FILE *in = fopen(file, "r");
D printf("FILE(%s)\n", file);
	if (in) {
		spp_io (in, stdout);
		fclose(in);
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	int i;
	FILE *out = stdout;
	char *arg;

	if (argc<2)
		spp_io (stdin, stdout);
	else {
		for(i=1;i<argc;i++) {
			if (!strcmp(argv[i],"-s")) {
				arg = argv[i+1];
				if (arg==NULL) arg = "";
				fprintf(out, "%s\n", arg);
				i++;
			} else
			if (!strcmp(argv[i],"-e")) {
				arg = argv[i+1];
				if (arg==NULL) arg = "";
				spp_eval(arg, out);
				i++;
			} else spp_file(argv[i], stdout);
		}
	}
	return 0;
}
