#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erwthma1.h"


//dynamic arrays (arxika den exoun tipota)
book *book_arr = NULL;
author *author_arr = NULL;
writes *writes_arr = NULL;

//plithos eggrafon
int book_count = 0;
int author_count = 0;
int writes_count = 0;

// gia automato writer_id
int next_writer_id = 1;



//functions
void print_menu(void) 
{
    printf("\n---- BOOKSTORE MENU ----\n");
    printf("1. Insert new writer \n");
    printf("2. Insert new book \n");
    printf("3. Search a writer \n");
    printf("4. Search a book \n");
    printf("5. Delete a writer \n");
    printf("6. Delete a book \n");
    printf("7. Exit\n");
}

//desmeush mnimis (malloc) kai antigrafh string - epistrefei neo char*
char *copy_string(const char *s)
{
    char *p = malloc(strlen(s) + 1);
    if (p == NULL)
        return NULL;

    strcpy(p, s);
    return p;
}


void save_authors_to_file(void)
{
    FILE *fp = fopen("authors.txt", "w");   // anoigei to arxeio gia eggrafi (w = write)
    if (fp == NULL)
    {
        printf("Error opening authors.txt for writing.\n");
        return;
    }

    // to plithos
    fprintf(fp, "%d\n", author_count);

    // kathe suggrafea (3 grammes gia ton kathena)
    int i;
    for (i = 0; i < author_count; i++)
    {
        fprintf(fp, "%d\n", author_arr[i].writer_id);
        fprintf(fp, "%s\n", author_arr[i].surname);
        fprintf(fp, "%s\n", author_arr[i].name);
    }

    fclose(fp);  
    printf("Authors saved to authors.txt\n");
}


void load_authors_from_file(void)
{
    FILE *fp = fopen("authors.txt", "r");   // anoigei to arxeio gia diavasma (r = read)
    if (fp == NULL)
    {
        // An den uparxei to arxeio, arxizoume me kena dedomena
        printf("authors.txt not found, starting with empty authors list.\n");
        author_arr = NULL;
        author_count = 0;
        next_writer_id = 1;
        return;
    }

    // Diavazoume to plithos twn suggrafewn
    if (fscanf(fp, "%d", &author_count) != 1) // elegxos oti diavastike swsta o arithmos twn suggrafewn apo to arxeio
    {
        printf("Error reading author count from file.\n");
        fclose(fp);
        author_arr = NULL;
        author_count = 0;
        next_writer_id = 1;
        return;
    }

    if (author_count == 0)
    {
        // Kanenas suggrafeas
        author_arr = NULL;
        next_writer_id = 1;
        fclose(fp);
        return;
    }

    // Desmeuoume mnimi gia ton pinaka suggrafewn
    author_arr = malloc(author_count * sizeof(author));
    if (author_arr == NULL)
    {
        printf("Memory error allocating authors array.\n");
        fclose(fp);
        author_count = 0;
        next_writer_id = 1;
        return;
    }

    int i;
    int max_id = 0;
    char buffer[256];

    for (i = 0; i < author_count; i++)
    {
        int id;

        // Diavazoume to id
        if (fscanf(fp, "%d", &id) != 1)
        {
            printf("Error reading writer_id from file.\n");
            break;
        }
        author_arr[i].writer_id = id;
        if (id > max_id)
            max_id = id;

        // Diavazoume surname
        if (fscanf(fp, "%255s", buffer) != 1)
        {
            printf("Error reading surname from file.\n");
            break;
        }
        author_arr[i].surname = copy_string(buffer);

        // Diavazoume name
        if (fscanf(fp, "%255s", buffer) != 1)
        {
            printf("Error reading name from file.\n");
            break;
        }
        author_arr[i].name = copy_string(buffer);

        // Arxika 0 vivlia (meta tha to enimerwnw apo writes_arr)
        author_arr[i].num_of_books = 0;
    }

    fclose(fp);

    // To epomeno id tha einai ena parapanw apo to megisto pou diavasame
    next_writer_id = max_id + 1;
    
    // extra asfaleia: akoma ki an to authors.txt den einai sorted,
	// emeis to kanoume sorted by writer_id.
	if (author_count > 1)
	{
	    qsort(author_arr, author_count, sizeof(author), compare_authors_by_id);
	}

    printf("Loaded %d authors from authors.txt\n", author_count);
}


void search_writer(void)
{
    char buffer[256];
    int found = 0;
    int i;

    if (author_count == 0)
    {
        printf("No writers in the system.\n");
        return;
    }

    printf("Give writer surname to search: ");
    scanf("%255s", buffer);

    for (i = 0; i < author_count; i++)
    {
        if (strcmp(author_arr[i].surname, buffer) == 0)
        {
            printf("\n--- Writer found ---\n");
            printf("ID: %d\n", author_arr[i].writer_id);
            printf("Surname: %s\n", author_arr[i].surname);
            printf("Name: %s\n", author_arr[i].name);

        
            printf("Number of books: %d\n", author_arr[i].num_of_books);

            printf("Books of this writer:\n");

            int writer_id = author_arr[i].writer_id;
            int printed_any = 0;
            int j, k;

            
            for (j = 0; j < writes_count; j++)
            {
                if (writes_arr[j].writer_id == writer_id)
                {
                    char *title = writes_arr[j].title;

                    
                    for (k = 0; k < book_count; k++)
                    {
                        if (strcmp(book_arr[k].title, title) == 0)
                        {
                            printf("    Title: %s\n", book_arr[k].title);
                            printf("    Release date: %d\n", book_arr[k].release_date);
                            printf("    Price: %.2f\n", book_arr[k].price);
                            printed_any = 1;
                            break;
                        }
                    }
                }
            }

            if (!printed_any)
            {
                printf("  (No books recorded for this writer)\n");
            }

            found = 1;
        }
    }

    if (!found)
    {
        printf("No writer with surname '%s' found.\n", buffer);
    }
}



//binary search
int find_author_index_by_id(int id)
{
    int low = 0;
    int high = author_count - 1;

    while (low <= high)
    {
        int mid = (low + high) / 2;

        if (author_arr[mid].writer_id == id)
            return mid;                  // vrethike

        if (author_arr[mid].writer_id < id)
            low = mid + 1;               // anazitisi sto deksia miso
        else
            high = mid - 1;              // anazitisi sto aristero miso
    }

    return -1; // den vrethike
}


// diagrafei enan suggrafea apo ton pinaka author_arr me vasi to id pou dinei o xristis.
void delete_writer(void)
{
    int id;
    int index;
    int i;

    if (author_count == 0)
    {
        printf("No writers in the system.\n");
        return;
    }

    printf("Give writer id to delete: ");
    scanf("%d", &id);

    // vriskoume se poia thesi einai autos o suggrafeas (binary search)
    index = find_author_index_by_id(id);

    if (index == -1)
    {
        printf("No writer with id %d found.\n", id);
        return;
    }

    // kratame to id tou suggrafea pou tha diagrafei
    int writer_id = author_arr[index].writer_id;

    // eleftheronoume tin mnimi pou eixame desmeusei gia surname kai name
    free(author_arr[index].surname);
    free(author_arr[index].name);

    // metakinoume ola ta stoixeia meta apo afton mia thesi aristera
    for (i = index; i < author_count - 1; i++)
    {
        author_arr[i] = author_arr[i + 1];
    }

    author_count--;

    // mikrainoume ton pinaka me realloc
    if (author_count == 0)
    {
        free(author_arr);
        author_arr = NULL;
    }
    else
    {
        author *temp = realloc(author_arr, author_count * sizeof(author));
        if (temp != NULL)
        {
            author_arr = temp;
        }
    }

    printf("Writer with id %d deleted.\n", id);

    //    Prepei na broume ola ta vivlia pou eixe grapsei autos o siggrafeas.

    // provisorinos pinakas me tous titlous pou exei o siggrafeas
    char **titles = NULL;
    int titles_count = 0;
    
    //mazewoume olous tous titlous pou exoun auto to writer_id sto writes_arr
    for (i = 0; i < writes_count; i++)
    {
        if (writes_arr[i].writer_id == writer_id)
        {
            char *title = writes_arr[i].title;
            int already_in_list = 0;
            int t;

            // elegxoume an exoume idi valei auto ton titlo ston pinaka titles
            for (t = 0; t < titles_count; t++)
            {
                if (strcmp(titles[t], title) == 0)
                {
                    already_in_list = 1;
                    break;
                }
            }

            if (!already_in_list)
            {
                char **temp_titles = realloc(titles, (titles_count + 1) * sizeof(char *));
                if (temp_titles == NULL)
                {
                    printf("Memory error while collecting titles.\n");
                    // den stamataw, apla den prosthetw alles eggrafes
                    break;
                }
                titles = temp_titles;
                
                //titles[titles_count] = title;  // kratame ton pointer (den antigrafoume string)
                titles[titles_count] = copy_string(title);  // antigrafoume to string
				if (titles[titles_count] == NULL)
				{
				    printf("Memory error while copying title.\n");
				}
                
                titles_count++;
            }
        }
    }
    
    // gia kathe titlo pou eixe autos o siggrafeas
    int t;
    for (t = 0; t < titles_count; t++)
    {
        const char *title = titles[t];

        // posa writes yparxoun gia auto to vivlio (gia olous tous suggrrafeis)
        int total_writes = count_writes_for_title(title);

        if (total_writes == 1)
        {
            // mono 1 eggrafi sto writes_arr, ara to vivlio einai monografia
            // -> diagrafoyme to vivlio apo to book_arr (kanei kai diagrafi writes gia auton ton titlo)
            int book_index = find_book_index_by_title(title);
            if (book_index != -1)
            {
                // xrisimopoioume tin delete_book 
                // !!!  i delete_book perimenei na dwsei o xristis titlo

                int i, j;

                // diagrafoume apo writes_arr oles tis eggrafes me auto ton titlo
                for (i = 0; i < writes_count; )
                {
                    if (strcmp(writes_arr[i].title, title) == 0)
                    {
                        free(writes_arr[i].title);

                        for (j = i; j < writes_count - 1; j++)
                        {
                            writes_arr[j] = writes_arr[j + 1];
						}
                        writes_count--;

                        if (writes_count == 0)
                        {
                            free(writes_arr);
                            writes_arr = NULL;
                            break;
                        }
                        else
                        {
                            writes *temp_w = realloc(writes_arr, writes_count * sizeof(writes));
                            if (temp_w != NULL)
                            {
                                writes_arr = temp_w;
                            }
                        }
                    }
                    else
                    {
                        i++;
                    }
                }

                // diagrafoume to vivlio apo book_arr
                free(book_arr[book_index].title);

                for (i = book_index; i < book_count - 1; i++)
                {
                    book_arr[i] = book_arr[i + 1];
				}
                book_count--;

                if (book_count == 0)
                {
                    free(book_arr);
                    book_arr = NULL;
                }
                else
                {
                    book *temp_b = realloc(book_arr, book_count * sizeof(book));
                    if (temp_b != NULL)
                    {
                        book_arr = temp_b;
                    }
                }

                printf("Monograph book '%s' deleted.\n", title);
            }
        }
        else
        {
            // den einai monografia (yparxoun ki alloi suggrafeis gia auto to vivlio)
            // diagrafoyme mono tin eggrafi tou siggrafea sto writes_arr
            delete_writes_for_title_and_writer(title, writer_id);
        }
    }

    // eleftheronoume ti mnimi pou desmeusame gia tous titlous
	if (titles != NULL)
	{
	    int t;
	    for (t = 0; t < titles_count; t++)
	    {
	        free(titles[t]);   // kathe ena string pou antigrapsame me copy_string
	    }
	    free(titles);          // kai meta ton pinaka deiktwn
	}
	
    printf("All related writes and monograph books for writer %d updated.\n", id);
}

// metraei poses eggrafes yparxoun sto writes_arr gia sugkekrimeno title
int count_writes_for_title(const char *title)
{
    int i;
    int count = 0;

    for (i = 0; i < writes_count; i++)
    {
        if (strcmp(writes_arr[i].title, title) == 0)
            count++;
    }

    return count;
}

// diagrafh eggrafwn apo to writes_arr gia sugkekrimeno title KAI sugkekrimeno writer_id
void delete_writes_for_title_and_writer(const char *title, int writer_id)
{
    int i, j;

    for (i = 0; i < writes_count; )
    {
        if (writes_arr[i].writer_id == writer_id &&
            strcmp(writes_arr[i].title, title) == 0)
        {
            // eleftherwnoume mnimi gia ton titlo
            free(writes_arr[i].title);

            // metakinoume ta stoixeia mia thesi aristera
            for (j = i; j < writes_count - 1; j++)
            {
                writes_arr[j] = writes_arr[j + 1];
            }

            writes_count--;

            if (writes_count == 0)
            {
                free(writes_arr);
                writes_arr = NULL;
                break;
            }
            else
            {
                writes *temp = realloc(writes_arr, writes_count * sizeof(writes));
                if (temp != NULL)
                    writes_arr = temp;
            }
            // den auksanoume to i, giati meta tin metakinisi exoume neo stoixeio sti thesi i
        }
        else
        {
            i++;
        }
    }
}

void load_books_from_file(void)
{
    FILE *fp = fopen("books.txt", "r");

    if (fp == NULL)
    {
        printf("books.txt not found, starting with empty books list.\n");
        book_arr = NULL;
        book_count = 0;
        return;
    }

    //  read count
    if (fscanf(fp, "%d", &book_count) != 1)
    {
        printf("Error reading book count from file.\n");
        fclose(fp);
        book_arr = NULL;
        book_count = 0;
        return;
    }

    if (book_count == 0)
    {
        book_arr = NULL;
        fclose(fp);
        return;
    }

    book_arr = malloc(book_count * sizeof(book));
    if (book_arr == NULL)
    {
        printf("Memory error allocating books array.\n");
        fclose(fp);
        book_count = 0;
        return;
    }


    {
        int i;
        char line[256];

        // consume leftover '\n' after fscanf count
        fgets(line, sizeof(line), fp);

        for (i = 0; i < book_count; i++)
        {
            // 1) Title as full line (can contain spaces)
            if (fgets(line, sizeof(line), fp) == NULL)
            {
                printf("Error reading title for book %d from file.\n", i + 1);
                break;
            }
            line[strcspn(line, "\n")] = 0;

            book_arr[i].title = copy_string(line);
            if (book_arr[i].title == NULL)
            {
                printf("Memory error copying title.\n");
                break;
            }

            // 2) release year
            if (fscanf(fp, "%d", &book_arr[i].release_date) != 1)
            {
                printf("Error reading release date for book %s from file.\n", book_arr[i].title);
                break;
            }

            // 3) price
            if (fscanf(fp, "%f", &book_arr[i].price) != 1)
            {
                printf("Error reading price for book %s from file.\n", book_arr[i].title);
                break;
            }

            // consume '\n' after reading numbers, so next fgets works
            fgets(line, sizeof(line), fp);
        }

        // if break happened early, fix count
        if (i < book_count)
        {
            book_count = i;
        }
    }

    fclose(fp);

    if (book_count > 1)
    {
        qsort(book_arr, book_count, sizeof(book), compare_books_by_title);
    }

    printf("Loaded %d books from books.txt\n", book_count);
}



void search_book(void)
{
    char buffer[256];
    int j;

    if (book_count == 0)
    {
        printf("No books in the system.\n");
        return;
    }

    printf("Give book title to search: ");
	while (getchar() != '\n') { }
	read_line(buffer, sizeof(buffer));


    // 1) Binary search sto book_arr (giati einai sorted by title)
    int idx = find_book_index_by_title(buffer);  // <-- ayti pleon einai BINARY
    if (idx == -1)
    {
        printf("No book with title '%s' found.\n", buffer);
        return;
    }

    // 2) Emfanisi stoixeiwn vivliou
    printf("\n--- Book found ---\n");
    printf("Title: %s\n", book_arr[idx].title);
    printf("Release Date: %d\n", book_arr[idx].release_date);
    printf("Price: %.2f EUR\n", book_arr[idx].price);

    // 3) Emfanisi suggrafeon tou vivliou mesw writes_arr
    printf("\n--- Writers of this book ---\n");
    {
        int authors_listed = 0;

        for (j = 0; j < writes_count; j++)
        {
            if (strcmp(writes_arr[j].title, buffer) == 0)
            {
                int writer_id = writes_arr[j].writer_id;
                author *a = find_author_by_id_manual(writer_id);

                if (a != NULL)
                {
                    printf(" ID: %d \n Surname: %s \n Name: %s\n",
                           a->writer_id, a->surname, a->name);
                    authors_listed = 1;
                }
            }
        }

        if (!authors_listed)
        {
            printf("No authors linked to this book.\n");
        }
    }
}



void delete_book(void)
{
    char title_buffer[256];
    int index;
    int i, j;

    if (book_count == 0)
    {
        printf("No books in the system.\n");
        return;
    }

    printf("Give book title to delete: ");
	while (getchar() != '\n') { }
	read_line(title_buffer, sizeof(title_buffer));


    // 1) Binary search (afou to find_book_index_by_title einai pleon binary)
    index = find_book_index_by_title(title_buffer);

    if (index == -1)
    {
        printf("No book with title '%s' found.\n", title_buffer);
        return;
    }

    // 2) Diagrafi eggrafwn apo writes_arr gia auto to title + update num_of_books
    for (i = 0; i < writes_count; )
    {
        if (strcmp(writes_arr[i].title, title_buffer) == 0)
        {
            int writer_id = writes_arr[i].writer_id;

            // find author & update counter (proairetiko)
            author *a = find_author_by_id_manual(writer_id);
            if (a != NULL && a->num_of_books > 0)
            {
                a->num_of_books--;
            }

            // free title string sto writes entry
            free(writes_arr[i].title);

            // shift left
            for (j = i; j < writes_count - 1; j++)
                writes_arr[j] = writes_arr[j + 1];

            writes_count--;

            // (den kanoume i++ giati efyge stoixeio kai irthe neo sti thesi i)
        }
        else
        {
            i++;
        }
    }

    // 3) mikrainoume ton writes_arr me realloc
    if (writes_count == 0)
    {
        free(writes_arr);
        writes_arr = NULL;
    }
    else
    {
        writes *tempw = realloc(writes_arr, writes_count * sizeof(writes));
        if (tempw != NULL)
            writes_arr = tempw;
    }

    // 4) Diagrafi vivliou apo book_arr
    free(book_arr[index].title);

    for (i = index; i < book_count - 1; i++)
        book_arr[i] = book_arr[i + 1];

    book_count--;

    // 5) realloc book_arr
    if (book_count == 0)
    {
        free(book_arr);
        book_arr = NULL;
    }
    else
    {
        book *tempb = realloc(book_arr, book_count * sizeof(book));
        if (tempb != NULL)
            book_arr = tempb;
    }

    printf("Book with title '%s' deleted.\n", title_buffer);
}



// Voithitiki synartisi gia eisagwgi eggrafis ston writes_arr
// Krataei ton writes_arr taksinomimeno: 1) writer_id, 2) title
// KAI den epitrepsei duplicates (idio title + idio writer_id).
void insert_write_entry(const char *book_title, int writer_id)
{
    writes new_write;
    writes *temp;
    int i;
    int insert_idx;

    // 0) Anti-duplicate: an yparxei idi, min to xana-vale??
    if (write_exists(book_title, writer_id))
    {
        return;
    }

    // 1) Dimiourgia tis neas eggrafis
    new_write.writer_id = writer_id;
    new_write.title = copy_string(book_title);
    if (new_write.title == NULL)
    {
        printf("Memory error for write title.\n");
        return;
    }

    // 2) Megalwnoume ton pinaka me realloc
    temp = realloc(writes_arr, (writes_count + 1) * sizeof(writes));
    if (temp == NULL)
    {
        printf("Memory error in realloc for writes_arr.\n");
        free(new_write.title);
        return;
    }
    writes_arr = temp;

    // 3) Vrisoume swsti thesi: prwta writer_id, meta title
    insert_idx = writes_count; // default sto telos
    for (i = 0; i < writes_count; i++)
    {
        if (new_write.writer_id < writes_arr[i].writer_id)
        {
            insert_idx = i;
            break;
        }
        else if (new_write.writer_id == writes_arr[i].writer_id)
        {
            if (strcmp(new_write.title, writes_arr[i].title) < 0)
            {
                insert_idx = i;
                break;
            }
        }
    }

    // 4) Metatopisi pros ta deksia
    for (i = writes_count; i > insert_idx; i--)
    {
        writes_arr[i] = writes_arr[i - 1];
    }

    // 5) Eisagogi
    writes_arr[insert_idx] = new_write;
    writes_count++;
}


//metraei posoi author exoun auto to surname
int find_author_count_by_surname(const char *surname)
{
    int i, c = 0;
    for (i = 0; i < author_count; i++)
    {
        if (strcmp(author_arr[i].surname, surname) == 0)
            c++;
    }
    return c;
}

/*
dinei writer_id gia surname:
an den yparxei -> zitaei name kai dimiourgei neo author

an yparxei 1 -> epistrefei auto

an yparxoun pano apo 1 -> emfanizei kai se vazei na dialekseis me writer_id (gia na min einai lathos)
*/

int find_author_id_by_surname_unique_or_choose(const char *surname)
{
    int i;
    int count = find_author_count_by_surname(surname);

    if (count == 0)
    {
        // den yparxei: dimiourgoume neo suggrafea (zitaei kai name)
        char name[256];
        printf("Author with surname '%s' not found.\n", surname);
        printf("Give name for new author '%s': ", surname);
        scanf("%255s", name);

        // dimiourgia me insert_author_sorted (Bhma 1)
        return insert_author_sorted(surname, name); // epistrefei new writer_id (h -1 se error)
    }

    if (count == 1)
    {
        // yparxei monadika: vrese kai gyrna id
        for (i = 0; i < author_count; i++)
        {
            if (strcmp(author_arr[i].surname, surname) == 0)
                return author_arr[i].writer_id;
        }
    }

    // pano apo 1 me idio surname: zitaepilog i me id
    printf("Found multiple authors with surname '%s':\n", surname);
    for (i = 0; i < author_count; i++)
    {
        if (strcmp(author_arr[i].surname, surname) == 0)
        {
            printf("  ID: %d | %s %s\n",
                   author_arr[i].writer_id,
                   author_arr[i].surname,
                   author_arr[i].name);
        }
    }

    printf("Give writer_id to use for surname '%s': ", surname);
    {
        int chosen_id;
        scanf("%d", &chosen_id);
        // elegxos oti yparxei
        if (find_author_index_by_id(chosen_id) == -1)
        {
            printf("Invalid writer_id. Skipping this author.\n");
            return -1;
        }
        return chosen_id;
    }
}


void save_writes_to_file(void)
{
    FILE *fp = fopen("writes.txt", "w");    // anoigei to arxeio gia eggrafi (w = write)
    if (fp == NULL)
    {
        printf("Error opening writes.txt for writing.\n");
        return;
    }

    // 1. Grafoume to plithos twn eggrafwn syndesis
    fprintf(fp, "%d\n", writes_count);

    // 2. Grafoume kathe eggrafi writes (titlos kai ID)
    int i;
    for (i = 0; i < writes_count; i++)
    {
        // Grafoume ton titlo tou vivliou
        fprintf(fp, "%s\n", writes_arr[i].title);
        
        // Grafoume ton kwdiko tou suggrafea
        fprintf(fp, "%d\n", writes_arr[i].writer_id);
    }

    fclose(fp);  
    printf("Write entries saved to writes.txt\n");
}

author *find_author_by_id_manual(int writer_id)
{
    int i;
    for (i = 0; i < author_count; i++)
    {
        if (author_arr[i].writer_id == writer_id)
            return &author_arr[i];
    }
    return NULL;
}


int find_book_index_by_title(const char *title)
{
    int low = 0;
    int high = book_count - 1;

    while (low <= high)
    {
        int mid = (low + high) / 2;
        int cmp = strcmp(book_arr[mid].title, title);

        if (cmp == 0)
            return mid;
        else if (cmp < 0)
            low = mid + 1;
        else
            high = mid - 1;
    }

    return -1;
}

int compare_books_by_title(const void *a, const void *b)
{
    const book *ba = (const book *)a;
    const book *bb = (const book *)b;
    return strcmp(ba->title, bb->title);
}

void load_writes_from_file(void)
{
    FILE *fp = fopen("writes.txt", "r");
    if (fp == NULL)
    {
        printf("writes.txt not found, starting with empty writes list.\n");
        writes_arr = NULL;
        writes_count = 0;
        return;
    }

    if (fscanf(fp, "%d", &writes_count) != 1)
    {
        printf("Error reading writes count from file.\n");
        fclose(fp);
        writes_arr = NULL;
        writes_count = 0;
        return;
    }

    if (writes_count == 0)
    {
        writes_arr = NULL;
        fclose(fp);
        return;
    }

    writes_arr = malloc(writes_count * sizeof(writes));
    if (writes_arr == NULL)
    {
        printf("Memory error allocating writes array.\n");
        fclose(fp);
        writes_arr = NULL;
        writes_count = 0;
        return;
    }


    {
        int i;
        char line[256];

        // consume leftover '\n' after fscanf(count)
        fgets(line, sizeof(line), fp);

        for (i = 0; i < writes_count; i++)
        {
            // title as full line (can contain spaces)
            if (fgets(line, sizeof(line), fp) == NULL)
            {
                printf("Error reading write title from file.\n");
                break;
            }
            line[strcspn(line, "\n")] = 0;

            writes_arr[i].title = copy_string(line);
            if (writes_arr[i].title == NULL)
            {
                printf("Memory error copying write title.\n");
                break;
            }

            // writer_id (next line)
            if (fscanf(fp, "%d", &writes_arr[i].writer_id) != 1)
            {
                printf("Error reading writer_id from writes file.\n");
                break;
            }

            // consume '\n' after the number
            fgets(line, sizeof(line), fp);
        }

        if (i < writes_count)
            writes_count = i;
    }

    fclose(fp);
    printf("Loaded %d writes from writes.txt\n", writes_count);
}


void recompute_num_of_books(void)
{
    int i, j;

    for (i = 0; i < author_count; i++)
        author_arr[i].num_of_books = 0;

    for (j = 0; j < writes_count; j++)
    {
        int idx = find_author_index_by_id(writes_arr[j].writer_id);
        if (idx != -1)
            author_arr[idx].num_of_books++;
    }
}



void save_books_to_file(void)
{
    FILE *fp = fopen("books.txt", "w");
    if (fp == NULL)
    {
        printf("Error opening books.txt for writing.\n");
        return;
    }

    // plithos vivliwn
    fprintf(fp, "%d\n", book_count);

    int i;
    for (i = 0; i < book_count; i++)
    {
        fprintf(fp, "%s\n", book_arr[i].title);
        fprintf(fp, "%d\n", book_arr[i].release_date);
        fprintf(fp, "%f\n", book_arr[i].price);
    }

    fclose(fp);
    printf("Books saved to books.txt\n");
}

void insert_new_book(void)
{
    book new_book;
    char buffer[256];
    int i, insert_idx;

    // 1) Title
    printf("Give book title: ");
	// clear leftover '\n' from previous scanf
	while (getchar() != '\n') { }
	read_line(buffer, sizeof(buffer));

    new_book.title = copy_string(buffer);
    if (new_book.title == NULL)
    {
        printf("Memory error for book title.\n");
        return;
    }

    // 2) Year (me elegxo)
    while (1)
    {
        printf("Give release year: ");
        if (scanf("%d", &new_book.release_date) != 1)
        {
            printf("Invalid input. Please give a number.\n");
            while (getchar() != '\n') { }
            continue;
        }
        if (new_book.release_date < 1500 || new_book.release_date > 2025)
        {
            printf("Year out of range. Try again.\n");
            continue;
        }
        break;
    }

    // 3) Price (me elegxo)
    while (1)
    {
        printf("Give price: ");
        if (scanf("%f", &new_book.price) != 1)
        {
            printf("Invalid input. Please give a number.\n");
            while (getchar() != '\n') { }
            continue;
        }
        if (new_book.price < 0.0f || new_book.price > 1000.0f)
        {
            printf("Price out of range. Try again.\n");
            continue;
        }
        break;
    }

    // 4) Insert sto book_arr (sorted by title)
    {
        book *temp = realloc(book_arr, (book_count + 1) * sizeof(book));
        if (temp == NULL)
        {
            printf("Memory error in realloc for book_arr.\n");
            free(new_book.title);
            return;
        }
        book_arr = temp;

        insert_idx = book_count;
        for (i = 0; i < book_count; i++)
        {
            if (strcmp(new_book.title, book_arr[i].title) < 0)
            {
                insert_idx = i;
                break;
            }
        }

        for (i = book_count; i > insert_idx; i--)
            book_arr[i] = book_arr[i - 1];

        book_arr[insert_idx] = new_book;
        book_count++;
    }

    printf("Book inserted.\n");

    // 5) Authors: posoi + surname (oxi writer_id)
    {
        int num_authors, a;
        char surname[256];

        printf("How many authors does this book have? ");
        scanf("%d", &num_authors);

        for (a = 0; a < num_authors; a++)
        {
            int writer_id;

            printf("Give surname of author %d: ", a + 1);
            scanf("%255s", surname);

            writer_id = find_author_id_by_surname_unique_or_choose(surname);
            if (writer_id == -1)
                continue;

            // MIA MONO eisagogi sto writes_arr (anti-duplicate)
            if (!write_exists(new_book.title, writer_id))
            {
                insert_write_entry(new_book.title, writer_id);

                // proairetiko: update num_of_books
                {
                    int idx = find_author_index_by_id(writer_id);
                    if (idx != -1)
                        author_arr[idx].num_of_books++;
                }
            }
        }
    }
}


// compare gia qsort
// H qsort einai generic, opote dinei "void *" (agnwstos typos).
// Emeis kanoume cast se (author *) gia na diavasoume writer_id.

int compare_authors_by_id(const void *a, const void *b)
{
    const author *aa = (const author *)a;
    const author *bb = (const author *)b;

    // pio "katharo" apo aa->id - bb->id
    if (aa->writer_id < bb->writer_id) return -1;
    if (aa->writer_id > bb->writer_id) return  1;
    return 0;
}


// insert author sorted
// Eisagei enan neo suggrafea ston author_arr
// kai krataei ton pinaka PANTA taksinomimeno by writer_id.
// Epistrefei to writer_id tou neou suggrafea, h -1 an fail.

int insert_author_sorted(const char *surname, const char *name)
{
    author new_author;

    // 1) Desmevoume mnimi gia surname/name (strings)
    new_author.surname = copy_string(surname);
    new_author.name    = copy_string(name);

    if (new_author.surname == NULL || new_author.name == NULL)
    {
        // an kati apetyxe, katharizoume oti desmeftike
        free(new_author.surname);
        free(new_author.name);
        return -1;
    }

    // 2) Auto assignment tou id (opws zitaei i ekfonisi)
    new_author.writer_id = next_writer_id++;
    new_author.num_of_books = 0;

    // 3) Megalwnoume ton pinaka me realloc +1
    author *temp = realloc(author_arr, (author_count + 1) * sizeof(author));
    if (temp == NULL)
    {
        free(new_author.surname);
        free(new_author.name);
        return -1;
    }
    author_arr = temp;

    // 4) Vrisoume se poia thesi prepei na mpei (gia na meinei sorted)
    int insert_idx = author_count; // default: sto telos
    int i;
    for (i = 0; i < author_count; i++)
    {
        if (new_author.writer_id < author_arr[i].writer_id)
        {
            insert_idx = i;
            break;
        }
    }

    // 5) Metakinoume ta stoixeia 1 thesi deksia gia na anoiksei "keno"
    for (i = author_count; i > insert_idx; i--)
    {
        author_arr[i] = author_arr[i - 1];
    }

    // 6) Bazoume ton neo suggrafea sti swsti thesi
    author_arr[insert_idx] = new_author;
    author_count++;

    return new_author.writer_id;
}


void insert_new_writer(void)
{
    char surname[256];
    char name[256];

    printf("Give writer surname: ");
    scanf("%255s", surname);

    printf("Give writer name: ");
    scanf("%255s", name);

    // xrisimopoioume tin sorted insert
    int new_id = insert_author_sorted(surname, name);
    if (new_id == -1)
    {
        printf("Memory error while inserting writer.\n");
        return;
    }

    printf("Writer inserted with id = %d\n", new_id);
}

int write_exists(const char *title, int writer_id)
{
    int i;
    for (i = 0; i < writes_count; i++)
    {
        if (writes_arr[i].writer_id == writer_id &&
            strcmp(writes_arr[i].title, title) == 0)
        {
            return 1;
        }
    }
    return 0;
}

//  read a full line (incl. spaces) and remove trailing '\n'
void read_line(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL)
    {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}



