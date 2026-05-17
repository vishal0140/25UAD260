#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNTS 100
#define MAX_HISTORY 10

struct clientData
{
    unsigned int acctNum;
    char firstName[20];
    char lastName[20];
    int pin;
    double balance;
};

struct transactionData
{
    unsigned int acctNum;
    char action[30];
    double amount;
};

// Function prototypes
void initializeFile(FILE *fPtr);
void addRecord(FILE *fPtr);
void updateRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void displayAll(FILE *fPtr);
void searchAccount(FILE *fPtr);
void transferMoney(FILE *fPtr);
void changePin(FILE *fPtr);
void exportToText(FILE *fPtr);
int loginAccount(FILE *fPtr, unsigned int accountNum);
int menu();

void pauseScreen()
{
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

// Main Function
int main()
{
    FILE *cfPtr;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("Unable to open file.\n");
            return 1;
        }

        initializeFile(cfPtr);
    }

    int choice;

    while ((choice = menu()) != 9)
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
            transferMoney(cfPtr);
            break;

        case 4:
            changePin(cfPtr);
            break;

        case 5:
            deleteRecord(cfPtr);
            break;

        case 6:
            displayAll(cfPtr);
            break;

        case 7:
            searchAccount(cfPtr);
            break;

        case 8:
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
    struct clientData blankClient = {0, "", "", 0, 0.0};

    for (int i = 0; i < MAX_ACCOUNTS; i++)
    {
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
}

// Menu
int menu()
{
    int choice;

    system("cls");

    printf("=========================================\n");
    printf("        ADVANCED BANK SYSTEM\n");
    printf("=========================================\n");

    printf("1. Create Account\n");
    printf("2. Deposit / Withdraw\n");
    printf("3. Transfer Money\n");
    printf("4. Change PIN\n");
    printf("5. Delete Account\n");
    printf("6. View All Accounts\n");
    printf("7. Search Account\n");
    printf("8. Export Accounts\n");
    printf("9. Exit\n");

    printf("\nEnter choice: ");
    scanf("%d", &choice);

    return choice;
}

// Create account
void addRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0, 0.0};

    unsigned int accountNum;

    printf("\nEnter account number: ");
    scanf("%u", &accountNum);

    if (accountNum < 1 || accountNum > MAX_ACCOUNTS)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

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

    printf("Create 4-digit PIN: ");
    scanf("%d", &client.pin);

    printf("Enter Initial Balance: ");
    scanf("%lf", &client.balance);

    if (client.balance < 0)
    {
        printf("Balance cannot be negative.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("\nAccount created successfully.\n");
}

// Login verification
int loginAccount(FILE *fPtr, unsigned int accountNum)
{
    struct clientData client;

    int pin;

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return 0;
    }

    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (pin != client.pin)
    {
        printf("Incorrect PIN.\n");
        return 0;
    }

    return 1;
}

// Deposit / Withdraw
void updateRecord(FILE *fPtr)
{
    struct clientData client;

    unsigned int accountNum;
    double amount;

    printf("\nEnter account number: ");
    scanf("%u", &accountNum);

    if (!loginAccount(fPtr, accountNum))
    {
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    printf("\nCurrent Balance: %.2f\n", client.balance);

    printf("Enter amount (+Deposit / -Withdraw): ");
    scanf("%lf", &amount);

    if (client.balance + amount < 0)
    {
        printf("Insufficient balance.\n");
        return;
    }

    client.balance += amount;

    fseek(fPtr,
          -sizeof(struct clientData),
          SEEK_CUR);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Transaction successful.\n");
    printf("Updated Balance: %.2f\n", client.balance);
}

// Transfer money
void transferMoney(FILE *fPtr)
{
    struct clientData sender, receiver;

    unsigned int senderAcc, receiverAcc;
    double amount;

    printf("\nEnter sender account number: ");
    scanf("%u", &senderAcc);

    if (!loginAccount(fPtr, senderAcc))
    {
        return;
    }

    printf("Enter receiver account number: ");
    scanf("%u", &receiverAcc);

    printf("Enter amount to transfer: ");
    scanf("%lf", &amount);

    // Read sender
    fseek(fPtr,
          (senderAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&sender, sizeof(struct clientData), 1, fPtr);

    // Read receiver
    fseek(fPtr,
          (receiverAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&receiver, sizeof(struct clientData), 1, fPtr);

    if (receiver.acctNum == 0)
    {
        printf("Receiver account not found.\n");
        return;
    }

    if (sender.balance < amount)
    {
        printf("Insufficient balance.\n");
        return;
    }

    sender.balance -= amount;
    receiver.balance += amount;

    // Update sender
    fseek(fPtr,
          (senderAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&sender, sizeof(struct clientData), 1, fPtr);

    // Update receiver
    fseek(fPtr,
          (receiverAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&receiver, sizeof(struct clientData), 1, fPtr);

    printf("Transfer successful.\n");
}

// Change PIN
void changePin(FILE *fPtr)
{
    struct clientData client;

    unsigned int accountNum;
    int newPin;

    printf("\nEnter account number: ");
    scanf("%u", &accountNum);

    if (!loginAccount(fPtr, accountNum))
    {
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    printf("Enter new PIN: ");
    scanf("%d", &newPin);

    client.pin = newPin;

    fseek(fPtr,
          -sizeof(struct clientData),
          SEEK_CUR);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("PIN changed successfully.\n");
}

// Delete account
void deleteRecord(FILE *fPtr)
{
    struct clientData blankClient = {0, "", "", 0, 0.0};
    struct clientData client;

    unsigned int accountNum;

    printf("\nEnter account number: ");
    scanf("%u", &accountNum);

    if (!loginAccount(fPtr, accountNum))
    {
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);

    printf("Account deleted successfully.\n");
}

// Display all accounts
void displayAll(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n=========================================================\n");
    printf("%-10s %-15s %-15s %-10s\n",
           "Account",
           "First Name",
           "Last Name",
           "Balance");

    printf("=========================================================\n");

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
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

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

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

// Export accounts
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

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
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

    printf("Accounts exported successfully.\n");
}
