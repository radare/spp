/* CPP */

TAG_CALLBACK(cpp_default)
{
	fprintf(out, "DEFAULT: (%s)\n", buf);
}

TAG_CALLBACK(cpp_error)
{
	fprintf(out,"\n");
	if (buf==NULL)
		return;
	fprintf(out, "ERROR: %s\n", buf);
	exit(1);
}

TAG_CALLBACK(cpp_if)
{
	/* TODO: check condition here */
	echo = 0;
}

TAG_CALLBACK(cpp_define)
{
	char *eq = strchr(buf, ' ');
	printf("DEFINE(%s)\n", buf);
	if (eq) {
		eq = '\0';
		setenv(buf, eq+1, 1);
	} else setenv(buf, "", 1);
}

TAG_CALLBACK(cpp_endif)
{
	echo = 1;
}

TAG_CALLBACK(cpp_include)
{
	//printf("CPP-INCLUDE(%s)\n", buf);
	spp_file(buf, out);
}

struct Tag cpp_tags[] = {
	{ "if", cpp_if },
	{ "endif", cpp_endif },
	{ "include", cpp_include },
	{ "define", cpp_define },
	{ "error", cpp_error },
	{ NULL, cpp_default },
	{ NULL }
};

/* arguments */

ARG_CALLBACK(cpp_arg_i)
{
	printf("INCLUDEDIR(%s)\n", arg);
}

ARG_CALLBACK(cpp_arg_d)
{
	char *eq = strchr(arg, '=');
	if (eq) {
		eq = '\0';
		setenv(arg, eq+1, 1);
	} else setenv(arg, "", 1);
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
	.tag_pre = "#",
	.tag_post = "\n",
	.default_echo = 1,
	.tag_begin = 1,
};
