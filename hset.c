#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include "hset.h"

struct chain_node {
  hset_elem data;
  struct chain_node* next;
};
typedef struct chain_node chain;

struct hset_header {
  size_t size;
  size_t capacity;               /* 0 < capacity */
  chain **table;             /* \length(table) == capacity */
  elem_equal_fn *elem_equal;
  elem_hash_fn *elem_hash;
  elem_free_fn *elem_free;
};

bool literal_equal_fn(hset_elem x, hset_elem y) {
  assert(x != NULL);
  assert(y != NULL);
  return *(char*)x == *(char*)y;
}

size_t literal_hash_fn(hset_elem x) {
  assert(x != NULL);
  return *(char*)x;
}


static inline size_t hashindex(hset H, hset_elem x) {
  return (*H->elem_hash)(x) % H->capacity;
}

static inline bool elemequal(hset H, hset_elem x, hset_elem y) {
  return (*H->elem_equal)(x, y);
}

static bool is_chain(hset H, chain* p, size_t i) {
  for (chain* q = p; q != NULL; q = q->next) {
    if (q->data == NULL) return false;
    if (hashindex(H, p->data) != i) return false;
  }
  return true;
}

static bool is_chains(hset H, size_t lo, size_t hi) {
  for (size_t i = lo; i < hi; i++) {
    if (!is_chain(H, H->table[i], i)) return false;
  }
  return true;
}

bool is_hset(hset H) {
    return  H != NULL
    && H->capacity > 0
    && H->elem_equal != NULL
    && H->elem_hash != NULL
    && is_chains(H, 0, H->capacity);
}

hset hset_new(size_t capacity,  
              elem_equal_fn *elem_equal,
              elem_hash_fn *elem_hash, 
              elem_free_fn *elem_free) {
  hset H = malloc(sizeof(struct hset_header));
  H->size = 0;
  H->capacity = capacity;
  H->table = calloc(capacity, sizeof(chain*));
  H->elem_equal = elem_equal;
  H->elem_hash = elem_hash;
  H->elem_free = elem_free;

  return H;
}

hset_elem hset_lookup(hset H, hset_elem x) {
  int i = hashindex(H, x);
  for (chain* p = H->table[i]; p != NULL; p = p->next) {
    if (elemequal(H, p->data, x)) {
      return p->data;
    }
  }

  return NULL;
}

static inline void hset_resize(hset H, size_t new_capacity) {
  chain** old_table = H->table;
  chain** new_table = calloc(new_capacity, sizeof(chain*));
  size_t old_capacity = H->capacity;
  H->capacity = new_capacity; // is_hset now violated!

  for (size_t i = 0; i < old_capacity; i++) {
    chain* p_next;
    for (chain* p = old_table[i]; p != NULL; p = p_next) {
      hset_elem x = p->data;
      p_next = p->next;
      size_t h = hashindex(H, x);
      p->next = new_table[h];
      new_table[h] = p;
      p = p_next;
    }
  }

  H->table = new_table;
  free(old_table);
}

hset_elem hset_insert(hset H, hset_elem x) {
  int i = hashindex(H, x);
  for (chain* p = H->table[i]; p != NULL; p = p->next) {
    if (elemequal(H, p->data, x)) {
      hset_elem old = p->data;
      p->data = x;
      return old;
    }
  }
  
  // prepend new element
  chain* p = malloc(sizeof(chain));
  p->data = x;
  p->next = H->table[i];
  H->table[i] = p;
  (H->size)++;

  if (H->size > H->capacity && H->capacity < UINT_MAX / 2) {
    hset_resize(H, 2*H->capacity);
  }
  return NULL;
}

void hset_free(hset H) {
  for (size_t i = 0; i < H->capacity; i++) {
    chain* p_next;
    for (chain* p = H->table[i]; p != NULL; p = p_next) {
      if (H->elem_free != NULL) (*H->elem_free)(p->data);
      p_next = p->next;
      free(p);
    }
  }

  free(H->table);
  free(H);
}
