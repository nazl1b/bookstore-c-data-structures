#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erwthma2.h"



int main(void)
{
    int choice;

    // Fortwsi dedomenwn apo ta arxeia
    load_authors_from_file();
    load_books_from_file();
    load_writes_from_file();


	// klisi gia enimerosi tou pediou num_of_books se olous tous authors, vasismeno sti lista writes
	update_author_book_counts();


    do {
        print_menu();
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            // katharismos tou input buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }

        switch (choice) {
            case 1:
                insert_new_writer();
                save_authors_to_file(); // Apothikeusi authors meta tin eisagwgi
                break;
            case 2:
                insert_new_book();
                save_books_to_file();   // Apothikeusi books
                save_writes_to_file();  // Apothikeusi writes
                save_authors_to_file(); // Apothikeusi authors (giati enimerwthike to num_of_books h mporei na dimiourgithike neos author)
                break;
            case 3:
                search_writer();
                break;
            case 4:
                search_book();
                break;
            case 5:
                delete_writer();
                save_authors_to_file(); // Apothikeusi author (diagrafh author)
                save_books_to_file();   // Apothikeusi books (mporei na diagrafikan vivlia-monografies)
                save_writes_to_file();  // Apothikeusi writes (diagrafh sxetikwn eggrafwn)
                break;
            case 6:
                delete_book();
                save_books_to_file();   // Apothikeusi books (diagrafh vivliou)
                save_writes_to_file();  // Apothikeusi writes (diagrafh sxetikwn eggrafwn)
                save_authors_to_file(); // Apothikeusi authors (enimerwsi tou num_of_books)
                break;
            case 7:
                save_authors_to_file(); // Apothikeusi olwn kata tin eksodo
                save_books_to_file();
                save_writes_to_file();
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 7);

    return 0;
}
