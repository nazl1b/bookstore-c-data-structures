#ifndef ERWTHMA3_B_H
#define ERWTHMA3_B_H

#include <stdio.h>
#include <stdlib.h>





/* APPLICATION-SPECIFIC STRUCTS (Data Containers) */

// Domi gia ta vivlia.
typedef struct book {
    char *title;
    int  release_date;
    float price;
} book;

// Domi gia tous suggrafeis.
typedef struct author {
    int   writer_id;
    char *surname;
    char *name;
    int   num_of_books; // Metritis (enimerwnetai apo ti lista writes)
} author;

// Domi gia ti syndesh (writes) metaksy book kai author.
typedef struct writes {
    char *title;      // Titlos vivliou
    int   writer_id;  // ID Suggrafear
} writes;

/* GENERIC LIST STRUCTS & FUNCTION POINTERS */

// Typos synartisis gia sygkrisi dedomenwn (xrisimopoieitai gia anazhthsh kai taksinomisi).
typedef int (*CompareFunc)(const void *data1, const void *data2); 

// O genikos komvos (Node). Periexei enan deikti typou void* gia opoiodipote eidos dedomenwn.
typedef struct Node {
    void *data;                 // Deiktis sta pragmatika dedomena (author*, book*, writes*)
    struct Node *next;
} Node;

// H geniki lista (List). Periexei tin kefalida kai deiktes se synartiseis.
typedef struct List {
    Node *head;
    int count;
    void (*free_data)(void *data);      // Synartisi gia apeleftherwsi ton strings mesa sto data
    int (*compare_data)(const void *data1, const void *data2); // Synartisi taksinomisis
} List;

/* GLOBAL LISTS & COUNTERS */

// Oi 3 genikes listes (apokryvoun ta dedomena tous mesa sto void* data).

extern List author_list;
extern List book_list;
extern List writes_list;

extern int next_writer_id;

/* PROTOTYPES (ALL FUNCTIONS MUST BE DECLARED) */

// Utility Functions
void print_menu(void);
char *copy_string(const char *s);
void clear_input_buffer(void);

// Generic List Operations (Basikes Leitourgies Genikis Listas)
void generic_insert_sorted(List *list, void *new_data);
void *generic_search(const List *list, const void *search_key, CompareFunc compare_key);
void generic_delete_node(List *list, const void *search_key, CompareFunc compare_key);
void generic_free_list(List *list);
void generic_delete_node_by_pointer(List *list, Node *target_node, Node *prev_node); // Helper gia grigorh diagrafh me deiktes

// Type-Specific Compare Functions (Synartiseis Sygkrisis)
int compare_authors_for_sort(const void *a, const void *b);       // Sygkrisi gia taxinomisi Author me ID
int compare_books_for_sort(const void *a, const void *b);         // Sygkrisi gia taxinomisi Book me title
int compare_writes_for_sort(const void *a, const void *b);        // Sygkrisi gia taxinomisi Writes me ID/Title
int compare_author_by_id_key(const void *data, const void *key);      // Sygkrisi Author me ID gia anazhthsh
int compare_author_by_surname_key(const void *data, const void *key); // Sygkrisi Author me Surname gia anazhthsh
int compare_book_by_title_key(const void *data, const void *key);     // Sygkrisi Book me Title gia anazhthsh

// Type-Specific Free Functions (Synartiseis Apeleftherwsis Mnimis gia strings mesa stis domes)
void free_author_data(void *data);
void free_book_data(void *data);
void free_writes_data(void *data);

// Initialization and Data Persistence
void initialize_lists(void); // Arxikopoiisi twn List me tous deiktes synartisewn
void load_authors_from_file(void);
void load_books_from_file(void);
void load_writes_from_file(void);
void save_authors_to_file(void);
void save_books_to_file(void);
void save_writes_to_file(void);
void save_data(void); // Synoliki apothikefsi

// Application Logic (CRUD and Helpers)
void update_author_book_counts(void);
void insert_new_writer(void);
void insert_new_book(void);
void search_writer(void);
void search_book(void);
void delete_writer(void);
void delete_book(void);
int count_writes_for_title(const char *title);           // Helper gia elegxo monografias
void delete_write_entry(const char *title, int writer_id); // Helper gia diagrafh sigkekrimenis writes
void read_line(char *buf, int size);
static int read_line_fp(FILE *fp, char *buf, int size);


#endif
