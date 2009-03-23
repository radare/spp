/* CPP */

TAG_CALLBACK(cpp_default)
{
	fprintf(out, "DEFAULT: (%s)\n", buf);
	return 0;
}

TAG_CALLBACK(cpp_error)
{
	fprintf(out,"\n");
	if (echo && buf != NULL) {
		fprintf(out, "ERROR: %s\n", buf);
		exit(1);
	}
	return 0;
}

TAG_CALLBACK(cpp_warning)
{
	fprintf(out,"\n");
	if (echo && buf != NULL)
		fprintf(out, "WARNING: %s\n", buf);
	return 0;
}

TAG_CALLBACK(cpp_if)
{
	char *var = getenv(buf+((*buf=='!')?1:0));
	if (var && *var=='1')
		echo = 1;
	else echo = 0;
	if (*buf=='!') echo = !!!echo;
	return 1;
}

TAG_CALLBACK(cpp_ifdef)
{
	char *var = getenv(buf);
	if (var) echo = 1;
	else echo = 0;
	return 1;
}

TAG_CALLBACK(cpp_else)
{
	echo = echo?0:1;
	return 0;
}

TAG_CALLBACK(cpp_ifndef)
{
	cpp_ifdef(buf, out);
	cpp_else(buf, out);
	return 1;
}

TAG_CALLBACK(cpp_define)
{
	char *eq = strchr(buf, ' ');
	if (eq) {
		*eq = '\0';
		setenv(buf, eq+1, 1);
	} else setenv(buf, "", 1);
	return 0;
}

TAG_CALLBACK(cpp_endif)
{
	echo = 1;
	return -1;
}

TAG_CALLBACK(cpp_include)
{
	if (echo) spp_file(buf, out);
	return 0;
}

struct Tag cpp_tags[] = {
	{ "ifdef", cpp_ifdef },
	{ "ifndef", cpp_ifndef },
	{ "endif", cpp_endif },
	{ "if", cpp_if },
	{ "include", cpp_include },
	{ "define", cpp_define },
	{ "error", cpp_error },
	{ "warning", cpp_warning },
	{ NULL, cpp_default },
	{ NULL }
};

/* arguments */

ARG_CALLBACK(cpp_arg_i)
{
	printf("INCLUDEDIR(%s)\n", arg);
	return 0;
}

ARG_CALLBACK(cpp_arg_d)
{
	// TODO: handle setenv==-1
	char *eq = strchr(arg, '=');
	if (eq) {
		*eq = '\0';
		setenv(arg, eq+1, 1);
	} else setenv(arg, "", 1);
	return 0;
}

struct Arg cpp_args[] = {
	{ "-I", "add include directory", 1, cpp_arg_i },
	{ "-D", "define value of variable", 1, cpp_arg_d },
	{ NULL }
};

struct Proc cpp_proc = {
	.name = "cpp",
	.tags = (struct Tag **)cpp_tags,
	.args = (struct Arg **)cpp_args,
	.token = " ",
	.eof = NULL,
	.tag_pre = "#",
	.tag_post = "\n",
	.multiline = "\\",
	.default_echo = 1,
	.chop = 0,
	.tag_begin = 1,
};
