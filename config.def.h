#include "p/sh.h"
#include "p/mc.h"
#include "p/acr.h"
#include "p/pod.h"
#include "p/cpp.h"

struct Proc *procs[] = {
	&cpp_proc,
	&pod_proc,
	&acr_proc,
	&sh_proc,
	&mc_proc,
	NULL
};

DEFAULT_PROC(cpp)

#define DEBUG 0
