#include <stdlib.h>

struct Arena {
    void* mem_block;
    int capacity;
    int bytes_used;
};

void arena_create(struct Arena* arena, int capacity) {
    arena->mem_block = malloc(capacity);
    arena->capacity = capacity;
    arena->bytes_used = 0;
}

void arena_destroy(const struct Arena* arena) {
    free(arena->mem_block);
}

void* arena_alloc(struct Arena* arena, int size) {
    if (arena->bytes_used + size > arena->capacity) {
        return NULL;
    }

    void* address = (unsigned char*)(arena->mem_block) + arena->bytes_used;

    arena->bytes_used += size;
    return address;
}

void arena_free_all(struct Arena* arena) {
    arena->bytes_used = 0;
}