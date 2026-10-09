# 📚 Bookstore Management System in C

A menu-based bookstore system in C for managing books and authors, saved in text files.
The same program is implemented **4 times**, each with a different data structure.
Team project for the Advanced Programming course at the University of Thessaly.

## Features
- Add, search and delete authors and books
- A book can have many authors, and an author can have many books
- When an author is deleted, books written only by them are also deleted
- Data is loaded from and saved to text files

## Implementations
| Folder | Data structure |
|---|---|
| `1-dynamic-arrays` | Dynamic arrays (`malloc`/`realloc`) with binary search |
| `2-linked-lists` | Sorted singly linked lists |
| `3-generic/a-generic-vector` | Generic dynamic array using `void*` and function pointers |
| `3-generic/b-generic-list` | Generic linked list using `void*` and function pointers |

## Tech
C · Dynamic memory · Pointers · Linked lists · Function pointers

## How to run
Go into a folder and compile, for example:
```bash
cd 1-dynamic-arrays
gcc main.c erwthma1.c -o bookstore
./bookstore
```
