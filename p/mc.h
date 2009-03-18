/* Mini MCMS */

TAG_CALLBACK(mc_echo)
{
	char b[1024];
	snprintf(b, 1023, "echo \"%s\"", buf);
printf("\n==> (%s)\n\n", b);
fflush(stdout);
	system(b);
}

TAG_CALLBACK(mc_system)
{
	if (out != stdout) {
		// pipe stdout to out fd
	}
	// printf("system(%s)\n", buf);
	system(buf);
}

TAG_CALLBACK(mc_include)
{
	spp_file(buf, out);
}

TAG_CALLBACK(mc_if)
{
	char *var = getenv(buf);
	if (var && *var=='1')
		echo = 1;
	else echo = 0;
}

TAG_CALLBACK(mc_else)
{
	echo = echo?0:1;
}

TAG_CALLBACK(mc_ifnot)
{
	mc_if(buf, out);
	mc_else(buf, out);
}

TAG_CALLBACK(mc_ifin)
{
	char *var, *ptr = strchr(buf, ' ');
	echo = 0;
	if (ptr) {
		*ptr='\0';
		var = getenv(buf);
		if (strstr(ptr+1, var)) {
			echo = 1;
		}
	}
}

TAG_CALLBACK(mc_endif)
{
	echo = 1;
}

TAG_CALLBACK(mc_default)
{
	fprintf(out, "\n** invalid command(%s)", buf);
}

struct Tag mc_tags[] = {
	{ "echo", mc_echo },
	{ "if", mc_if },
	{ "else", mc_else },
	{ "endif", mc_endif },
	{ "ifin", mc_ifin },
	{ "ifnot", mc_ifnot },
	{ "include", mc_include },
	{ "system", mc_system },
	{ NULL, mc_default },
	{ NULL }
};

struct Arg mc_args[] = {
	{ NULL }
};

struct Proc mc_proc = {
	.name = "mc",
	.tags = (struct Tag **)mc_tags,
	.args = (struct Arg **)mc_args,
	.token = " ",
	.tag_pre = "{{",
	.tag_post = "}}",
	.default_echo = 1,
	.tag_begin = 0,
};
