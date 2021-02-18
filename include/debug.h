#ifndef MAIN_H
#define MAIN_H

#define DEBUG

#ifdef DEBUG
#define assert(a) if(!(a)) { \
    while(1) {} \
}

#else
#define assert(a) ((void)0)
#endif

#endif
