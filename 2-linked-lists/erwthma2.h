#ifndef ERWTHMA2_H
#define ERWTHMA2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Otan ena vivlio exei polloys suggrafeis h ena suggrafeas polla vivlia, xreiazomaste listes.
// H parakatw domh xrisimopoieitai gia ta stoixeia tou vivliou.
typedef struct book {
    char *title;
    int  release_date;
    float price;
} book;



// H parakatw domh xrisimopoieitai gia ta stoixeia tou suggrafea.
typedef struct author {
    int   writer_id;
    char *surname;
    char *name;
    int   num_of_books;   // posa vivlia exei / sumfwna me writes (tha enimerwnetai)
} author;

// H parakatw domh apotelei th syndesh metaksy vivliou kai suggrafea (pollos-pros-pollous).
// Einai h "auxiliary" lista.
typedef struct writes {
    char *title;      // titlos vivliou (key)
    int   writer_id;  // id suggrrafea (key)
} writes;

/* LIST NODE STRUCTS */

// Komvos gia ti lista Authors
typedef struct author_node {
    author data;
    struct author_node *next;
} author_node;

// Komvos gia ti lista Books
typedef struct book_node {
    book data;
    struct book_node *next;
} book_node;



// Komvos gia ti lista Writes (syndeseis)
typedef struct writes_node {
    writes data;
    struct writes_node *next;
} writes_node;


/* PROTOTYPES */

void print_menu(void);
char *copy_string(const char *s); // Utility gia desmeusi mnimis kai antigrafi string

/* load/save */
void load_authors_from_file(void);
void save_authors_to_file(void);
void load_books_from_file(void);
void save_books_to_file(void);
void load_writes_from_file(void);
void save_writes_to_file(void);

/* basic operations */
void insert_new_writer(void);
void insert_new_book(void);
void search_writer(void);
void search_book(void);
void delete_writer(void);
void delete_book(void);

/* helpers authors */
author_node *find_author_by_id(int id); // Anazhthsh suggrafea me id
void update_author_book_counts(void);   // Enimerwsi tou num_of_books gia olous tous authors

/* helpers books */
book_node *find_book_by_title(const char *title); // Anazhthsh vivliou me titlo

/* helpers writes */
int  count_writes_for_title(const char *title);                            // Metraei posoi suggrafeis exoun grapsei to sygkekrimeno vivlio
void delete_writes_for_title_and_writer(const char *title, int writer_id); // Diagrafei mia sygkekrimeni eggrafi syndesis (writes)
void delete_all_writes_for_title(const char *title);                       // Diagrafei OLES tis eggrafes syndesis gia enan titlo

/* insertion in sorted lists */
void insert_author_node_sorted(author new_author); // Eisagwgh author taksinomimena me writer_id
void insert_book_node_sorted(book new_book);       // Eisagwgh book taksinomimena me titlo (alphavhtika)
void insert_write_entry(const char *book_title, int writer_id); // Eisagwgh write entry taksinomimena me writer_id kai meta title

/* internal delete book helper */
void delete_book_by_title_internal(const char *title); // Diagrafh vivliou mazi me tis sxetikes eggrafes writes kai enimerwsi num_of_books

void read_line(char *buf, int size);


#endif