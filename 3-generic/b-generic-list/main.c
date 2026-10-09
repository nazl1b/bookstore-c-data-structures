#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "erwthma3.b.h"


int main(void)
{
    int choice;
    initialize_lists(); // Arxikopoiisi Listes me Compare/Free functions
    
    // Fortwsi dedomenwn
    load_authors_from_file();
    load_books_from_file(); 
    load_writes_from_file();
    
    update_author_book_counts(); // Epanaupologismos tou num_of_books twn authors

    do {
        print_menu();
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clear_input_buffer(); 
            continue;
        }

        switch (choice) {
            case 1: insert_new_writer();
            		save_authors_to_file();
					break;
            case 2: insert_new_book();
					save_data();
					break;
            case 3: search_writer();
					break;
            case 4: search_book();
					break;
            case 5: delete_writer();
					save_data();
					break;
            case 6: delete_book();
					save_data();
					break;
            case 7: save_data(); printf("Exiting...\n"); break;
            default: printf("Invalid choice.\n");
        }

    } while (choice != 7);

    // Apeleftherwsi olis tis desmeymenis mnimis kata tin eksodo
    generic_free_list(&author_list);
    generic_free_list(&book_list);
    generic_free_list(&writes_list);

    return 0;
}