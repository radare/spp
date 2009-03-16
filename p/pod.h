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

#define TAG_PRE "="
#define TAG_POST "\n"
#define TOKEN " "
#define TAG_BEGIN 1
#define ECHO 0

struct Tag tags[] = {
	{ "head1", pod_head1 },
	{ "cut", pod_cut },
	{ NULL, pod_default },
	NULL
};

