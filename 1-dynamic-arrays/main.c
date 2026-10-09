#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erwthma1.h"


int main(void) 
{
    int choice;
    
    load_authors_from_file();   // PROTA FORTONUME APO TO ARXEIO
	load_books_from_file();
    load_writes_from_file();
    recompute_num_of_books();
	
    do 
	{
        print_menu();
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) 
		{
            case 1:
                insert_new_writer();
                save_authors_to_file();
                break;
            case 2:
                insert_new_book();
                save_books_to_file();
                save_writes_to_file();
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

    return 0;
}
