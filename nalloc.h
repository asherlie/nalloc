#include "shared.h"

struct nmem{
    int count, entry_sz, mem_id;
    char ip[20];
};

struct nmem nalloc(int sz, int entries, char* ip);
_Bool nemlcpy(void* src, struct nmem dest, int offset, int nbytes);
_Bool lnemcpy(struct nmem src, void* dest, int offset, int nbytes);
