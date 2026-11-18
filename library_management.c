/*
 * ===========================================================
 *  Project  : Library Management System
 *  Subject  : Programming for Problem Solving Using C (CSE-101)
 *  Name     : Daksh Singh
 *  Roll No  : 29072
 *  Branch   : CSE
 *  Section  : 2
 * ===========================================================
 *  Features : Add / View / Search / Update / Delete books,
 *             Issue and Return books, view issued books,
 *             late-fine calculation, data saved in files.
 *  Concepts : Structures, arrays, pointers, dynamic memory
 *             (malloc/realloc/free), functions, strings,
 *             loops, switch-case, file handling.
 * ===========================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define BOOK_FILE     "books.dat"
#define ISSUE_FILE    "issues.dat"
#define ISSUE_DAYS    14      /* days a book can be kept        */
#define FINE_PER_DAY  2       /* fine in Rs. per day after due  */

/* ---------- Structures ---------- */
typedef struct {
    int  id;
    char title[100];
    char author[60];
    int  total;       /* total copies          */
    int  available;   /* copies on the shelf   */
} Book;

typedef struct {
    int    bookId;
    int    rollNo;
    char   studentName[50];
    time_t issueDate;
} Issue;

/* ---------- Global dynamic arrays ---------- */
Book  *books  = NULL;
Issue *issues = NULL;
int bookCount  = 0;
int issueCount = 0;

/* ---------- Input helpers ---------- */
void readLine(const char *prompt, char *buf, int size)
{
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}

int readInt(const char *prompt)
{
    char line[50], *end;
    long val;
    while (1) {
        readLine(prompt, line, sizeof(line));
        val = strtol(line, &end, 10);
        if (end != line && *end == '\0' && val >= 0)
            return (int)val;
        printf("  Invalid input. Please enter a positive number.\n");
    }
}

int containsIgnoreCase(const char *text, const char *word)
{
    int i, j, n = strlen(text), m = strlen(word);
    if (m == 0) return 1;
    for (i = 0; i + m <= n; i++) {
        for (j = 0; j < m; j++)
            if (tolower((unsigned char)text[i + j]) != tolower((unsigned char)word[j]))
                break;
        if (j == m) return 1;
    }
    return 0;
}

/* ---------- File handling ---------- */
void loadData(void)
{
    FILE *fp = fopen(BOOK_FILE, "rb");
    if (fp != NULL) {
        if (fread(&bookCount, sizeof(int), 1, fp) == 1 && bookCount > 0) {
            books = (Book *)malloc(bookCount * sizeof(Book));
            if (books == NULL || fread(books, sizeof(Book), bookCount, fp) != (size_t)bookCount) {
                free(books);
                books = NULL;
                bookCount = 0;
            }
        } else {
            bookCount = 0;
        }
        fclose(fp);
    }

    fp = fopen(ISSUE_FILE, "rb");
    if (fp != NULL) {
        if (fread(&issueCount, sizeof(int), 1, fp) == 1 && issueCount > 0) {
            issues = (Issue *)malloc(issueCount * sizeof(Issue));
            if (issues == NULL || fread(issues, sizeof(Issue), issueCount, fp) != (size_t)issueCount) {
                free(issues);
                issues = NULL;
                issueCount = 0;
            }
        } else {
            issueCount = 0;
        }
        fclose(fp);
    }
}

void saveData(void)
{
    FILE *fp = fopen(BOOK_FILE, "wb");
    if (fp == NULL) {
        printf("  Error: could not save book data!\n");
    } else {
        fwrite(&bookCount, sizeof(int), 1, fp);
        if (bookCount > 0) fwrite(books, sizeof(Book), bookCount, fp);
        fclose(fp);
    }

    fp = fopen(ISSUE_FILE, "wb");
    if (fp == NULL) {
        printf("  Error: could not save issue data!\n");
    } else {
        fwrite(&issueCount, sizeof(int), 1, fp);
        if (issueCount > 0) fwrite(issues, sizeof(Issue), issueCount, fp);
        fclose(fp);
    }
}

/* ---------- Lookup helpers ---------- */
Book *findBook(int id)
{
    int i;
    for (i = 0; i < bookCount; i++)
        if (books[i].id == id)
            return &books[i];
    return NULL;
}

int findIssueIndex(int bookId, int rollNo)
{
    int i;
    for (i = 0; i < issueCount; i++)
        if (issues[i].bookId == bookId && issues[i].rollNo == rollNo)
            return i;
    return -1;
}

void printHeader(void)
{
    printf("\n%-6s %-30s %-20s %-6s %-9s\n", "ID", "Title", "Author", "Total", "Available");
    printf("-----------------------------------------------------------------------\n");
}

void printBook(const Book *b)
{
    printf("%-6d %-30.30s %-20.20s %-6d %-9d\n",
           b->id, b->title, b->author, b->total, b->available);
}

/* ---------- Features ---------- */
void addBook(void)
{
    Book b;
    Book *tmp;

    printf("\n--- Add New Book ---\n");
    b.id = readInt("Book ID: ");
    if (findBook(b.id) != NULL) {
        printf("  A book with this ID already exists.\n");
        return;
    }
    readLine("Title : ", b.title, sizeof(b.title));
    readLine("Author: ", b.author, sizeof(b.author));
    if (strlen(b.title) == 0) {
        printf("  Title cannot be empty.\n");
        return;
    }
    b.total = readInt("Number of copies: ");
    if (b.total == 0) {
        printf("  Copies must be at least 1.\n");
        return;
    }
    b.available = b.total;

    tmp = (Book *)realloc(books, (bookCount + 1) * sizeof(Book));
    if (tmp == NULL) {
        printf("  Memory allocation failed.\n");
        return;
    }
    books = tmp;
    books[bookCount++] = b;
    saveData();
    printf("  Book added successfully!\n");
}

void displayBooks(void)
{
    int i;
    printf("\n--- All Books ---\n");
    if (bookCount == 0) {
        printf("  No books in the library yet.\n");
        return;
    }
    printHeader();
    for (i = 0; i < bookCount; i++)
        printBook(&books[i]);
}

void searchBook(void)
{
    char keyword[100];
    int i, found = 0;

    printf("\n--- Search Book ---\n");
    readLine("Enter title or author keyword: ", keyword, sizeof(keyword));
    for (i = 0; i < bookCount; i++) {
        if (containsIgnoreCase(books[i].title, keyword) ||
            containsIgnoreCase(books[i].author, keyword)) {
            if (!found) printHeader();
            printBook(&books[i]);
            found = 1;
        }
    }
    if (!found) printf("  No matching books found.\n");
}

void updateBook(void)
{
    int id, newTotal, issuedNow;
    char buf[100];
    Book *b;

    printf("\n--- Update Book ---\n");
    id = readInt("Enter Book ID to update: ");
    b = findBook(id);
    if (b == NULL) {
        printf("  Book not found.\n");
        return;
    }
    issuedNow = b->total - b->available;

    readLine("New title (press Enter to keep): ", buf, sizeof(buf));
    if (strlen(buf) > 0) strcpy(b->title, buf);

    readLine("New author (press Enter to keep): ", buf, sizeof(buf));
    if (strlen(buf) > 0) strcpy(b->author, buf);

    newTotal = readInt("New total copies (current shown below): ");
    if (newTotal < issuedNow) {
        printf("  Cannot set copies below the %d currently issued. Copies unchanged.\n", issuedNow);
    } else {
        b->total = newTotal;
        b->available = newTotal - issuedNow;
    }
    saveData();
    printf("  Book updated.\n");
}

void deleteBook(void)
{
    int id, i;
    Book *b;

    printf("\n--- Delete Book ---\n");
    id = readInt("Enter Book ID to delete: ");
    b = findBook(id);
    if (b == NULL) {
        printf("  Book not found.\n");
        return;
    }
    if (b->available != b->total) {
        printf("  Cannot delete: some copies are currently issued.\n");
        return;
    }
    for (i = (int)(b - books); i < bookCount - 1; i++)
        books[i] = books[i + 1];
    bookCount--;
    saveData();
    printf("  Book deleted.\n");
}

void issueBook(void)
{
    Issue rec;
    Issue *tmp;
    Book *b;

    printf("\n--- Issue Book ---\n");
    rec.bookId = readInt("Book ID: ");
    b = findBook(rec.bookId);
    if (b == NULL) {
        printf("  Book not found.\n");
        return;
    }
    if (b->available == 0) {
        printf("  Sorry, no copies of \"%s\" are available.\n", b->title);
        return;
    }
    rec.rollNo = readInt("Student Roll No: ");
    if (findIssueIndex(rec.bookId, rec.rollNo) != -1) {
        printf("  This student has already issued this book.\n");
        return;
    }
    readLine("Student Name: ", rec.studentName, sizeof(rec.studentName));
    rec.issueDate = time(NULL);

    tmp = (Issue *)realloc(issues, (issueCount + 1) * sizeof(Issue));
    if (tmp == NULL) {
        printf("  Memory allocation failed.\n");
        return;
    }
    issues = tmp;
    issues[issueCount++] = rec;
    b->available--;
    saveData();
    printf("  Book issued! Please return within %d days.\n", ISSUE_DAYS);
}

void returnBook(void)
{
    int bookId, rollNo, idx, i, days, fine = 0;
    Book *b;

    printf("\n--- Return Book ---\n");
    bookId = readInt("Book ID: ");
    rollNo = readInt("Student Roll No: ");
    idx = findIssueIndex(bookId, rollNo);
    if (idx == -1) {
        printf("  No such issue record found.\n");
        return;
    }

    days = (int)(difftime(time(NULL), issues[idx].issueDate) / (60 * 60 * 24));
    if (days > ISSUE_DAYS)
        fine = (days - ISSUE_DAYS) * FINE_PER_DAY;

    for (i = idx; i < issueCount - 1; i++)
        issues[i] = issues[i + 1];
    issueCount--;

    b = findBook(bookId);
    if (b != NULL) b->available++;
    saveData();

    printf("  Book returned after %d day(s).\n", days);
    if (fine > 0)
        printf("  Late fine to pay: Rs. %d\n", fine);
    else
        printf("  No fine. Thank you!\n");
}

void viewIssuedBooks(void)
{
    int i;
    char date[30];
    Book *b;

    printf("\n--- Issued Books ---\n");
    if (issueCount == 0) {
        printf("  No books are currently issued.\n");
        return;
    }
    printf("%-8s %-28s %-8s %-18s %-12s\n", "BookID", "Title", "Roll No", "Student", "Issued On");
    printf("-----------------------------------------------------------------------------\n");
    for (i = 0; i < issueCount; i++) {
        b = findBook(issues[i].bookId);
        strftime(date, sizeof(date), "%d-%m-%Y", localtime(&issues[i].issueDate));
        printf("%-8d %-28.28s %-8d %-18.18s %-12s\n",
               issues[i].bookId,
               b ? b->title : "(deleted)",
               issues[i].rollNo,
               issues[i].studentName,
               date);
    }
}

/* ---------- Menu ---------- */
void showMenu(void)
{
    printf("\n=======================================\n");
    printf("       LIBRARY MANAGEMENT SYSTEM\n");
    printf("  Made by: Daksh Singh | Roll No: 29072\n");
    printf("      CSE | Section 2 | PPS (CSE-101)\n");
    printf("=======================================\n");
    printf(" 1. Add Book\n");
    printf(" 2. Display All Books\n");
    printf(" 3. Search Book\n");
    printf(" 4. Update Book\n");
    printf(" 5. Delete Book\n");
    printf(" 6. Issue Book\n");
    printf(" 7. Return Book\n");
    printf(" 8. View Issued Books\n");
    printf(" 0. Exit\n");
    printf("---------------------------------------\n");
}

int main(void)
{
    int choice;

    loadData();

    do {
        showMenu();
        choice = readInt("Enter your choice: ");
        switch (choice) {
            case 1: addBook();         break;
            case 2: displayBooks();    break;
            case 3: searchBook();      break;
            case 4: updateBook();      break;
            case 5: deleteBook();      break;
            case 6: issueBook();       break;
            case 7: returnBook();      break;
            case 8: viewIssuedBooks(); break;
            case 0: printf("\nData saved. Goodbye!\n"); break;
            default: printf("  Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    saveData();
    free(books);
    free(issues);
    return 0;
}
