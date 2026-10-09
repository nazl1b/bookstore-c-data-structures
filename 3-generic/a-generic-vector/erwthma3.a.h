#ifndef ERWTHMA3_A_H
#define ERWTHMA3_A_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Domes Dedomenon (Structures) */
typedef struct book {
    char *title;
    int release_date;
    float price;
} book;

typedef struct author {
    int writer_id;
    char *surname;
    char *name;
    int num_of_books;
} author;

typedef struct writes {
    char *title;
    int writer_id;
} writes;

/* Generic Vector Domi */
typedef int  (*g_cmp)(const void *a, const void *b);
typedef int  (*g_cmp_key)(const void *elem, const void *key);
typedef void (*g_free_elem)(void *elem);

typedef struct {
    void  *data;      // Deiktis sta dedomena
    size_t elem_size; // Megethos kathe stoixeiou
    int    size;      // Posa stoixeia iparxoun tora
    int    cap;       // Sinoliki xoritikotita
} gvec;

typedef struct {
    int writer_id;
    const char *title;
} writes_key;

/* Katholikes Metavlites (Global Storage) */
/* Xrisimopoioume extern gia na kserei o linker oti orizontai sto .c arxeio */
extern gvec books;
extern gvec authors;
extern gvec writesV;

/* Diloseis Sinartiseon (Function Prototypes) */

/* Omada 1: Sinartiseis Generic Vector */
void gvec_init(gvec *v, size_t elem_size);
void gvec_free(gvec *v, g_free_elem free_elem);
int  gvec_push_back(gvec *v, const void *elem);
int  gvec_insert_sorted(gvec *v, const void *elem, g_cmp_key cmp_elem_to_elemkey);
void gvec_sort(gvec *v, g_cmp cmp);
void *gvec_at(gvec *v, int idx);

/* Omada 2: Diaxeirisi Arxeion (File I/O) */
void load_authors_from_file(void);
void load_books_from_file(void);
void load_writes_from_file(void);
void save_authors_to_file(void);
void save_books_to_file(void);
void save_writes_to_file(void);

/* Omada 3: Leitourgies Vivliopoleiou (Operations) */
void insert_new_writer(void);
void insert_new_book(void);
void search_writer(void);
void search_book(void);
void delete_writer(void);
void delete_book(void);
void recompute_num_of_books(void);

/* Omada 4: Voithitikes Sinartiseis (Helpers) */
void print_menu(void);
void clear_stdin_line(void);
void free_book_elem(void *e);
void free_author_elem(void *e);
void free_writes_elem(void *e);

#endif