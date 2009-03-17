#include "p/sh.h"
#include "p/pod.h"
#include "p/cpp.h"

struct Proc *procs[] = {
	&cpp_proc,
	&pod_proc,
	&sh_proc,
	NULL
};

DEFAULT_PROC(cpp)

#define DEBUG 0
