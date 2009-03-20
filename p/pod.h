/* CPP */

TAG_CALLBACK(pod_default)
{
	fprintf(out, "DEFAULT: (%s)\n", buf);
}

TAG_CALLBACK(pod_cut)
{
	fprintf(out, "\n");
	echo = 0;
	//fprintf(out, "DEFAULT: (%s)\n", buf);
}

TAG_CALLBACK(pod_head1)
{
	int i, len = strlen(buf);
	echo = 1;
	fprintf(out,"\n");
	if (buf==NULL)
		return;
	fprintf(out, "%s\n", buf);
	for(i=0;i<len;i++)
		fprintf(out, "%c", '=');
	fprintf(out, "\n");
}

struct Tag pod_tags[] = {
	{ "head1", pod_head1 },
	{ "cut", pod_cut },
	{ NULL, pod_default },
	{ NULL }
};

struct Arg pod_args[] = {
	{ NULL }
};

struct Proc pod_proc = {
	.name = "pod",
	.tags = (struct Tag **)pod_tags,
	.args = (struct Arg **)pod_args,
	.token = " ",
	.eof = NULL,
	.tag_pre = "=",
	.tag_post = "\n",
	.multiline = NULL,
	.default_echo = 0,
	.chop = 0,
	.tag_begin = 1,
};
