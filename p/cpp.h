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

TAG_CALLBACK(cpp_endif)
{
	echo = 1;
}

TAG_CALLBACK(cpp_include)
{
	//printf("CPP-INCLUDE(%s)\n", buf);
	spp_file(buf, out);
}

#define TAG_PRE "#"
#define TAG_POST "\n"
#define TOKEN " "
#define TAG_BEGIN 1
#define ECHO 1

struct Tag cpp_tags[] = {
	{ "if", cpp_if },
	{ "endif", cpp_endif },
	{ "include", cpp_include },
#if 0
	{ "define", cpp_define },
#endif
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
	printf("USERDEFINE(%s)\n", arg);
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
	.token = TOKEN,
	.tag_pre = TAG_PRE,
	.tag_post = TAG_POST,
	.default_echo = ECHO,
	.tag_begin = TAG_BEGIN,
};
