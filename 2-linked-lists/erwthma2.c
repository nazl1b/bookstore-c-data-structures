#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"erwthma2.h"


/* GLOBAL LIST HEADS & COUNTERS */

// Kerfalides gia tis 3 listes
author_node *author_head = NULL;
book_node   *book_head   = NULL;
writes_node *writes_head = NULL;

// Metrites gia to plithos twn stoixeiwn se kathe lista
int author_count = 0;
int book_count   = 0;
int writes_count = 0;

int next_writer_id = 1;   // gia automato id twn suggrafevn


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

/* desmeusi mnimis kai antigrafi string */
char *copy_string(const char *s)
{
    char *p = (char *)malloc(strlen(s) + 1); // Desmeusi gia to string + null terminator
    if (p == NULL)
        return NULL;
    strcpy(p, s);
    return p;
}

/* LOAD / SAVE AUTHORS */

// Fortwsi list author apo arxeio
void load_authors_from_file(void)
{
    FILE *fp = fopen("authors.txt", "r");
    if (fp == NULL) {
        printf("authors.txt not found, starting with empty authors list.\n");
        author_head = NULL;
        author_count = 0;
        next_writer_id = 1;
        return;
    }

    int count;
    if (fscanf(fp, "%d", &count) != 1) { // Diavazei ton synoliko arithmo twn author
        printf("Error reading author count from file.\n");
        fclose(fp);
        author_head = NULL;
        author_count = 0;
        next_writer_id = 1;
        return;
    }

    author_head = NULL;
    author_count = 0;

    int i;
    int max_id = 0;
    char buffer[256];

    for (i = 0; i < count; i++) {
        author a;
        if (fscanf(fp, "%d", &a.writer_id) != 1) // writer_id
            break;

        if (fscanf(fp, "%255s", buffer) != 1)    // surname
            break;
        a.surname = copy_string(buffer);         // Desmeusi mnimis kai antigrafi

        if (fscanf(fp, "%255s", buffer) != 1)    // name
            break;
        a.name = copy_string(buffer);            // Desmeusi mnimis kai antigrafi

        a.num_of_books = 0;  /* tha mporei na upologistei apo writes an theloume, to kanei h update_author_book_counts() */

        if (a.writer_id > max_id)
            max_id = a.writer_id;

        insert_author_node_sorted(a);
        author_count++;
    }

    next_writer_id = max_id + 1; // Orismos tou epomenou diathesimou id

    fclose(fp);
    printf("Loaded %d authors from authors.txt\n", author_count);
}

// Apothikeusi list author se arxeio
void save_authors_to_file(void)
{
    FILE *fp = fopen("authors.txt", "w");
    if (fp == NULL) {
        printf("Error opening authors.txt for writing.\n");
        return;
    }

    fprintf(fp, "%d\n", author_count); // Grafei ton synoliko arithmo

    author_node *cur = author_head;
    while (cur != NULL) {
        fprintf(fp, "%d\n", cur->data.writer_id);
        fprintf(fp, "%s\n", cur->data.surname);
        fprintf(fp, "%s\n", cur->data.name);
        cur = cur->next;
    }

    fclose(fp);
    printf("Authors saved to authors.txt\n");
}

/* LOAD / SAVE BOOKS */

void load_books_from_file(void)
{
    FILE *fp = fopen("books.txt", "r");
    if (fp == NULL) {
        printf("books.txt not found, starting with empty books list.\n");
        book_head = NULL;
        book_count = 0;
        return;
    }

    int count;
    if (fscanf(fp, "%d", &count) != 1) {
        printf("Error reading book count from file.\n");
        fclose(fp);
        book_head = NULL;
        book_count = 0;
        return;
    }

    // katharizoume to upoloipo ths grammis meta to count
    int ch;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}

    book_head = NULL;
    book_count = 0;

	int i;
    for (i = 0; i < count; i++)
    {
        book b;
        char title_line[512];

        // 1) title ws olokliri grammh (mporei na exei kena)
        if (fgets(title_line, sizeof(title_line), fp) == NULL)
            break;
        title_line[strcspn(title_line, "\n")] = '\0';

        b.title = copy_string(title_line);
        if (b.title == NULL) {
            printf("Memory error copying book title.\n");
            break;
        }

        // 2) release_date (epomeni grammh)
        if (fscanf(fp, "%d", &b.release_date) != 1) {
            free(b.title);
            break;
        }

        // 3) price (epomeni grammh)
        if (fscanf(fp, "%f", &b.price) != 1) {
            free(b.title);
            break;
        }

        // 4) katharizoume to '\n' meta to price
        while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}

        // 5) insert sorted
        insert_book_node_sorted(b);
        book_count++;
    }

    fclose(fp);
    printf("Loaded %d books from books.txt\n", book_count);
}


// Apothikeusi list book se arxeio
void save_books_to_file(void)
{
    FILE *fp = fopen("books.txt", "w");
    if (fp == NULL) {
        printf("Error opening books.txt for writing.\n");
        return;
    }

    fprintf(fp, "%d\n", book_count);

    book_node *cur = book_head;
    while (cur != NULL) {
        fprintf(fp, "%s\n", cur->data.title);
        fprintf(fp, "%d\n", cur->data.release_date);
        fprintf(fp, "%f\n", cur->data.price);
        cur = cur->next;
    }

    fclose(fp);
    printf("Books saved to books.txt\n");
}

/* LOAD / SAVE WRITES */

void load_writes_from_file(void)
{
    FILE *fp = fopen("writes.txt", "r");
    if (fp == NULL) {
        printf("writes.txt not found, starting with empty writes list.\n");
        writes_head = NULL;
        writes_count = 0;
        return;
    }

    int count;
    if (fscanf(fp, "%d", &count) != 1) {
        printf("Error reading writes count from file.\n");
        fclose(fp);
        writes_head = NULL;
        writes_count = 0;
        return;
    }

    // katharizoume to upoloipo ths grammis meta to count
    int ch;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}

    writes_head = NULL;
    writes_count = 0;

	int i;
    for ( i = 0; i < count; i++)
    {
        char title_line[512];

        // 1) diavazoume titlo ws OLH grammh (mporei na exei kena)
        if (fgets(title_line, sizeof(title_line), fp) == NULL)
            break;

        title_line[strcspn(title_line, "\n")] = '\0';

        // 2) diavazoume writer_id (epomenh grammh)
        int writer_id;
        if (fscanf(fp, "%d", &writer_id) != 1)
            break;

        // 3) katharizoume to '\n' meta ton arithmo, gia na doulepsei to epomeno fgets
        while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}

        // 4) eisagogi stin lista (insert_write_entry prepei na kanei copy_string mesa)
        insert_write_entry(title_line, writer_id);
    }

    fclose(fp);
    printf("Loaded writes from writes.txt\n");
}


// Apothikeusi list writes se arxeio
void save_writes_to_file(void)
{
    FILE *fp = fopen("writes.txt", "w");
    if (fp == NULL) {
        printf("Error opening writes.txt for writing.\n");
        return;
    }

    fprintf(fp, "%d\n", writes_count);

    writes_node *cur = writes_head;
    while (cur != NULL) {
        fprintf(fp, "%s\n", cur->data.title);
        fprintf(fp, "%d\n", cur->data.writer_id);
        cur = cur->next;
    }

    fclose(fp);
    printf("Write entries saved to writes.txt\n");
}

/* INSERT NEW WRITER */

// Eisagwgh author se taksinomimeni lista (me writer_id)
void insert_author_node_sorted(author new_author)
{
    author_node *node = (author_node *)malloc(sizeof(author_node));
    if (node == NULL) {
        printf("Memory error in insert_author_node_sorted.\n");
        return;
    }

    node->data = new_author;
    node->next = NULL;

    // Eisagwgi stin arxi h an einai adeia
    if (author_head == NULL || author_head->data.writer_id > new_author.writer_id) {
        node->next = author_head;
        author_head = node;
    } else {
        // Vriskoume ti thesi eisagwgis
        author_node *cur = author_head;
        while (cur->next != NULL && cur->next->data.writer_id < new_author.writer_id)
            cur = cur->next;
        // Eisagwgi meta to cur
        node->next = cur->next;
        cur->next = node;
    }
}

// Dimiourgia kai eisagwgi neou suggrafea
void insert_new_writer(void)
{
    author new_author;
    char buffer[256];

    printf("Give writer surname: ");
    scanf("%255s", buffer);
    new_author.surname = copy_string(buffer);
    if (new_author.surname == NULL) {
        printf("Memory error for surname.\n");
        return;
    }

    printf("Give writer name: ");
    scanf("%255s", buffer);
    new_author.name = copy_string(buffer);
    if (new_author.name == NULL) {
        printf("Memory error for name.\n");
        free(new_author.surname); // Apeleftherwsi ths prohgoumenhs desmevmenhs mnimhs
        return;
    }

    new_author.writer_id = next_writer_id++; // Aftomati athetisi id
    new_author.num_of_books = 0;

    insert_author_node_sorted(new_author);
    author_count++;

    printf("Writer inserted with id = %d\n", new_author.writer_id);
}

/* INSERT NEW BOOK */

// Eisagwgi vivliou se taksinomimeni lista (me title - alphavhtika)
void insert_book_node_sorted(book new_book)
{
    book_node *node = (book_node *)malloc(sizeof(book_node));
    if (node == NULL) {
        printf("Memory error in insert_book_node_sorted.\n");
        return;
    }

    node->data = new_book;
    node->next = NULL;

    // Eisagwgi stin arxi h an einai adeia
    if (book_head == NULL || strcmp(new_book.title, book_head->data.title) < 0) {
        node->next = book_head;
        book_head = node;
    } else {
        // Vriskoume ti thesi eisagwgis
        book_node *cur = book_head;
        while (cur->next != NULL &&
               strcmp(cur->next->data.title, new_book.title) < 0)
        {
            cur = cur->next;
        }
        // Eisagwgi meta to cur
        node->next = cur->next;
        cur->next = node;
    }
}

/* insert write entry sorted by writer_id, then title */
// Eisagwgi eggrafhs syndesis (writes) se taksinomimeni lista (writer_id, meta title)
void insert_write_entry(const char *book_title, int writer_id)
{
    writes_node *node = (writes_node *)malloc(sizeof(writes_node));
    // ELEGXOS MALLOC
    if (node == NULL) {
        printf("Memory error for write node.\n");
        return;
    }
    
    node->data.title = copy_string(book_title);
    node->data.writer_id = writer_id;
    
    if (node->data.title == NULL) {
        printf("Memory error for write title.\n");
        free(node);
        return;
    }
    node->next = NULL;

    if (writes_head == NULL) {
        writes_head = node;
    } else {
        writes_node *cur = writes_head;
        writes_node *prev = NULL;

        while (cur != NULL) {
            
            // **PROSTHIKI: Elegxos gia Diplotypi Eggrafi**
            if (cur->data.writer_id == writer_id &&
                strcmp(cur->data.title, book_title) == 0)
            {
                printf("Warning: Write entry (%d, %s) already exists. Insertion aborted.\n", writer_id, book_title);
                free(node->data.title); // Apeleftherwsi tou desmevmenou titlou
                free(node);             // Apeleftherwsi tou komvou
                return;                 // Diakopi ths synarthshs
            }

            // Synthiki Taksinomisis: Writer ID se auksoysa seira, meta Title se auksoysa seira
            if (cur->data.writer_id > writer_id)
                break;
            if (cur->data.writer_id == writer_id &&
                strcmp(cur->data.title, book_title) > 0)
                break;

            prev = cur;
            cur = cur->next;
        }
        
        // Eisagwgi tou neou komvou
        if (prev == NULL) {
            node->next = writes_head;
            writes_head = node;
        } else {
            node->next = cur;
            prev->next = node;
        }
    }

    writes_count++;
}


// Dimiourgia kai eisagwgi neou vivliou
void insert_new_book(void)
{
    book new_book;
    char buffer[256];
    int num, i;
    
    printf("Give book title: ");
    // clear buffer (leftover '\n')
    while (getchar() != '\n'); 
    read_line(buffer, sizeof(buffer)); // anagnosi titlou me kena

    new_book.title = copy_string(buffer);
    
    if (new_book.title == NULL)
	 {
        printf("Memory error for book title.\n");
        return;
    }

    /* ELEGXOS GIA RELEASE YEAR */
    while (1)
    {
        printf("Give release year : ");
        if (scanf("%d", &new_book.release_date) != 1)
        {
            printf("Invalid input. Please give a number.\n");

            // katharizei to input buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }

            continue; // ksanarwtame
        }

        if (new_book.release_date < 1500 || new_book.release_date > 2025) // Elegxos orion
        {
            printf("Year out of range. Try again.\n");
            continue; // ksanarwtame
        }

        // an ftasame edw, i timi einai ok
        break;
    }

    /* ELEGXOS GIA PRICE */
    while (1)
    {
        printf("Give price: ");
        if (scanf("%f", &new_book.price) != 1)
        {
            printf("Invalid input. Please give a number.\n");

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }

            continue; // ksanarwtame
        }

        if (new_book.price < 0.0f || new_book.price > 1000.0f) // Elegxos orion
        {
            printf("Price out of range. Try again.\n");
            continue; // ksanarwtame
        }

        break;
    }
    
    /* eisagoume to vivlio stin taksinomimeni lista */
    insert_book_node_sorted(new_book);
    book_count++;

    /* sundesi me suggrafeis (writes + author list) */
    printf("How many authors does this book have? ");
    int num_authors;
    scanf("%d", &num_authors);

    for (i = 0; i < num_authors; i++) {
        int found = 0;
        author_node *acur;
        author new_author;
        int writer_id;

        printf("Give surname of author %d: ", i + 1);
        scanf("%255s", buffer);

        /* psaxnoume an yparxei hdh autos o suggrafeas */
        acur = author_head;
        while (acur != NULL) {
            if (strcmp(acur->data.surname, buffer) == 0) {
                found = 1;
                break;
            }
            acur = acur->next;
        }

        if (!found) {
            /* dimiourgoume neo suggrafea */
            new_author.surname = copy_string(buffer);
            if (new_author.surname == NULL) {
                printf("Memory error for surname.\n");
                continue;
            }
            printf("Give name for new author with surname %s: ", buffer);
            scanf("%255s", buffer);
            new_author.name = copy_string(buffer);
            if (new_author.name == NULL) {
                printf("Memory error for name.\n");
                free(new_author.surname);
                continue;
            }
            new_author.writer_id = next_writer_id++;
            new_author.num_of_books = 0;

            insert_author_node_sorted(new_author);
            author_count++;
            
            // Vriskoume ton neotera dimiourgimeno author gia na paroume to id tou (logw taksinomhshs)
            acur = author_head;
            while (acur != NULL && acur->data.writer_id != new_author.writer_id)
                acur = acur->next;

            printf("New author created with id = %d\n", new_author.writer_id);
        }

        if (acur != NULL) {
            writer_id = acur->data.writer_id;
            acur->data.num_of_books++; // Auksisi metriti vivliwn tou suggrafea
            insert_write_entry(new_book.title, writer_id); // Eisagwgi syndesis sto writes
        }
    }

    printf("Book inserted successfully.\n");
}

/* SEARCH WRITER */

// Metraei posoi suggrafeis exoun grapsei ena sygkekrimeno vivlio
int count_writes_for_title(const char *title)
{
    int count = 0;
    writes_node *wcur = writes_head;

    while (wcur != NULL) {
        if (strcmp(wcur->data.title, title) == 0)
            count++;
        wcur = wcur->next;
    }
    return count;
}

// Vriskei author me ID (sequential search)
author_node *find_author_by_id(int id)            //use Sequential Search logo list
{
    author_node *cur = author_head;
    while (cur != NULL) {
        if (cur->data.writer_id == id)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

/* HELPER: UPDATE BOOK COUNTS FOR AUTHORS */

// Enimerwnei to num_of_books twn authors me vasi ti lista writes
void update_author_book_counts(void)
{
    // 1. Reset num_of_books gia olous tous authors
    author_node *acur = author_head;
    while (acur != NULL) {
        acur->data.num_of_books = 0;
        acur = acur->next;
    }

    // 2. Diatrexis tis writes kai auksisi tou counter
    writes_node *wcur = writes_head;
    while (wcur != NULL) {
        author_node *author = find_author_by_id(wcur->data.writer_id);
        if (author != NULL) {
            author->data.num_of_books++; // Auksisi tou metriti tou sygkekrimenou author
        }
        wcur = wcur->next;
    }

    printf("Author book counts updated based on writes list.\n");
}

// Vriskei book me title (sequential search)
book_node *find_book_by_title(const char *title)  //use Sequential Search logo list
{
    book_node *cur = book_head;
    while (cur != NULL) {
        if (strcmp(cur->data.title, title) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

// Anazhthsh suggrafea me vasi to surname
void search_writer(void)
{
    char buffer[256];
    int found = 0;
    author_node *cur;

    if (author_head == NULL) {
        printf("No writers in the system.\n");
        return;
    }

    printf("Give writer surname to search: ");
    scanf("%255s", buffer);

    cur = author_head;
    while (cur != NULL) {
        if (strcmp(cur->data.surname, buffer) == 0) {
            int writer_id = cur->data.writer_id;

            printf("\n--- Writer found ---\n");
            printf("ID: %d\n", cur->data.writer_id);
            printf("Surname: %s\n", cur->data.surname);
            printf("Name: %s\n", cur->data.name);

            /* metrame posa vivlia exei apo writes */
            int books_found = 0;
            writes_node *wcur = writes_head;
            while (wcur != NULL) {
                if (wcur->data.writer_id == writer_id)
                    books_found++;
                wcur = wcur->next;
            }

            printf("Number of books: %d\n", books_found);
            printf("Books of this writer:\n");

            if (books_found == 0) {
                printf("  (No books recorded for this writer)\n");
            } else {
                /* ksanadiatrexoume to writes gia na emfanisoume tis plirofories kathe vivliou */
                wcur = writes_head;
                while (wcur != NULL) {
                    if (wcur->data.writer_id == writer_id) {
                        const char *title = wcur->data.title;
                        book_node *bcur = find_book_by_title(title); // Vriskoume ta pliri stoixeia tou vivliou
                        if (bcur != NULL) {
                            printf("  - Title: %s\n", bcur->data.title);
                            printf("    Release date: %d\n", bcur->data.release_date);
                            printf("    Price: %.2f\n", bcur->data.price);
                        }
                    }
                    wcur = wcur->next;
                }
            }

            found = 1; // De synexizoume anazhthshs meta tin prwth evresh (anarotikos tropo)
        }
        cur = cur->next;
    }

    if (!found)
        printf("No writer with surname '%s' found.\n", buffer);
}

/* SEARCH BOOK */

// Anazhthsh vivliou me vasi ton title
void search_book(void)
{
    char buffer[256];

    if (book_head == NULL) {
        printf("No books in the system.\n");
        return;
    }

    printf("Give book title to search: ");
    while (getchar() != '\n'); // Καθαρισμός buffer
    read_line(buffer, sizeof(buffer));

    book_node *bcur = find_book_by_title(buffer);
    if (bcur == NULL) {
        printf("No book with title '%s' found.\n", buffer);
        return;
    }

    printf("\n--- Book found ---\n");
    printf("Title: %s\n", bcur->data.title);
    printf("Release Date: %d\n", bcur->data.release_date);
    printf("Price: %.2f\n", bcur->data.price);

    /* vres tous suggrafeis tou vivliou apo to writes */
    printf("\n--- Writers of this book ---\n");
    int authors_found = 0;
    writes_node *wcur = writes_head;

    while (wcur != NULL) {
        if (strcmp(wcur->data.title, buffer) == 0) {
            int writer_id = wcur->data.writer_id;
            author_node *acur = find_author_by_id(writer_id); // Vriskoume ta pliri stoixeia tou suggrafea
            if (acur != NULL) {
                printf("ID: %d \n Surname: %s \n Name: %s\n",
                       acur->data.writer_id,
                       acur->data.surname,
                       acur->data.name);
                authors_found = 1;
            }
        }
        wcur = wcur->next;
    }

    if (!authors_found)
        printf("No authors linked to this book.\n");
}

/* DELETE BOOK (Helper/Internal) */

// Diagrafei OLES tis eggrafes writes gia enan titlo, xwris na enimerwnei tous authors
void delete_all_writes_for_title(const char *title)
{
    writes_node *cur = writes_head;
    writes_node *prev = NULL;

    while (cur != NULL) {
        if (strcmp(cur->data.title, title) == 0) {
            writes_node *todel = cur;
            if (prev == NULL) {
                writes_head = cur->next;
                cur = writes_head;
            } else {
                prev->next = cur->next;
                cur = cur->next;
            }
            free(todel->data.title);
            free(todel);
            writes_count--;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

// Diagrafh vivliou: Diagrafei writes, enimerwnei num_of_books, diagrafei to book
void delete_book_by_title_internal(const char *title)
{
    /* 1) diagrafoume oles tis eggrafes writes gia auto to vivlio
       kai meionoume num_of_books twn siggrafevn */
    writes_node *cur = writes_head;
    writes_node *prev = NULL;

    while (cur != NULL) {
        if (strcmp(cur->data.title, title) == 0) {
            /* meionoume num_of_books tou siggrafea */
            author_node *a = find_author_by_id(cur->data.writer_id);
            if (a != NULL && a->data.num_of_books > 0)
                a->data.num_of_books--;

            writes_node *todel = cur;
            if (prev == NULL) {
                writes_head = cur->next;
                cur = writes_head; // Proxorame sto epomeno
            } else {
                prev->next = cur->next;
                cur = cur->next; // Proxorame sto epomeno
            }
            free(todel->data.title);
            free(todel);
            writes_count--;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }

    /* 2) diagrafoume to vivlio apo tin lista vivliwn */
    book_node *bcur = book_head;
    book_node *bprev = NULL;

    while (bcur != NULL) {
        if (strcmp(bcur->data.title, title) == 0) {
            if (bprev == NULL) {
                book_head = bcur->next;
            } else {
                bprev->next = bcur->next;
            }
            free(bcur->data.title);
            free(bcur);
            book_count--;
            printf("Book '%s' deleted.\n", title);
            return;
        }
        bprev = bcur;
        bcur = bcur->next;
    }
}

// Kentriki synartisi diagrafis vivliou
void delete_book(void)
{
    char buffer[256];

    if (book_head == NULL) {
        printf("No books in the system.\n");
        return;
    }

    printf("Give book title to search: ");
    while (getchar() != '\n'); // clear buffer
    read_line(buffer, sizeof(buffer));

    book_node *bcur = find_book_by_title(buffer);
    if (bcur == NULL) {
        printf("No book with title '%s' found.\n", buffer);
        return;
    }

    delete_book_by_title_internal(buffer);
}

/* DELETE WRITES (helper for delete_writer) */

// Diagrafei mia sygkekrimeni eggrafi syndesis (writes) me title kai writer_id
void delete_writes_for_title_and_writer(const char *title, int writer_id)
{
    writes_node *cur = writes_head;
    writes_node *prev = NULL;

    while (cur != NULL) {
        if (cur->data.writer_id == writer_id &&
            strcmp(cur->data.title, title) == 0)
        {
            writes_node *todel = cur;
            if (prev == NULL) {
                writes_head = cur->next;
                cur = writes_head;
            } else {
                prev->next = cur->next;
                cur = cur->next;
            }
            free(todel->data.title);
            free(todel);
            writes_count--;
            return; // Vrethike kai diagrafike, synexizoume sthn delete_writer
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

/* DELETE WRITER (ME MONOGRAFIAS) */

// Diagrafh suggrafea
void delete_writer(void)
{
    int id;

    if (author_head == NULL) {
        printf("No writers in the system.\n");
        return;
    }

    printf("Give writer id to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        return;
    }

    author_node *cur = author_head;
    author_node *prev = NULL;

    while (cur != NULL && cur->data.writer_id != id) {
        prev = cur;
        cur = cur->next;
    }

    if (cur == NULL) {
        printf("No writer with id %d found.\n", id);
        return;
    }

    int writer_id = cur->data.writer_id;

    /* 1) mazeyoume olous tous monadikous titlous pou exei grapseei autos o siggrafeas */
    char **titles = NULL;
    int titles_count = 0;
    writes_node *wcur = writes_head;

    while (wcur != NULL) {
        if (wcur->data.writer_id == writer_id) {
            const char *t = wcur->data.title;
            int already_in_list = 0;
            int i;
            
            // Elegxoume an o titlos yparxei idi sto titles[]
            for (i = 0; i < titles_count; i++) {
                if (strcmp(titles[i], t) == 0) {
                    already_in_list = 1;
                    break;
                }
            }
            
            if (!already_in_list) {
                // Prosthetoume ton neo titlo
                char **new_titles = (char **)realloc(titles, (titles_count + 1) * sizeof(char *));
                if (new_titles == NULL) {
                    printf("Memory error while collecting titles.\n");
                    break;
                }
                titles = new_titles;
                titles[titles_count] = copy_string(t);  /* antigrafoume to string gia asfaleia */
                if (titles[titles_count] == NULL) {
                    printf("Memory error while copying title.\n");
                    break;
                }
                titles_count++;
            }
        }
        wcur = wcur->next;
    }

    /* 2) gia kathe titlo elegxoume an to vivlio einai monografia i oxi */
    int t;
    for (t = 0; t < titles_count; t++) {
        const char *title = titles[t];
        int total_w = count_writes_for_title(title); // Synolikos arithmos suggrafevn gia auto to vivlio

        if (total_w == 1) {
            /* monografia: diagrafoume to vivlio kai tin eggrafi apo writes */
            delete_book_by_title_internal(title); // Diagrafei to vivlio, writes kai enimerwnei num_of_books (to num_of_books tha ginei 0)
        } else if (total_w > 1) {
            /* syggrafeas me allous: diagrafoume MONO tin eggrafi gia auton ton writer_id */
            delete_writes_for_title_and_writer(title, writer_id);
            // Enimerwsi num_of_books tou author (cur->data.num_of_books--) tha ginei sto vima 3
            if (cur->data.num_of_books > 0)
                cur->data.num_of_books--;
        }
    }

    /* 3) diagrafoume ton siggrafea apo ti lista author_head */
    if (prev == NULL) {
        author_head = cur->next;
    } else {
        prev->next = cur->next;
    }
    free(cur->data.surname);
    free(cur->data.name);
    free(cur);
    author_count--;

    printf("Writer with id %d deleted.\n", id);
    printf("All related writes and monograph books for writer %d updated.\n", id);

    /* 4) eleftherwnoume tous provor. titlous (realloc array) */
    if (titles != NULL) {
        for (t = 0; t < titles_count; t++)
            free(titles[t]);
        free(titles);
    }
}

void read_line(char *buf, int size) {
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    // afairesi tou xaraktira allagis grammis '\n' pou prosthetei i fgets
    buf[strcspn(buf, "\n")] = '\0';
}
