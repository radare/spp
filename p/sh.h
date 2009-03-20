/* SH */

#if 0
static char *eof = NULL;
static char *input = NULL;
#endif

TAG_CALLBACK(sh_default)
{
	if (out != stdout) {
		// pipe stdout to out fd
	}
#if 0
	ptr = strstr(buf, "<<");
	if (ptr) {
		*ptr='\0';
		for(ptr = ptr+2;*ptr==' ';ptr=ptr+1);
		free(eof);
		eof = strdup(ptr);
		return;
	}

	// printf("system(%s)\n", buf);
	if (eof)	
#endif
	system(buf);
}

struct Tag sh_tags[] = {
	{ NULL, sh_default },
	{ NULL }
};

struct Arg sh_args[] = {
	{ NULL }
};

struct Proc sh_proc = {
	.name = "sh",
	.tags = (struct Tag **)sh_tags,
	.args = (struct Arg **)sh_args,
	.eof = NULL,
	.token = NULL,
	.tag_pre = "{{",
	.tag_post = "}}",
	.multiline = "\\",
	.default_echo = 1,
	.chop = 0,
	.tag_begin = 0,
};
