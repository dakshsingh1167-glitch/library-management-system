# Library Management System (C)

Capstone project for **Programming for Problem Solving Using C (CSE-101)**.

## Student Details

| Field   | Details     |
|---------|-------------|
| Name    | Daksh Singh |
| Roll No | 29072       |
| Branch  | CSE         |
| Section | 2           |
| Subject | PPS (CSE-101) |

## About

A menu-driven console application written only in C that helps a library manage its books and track which student has issued which book. All data is saved in files, so it is still there the next time the program runs.

## Features

- Add, display, search, update, and delete books
- Issue a book to a student (by roll number) and return it
- View all currently issued books with issue dates
- Late fine calculation (books are due after 14 days, Rs. 2 per extra day)
- Input validation and persistent storage using binary files

## C Concepts Used

| Syllabus Topic | Where it is used |
|---|---|
| Structures | `Book` and `Issue` records |
| Arrays and Strings | Titles, authors, and names; case-insensitive search |
| Pointers | `Book *findBook()`, passing records by pointer |
| Dynamic Memory Allocation | `malloc`, `realloc`, `free` for the book and issue lists |
| Functions | One function per feature |
| Loops and Conditionals | Main menu loop with `switch-case` |
| File Handling | `fopen`, `fread`, `fwrite`, `fclose` for `books.dat` and `issues.dat` |
| Header Files | `stdio.h`, `stdlib.h`, `string.h`, `ctype.h`, `time.h` |

## How to Compile and Run

```bash
gcc library_management.c -o library
./library
```

On Windows (MinGW):

```bash
gcc library_management.c -o library.exe
library.exe
```

## Files

- `library_management.c`: complete source code
- `books.dat` and `issues.dat`: created automatically when you run the program
