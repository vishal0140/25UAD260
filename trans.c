#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100

struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// Function prototypes
void initializeFile(FILE *fPtr);
void addRecord(FILE *fPtr);
void updateRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void displayAll(FILE *fPtr);
void searchAccount(FILE *fPtr);
int menu();

int main()
{
    FILE *cfPtr;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("File could not be opened.\n");
            return 1;
        }

        initializeFile(cfPtr);
    }

    int choice;

    while ((choice = menu()) != 6)
    {
        switch (choice)
        {
        case 1:
            addRecord(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            deleteRecord(cfPtr);
            break;

        case 4:
            displayAll(cfPtr);
            break;

        case 5:
            searchAccount(cfPtr);
            break;

        default:
            printf("Invalid choice.\n");
        }
    }

    fclose(cfPtr);

    printf("Program closed successfully.\n");

    return 0;
}

// Initialize empty records
void initializeFile(FILE *fPtr)
{
    struct clientData blankClient = {0, "", "", 0.0};

    for (int i = 0; i < MAX_ACCOUNTS; i++)
    {
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
}

// Menu
int menu()
{
    int choice;

    printf("\n===== BANK MANAGEMENT SYSTEM =====\n");
    printf("1. Add New Account\n");
    printf("2. Update Account\n");
    printf("3. Delete Account\n");
    printf("4. View All Accounts\n");
    printf("5. Search Account\n");
    printf("6. Exit\n");
    printf("Enter choice: ");

    scanf("%d", &choice);

    return choice;
}

// Add account
void addRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("Enter account number (1-100): ");
    scanf("%u", &accountNum);

    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    client.acctNum = accountNum;

    printf("Enter First Name: ");
    scanf("%9s", client.firstName);

    printf("Enter Last Name: ");
    scanf("%14s", client.lastName);

    printf("Enter Balance: ");
    scanf("%lf", &client.balance);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account added successfully.\n");
}

// Update account
void updateRecord(FILE *fPtr)
{
    struct clientData client;

    unsigned int accountNum;
    double amount;

    printf("Enter account number: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("\nAccount Found\n");
    printf("Name: %s %s\n", client.firstName, client.lastName);
    printf("Current Balance: %.2f\n", client.balance);

    printf("Enter amount (+deposit / -withdraw): ");
    scanf("%lf", &amount);

    client.balance += amount;

    fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Balance updated successfully.\n");
}

// Delete account
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("Enter account number to delete: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account does not exist.\n");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted successfully.\n");
}

// Display all accounts
void displayAll(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n%-10s %-15s %-15s %-10s\n",
           "Account",
           "First Name",
           "Last Name",
           "Balance");

    printf("-----------------------------------------------------\n");

    while (fread(&client, sizeof(struct clientData), 1, fPtr))
    {
        if (client.acctNum != 0)
        {
            printf("%-10u %-15s %-15s %-10.2f\n",
                   client.acctNum,
                   client.firstName,
                   client.lastName,
                   client.balance);
        }
    }
}

// Search account
void searchAccount(FILE *fPtr)
{
    struct clientData client;

    unsigned int accountNum;

    printf("Enter account number to search: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("\n===== ACCOUNT DETAILS =====\n");
    printf("Account Number : %u\n", client.acctNum);
    printf("First Name     : %s\n", client.firstName);
    printf("Last Name      : %s\n", client.lastName);
    printf("Balance        : %.2f\n", client.balance);
}
