/* CPP */

TAG_CALLBACK(sh_default)
{
	if (out != stdout) {
		// pipe stdout to out fd
	}
	// printf("system(%s)\n", buf);
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
	.token = NULL,
	.tag_pre = "{{",
	.tag_post = "}}",
	.default_echo = 1,
	.tag_begin = 0,
};
