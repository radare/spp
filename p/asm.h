TAG_CALLBACK(asm_default)
{
        return 0;
}

TAG_CALLBACK(asm_include)
{
	printf("include asm\n");
	if (echo[ifl]) spp_file(buf, out);
	return 0;
}


PUT_CALLBACK(asm_fputs)
{
	do_printf (out, "%s", buf);
	return 0;
}

struct Tag asm_tags[] = {
	{ "inc", asm_include },
	{ NULL, asm_default },
	{ NULL }
};

struct Arg asm_args[] = {
	{ NULL }
};

struct Proc asm_proc = {
	.name = "asm",
	.tags = (struct Tag **)asm_tags,
	.args = (struct Arg **)asm_args,
	.token = " ",
	.eof = NULL,
	.tag_pre = ".",
	.tag_post = "\n",
	.multiline = "\\\n",
	.default_echo = 1,
	.fputs = asm_fputs,
	.chop = 0,
	.tag_begin = 1,
};