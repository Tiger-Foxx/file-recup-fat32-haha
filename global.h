#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <openssl/sha.h>
#include <inttypes.h>
#include <math.h>

#define SHA_DIGEST_LENGTH 20

#include "fsinfo.h"
#include "recover.h"

#endif