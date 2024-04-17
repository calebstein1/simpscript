#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "heap.h"

heap_hdr_t g_heap[GLOBAL_HEAP_SIZE] = {};
heap_hdr_t *heap_ptr = g_heap;
int max_heap_blocks = GLOBAL_HEAP_SIZE / sizeof(heap_hdr_t);

void init_heap() {
    heap_hdr_t hdr = { .sig = HEADER_SIG, .type = NONE, .size = max_heap_blocks };
    *heap_ptr = hdr;
}

void *simp_alloc(int size, ptr_type_e type) {
    int num_blocks = (size / sizeof(heap_hdr_t)) + 1;
    int offset_blocks = 0;
    heap_ptr = g_heap;
    while (heap_ptr->size < num_blocks || heap_ptr->type) {
        if (offset_blocks + num_blocks >= max_heap_blocks) {
            printf("Heap full\n");
            exit(-1);
        }
        offset_blocks += heap_ptr->size;
        heap_ptr += (heap_ptr->size + 1);
    }
    heap_ptr->size = num_blocks;
    heap_ptr->type = type;
    void *alloc_ptr = heap_ptr + 1;
    heap_ptr += (heap_ptr->size + 1);
    if (heap_ptr->sig != HEADER_SIG) {
        heap_hdr_t hdr = { .sig = HEADER_SIG, .type = NONE };
        if (!*(char *)((char *)(heap_ptr + 1) + 1)) {
            hdr.size = max_heap_blocks - ((heap_ptr - g_heap) / sizeof(heap_hdr_t));
            *heap_ptr = hdr;
        }
    }

    return alloc_ptr;
}

void simp_free(char *ptr) {
    if (pe) return;

    heap_ptr = (heap_hdr_t *)ptr - 1;
    if (heap_ptr->sig != HEADER_SIG) return;

    heap_ptr->type = NONE;
    int extra_blocks = 0;
    heap_hdr_t *lookahead = heap_ptr;
    do {
        lookahead += (heap_ptr->size + 1);
        if (!lookahead->type) {
            extra_blocks += (lookahead->size + 1);
        }
    } while (!lookahead->type);

    heap_ptr->size += extra_blocks;
}