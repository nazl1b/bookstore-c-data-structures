#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erwthma3.a.h"


/* global storage (generic) - Oi 3 vasikoi pinakes mas */
gvec books;
gvec authors;
gvec writesV;
int next_writer_id = 1;


/* Boithitikes gia prosvasi se stoixeia meso deikton */
void *gptr(void *base, size_t elem_size, int idx) {
    return (char*)base + (size_t)idx * elem_size;
}
const void *gptrc(const void *base, size_t elem_size, int idx) {
    return (const char*)base + (size_t)idx * elem_size;
}

/* Arxikopoiisi tou vector */
void gvec_init(gvec *v, size_t elem_size) {
    v->data = NULL;
    v->elem_size = elem_size;
    v->size = 0;
    v->cap = 0;
}

/* Desmeusi mnimis gia epiktasi xoritikotitas */
int gvec_reserve(gvec *v, int new_cap) {
    void *tmp;
    if (new_cap <= v->cap) return 1;
    tmp = realloc(v->data, (size_t)new_cap * v->elem_size);
    if (!tmp) return 0;
    v->data = tmp;
    v->cap = new_cap;
    return 1;
}

/* Ayksisi xoritikotitas an gemisei o pinakas */
int gvec_grow_if_needed(gvec *v, int need_size) {
    int new_cap;
    if (need_size <= v->cap) return 1;
    new_cap = (v->cap == 0) ? 4 : v->cap * 2;
    while (new_cap < need_size) new_cap *= 2;
    return gvec_reserve(v, new_cap);
}

/* Prosthiki stoixeiou sto telos */
int gvec_push_back(gvec *v, const void *elem) {
    if (!gvec_grow_if_needed(v, v->size + 1)) return 0;
    memcpy(gptr(v->data, v->elem_size, v->size), elem, v->elem_size);
    v->size++;
    return 1;
}

/* Eisagogi se sigkekrimeni thesi */
int gvec_insert_at(gvec *v, int idx, const void *elem) {
    void *dst;
    size_t move_bytes;

    if (idx < 0 || idx > v->size) return 0;
    if (!gvec_grow_if_needed(v, v->size + 1)) return 0;

    dst = gptr(v->data, v->elem_size, idx);
    move_bytes = (size_t)(v->size - idx) * v->elem_size;
    memmove((char*)dst + v->elem_size, dst, move_bytes);
    memcpy(dst, elem, v->elem_size);

    v->size++;
    return 1;
}

/* Diagrafi apo sigkekrimeni thesi kai eleutherosi mnimis */
int gvec_remove_at(gvec *v, int idx, g_free_elem free_elem) {
    void *victim;
    size_t move_bytes;

    if (idx < 0 || idx >= v->size) return 0;

    victim = gptr(v->data, v->elem_size, idx);
    if (free_elem) free_elem(victim);

    move_bytes = (size_t)(v->size - idx - 1) * v->elem_size;
    memmove(victim, (char*)victim + v->elem_size, move_bytes);

    v->size--;

    if (v->size == 0) {
        free(v->data);
        v->data = NULL;
        v->cap = 0;
    }
    return 1;
}

/* Eleutherosi olis tis mnimis tou vector */
void gvec_free(gvec *v, g_free_elem free_elem) {
    int i;
    if (free_elem) {
        for (i = 0; i < v->size; i++) {
            free_elem(gptr(v->data, v->elem_size, i));
        }
    }
    free(v->data);
    v->data = NULL;
    v->size = 0;
    v->cap = 0;
}

/* Prosvasi se stoixeio meso index */
void *gvec_at(gvec *v, int idx) {
    return gptr(v->data, v->elem_size, idx);
}
static const void *gvec_at_c(const gvec *v, int idx) {
    return gptrc(v->data, v->elem_size, idx);
}

/* Taksinomisi pinaka meso qsort */
void gvec_sort(gvec *v, g_cmp cmp) {
    if (v->size > 1) qsort(v->data, (size_t)v->size, v->elem_size, cmp);
}

/* Binary search gia eyresi thesis eisagogis */
int gvec_lower_bound(const gvec *v, const void *key, g_cmp_key cmp_elem_key) {
    int low, high;
    low = 0;
    high = v->size;
    while (low < high) {
        int mid;
        const void *elem;
        int c;

        mid = low + (high - low) / 2;
        elem = gvec_at_c(v, mid);
        c = cmp_elem_key(elem, key);

        if (c < 0) low = mid + 1;
        else high = mid;
    }
    return low;
}

/* Binary search gia eyresi index sigkekrimenou stoixeiou */
int gvec_bsearch_index(const gvec *v, const void *key, g_cmp_key cmp_elem_key) {
    int low, high;
    low = 0;
    high = v->size - 1;

    while (low <= high) {
        int mid;
        const void *elem;
        int c;

        mid = low + (high - low) / 2;
        elem = gvec_at_c(v, mid);
        c = cmp_elem_key(elem, key);

        if (c == 0) return mid;
        if (c < 0) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

/* Taksinomimeni eisagogi stoixeion */
int gvec_insert_sorted(gvec *v, const void *elem, g_cmp_key cmp_elem_to_elemkey) {
    int pos;
    pos = gvec_lower_bound(v, elem, cmp_elem_to_elemkey);
    return gvec_insert_at(v, pos, elem);
}

/* helpers - Boithitikes gia strings kai input */
char *copy_string(const char *s) {
    char *p;
    p = (char*)malloc(strlen(s) + 1);
    if (!p) return NULL;
    strcpy(p, s);
    return p;
}

/* Anagnosi grammis me ipostirixi kenon */
void read_line(char *buf, int size) {
    if (!fgets(buf, size, stdin)) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}

/* Katharismos tis eisodou meta apo scanf */
void clear_stdin_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void print_menu(void) {
    printf("\n---- BOOKSTORE MENU ----\n");
    printf("1. Insert new writer record\n");
    printf("2. Insert new book record\n");
    printf("3. Search a writer record\n");
    printf("4. Search a book record\n");
    printf("5. Delete a writer record\n");
    printf("6. Delete a book record\n");
    printf("7. Exit\n");
}


/* callbacks (compare/free) - Eidikes sinartiseis gia kathe typo stoixeiou */
void free_book_elem(void *e) {
    book *b;
    b = (book*)e;
    free(b->title);
}
void free_author_elem(void *e) {
    author *a;
    a = (author*)e;
    free(a->surname);
    free(a->name);
}
void free_writes_elem(void *e) {
    writes *w;
    w = (writes*)e;
    free(w->title);
}

/* Sinkriseis gia taksinomisi kai anazitisi */
int cmp_book_elem(const void *a, const void *b) {
    const book *x;
    const book *y;
    x = (const book*)a;
    y = (const book*)b;
    return strcmp(x->title, y->title);
}
int cmp_author_elem(const void *a, const void *b) {
    const author *x;
    const author *y;
    x = (const author*)a;
    y = (const author*)b;
    if (x->writer_id < y->writer_id) return -1;
    if (x->writer_id > y->writer_id) return  1;
    return 0;
}
int cmp_writes_elem(const void *a, const void *b) {
    const writes *x;
    const writes *y;
    x = (const writes*)a;
    y = (const writes*)b;
    if (x->writer_id < y->writer_id) return -1;
    if (x->writer_id > y->writer_id) return  1;
    return strcmp(x->title, y->title);
}

int cmp_book_elem_to_title_key(const void *elem, const void *key) {
    const book *b;
    const char *title;
    b = (const book*)elem;
    title = (const char*)key;
    return strcmp(b->title, title);
}
int cmp_author_elem_to_id_key(const void *elem, const void *key) {
    const author *a;
    int id;
    a = (const author*)elem;
    id = *(const int*)key;
    if (a->writer_id < id) return -1;
    if (a->writer_id > id) return  1;
    return 0;
}



int cmp_writes_elem_to_key(const void *elem, const void *key) {
    const writes *w;
    const writes_key *k;
    w = (const writes*)elem;
    k = (const writes_key*)key;

    if (w->writer_id < k->writer_id) return -1;
    if (w->writer_id > k->writer_id) return  1;
    return strcmp(w->title, k->title);
}

int cmp_book_elem_to_elemkey(const void *elem, const void *key_elem) {
    return cmp_book_elem(elem, key_elem);
}
int cmp_author_elem_to_elemkey(const void *elem, const void *key_elem) {
    return cmp_author_elem(elem, key_elem);
}
int cmp_writes_elem_to_elemkey(const void *elem, const void *key_elem) {
    return cmp_writes_elem(elem, key_elem);
}

/* find helpers - Boithitikes anazitisis meso ID kai Titlou */
int find_author_index_by_id(int id) {
    return gvec_bsearch_index(&authors, &id, cmp_author_elem_to_id_key);
}
int find_book_index_by_title(const char *title) {
    return gvec_bsearch_index(&books, title, cmp_book_elem_to_title_key);
}
author *find_author_by_id_manual(int writer_id) {
    int idx;
    idx = find_author_index_by_id(writer_id);
    if (idx == -1) return NULL;
    return (author*)gvec_at(&authors, idx);
}
int write_exists(const char *title, int writer_id) {
    writes_key k;
    k.writer_id = writer_id;
    k.title = title;
    return gvec_bsearch_index(&writesV, &k, cmp_writes_elem_to_key) != -1;
}

/* file io - Diastisi kai grapsimo arxeion txt */
void save_authors_to_file(void) {
    FILE *fp;
    int i;

    fp = fopen("authors.txt", "w");
    if (!fp) { printf("Error opening authors.txt for writing.\n"); return; }

    fprintf(fp, "%d\n", authors.size);
    for (i = 0; i < authors.size; i++) {
        const author *a;
        a = (const author*)gvec_at_c(&authors, i);
        fprintf(fp, "%d\n", a->writer_id);
        fprintf(fp, "%s\n", a->surname);
        fprintf(fp, "%s\n", a->name);
    }
    fclose(fp);
}

void load_authors_from_file(void) {
    FILE *fp;
    int count, i, max_id;

    fp = fopen("authors.txt", "r");
    if (!fp) {
        printf("authors.txt not found, starting with empty authors list.\n");
        next_writer_id = 1;
        return;
    }

    count = 0;
    if (fscanf(fp, "%d", &count) != 1) {
        printf("Error reading author count from file.\n");
        fclose(fp);
        next_writer_id = 1;
        return;
    }

    max_id = 0;
    for (i = 0; i < count; i++) {
        int id;
        char buf[256];
        author a;

        if (fscanf(fp, "%d", &id) != 1) break;
        if (fscanf(fp, "%255s", buf) != 1) break;
        a.surname = copy_string(buf);
        if (fscanf(fp, "%255s", buf) != 1) { free(a.surname); break; }
        a.name = copy_string(buf);

        if (!a.surname || !a.name) {
            free(a.surname); free(a.name);
            break;
        }

        a.writer_id = id;
        a.num_of_books = 0;

        if (!gvec_push_back(&authors, &a)) {
            free(a.surname); free(a.name);
            break;
        }

        if (id > max_id) max_id = id;
    }

    fclose(fp);
    gvec_sort(&authors, cmp_author_elem);
    next_writer_id = max_id + 1;
}

void save_books_to_file(void) {
    FILE *fp;
    int i;

    fp = fopen("books.txt", "w");
    if (!fp) { printf("Error opening books.txt for writing.\n"); return; }

    fprintf(fp, "%d\n", books.size);
    for (i = 0; i < books.size; i++) {
        const book *b;
        b = (const book*)gvec_at_c(&books, i);
        fprintf(fp, "%s\n", b->title);
        fprintf(fp, "%d\n", b->release_date);
        fprintf(fp, "%f\n", b->price);
    }
    fclose(fp);
}

void load_books_from_file(void) {
    FILE *fp;
    int count, i;
    char line[512];

    fp = fopen("books.txt", "r");
    if (!fp) {
        printf("books.txt not found, starting with empty books list.\n");
        return;
    }

    if (fscanf(fp, "%d", &count) != 1) {
        printf("Error reading book count from file.\n");
        fclose(fp);
        return;
    }
    
    // katharismos tou '/n' meta ton arithmo count oste h fgets na diabasei ton prvto titlo
    fgets(line, sizeof(line), fp); 

    for (i = 0; i < count; i++) {
        book b;

        // diabasma olokliris grammis gia ton titlo (epitrepei kena)
        if (!fgets(line, sizeof(line), fp)) break;
        line[strcspn(line, "\n")] = 0; // afairesi /n

        b.title = copy_string(line);
        if (!b.title) break;

        // diabasma ton upoloipon pedion
        if (fscanf(fp, "%d", &b.release_date) != 1) { free(b.title); break; }
        if (fscanf(fp, "%f", &b.price) != 1) { free(b.title); break; }

        // katharismos tou '/n' meta to price gia epomeni epanaliphi
        fgets(line, sizeof(line), fp);

        if (!gvec_push_back(&books, &b)) { 
            free(b.title); 
            break; 
        }
    }

    fclose(fp);
    gvec_sort(&books, cmp_book_elem);
}

void save_writes_to_file(void) {
    FILE *fp;
    int i;

    fp = fopen("writes.txt", "w");
    if (!fp) { printf("Error opening writes.txt for writing.\n"); return; }

    fprintf(fp, "%d\n", writesV.size);
    for (i = 0; i < writesV.size; i++) {
        const writes *w;
        w = (const writes*)gvec_at_c(&writesV, i);
        fprintf(fp, "%s\n", w->title);
        fprintf(fp, "%d\n", w->writer_id);
    }
    fclose(fp);
}

void load_writes_from_file(void) {
    FILE *fp;
    int count, i;
    char line[512];

    fp = fopen("writes.txt", "r");
    if (!fp) {
        printf("writes.txt not found, starting with empty writes list.\n");
        return;
    }

    count = 0;
    if (fscanf(fp, "%d", &count) != 1) {
        printf("Error reading writes count from file.\n");
        fclose(fp);
        return;
    }

    // katharismos '/n' meta thn anagnosi tou count
    fgets(line, sizeof(line), fp);

    for (i = 0; i < count; i++) {
        writes w;

        // anagnosi olokliris grammis gia ton titlo (upostirizei kena)
        if (!fgets(line, sizeof(line), fp)) break;
        line[strcspn(line, "\n")] = 0; // afairesh '/n'

        w.title = copy_string(line);
        if (!w.title) break;

        // read ID tou siggrafea
        if (fscanf(fp, "%d", &w.writer_id) != 1) { 
            free(w.title); 
            break; 
        }

        // katharismos '/n' meta to ID gia na einai etoimi i fgets sthn epomeni epanaliphi
        fgets(line, sizeof(line), fp);

        if (!gvec_push_back(&writesV, &w)) { 
            free(w.title); 
            break; 
        }
    }

    fclose(fp);
    gvec_sort(&writesV, cmp_writes_elem);
}

/* recompute - Ipologismos ek neou tou arithmou ton vivlion ana siggrafea */
void recompute_num_of_books(void) {
    int i, j;

    for (i = 0; i < authors.size; i++) {
        author *a;
        a = (author*)gvec_at(&authors, i);
        a->num_of_books = 0;
    }

    for (j = 0; j < writesV.size; j++) {
        const writes *w;
        int idx;
        author *a;

        w = (const writes*)gvec_at_c(&writesV, j);
        idx = find_author_index_by_id(w->writer_id);
        if (idx != -1) {
            a = (author*)gvec_at(&authors, idx);
            a->num_of_books++;
        }
    }
}

/* operations - Vasikes leitourgies tou programmatos */
int insert_author_sorted(const char *surname, const char *name) {
    author a;

    a.writer_id = next_writer_id++;
    a.surname = copy_string(surname);
    a.name = copy_string(name);
    a.num_of_books = 0;

    if (!a.surname || !a.name) {
        free(a.surname); free(a.name);
        return -1;
    }

    if (!gvec_insert_sorted(&authors, &a, cmp_author_elem_to_elemkey)) {
        free(a.surname); free(a.name);
        return -1;
    }
    return a.writer_id;
}

void insert_new_writer(void) {
    char surname[256], name[256];
    int id;

    printf("Give writer surname: ");
    scanf("%255s", surname);
    printf("Give writer name: ");
    scanf("%255s", name);

    id = insert_author_sorted(surname, name);
    if (id == -1) printf("Memory error while inserting writer.\n");
    else printf("Writer inserted with id = %d\n", id);
}

int find_author_count_by_surname(const char *surname) {
    int i, c;
    c = 0;
    for (i = 0; i < authors.size; i++) {
        const author *a;
        a = (const author*)gvec_at_c(&authors, i);
        if (strcmp(a->surname, surname) == 0) c++;
    }
    return c;
}

int find_author_id_by_surname_unique_or_choose(const char *surname) {
    int count, i;
    count = find_author_count_by_surname(surname);

    if (count == 0) {
        char name[256];
        printf("Author with surname '%s' not found.\n", surname);
        printf("Give name for new author '%s': ", surname);
        scanf("%255s", name);
        return insert_author_sorted(surname, name);
    }

    if (count == 1) {
        for (i = 0; i < authors.size; i++) {
            const author *a;
            a = (const author*)gvec_at_c(&authors, i);
            if (strcmp(a->surname, surname) == 0) return a->writer_id;
        }
    }

    printf("Found multiple authors with surname '%s':\n", surname);
    for (i = 0; i < authors.size; i++) {
        const author *a;
        a = (const author*)gvec_at_c(&authors, i);
        if (strcmp(a->surname, surname) == 0) {
            printf("  ID: %d | %s %s\n", a->writer_id, a->surname, a->name);
        }
    }

    printf("Give writer_id to use for surname '%s': ", surname);
    {
        int chosen_id;
        scanf("%d", &chosen_id);
        if (find_author_index_by_id(chosen_id) == -1) {
            printf("Invalid writer_id. Skipping this author.\n");
            return -1;
        }
        return chosen_id;
    }
}

void insert_write_entry(const char *book_title, int writer_id) {
    writes w;

    if (write_exists(book_title, writer_id)) return;

    w.writer_id = writer_id;
    w.title = copy_string(book_title);
    if (!w.title) {
        printf("Memory error for write title.\n");
        return;
    }

    if (!gvec_insert_sorted(&writesV, &w, cmp_writes_elem_to_elemkey)) {
        free(w.title);
    }
}

int insert_book_sorted(const char *title, int year, float price) {
    book b;

    b.title = copy_string(title);
    b.release_date = year;
    b.price = price;

    if (!b.title) return 0;

    if (!gvec_insert_sorted(&books, &b, cmp_book_elem_to_elemkey)) {
        free(b.title);
        return 0;
    }
    return 1;
}

void insert_new_book(void) {
    char buffer[512];
    int year;
    float price;
    int num_authors;
    int a;

    printf("Give book title: ");
    clear_stdin_line();
    read_line(buffer, (int)sizeof(buffer));

    while (1) {
        printf("Give release year: ");
        if (scanf("%d", &year) != 1) {
            printf("Invalid input. Please give a number.\n");
            clear_stdin_line();
            continue;
        }
        if (year < 1500 || year > 2025) {
            printf("Year out of range. Try again.\n");
            continue;
        }
        break;
    }

    while (1) {
        printf("Give price: ");
        if (scanf("%f", &price) != 1) {
            printf("Invalid input. Please give a number.\n");
            clear_stdin_line();
            continue;
        }
        if (price < 0.0f || price > 100000.0f) {
            printf("Price out of range. Try again.\n");
            continue;
        }
        break;
    }

    if (!insert_book_sorted(buffer, year, price)) {
        printf("Memory error in book insertion.\n");
        return;
    }

    printf("How many authors does this book have? ");
    scanf("%d", &num_authors);

    for (a = 0; a < num_authors; a++) {
        char surname[256];
        int writer_id;

        printf("Give surname of author %d: ", a + 1);
        scanf("%255s", surname);

        writer_id = find_author_id_by_surname_unique_or_choose(surname);
        if (writer_id == -1) continue;

        insert_write_entry(buffer, writer_id);
    }

    recompute_num_of_books();
    printf("Book inserted.\n");
}

void search_writer(void) {
    char buffer[256];
    int found;
    int i;

    found = 0;

    if (authors.size == 0) {
        printf("No writers in the system.\n");
        return;
    }

    printf("Give writer surname to search: ");
    scanf("%255s", buffer);

    for (i = 0; i < authors.size; i++) {
        const author *a;
        a = (const author*)gvec_at_c(&authors, i);

        if (strcmp(a->surname, buffer) == 0) {
            int printed_any;
            int j;

            found = 1;
            printf("\n--- Writer found ---\n");
            printf("ID: %d\n", a->writer_id);
            printf("Surname: %s\n", a->surname);
            printf("Name: %s\n", a->name);
            printf("Number of books: %d\n", a->num_of_books);
            printf("Books of this writer:\n");

            printed_any = 0;
            for (j = 0; j < writesV.size; j++) {
                const writes *w;
                int bidx;
                const book *b;

                w = (const writes*)gvec_at_c(&writesV, j);
                if (w->writer_id == a->writer_id) {
                    bidx = find_book_index_by_title(w->title);
                    if (bidx != -1) {
                        b = (const book*)gvec_at_c(&books, bidx);
                        printf("    Title: %s\n", b->title);
                        printf("    Release date: %d\n", b->release_date);
                        printf("    Price: %.2f\n", b->price);
                        printed_any = 1;
                    }
                }
            }

            if (!printed_any) printf("  (No books recorded for this writer)\n");
        }
    }

    if (!found) printf("No writer with surname '%s' found.\n", buffer);
}

void search_book(void) {
    char buffer[512];
    int idx;
    int j;
    int listed;

    if (books.size == 0) {
        printf("No books in the system.\n");
        return;
    }

    printf("Give book title to search: ");
    clear_stdin_line();
    read_line(buffer, (int)sizeof(buffer));

    idx = find_book_index_by_title(buffer);
    if (idx == -1) {
        printf("No book with title '%s' found.\n", buffer);
        return;
    }

    {
        const book *b;
        b = (const book*)gvec_at_c(&books, idx);
        printf("\n--- Book found ---\n");
        printf("Title: %s\n", b->title);
        printf("Release Date: %d\n", b->release_date);
        printf("Price: %.2f EUR\n", b->price);
    }

    printf("\n--- Writers of this book ---\n");
    listed = 0;
    for (j = 0; j < writesV.size; j++) {
        const writes *w;
        author *a;

        w = (const writes*)gvec_at_c(&writesV, j);
        if (strcmp(w->title, buffer) == 0) {
            a = find_author_by_id_manual(w->writer_id);
            if (a) {
                printf(" ID: %d \n Surname: %s \n Name: %s\n",
                       a->writer_id, a->surname, a->name);
                listed = 1;
            }
        }
    }
    if (!listed) printf("No authors linked to this book.\n");
}

int count_writes_for_title(const char *title) {
    int i, c;
    c = 0;
    for (i = 0; i < writesV.size; i++) {
        const writes *w;
        w = (const writes*)gvec_at_c(&writesV, i);
        if (strcmp(w->title, title) == 0) c++;
    }
    return c;
}

void delete_writes_for_title_and_writer(const char *title, int writer_id) {
    int i;
    i = 0;
    while (i < writesV.size) {
        const writes *w;
        w = (const writes*)gvec_at_c(&writesV, i);

        if (w->writer_id == writer_id && strcmp(w->title, title) == 0) {
            gvec_remove_at(&writesV, i, free_writes_elem);
        } else {
            i++;
        }
    }
}

void delete_book(void) {
    char title_buffer[512];
    int index;
    int i;

    if (books.size == 0) {
        printf("No books in the system.\n");
        return;
    }

    printf("Give book title to delete: ");
    clear_stdin_line();
    read_line(title_buffer, (int)sizeof(title_buffer));

    index = find_book_index_by_title(title_buffer);
    if (index == -1) {
        printf("No book with title '%s' found.\n", title_buffer);
        return;
    }

    i = 0;
    while (i < writesV.size) {
        const writes *w;
        w = (const writes*)gvec_at_c(&writesV, i);
        if (strcmp(w->title, title_buffer) == 0) {
            gvec_remove_at(&writesV, i, free_writes_elem);
        } else {
            i++;
        }
    }

    gvec_remove_at(&books, index, free_book_elem);

    recompute_num_of_books();
    printf("Book with title '%s' deleted.\n", title_buffer);
}

void delete_writer(void) {
    int id;
    int index;
    int writer_id;
    char **titles;
    int titles_count;
    int i;

    if (authors.size == 0) {
        printf("No writers in the system.\n");
        return;
    }

    printf("Give writer id to delete: ");
    scanf("%d", &id);

    index = find_author_index_by_id(id);
    if (index == -1) {
        printf("No writer with id %d found.\n", id);
        return;
    }

    writer_id = id;
    titles = NULL;
    titles_count = 0;

    for (i = 0; i < writesV.size; i++) {
        const writes *w;
        int exists;
        int t;
        char **tmp;

        w = (const writes*)gvec_at_c(&writesV, i);
        if (w->writer_id != writer_id) continue;

        exists = 0;
        for (t = 0; t < titles_count; t++) {
            if (strcmp(titles[t], w->title) == 0) { exists = 1; break; }
        }
        if (!exists) {
            tmp = (char**)realloc(titles, (titles_count + 1) * sizeof(char*));
            if (!tmp) break;
            titles = tmp;
            titles[titles_count] = copy_string(w->title);
            if (!titles[titles_count]) break;
            titles_count++;
        }
    }

    gvec_remove_at(&authors, index, free_author_elem);

    for (i = 0; i < titles_count; i++) {
        const char *title;
        int total;
        int j;

        title = titles[i];
        total = count_writes_for_title(title);

        if (total == 1) {
            j = 0;
            while (j < writesV.size) {
                const writes *w;
                w = (const writes*)gvec_at_c(&writesV, j);
                if (strcmp(w->title, title) == 0) {
                    gvec_remove_at(&writesV, j, free_writes_elem);
                } else {
                    j++;
                }
            }

            {
                int bidx;
                bidx = find_book_index_by_title(title);
                if (bidx != -1) {
                    gvec_remove_at(&books, bidx, free_book_elem);
                    printf("Monograph book '%s' deleted.\n", title);
                }
            }
        } else {
            delete_writes_for_title_and_writer(title, writer_id);
        }
    }

    for (i = 0; i < titles_count; i++) free(titles[i]);
    free(titles);

    recompute_num_of_books();
    printf("Writer with id %d deleted.\n", id);
}


