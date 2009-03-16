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

struct Tag tags[] = {
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

