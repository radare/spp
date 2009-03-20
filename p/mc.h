/* Mini MCMS */

TAG_CALLBACK(mc_set)
{
	char *eq = strchr(buf, ' ');
	if (eq) {
		*eq = '\0';
		setenv(buf, eq+1, 1);
	} else setenv(buf, "", 1);
}

TAG_CALLBACK(mc_get)
{
	char *var = getenv(buf);
	if (var) fprintf(out, "%s", var);
}

TAG_CALLBACK(mc_add)
{
	char *eq = strchr(buf, ' ');
	char *var;
	int ret = 0;
	if (eq) {
		*eq = '\0';
		var = getenv(buf);
		if (var == NULL) ret = 0;
		else ret = atoi(var);
		ret += atoi(eq+1);
		setenv(buf, eq+1, 1);
	} else { /* syntax error */ }
}

TAG_CALLBACK(mc_sub)
{
	char *eq = strchr(buf, ' ');
	char *var;
	int ret = 0;
	if (eq) {
		*eq = '\0';
		var = getenv(buf);
		if (var == NULL) ret = 0;
		else ret = atoi(var);
		ret -= atoi(eq+1);
		setenv(buf, eq+1, 1);
	} else { /* syntax error */ }
}

TAG_CALLBACK(mc_echo)
{
	char b[1024];
	snprintf(b, 1023, "echo \"%s\"", buf);
//printf("\n==> (%s)\n\n", b);
//fflush(stdout);
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
	if (echo) {
		fprintf(stderr, "INCLUDE(%s)\n", buf);
		spp_file(buf, out);
	} else {
		printf("INCLUDE IGNORED\n");
	}
}

TAG_CALLBACK(mc_if)
{
	char *var = getenv(buf);
	if (var && *var!='0' && *var != '\0')
		echo = 1;
	else echo = 0;
}

/* {{ ifeq $path / }} */
TAG_CALLBACK(mc_ifeq)
{
	char *eq = strchr(buf, ' ');
	if (eq) {
		char *value = buf;
		*eq = '\0';
		value = getenv(value);
		if (value && !strcmp(value, eq+1)) {
			echo = 1;
		} else echo = 0;
	} else echo = 1;
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
	{ "get", mc_get },
	{ "set", mc_set },
	{ "add", mc_add },
	{ "sub", mc_sub },
	{ "echo", mc_echo },
	{ "ifin", mc_ifin },
	{ "ifnot", mc_ifnot },
	{ "ifeq", mc_ifeq },
	{ "if", mc_if },
	{ "else", mc_else },
	{ "endif", mc_endif },
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
	.eof = NULL,
	.tag_pre = "{{",
	.tag_post = "}}",
	.chop = 1,
	.multiline = NULL,
	.default_echo = 1,
	.tag_begin = 0,
};
