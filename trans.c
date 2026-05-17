#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100

struct clientData
{
    unsigned int acctNum;
    char lastName[20];
    char firstName[20];
    double balance;
};

// Function Prototypes
void initializeFile(FILE *fPtr);
void addRecord(FILE *fPtr);
void updateRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void displayAll(FILE *fPtr);
void searchAccount(FILE *fPtr);
void exportToText(FILE *fPtr);
int countAccounts(FILE *fPtr);
int menu();

void pauseScreen()
{
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

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

    while ((choice = menu()) != 7)
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

        case 6:
            exportToText(cfPtr);
            break;

        default:
            printf("Invalid choice.\n");
        }

        pauseScreen();
    }

    fclose(cfPtr);

    printf("\nProgram closed successfully.\n");

    return 0;
}

// Initialize file
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

    system("cls"); // use "clear" for Linux/Mac

    printf("====================================\n");
    printf("      BANK MANAGEMENT SYSTEM\n");
    printf("====================================\n");

    printf("1. Add New Account\n");
    printf("2. Deposit / Withdraw\n");
    printf("3. Delete Account\n");
    printf("4. View All Accounts\n");
    printf("5. Search Account\n");
    printf("6. Export Accounts to Text File\n");
    printf("7. Exit\n");

    printf("\nTotal Accounts: %d\n", MAX_ACCOUNTS);

    printf("\nEnter choice: ");
    scanf("%d", &choice);

    return choice;
}

// Add account
void addRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("\nEnter account number (1-100): ");
    scanf("%u", &accountNum);

    if (accountNum < 1 || accountNum > MAX_ACCOUNTS)
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
    scanf("%19s", client.firstName);

    printf("Enter Last Name: ");
    scanf("%19s", client.lastName);

    printf("Enter Initial Balance: ");
    scanf("%lf", &client.balance);

    if (client.balance < 0)
    {
        printf("Balance cannot be negative.\n");
        return;
    }

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("\nAccount created successfully.\n");
}

// Deposit / Withdraw
void updateRecord(FILE *fPtr)
{
    struct clientData client;

    unsigned int accountNum;
    double amount;

    printf("\nEnter account number: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("\nAccount Holder : %s %s\n",
           client.firstName,
           client.lastName);

    printf("Current Balance: %.2f\n", client.balance);

    printf("\nEnter amount (+Deposit / -Withdraw): ");
    scanf("%lf", &amount);

    if (client.balance + amount < 0)
    {
        printf("Insufficient balance.\n");
        return;
    }

    client.balance += amount;

    fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Transaction successful.\n");
    printf("Updated Balance: %.2f\n", client.balance);
}

// Delete account
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};

    unsigned int accountNum;

    printf("\nEnter account number to delete: ");
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

    printf("\n============================================================\n");
    printf("%-10s %-15s %-15s %-10s\n",
           "Account",
           "First Name",
           "Last Name",
           "Balance");

    printf("============================================================\n");

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

    printf("\nEnter account number: ");
    scanf("%u", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("\n========== ACCOUNT DETAILS ==========\n");

    printf("Account Number : %u\n", client.acctNum);
    printf("First Name     : %s\n", client.firstName);
    printf("Last Name      : %s\n", client.lastName);
    printf("Balance        : %.2f\n", client.balance);
}

// Export data to text file
void exportToText(FILE *fPtr)
{
    FILE *txtPtr;

    struct clientData client;

    txtPtr = fopen("accounts.txt", "w");

    if (txtPtr == NULL)
    {
        printf("Unable to create text file.\n");
        return;
    }

    rewind(fPtr);

    fprintf(txtPtr,
            "%-10s %-15s %-15s %-10s\n",
            "Account",
            "FirstName",
            "LastName",
            "Balance");

    while (fread(&client, sizeof(struct clientData), 1, fPtr))
    {
        if (client.acctNum != 0)
        {
            fprintf(txtPtr,
                    "%-10u %-15s %-15s %-10.2f\n",
                    client.acctNum,
                    client.firstName,
                    client.lastName,
                    client.balance);
        }
    }

    fclose(txtPtr);

    printf("Accounts exported to accounts.txt successfully.\n");
}
