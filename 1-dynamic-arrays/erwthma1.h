#ifndef ERWTHMA1_H
#define ERWTHMA1_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//structures
typedef struct book 
{
    char *title;
    int release_date;
    float price;
} book;

typedef struct author 
{
    int writer_id;
    char *surname;
    char *name;
    int num_of_books;
} author;

typedef struct writes 
{
    char *title;
    int writer_id;
} writes;



// 1. VOITHITIKES SYNARTISEIS (UTILITIES)
char *copy_string(const char *s);            // Desmeusi mnimis kai antigrafi string 
void read_line(char *buf, int size);         // Asfalis anagnwsi grammis me kena
void print_menu(void);                       // Emfanisi tou kentrikou menu

// 2. DIAXIRISI SYGGRAFEWN (AUTHORS)
void insert_new_writer(void);                                                   // Eisagwgi neou syggrafea apo ton xristi 
int insert_author_sorted(const char *surname, const char *name);                // Taxinomimeni eisagwgi ston pinaka 
void delete_writer(void);                                                       // Diagrafi syggrafea kai enimerwsi pinakwn 
void search_writer(void);                                                       // Anazitisi syggrafea me to epitheto 
int find_author_index_by_id(int id);                                            // Dyadiki anazitisi thesis vasi ID
author *find_author_by_id_manual(int writer_id);                               // Eyresi deikti syggrafea vasi ID
int find_author_count_by_surname(const char *surname);                         // Metrisi syggrafeawn me idio epitheto 
int find_author_id_by_surname_unique_or_choose(const char *surname);           // Epilogi ID se synwnymies
int compare_authors_by_id(const void *a, const void *b);                      // Synartisi sygkrisis gia qsort 


// 3. DIAXIRISI VIVLIWN (BOOKS)

void insert_new_book(void);                                           // Eisagwgi neou vivliou kai syggrafeawn 
void delete_book(void);                                               // Diagrafi vivliou kai enimerwsi tou writes 
void search_book(void);                                              // Anazitisi vivliou vasi titlou 
int find_book_index_by_title(const char *title);                     // Dyadiki anazitisi thesis vasi titlou 
int compare_books_by_title(const void *a, const void *b);           // Sygkrisi titlwn gia qsort


// 4. DIAXIRISI SYNDESEWN (WRITES - Schesi Vivliou-Syggrafea)
void insert_write_entry(const char *book_title, int writer_id);                      // Syndesi vivliou me syggrafea 
void delete_writes_for_title_and_writer(const char *title, int writer_id);          // Diagrafi mias syndesis 
int count_writes_for_title(const char *title);                                     // Elegxos an einai monografia 
int write_exists(const char *title, int writer_id);                               // Elegxos gia diplotypa 
void recompute_num_of_books(void);                                               // Epanypologismos vivliwn ana syggrafea 


// 5. DIAXIRISI ARCHEIWN (FILES)
void load_authors_from_file(void);           // Fortwsi apo authors.txt 
void save_authors_to_file(void);             // Apothikeusi sto authors.txt 
void load_books_from_file(void);             // Fortwsi apo books.txt 
void save_books_to_file(void);               // Apothikeusi sto books.txt 
void load_writes_from_file(void);            // Fortwsi apo writes.txt
void save_writes_to_file(void);              // Apothikeusi sto writes.txt 

#endif