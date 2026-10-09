#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erwthma3.b.h"


int next_writer_id = 1;

List author_list;
List book_list;
List writes_list;

/* I. UTILITIES IMPLEMENTATION */

void print_menu(void)
{
    printf("\n---- BOOKSTORE MENU ----\n");
    printf("1. Insert new writer\n");
    printf("2. Insert new book\n");
    printf("3. Search a writer\n");
    printf("4. Search a book\n");
    printf("5. Delete a writer\n");
    printf("6. Delete a book\n");
    printf("7. Exit\n");
}

// Dimiourgei ena neo string sti mnhmh (me malloc) kai antigrafei to s.
// Epistrefei ton deikti sto neo string h NULL an apotuxei i desmefsi.
char *copy_string(const char *s)
{
    char *p = (char *)malloc(strlen(s) + 1);
    if (p == NULL) return NULL;
    strcpy(p, s);
    return p;
}

// Katharizei to input buffer meta apo lathos scanf, oste na apotrepontai provlimata stin epomeni anagnosi eisodou.
void clear_input_buffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

// Arxikopoiisi Listes me tous deiktes synartisewn
void initialize_lists(void)
{
    author_list.head = NULL;
    author_list.count = 0;
    author_list.free_data = free_author_data;         // Synartisi gia apeleftherwsi author strings
    author_list.compare_data = compare_authors_for_sort; // Synartisi gia taxinomisi (writer_id)

    book_list.head = NULL;
    book_list.count = 0;
    book_list.free_data = free_book_data;           // Synartisi gia apeleftherwsi book strings
    book_list.compare_data = compare_books_for_sort;  // Synartisi gia taxinomisi (title)

    writes_list.head = NULL;
    writes_list.count = 0;
    writes_list.free_data = free_writes_data;         // Synartisi gia apeleftherwsi writes strings
    writes_list.compare_data = compare_writes_for_sort; // Synartisi gia taxinomisi (writer_id, title)
}

/* II. GENERIC LIST IMPLEMENTATION */

// Eisagwgi se Geniki Taksinomimeni Lista
void generic_insert_sorted(List *list, void *new_data)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) { printf("Memory error in generic_insert_sorted.\n"); free(new_data); return; }
    new_node->data = new_data;
    new_node->next = NULL;

    // Eidikos xeirismos gia tin Writes list (elegxos diplotypias)
    if (list == &writes_list) {
        // H anazhthsh ginetai me to compare_data (taxinomisis), to opoio xreiazetai na einai 0 gia diplotypo.
        if (generic_search(list, new_data, list->compare_data) != NULL) {
            // An einai diplotypo, apeleftherwnoume ti mnimh pou desmevthike gia to new_data
            list->free_data(new_data); // Apeleftherwsi strings (title)
            free(new_data);            // Apeleftherwsi tis domis writes*
            free(new_node);            // Apeleftherwsi tou komvou
            return;
        }
    }
    
    // Taxinomimeni eisagwgi xrisimopoiwntas ton deikti list->compare_data
    if (list->head == NULL || list->compare_data(new_data, list->head->data) < 0) {
        new_node->next = list->head;
        list->head = new_node;
    } else {
        Node *cur = list->head;
        // Diatrexoume oso to new_data einai megalytero i iso tou epomenou stoixeiou (gia taxinomisi)
        while (cur->next != NULL && list->compare_data(new_data, cur->next->data) >= 0) {
            cur = cur->next;
        }
        new_node->next = cur->next;
        cur->next = new_node;
    }
    list->count++;
}

// Geniki Anazhthsh: Vriskei to void* data pou tairiazei me to search_key
void *generic_search(const List *list, const void *search_key, CompareFunc compare_key)
{
    Node *cur = list->head;
    while (cur != NULL) {
        // H compare_key sygkrinei ta dedomena tou komvou (cur->data) me to kleidi anazhthshs (search_key)
        if (compare_key(cur->data, search_key) == 0) { 
            return cur->data; // Epistrefei ton deikti sta dedomena
        }
        cur = cur->next;
    }
    return NULL;
}

// Geniki Diagrafi: Vriskei ton komvo me vasi to search_key kai ton diagrafei.
void generic_delete_node(List *list, const void *search_key, CompareFunc compare_key)
{
    Node *cur = list->head;
    Node *prev = NULL;

    // Vres ton komvo pros diagrafi
    while (cur != NULL && compare_key(cur->data, search_key) != 0) {
        prev = cur;
        cur = cur->next;
    }

    if (cur == NULL) { return; } // Den vrethike

    // Symmetoxi tou komvou stin lista
    if (prev == NULL) {
        list->head = cur->next;
    } else {
        prev->next = cur->next;
    }

    // Apeleftherwsi Mnimis: 1) strings, 2) domi dedomenwn, 3) komvos
    if (list->free_data != NULL) {
        list->free_data(cur->data); // Apeleftherwsi strings (an yparxoun)
    }
    free(cur->data); // Apeleftherwsi ths domis (author*, book*, writes*)
    free(cur);       // Apeleftherwsi tou komvou Node*
    list->count--;
}

// Geniki Apeleftherwsi olis tis Listas
void generic_free_list(List *list)
{
    Node *cur = list->head;
    while (cur != NULL) {
        Node *todel = cur;
        cur = cur->next;
        
        if (list->free_data != NULL) {
            list->free_data(todel->data);
        }
        free(todel->data);
        free(todel);
    }
    list->head = NULL;
    list->count = 0;
}

// Geniki Diagrafi me Deiktes: Xrisimopoieitai otan i thesi tou komvou einai idi gnwsti
void generic_delete_node_by_pointer(List *list, Node *target_node, Node *prev_node)
{
    if (target_node == NULL) { return; }

    if (prev_node == NULL) {
        list->head = target_node->next;
    } else {
        prev_node->next = target_node->next;
    }

    // Apeleftherwsi Mnimis: 1) strings, 2) domi dedomenwn, 3) komvos
    if (list->free_data != NULL) {
        list->free_data(target_node->data);
    }
    free(target_node->data);
    free(target_node);
    list->count--;
}

// Metraei posoi suggrafeis exoun grapsei ena vivlio (gia elegxo monografias)
int count_writes_for_title(const char *title)
{
    int count = 0;
    Node *wcur = writes_list.head;
    
    while (wcur != NULL) {
        writes *w = (writes *)wcur->data;
        if (strcmp(w->title, title) == 0)
            count++;
        wcur = wcur->next;
    }
    return count;
}

/* III. TYPE-SPECIFIC FUNCTIONS IMPLEMENTATION */

// COMPARE FUNCTIONS (Metatropi tou void* ston swsto typo gia sygkrisi) 

// Sygkrisi gia taxinomisi Author me writer_id (panta me deiktes stis domes)
int compare_authors_for_sort(const void *a, const void *b)
{
    const author *auth_a = (const author *)a;
    const author *auth_b = (const author *)b;
    if (auth_a->writer_id < auth_b->writer_id) return -1;
    if (auth_a->writer_id > auth_b->writer_id) return 1;
    return 0;
}

// Sygkrisi Author data me ena kleidi writer_id (gia generic_search)
int compare_author_by_id_key(const void *data, const void *key)
{
    const author *auth = (const author *)data;
    const int *id_key = (const int *)key; // To kleidi anazhthshs einai to int ID
    if (auth->writer_id < *id_key) return -1;
    if (auth->writer_id > *id_key) return 1;
    return 0;
}

// Sygkrisi Author data me ena kleidi surname (gia generic_search)
int compare_author_by_surname_key(const void *data, const void *key)
{
    const author *auth = (const author *)data;
    const char *surname_key = (const char *)key; // To kleidi anazhthshs einai to string surname
    return strcmp(auth->surname, surname_key);
}

// Sygkrisi gia taxinomisi Book me title
int compare_books_for_sort(const void *a, const void *b)
{
    const book *book_a = (const book *)a;
    const book *book_b = (const book *)b;
    return strcmp(book_a->title, book_b->title);
}

// Sygkrisi Book data me ena kleidi title (gia generic_search)
int compare_book_by_title_key(const void *data, const void *key)
{
    const book *b = (const book *)data;
    const char *title_key = (const char *)key;
    return strcmp(b->title, title_key);
}

// Sygkrisi gia taxinomisi Writes (kata writer_id, meta kata title)
int compare_writes_for_sort(const void *a, const void *b)
{
    const writes *write_a = (const writes *)a;
    const writes *write_b = (const writes *)b;
    
    // Taxinomisi kata writer_id
    if (write_a->writer_id < write_b->writer_id) return -1;
    if (write_a->writer_id > write_b->writer_id) return 1;
    
    // An ta IDs einai isa, taxinomisi kata title
    return strcmp(write_a->title, write_b->title);
}

// FREE FUNCTIONS (Apeleftherwsi strings mesa stis domes) 

void free_author_data(void *data)
{
    author *a = (author *)data;
    if (a->surname) free(a->surname);
    if (a->name) free(a->name);
}

void free_book_data(void *data)
{
    book *b = (book *)data;
    if (b->title) free(b->title);
}

void free_writes_data(void *data)
{
    writes *w = (writes *)data;
    if (w->title) free(w->title);
}

/* IV. APPLICATION LOGIC IMPLEMENTATION */

// LOAD/SAVE FUNCTIONS (Xrisimopoioun tis List) 

void load_authors_from_file(void)
{
    FILE *fp = fopen("authors.txt", "r");
    if (fp == NULL) { printf("authors.txt not found.\n"); next_writer_id = 1; return; }

    int count;
    if (fscanf(fp, "%d", &count) != 1) { printf("Error reading count.\n"); fclose(fp); next_writer_id = 1; return; }

    int max_id = 0;
    char buffer[256];
    int i;

    for ( i = 0; i < count; i++) {
        author *a = (author *)malloc(sizeof(author)); // Desmevsi mnimis gia ti domi
        if (!a) break;

        if (fscanf(fp, "%d", &a->writer_id) != 1) { free(a); break; }
        if (fscanf(fp, "%255s", buffer) != 1) { free(a); break; }
        a->surname = copy_string(buffer);
        if (fscanf(fp, "%255s", buffer) != 1) { free(a->surname); free(a); break; }
        a->name = copy_string(buffer);

        a->num_of_books = 0; // Tha enimerwthei apo update_author_book_counts()
        if (a->writer_id > max_id) max_id = a->writer_id;

        generic_insert_sorted(&author_list, a); // Eisagwgi me tin generic synartisi
    }

    next_writer_id = max_id + 1;
    fclose(fp);
    printf("Loaded %d authors from authors.txt\n", author_list.count);
}

void load_books_from_file(void)
{
    FILE *fp = fopen("books.txt", "r");
    if (fp == NULL) { printf("books.txt not found.\n"); return; }

    int count;
    if (fscanf(fp, "%d", &count) != 1) {
        printf("Error reading count.\n");
        fclose(fp);
        return;
    }

    // Fae to '\n' meta to count
    fgetc(fp);

    char buffer[512];
    int i;
    for ( i = 0; i < count; i++) {
        book *b = (book *)malloc(sizeof(book));
        if (!b) break;

        // Diavase title se olokliri grammi (me kena)
        if (!read_line_fp(fp, buffer, (int)sizeof(buffer))) { free(b); break; }
        b->title = copy_string(buffer);
        if (!b->title) { free(b); break; }

        // Diavase release_date se grammi
        if (!read_line_fp(fp, buffer, (int)sizeof(buffer))) { free(b->title); free(b); break; }
        b->release_date = atoi(buffer);

        // Diavase price se grammi
        if (!read_line_fp(fp, buffer, (int)sizeof(buffer))) { free(b->title); free(b); break; }
        b->price = (float)atof(buffer);

        generic_insert_sorted(&book_list, b);
    }

    fclose(fp);
    printf("Loaded %d books from books.txt\n", book_list.count);
}

void load_writes_from_file(void)
{
    FILE *fp = fopen("writes.txt", "r");
    if (fp == NULL) { printf("writes.txt not found.\n"); return; }

    int count;
    if (fscanf(fp, "%d", &count) != 1) {
        printf("Error reading count.\n");
        fclose(fp);
        return;
    }

    // Fae to '\n' meta to count
    fgetc(fp);

    char buffer[512];
    int i;
    
    for ( i = 0; i < count; i++) {
        writes *w = (writes *)malloc(sizeof(writes));
        if (!w) break;

        // Diavase title se olokliri grammi (me kena)
        if (!read_line_fp(fp, buffer, (int)sizeof(buffer))) { free(w); break; }
        w->title = copy_string(buffer);
        if (!w->title) { free(w); break; }

        // Diavase writer_id se grammi
        if (!read_line_fp(fp, buffer, (int)sizeof(buffer))) { free(w->title); free(w); break; }
        w->writer_id = atoi(buffer);

        generic_insert_sorted(&writes_list, w);
    }

    fclose(fp);
    printf("Loaded writes from writes.txt\n");
}

//helper
// Diavazei mia grammi apo FILE* kai afairei to '\n'
static int read_line_fp(FILE *fp, char *buf, int size)
{
    if (!fgets(buf, size, fp)) return 0;
    buf[strcspn(buf, "\n")] = '\0';
    return 1;
}


void save_authors_to_file(void)
{
    FILE *fp = fopen("authors.txt", "w");
    if (fp == NULL) { printf("Error opening authors.txt for writing.\n"); return; }

    fprintf(fp, "%d\n", author_list.count);

    Node *cur = author_list.head;
    while (cur != NULL) {
        author *a = (author *)cur->data; // Metatropi void* se author*
        fprintf(fp, "%d\n", a->writer_id);
        fprintf(fp, "%s\n", a->surname);
        fprintf(fp, "%s\n", a->name);
        cur = cur->next;
    }
    fclose(fp);
    printf("Authors saved to authors.txt\n");
}

void save_books_to_file(void)
{
    FILE *fp = fopen("books.txt", "w");
    if (fp == NULL) { printf("Error opening books.txt for writing.\n"); return; }

    fprintf(fp, "%d\n", book_list.count);

    Node *cur = book_list.head;
    while (cur != NULL) {
        book *b = (book *)cur->data;
        fprintf(fp, "%s\n", b->title);
        fprintf(fp, "%d\n", b->release_date);
        fprintf(fp, "%f\n", b->price);
        cur = cur->next;
    }
    fclose(fp);
    printf("Books saved to books.txt\n");
}

void save_writes_to_file(void)
{
    FILE *fp = fopen("writes.txt", "w");
    if (fp == NULL) { printf("Error opening writes.txt for writing.\n"); return; }

    fprintf(fp, "%d\n", writes_list.count);

    Node *cur = writes_list.head;
    while (cur != NULL) {
        writes *w = (writes *)cur->data;
        fprintf(fp, "%s\n", w->title);
        fprintf(fp, "%d\n", w->writer_id);
        cur = cur->next;
    }
    fclose(fp);
    printf("Write entries saved to writes.txt\n");
}

void save_data(void)
{
    save_authors_to_file();
    save_books_to_file();
    save_writes_to_file();
}

// CRUD LOGIC 

// Epana-ypologismos tou num_of_books twn authors me vasi ti lista writes
void update_author_book_counts(void)
{
    // 1. Reset metritwn
    Node *acur = author_list.head;
    while (acur != NULL) {
        author *a = (author *)acur->data;
        a->num_of_books = 0;
        acur = acur->next;
    }

    // 2. Diatrexi writes kai enimerwsi authors
    Node *wcur = writes_list.head;
    while (wcur != NULL) {
        writes *w = (writes *)wcur->data;
        // Anazhthsh author me ID, xrisimopoiwntas to generic_search
        author *a = (author *)generic_search(
            &author_list, 
            &w->writer_id, // To kleidi anazhthshs einai to writer_id
            compare_author_by_id_key // H synartisi pou sygkrinaei me ID
        );
        if (a != NULL) {
            a->num_of_books++;
        }
        wcur = wcur->next;
    }
    printf("Author book counts updated based on writes list.\n");
}

// Eisagwgi neou suggrafea
void insert_new_writer(void)
{
    author *new_author = (author *)malloc(sizeof(author));
    if (new_author == NULL) { printf("Memory error.\n"); return; }
    
    char buffer[256];
    printf("Give writer surname: ");
    scanf("%255s", buffer);
    new_author->surname = copy_string(buffer);
    if (new_author->surname == NULL) { free(new_author); return; }

    printf("Give writer name: ");
    scanf("%255s", buffer);
    new_author->name = copy_string(buffer);
    if (new_author->name == NULL) { free(new_author->surname); free(new_author); return; }

    new_author->writer_id = next_writer_id++;
    new_author->num_of_books = 0;

    generic_insert_sorted(&author_list, new_author); // Eisagwgi Author (taxinomimenos me ID)
    printf("Writer inserted with id = %d\n", new_author->writer_id);
}

// Eisagwgi neou vivliou (title me kena)
void insert_new_book(void)
{
    book *new_book = (book *)malloc(sizeof(book));
    if (new_book == NULL) {
        printf("Memory error for new book structure.\n");
        return;
    }

    char buffer[256];

    // Diavazei olokliri grammi gia title (me kena)
    printf("Give book title: ");
    clear_input_buffer();       // katharizei to \n apo proigoumeno scanf (menu choice klt)
    read_line(buffer, 256);     // diabazei title me kena

    if (buffer[0] == '\0') {
        printf("Empty title not allowed.\n");
        free(new_book);
        return;
    }

    new_book->title = copy_string(buffer);
    if (new_book->title == NULL) {
        free(new_book);
        return;
    }

    // Elegxos Year
    while (1) {
        printf("Give release year : ");
        if (scanf("%d", &new_book->release_date) != 1) {
            printf("Invalid input.\n");
            clear_input_buffer();
            continue;
        }
        if (new_book->release_date < 1500 || new_book->release_date > 2025) {
            printf("Year out of range.\n");
            continue;
        }
        break;
    }

    // Elegxos Price
    while (1) {
        printf("Give price : ");
        if (scanf("%f", &new_book->price) != 1) {
            printf("Invalid input.\n");
            clear_input_buffer();
            continue;
        }
        if (new_book->price < 0.0f || new_book->price > 1000.0f) {
            printf("Price out of range.\n");
            continue;
        }
        break;
    }

    generic_insert_sorted(&book_list, new_book); // Eisagwgi Book (taxinomimenos me title)

    int num_authors;
    printf("How many authors does this book have? ");
    scanf("%d", &num_authors);
    int i;

    for ( i = 0; i < num_authors; i++) {
        char surname_buffer[256];

        printf("Give surname of author %d: ", i + 1);
        scanf("%255s", surname_buffer);

        // Anazhthsh Author me surname
        author *acur = (author *)generic_search(
            &author_list,
            surname_buffer,
            compare_author_by_surname_key
        );

        if (acur == NULL) {
            // Dimiourgia neou Author
            author *new_author = (author *)malloc(sizeof(author));
            if (!new_author) {
                printf("Memory error.\n");
                continue;
            }

            new_author->surname = copy_string(surname_buffer);
            if (!new_author->surname) {
                free(new_author);
                continue;
            }

            printf("Give name for new author with surname %s: ", surname_buffer);
            scanf("%255s", buffer);

            new_author->name = copy_string(buffer);
            if (!new_author->name) {
                free(new_author->surname);
                free(new_author);
                continue;
            }

            new_author->writer_id = next_writer_id++;
            new_author->num_of_books = 0;

            generic_insert_sorted(&author_list, new_author);
            acur = new_author;

            printf("New author created with id = %d\n", new_author->writer_id);
        }

        if (acur != NULL) {
            // Dimiourgia neas eggrafis writes
            writes *new_write = (writes *)malloc(sizeof(writes));
            if (!new_write) {
                printf("Memory error.\n");
                continue;
            }

            new_write->title = copy_string(new_book->title);
            if (!new_write->title) {
                free(new_write);
                continue;
            }
            new_write->writer_id = acur->writer_id;

            // Eisagwgi sto writes (me elegxo diplotypou apo to generic_insert_sorted)
            generic_insert_sorted(&writes_list, new_write);

            // Aplos tropos: afou eisagame writes, anevase metriti
            // (an thes 100% akriveia, kane update_author_book_counts() sto telos)
            acur->num_of_books++;
        }
    }

    printf("Book inserted successfully.\n");
}


// Anazhthsh suggrafea me surname
void search_writer(void)
{
    char buffer[256];
    printf("Give writer surname to search: ");
    scanf("%255s", buffer);

    // Anazhthsh author me surname
    author *found_author = (author *)generic_search(
        &author_list, 
        buffer, 
        compare_author_by_surname_key
    );

    if (found_author == NULL) {
        printf("No writer with surname '%s' found.\n", buffer);
        return;
    }
    
    printf("\n--- Writer found ---\n");
    printf("ID: %d\n", found_author->writer_id);
    printf("Surname: %s\n", found_author->surname);
    printf("Name: %s\n", found_author->name);
    printf("Number of books: %d\n", found_author->num_of_books);

    printf("Books of this writer:\n");
    
    // Diatrexi tis writes list gia na vrei ta vivlia tou suggrafea
    Node *wcur = writes_list.head;
    int writer_id_key = found_author->writer_id;

    while (wcur != NULL) {
        writes *w = (writes *)wcur->data;

        if (w->writer_id == writer_id_key) {
            // Anazhthsh tou plirous Book data me to title apo to writes
            book *b = (book *)generic_search(
                &book_list, 
                w->title, 
                compare_book_by_title_key
            );
            
            if (b != NULL) {
                printf("   Title: %s\n", b->title);
                printf("    Release date: %d\n", b->release_date);
                printf("    Price: %.2f\n", b->price);
            }
        }
        wcur = wcur->next;
    }
}

// Anazhthsh vivliou me title
void search_book(void)
{
    char buffer[256];

    printf("Give book title to search: ");
    clear_input_buffer();     // katharizei to \n apo proigoumena scanf
    read_line(buffer, 256);   // diabazei olokliri grammi (me kena)

    if (buffer[0] == '\0') {
        printf("Empty title not allowed.\n");
        return;
    }

    // Anazhthsh book me title
    book *found_book = (book *)generic_search(
        &book_list,
        buffer,
        compare_book_by_title_key
    );

    if (found_book == NULL) {
        printf("No book with title '%s' found.\n", buffer);
        return;
    }

    printf("\n--- Book found ---\n");
    printf("Title: %s\n", found_book->title);
    printf("Release Date: %d\n", found_book->release_date);
    printf("Price: %.2f\n", found_book->price);

    printf("\n--- Writers of this book ---\n");
    Node *wcur = writes_list.head;

    // Diatrexi tis writes list gia na vrei tous suggrafeis tou vivliou
    while (wcur != NULL) {
        writes *w = (writes *)wcur->data;

        if (strcmp(w->title, found_book->title) == 0) {
            int writer_id_key = w->writer_id;

            // Anazhthsh tou plirous Author data me to ID apo to writes
            author *a = (author *)generic_search(
                &author_list,
                &writer_id_key,
                compare_author_by_id_key
            );

            if (a != NULL) {
                printf("ID: %d \n Surname: %s \n Name: %s\n",
                       a->writer_id, a->surname, a->name);
            }
        }

        wcur = wcur->next;
    }
}



// Synartisi diagrafis writes entry (xrisimopoieitai gia na vrei kai na diagrafei me ti generic)
void delete_write_entry(const char *title, int writer_id)
{
    // Dimiourgia proswrinis domis writes gia na xrisimopoiithei ws kleidi anazhthshs
    writes key = { .title = (char *)title, .writer_id = writer_id }; 
    // H generic_delete_node tha xrisimopoiisei tin compare_writes_for_sort gia na vrei to 0
    generic_delete_node(&writes_list, &key, compare_writes_for_sort);
}


// Diagrafi suggrafea (me xeirismo monografiwn)
void delete_writer(void)
{
    int id;
    printf("Give writer id to delete: ");
    if (scanf("%d", &id) != 1) {
        clear_input_buffer();
        printf("Invalid input.\n");
        return;
    }

    // Vres ton writer me vasi to id
    author *target_author = (author *)generic_search(
        &author_list,
        &id,
        compare_author_by_id_key
    );

    if (target_author == NULL) {
        printf("No writer with id %d found.\n", id);
        return;
    }

    // Krata antigrafa surname/name prin diagrapseis (gia na min kaneis use-after-free)
    char *surname_copy = copy_string(target_author->surname);
    char *name_copy    = copy_string(target_author->name);

    // 1) Mazepse ola ta titles pou epireazontai (unique) kai svise OLA ta writes tou writer
    char **affected_titles = NULL;
    int affected_count = 0;

    Node *wcur  = writes_list.head;
    Node *wprev = NULL;

    while (wcur != NULL) {
        writes *w = (writes *)wcur->data;

        if (w->writer_id == id) {
            const char *title = w->title;

            // Vale to title sto affected_titles mono mia fora
            int exists = 0;
            int t;
            for ( t = 0; t < affected_count; t++) {
                if (strcmp(affected_titles[t], title) == 0) {
                    exists = 1;
                    break;
                }
            }
            if (!exists) {
                char **tmp = (char **)realloc(affected_titles,
                                              (affected_count + 1) * sizeof(char *));
                if (tmp == NULL) {
                    printf("Memory error.\n");
                    break;
                }
                affected_titles = tmp;

                affected_titles[affected_count] = copy_string(title);
                if (affected_titles[affected_count] == NULL) {
                    printf("Memory error copying title.\n");
                    break;
                }
                affected_count++;
            }

            // Svise to writes node (safe diagrafi mesa sto loop)
            Node *todel = wcur;
            wcur = wcur->next;
            generic_delete_node_by_pointer(&writes_list, todel, wprev);

            // To wprev den allazei giati svistike o trexon komvos
            continue;
        }

        wprev = wcur;
        wcur  = wcur->next;
    }

    // 2) Gia kathe epireasmeno title: an den exei meinei kanenas writer, svise kai to book (monograph)
    int i;
    for (i = 0; i < affected_count; i++)
	 {
        if (count_writes_for_title(affected_titles[i]) == 0) {
            generic_delete_node(&book_list, affected_titles[i], compare_book_by_title_key);
        }
        free(affected_titles[i]);
    }
    free(affected_titles);

    // 3) Telika svise ton writer
    generic_delete_node(&author_list, &id, compare_author_by_id_key);

    // Safe print (xrisimopoiei ta copies, oxi ton freed target_author)
    printf("Writer '%s %s' (id %d) deleted.\n",
           surname_copy ? surname_copy : "?",
           name_copy ? name_copy : "?",
           id);

    free(surname_copy);
    free(name_copy);

    // 4) Epana-ypologise metrites vivliwn gia olous t??? authors
    update_author_book_counts();
}


// Diagrafi vivliou
void delete_book(void)
{
    char buffer[256];

    printf("Give book title to delete: ");
    clear_input_buffer();     // katharizei to \n apo proigoumena scanf
    read_line(buffer, 256);   // diabazei olokliri grammi (me kena)

    if (buffer[0] == '\0') {
        printf("Empty title not allowed.\n");
        return;
    }

    // Anazhthsh vivliou
    book *target_book = (book *)generic_search(
        &book_list,
        buffer,
        compare_book_by_title_key
    );

    if (target_book == NULL) {
        printf("No book with title '%s' found.\n", buffer);
        return;
    }

    // Krata antigrafo title prin diagrapseis (gia na min kaneis use-after-free sto printf)
    char *title_copy = copy_string(target_book->title);
    if (title_copy == NULL) {
        printf("Memory error.\n");
        return;
    }

    // 1) Diagrafi writes pou antistoixoun sto vivlio
    Node *wcur  = writes_list.head;
    Node *wprev = NULL;

    while (wcur != NULL) {
        writes *w = (writes *)wcur->data;

        if (strcmp(w->title, title_copy) == 0) 
		{
            

            Node *todel = wcur;
            wcur = wcur->next;

            generic_delete_node_by_pointer(&writes_list, todel, wprev);
            continue;
        }

        wprev = wcur;
        wcur  = wcur->next;
    }

    // 2) Diagrafi tou vivliou apo tin book_list
    generic_delete_node(&book_list, title_copy, compare_book_by_title_key);

    printf("Book '%s' deleted, and related authors updated.\n", title_copy);

    free(title_copy);

    // 3) Epana-ypologise metrites vivliwn gia olous tous authors (panta swsto)
    update_author_book_counts();
}


void read_line(char *buf, int size)
{
    if (!fgets(buf, size, stdin)) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0'; // afairesh '\n'
}
