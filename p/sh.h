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

static int sh_pipe_enabled = 0;
static char *sh_pipe_cmd = NULL;

TAG_CALLBACK(sh_pipe)
{
	sh_pipe_enabled = 1;
	free (sh_pipe_cmd);
	sh_pipe_cmd = strdup (buf);
}

TAG_CALLBACK(sh_endpipe)
{
	sh_pipe_enabled = 0;
	free (sh_pipe_cmd);
	sh_pipe_cmd = NULL;
}

PUT_CALLBACK(sh_fputs)
{
	if (sh_pipe_enabled) {
		char str[1024]; // XXX
		sprintf(str, "echo \"%s\" | %s", buf, sh_pipe_cmd); // XXX
		system(str);
	} else fprintf(out, "%s", buf);
}

struct Tag sh_tags[] = {
	{ "pipe", sh_pipe },
	{ "endpipe", sh_endpipe },
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
	.fputs = NULL,
	.eof = NULL,
	.token = NULL,
	.tag_pre = "{{",
	.tag_post = "}}",
	.fputs = sh_fputs,
	.multiline = "\\",
	.default_echo = 1,
	.chop = 0,
	.tag_begin = 0,
};
