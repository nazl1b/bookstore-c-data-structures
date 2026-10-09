#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erwthma3.a.h"


int main(void) {
    int choice;

    /* Arxikopoiisi ton dinamikon pinakon */
    gvec_init(&books, sizeof(book));
    gvec_init(&authors, sizeof(author));
    gvec_init(&writesV, sizeof(writes));


	/* Fortosi dedomenon apo arxeia kata tin ekkinisi */
	load_authors_from_file();
    printf("Loaded %d authors from authors.txt\n", authors.size);

    load_books_from_file();
    printf("Loaded %d books from books.txt\n", books.size);

    load_writes_from_file();
    printf("Loaded %d entries from writes.txt\n", writesV.size);

    recompute_num_of_books();
    printf("Author book counts updated based on writes list.\n");

    do {
        print_menu();
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input\n");
            clear_stdin_line();
            continue;
        }

        switch (choice) {
            case 1:
                insert_new_writer();
                save_authors_to_file();
                break;
            case 2:
                insert_new_book();
                save_books_to_file();
                save_writes_to_file();
                save_authors_to_file();
                break;
            case 3:
                search_writer();
                break;
            case 4:
                search_book();
                break;
            case 5:
                delete_writer();
                save_authors_to_file();
                save_writes_to_file();
                save_books_to_file();
                break;
            case 6:
                delete_book();
                save_books_to_file();
                save_writes_to_file();
                save_authors_to_file();
                break;
            case 7:
                save_authors_to_file();
                save_books_to_file();
                save_writes_to_file();
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice\n");
        }
    } while (choice != 7);

    /* Eleutherosi mnimis prin ton termatismo */
    gvec_free(&writesV, free_writes_elem);
    gvec_free(&books, free_book_elem);
    gvec_free(&authors, free_author_elem);

    return 0;
}