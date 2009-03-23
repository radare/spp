/* Mini MCMS :: renamed to 'spp'? */

/* Should be dynamic buffer */
static char *cmd_to_str(const char *cmd)
{
	char *out = (char *)malloc(4096);
	int ret = 0, len = 0, outlen = 4096;
	FILE *fd = popen(cmd, "r");
	if (fd == NULL)
		return 0;
	do {
		len += ret;
		ret = fread(out+len, 1, 1023, fd);
		if (ret+1024>outlen) {
			outlen += 4096;
			out = realloc (out, outlen);
		}
	} while (ret>0);
	pclose(fd);
	out[len]='\0';
	return out;
}

TAG_CALLBACK(mc_set)
{
	char *eq, *val = "";
	if (!echo) return 0;
	eq = strchr(buf, ' ');
	if (eq) {
		*eq = '\0';
		val = eq + 1;
	}
	setenv(buf, val, 1);
	return 0;
}

TAG_CALLBACK(mc_get)
{
	char *var = getenv(buf);
	if (!echo) return 0;
	if (var) fprintf(out, "%s", var);
	return 0;
}

TAG_CALLBACK(mc_add)
{
	char res[32];
	char *var, *eq = strchr(buf, ' ');
	int ret = 0;
	if (!echo) return 0;
	if (eq) {
		*eq = '\0';
		var = getenv(buf);
		if (var != NULL)
			ret = atoi(var);
		ret += atoi(eq+1);
		sprintf(res, "%d", ret);
		setenv(buf, res, 1);
	} else { /* syntax error */ }
	return 0;
}

TAG_CALLBACK(mc_sub)
{
	char *eq = strchr(buf, ' ');
	char *var;
	int ret = 0;
	if (!echo) return 0;
	if (eq) {
		*eq = '\0';
		var = getenv(buf);
		if (var == NULL) ret = 0;
		else ret = atoi(var);
		ret -= atoi(eq+1);
		setenv(buf, eq+1, 1);
	} else { /* syntax error */ }
	return 0;
}

// XXX This method needs some love
TAG_CALLBACK(mc_trace)
{
	char b[1024];
	if (!echo) return 0;
	snprintf(b, 1023, "echo '%s' > /dev/stderr", buf);
	system(b);
	return 0;
}

/* TODO: deprecate */
TAG_CALLBACK(mc_echo)
{
	if (!echo) return 0;
	fprintf(out, "%s", buf);
	// TODO: add variable replacement here?? not necessary, done by {{get}}
	return 0;
}

TAG_CALLBACK(mc_system)
{
	char *str;
	if (!echo) return 0;
	str = cmd_to_str(buf);
	fprintf(out, "%s", str);
	free(str);
	return 0;
}

TAG_CALLBACK(mc_include)
{
	if (!echo) return 0;
	spp_file(buf, out);
	return 0;
}

TAG_CALLBACK(mc_if)
{
	char *var = getenv(buf);
	if (var && *var!='0' && *var != '\0')
		echo = 1;
	else echo = 0;
	return 1;
}

/* {{ ifeq $path / }} */
TAG_CALLBACK(mc_ifeq)
{
	char *value = buf;
	char *eq = strchr(buf, ' ');
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
	}
	return 1;
}

TAG_CALLBACK(mc_else)
{
	echo = echo?0:1;
	return 0;
}

TAG_CALLBACK(mc_ifnot)
{
	mc_if(buf, out);
	mc_else(buf, out);
	return 1;
}

TAG_CALLBACK(mc_ifin)
{
	char *var, *ptr;
	if (!echo) return 1;
	ptr = strchr(buf, ' ');
	echo = 0;
	if (ptr) {
		*ptr='\0';
		var = getenv(buf);
		if (strstr(ptr+1, var)) {
			echo = 1;
		}
	}
	return 1;
}

TAG_CALLBACK(mc_endif)
{
	return -1;
}

TAG_CALLBACK(mc_default)
{
	if (!echo) return 0;
	fprintf(out, "\n** invalid command(%s)", buf);
	return 0;
}


static int mc_pipe_enabled = 0;
static char *mc_pipe_cmd = NULL;

TAG_CALLBACK(mc_pipe)
{
	mc_pipe_enabled = 1;
	free (mc_pipe_cmd);
	mc_pipe_cmd = strdup (buf);
	return 0;
}

TAG_CALLBACK(mc_endpipe)
{
	mc_pipe_enabled = 0;
	free (mc_pipe_cmd);
	mc_pipe_cmd = NULL;
	return 0;
}

PUT_CALLBACK(mc_fputs)
{
	if (mc_pipe_enabled) {
		char str[1024]; // XXX
		sprintf(str, "echo '%s' | %s", buf, mc_pipe_cmd); // XXX
		system(str);
	} else fprintf(out, "%s", buf);
	return 0;
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

ARG_CALLBACK(mc_arg_d)
{
	// TODO: Handle error
	char *eq = strchr(arg, '=');
	if (eq) {
		*eq = '\0';
		setenv(arg, eq+1, 1);
	} else setenv(arg, "", 1);
	return 0;
}

struct Arg mc_args[] = {
	{ "-D", "define value of variable", 1, mc_arg_d },
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
