/* CPP */

TAG_CALLBACK(sh_default)
{
	//printf("system(%s)\n", buf);
	system(buf);
}

#define TAG_PRE "{{"
#define TAG_POST "}}"
#define TOKEN NULL
#define TAG_BEGIN 0
#define ECHO 1

struct Tag tags[] = {
	{ NULL, sh_default },
	NULL
};

