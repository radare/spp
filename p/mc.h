/* Mini MCMS */

TAG_CALLBACK(mc_set)
{
	char *eq = strchr(buf, ' ');
	if (!echo) return;
	if (eq) {
		*eq = '\0';
		setenv(buf, eq+1, 1);
	} else setenv(buf, "", 1);
}

TAG_CALLBACK(mc_get)
{
	char *var = getenv(buf);
	if (!echo) return;
	if (var) fprintf(out, "%s", var);
}

TAG_CALLBACK(mc_add)
{
	char res[32];
	char *eq = strchr(buf, ' ');
	char *var;
	int ret = 0;
	if (!echo) return;
	if (eq) {
		*eq = '\0';
		var = getenv(buf);
		if (var != NULL)
			ret = atoi(var);
		ret += atoi(eq+1);
		sprintf(res, "%d", ret);
		setenv(buf, res, 1);
	} else { /* syntax error */ }
}

TAG_CALLBACK(mc_sub)
{
	char *eq = strchr(buf, ' ');
	char *var;
	int ret = 0;
	if (!echo) return;
	if (eq) {
		*eq = '\0';
		var = getenv(buf);
		if (var == NULL) ret = 0;
		else ret = atoi(var);
		ret -= atoi(eq+1);
		setenv(buf, eq+1, 1);
	} else { /* syntax error */ }
}

TAG_CALLBACK(mc_trace)
{
	char b[1024];
	if (!echo) return;
	snprintf(b, 1023, "echo \"%s\" > /dev/stderr", buf);
	system(b);
}

TAG_CALLBACK(mc_echo)
{
	//char b[1024];
	if (!echo) return;
	fprintf(out, "%s", buf);
	//snprintf(b, 1023, "echo \"%s\"", buf);
//printf("\n==> (%s)\n\n", b);
//fflush(stdout);
	//system(b);
}

TAG_CALLBACK(mc_system)
{
	if (!echo) return;
	if (out != stdout) {
		// pipe stdout to out fd
	}
	// printf("system(%s)\n", buf);
	system(buf);
}

TAG_CALLBACK(mc_include)
{
	if (!echo) return;
	spp_file(buf, out);
}

TAG_CALLBACK(mc_if)
{
	char *var = getenv(buf);
	if (!echo) return;
	if (var && *var!='0' && *var != '\0')
		echo = 1;
	else echo = 0;
}

/* {{ ifeq $path / }} */
TAG_CALLBACK(mc_ifeq)
{
	char *value = buf;
	char *eq = strchr(buf, ' ');
	if (!echo) return;
	if (eq) {
		*eq = '\0';
		value = getenv(value);
		if (value && !strcmp(value, eq+1)) {
			echo = 1;
		} else echo = 0;
	} else {
		echo = 1;
		value = getenv(buf);
//fprintf(stderr, "IFEQ CHECK HERE (%s)\n", value);
		if (value==NULL || *value=='\0')
			echo = 1;
		else echo = 0;
//fprintf(stderr, "RESULT=%d\n", echo);
/* SYNTAX ERRORR */
	}
}

TAG_CALLBACK(mc_else)
{
	echo = echo?0:1;
}

TAG_CALLBACK(mc_ifnot)
{
	if (!echo) return;
	mc_if(buf, out);
	mc_else(buf, out);
}

TAG_CALLBACK(mc_ifin)
{
	char *var, *ptr;
	if (!echo) return;
	ptr = strchr(buf, ' ');
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
	if (!echo) return;
	fprintf(out, "\n** invalid command(%s)", buf);
}


static int mc_pipe_enabled = 0;
static char *mc_pipe_cmd = NULL;

TAG_CALLBACK(mc_pipe)
{
	mc_pipe_enabled = 1;
	free (mc_pipe_cmd);
	mc_pipe_cmd = strdup (buf);
}

TAG_CALLBACK(mc_endpipe)
{
	mc_pipe_enabled = 0;
	free (mc_pipe_cmd);
	mc_pipe_cmd = NULL;
}

PUT_CALLBACK(mc_fputs)
{
	if (mc_pipe_enabled) {
		char str[1024]; // XXX
		sprintf(str, "echo \"%s\" | %s", buf, mc_pipe_cmd); // XXX
		system(str);
	} else fprintf(out, "%s", buf);
}

struct Tag mc_tags[] = {
	{ "get", mc_get },
	{ "set", mc_set },
	{ "add", mc_add },
	{ "sub", mc_sub },
	{ "echo", mc_echo },
	{ "trace", mc_trace },
	{ "ifin", mc_ifin },
	{ "ifnot", mc_ifnot },
	{ "ifeq", mc_ifeq },
	{ "if", mc_if },
	{ "else", mc_else },
	{ "endif", mc_endif },
	{ "pipe", mc_pipe },
	{ "endpipe", mc_endpipe },
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
	.fputs = mc_fputs,
	.multiline = NULL,
	.default_echo = 1,
	.tag_begin = 0,
};
