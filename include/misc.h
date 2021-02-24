#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

#define DEBUG

#ifdef DEBUG
#define assert(a) if(!(a)) { \
    while(1) {} \
}

#else
#define assert(a) ((void)0)
#endif

#define NULL ((void*)0)

#endif
