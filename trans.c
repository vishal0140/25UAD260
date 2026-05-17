#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

#define MAX_ACCOUNTS 100
#define MAX_ATTEMPTS 3
#define ADMIN_PASSWORD "admin123"

struct clientData
{
    unsigned int acctNum;
    char firstName[20];
    char lastName[20];
    int pin;
    int failedAttempts;
    int locked;
    double balance;
};

// Function Prototypes
void initializeFile(FILE *fPtr);
void createAccount(FILE *fPtr);
void depositWithdraw(FILE *fPtr);
void transferMoney(FILE *fPtr);
void deleteAccount(FILE *fPtr);
void displayAccounts(FILE *fPtr);
void searchAccount(FILE *fPtr);
void exportAccounts(FILE *fPtr);
void miniStatement(FILE *fPtr);
void calculateInterest(FILE *fPtr);
void adminPanel(FILE *fPtr);

int login(FILE *fPtr, unsigned int accountNum);
int menu();

void pauseScreen()
{
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

// MAIN
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

    while ((choice = menu()) != 10)
    {
        switch (choice)
        {
        case 1:
            createAccount(cfPtr);
            break;

        case 2:
            depositWithdraw(cfPtr);
            break;

        case 3:
            transferMoney(cfPtr);
            break;

        case 4:
            miniStatement(cfPtr);
            break;

        case 5:
            calculateInterest(cfPtr);
            break;

        case 6:
            searchAccount(cfPtr);
            break;

        case 7:
            displayAccounts(cfPtr);
            break;

        case 8:
            exportAccounts(cfPtr);
            break;

        case 9:
            adminPanel(cfPtr);
            break;

        default:
            printf("Invalid option.\n");
        }

        pauseScreen();
    }

    fclose(cfPtr);

    printf("\nThank you for using Advanced Bank System.\n");

    return 0;
}

// Initialize file
void initializeFile(FILE *fPtr)
{
    struct clientData blank = {0, "", "", 0, 0, 0, 0.0};

    for (int i = 0; i < MAX_ACCOUNTS; i++)
    {
        fwrite(&blank, sizeof(struct clientData), 1, fPtr);
    }
}

// Menu
int menu()
{
    int choice;

    system(CLEAR);

    printf("============================================\n");
    printf("        PROFESSIONAL BANK SYSTEM\n");
    printf("============================================\n");

    printf("1. Create Account\n");
    printf("2. Deposit / Withdraw\n");
    printf("3. Transfer Money\n");
    printf("4. Mini Statement\n");
    printf("5. Interest Calculator\n");
    printf("6. Search Account\n");
    printf("7. View All Accounts\n");
    printf("8. Export Accounts\n");
    printf("9. Admin Panel\n");
    printf("10. Exit\n");

    printf("\nEnter choice: ");
    scanf("%d", &choice);

    return choice;
}

// Login
int login(FILE *fPtr, unsigned int accountNum)
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

    if (client.locked)
    {
        printf("Account is locked.\n");
        return 0;
    }

    printf("Enter PIN: ");
    scanf("%d", &pin);

    if (pin != client.pin)
    {
        client.failedAttempts++;

        if (client.failedAttempts >= MAX_ATTEMPTS)
        {
            client.locked = 1;
            printf("Account locked after too many attempts.\n");
        }
        else
        {
            printf("Incorrect PIN.\n");
        }

        fseek(fPtr,
              (accountNum - 1) * sizeof(struct clientData),
              SEEK_SET);

        fwrite(&client, sizeof(struct clientData), 1, fPtr);

        return 0;
    }

    client.failedAttempts = 0;

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    return 1;
}

// Create account
void createAccount(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0, 0, 0, 0.0};

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

    printf("Create PIN: ");
    scanf("%d", &client.pin);

    printf("Enter Initial Balance: ");
    scanf("%lf", &client.balance);

    if (client.balance < 0)
    {
        printf("Invalid balance.\n");
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Account created successfully.\n");
}

// Deposit / Withdraw
void depositWithdraw(FILE *fPtr)
{
    struct clientData client;

    unsigned int accountNum;
    double amount;

    printf("\nEnter account number: ");
    scanf("%u", &accountNum);

    if (!login(fPtr, accountNum))
    {
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    printf("Current Balance: %.2f\n", client.balance);

    printf("Enter amount (+Deposit / -Withdraw): ");
    scanf("%lf", &amount);

    if (client.balance + amount < 0)
    {
        printf("Insufficient funds.\n");
        return;
    }

    client.balance += amount;

    fseek(fPtr,
          -sizeof(struct clientData),
          SEEK_CUR);

    fwrite(&client, sizeof(struct clientData), 1, fPtr);

    printf("Transaction successful.\n");
}

// Transfer money
void transferMoney(FILE *fPtr)
{
    struct clientData sender, receiver;

    unsigned int senderAcc, receiverAcc;
    double amount;

    printf("\nSender Account: ");
    scanf("%u", &senderAcc);

    if (!login(fPtr, senderAcc))
    {
        return;
    }

    printf("Receiver Account: ");
    scanf("%u", &receiverAcc);

    printf("Amount: ");
    scanf("%lf", &amount);

    fseek(fPtr,
          (senderAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&sender, sizeof(struct clientData), 1, fPtr);

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

    fseek(fPtr,
          (senderAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&sender, sizeof(struct clientData), 1, fPtr);

    fseek(fPtr,
          (receiverAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&receiver, sizeof(struct clientData), 1, fPtr);

    printf("Transfer completed successfully.\n");
}

// Mini Statement
void miniStatement(FILE *fPtr)
{
    struct clientData client;

    unsigned int accountNum;

    printf("\nEnter account number: ");
    scanf("%u", &accountNum);

    if (!login(fPtr, accountNum))
    {
        return;
    }

    fseek(fPtr,
          (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client, sizeof(struct clientData), 1, fPtr);

    printf("\n========== MINI STATEMENT ==========\n");
    printf("Account Number : %u\n", client.acctNum);
    printf("Name           : %s %s\n",
           client.firstName,
           client.lastName);
    printf("Balance        : %.2f\n",
           client.balance);
}

// Interest Calculator
void calculateInterest(FILE *fPtr)
{
    double principal, rate, years, interest;

    printf("\nEnter Principal Amount: ");
    scanf("%lf", &principal);

    printf("Enter Interest Rate: ");
    scanf("%lf", &rate);

    printf("Enter Time (Years): ");
    scanf("%lf", &years);

    interest = (principal * rate * years) / 100;

    printf("\nSimple Interest = %.2f\n", interest);
    printf("Total Amount    = %.2f\n",
           principal + interest);
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
    printf("Name           : %s %s\n",
           client.firstName,
           client.lastName);
    printf("Balance        : %.2f\n",
           client.balance);
}

// Display all accounts
void displayAccounts(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n===================================================\n");
    printf("%-10s %-15s %-15s %-10s\n",
           "Account",
           "First Name",
           "Last Name",
           "Balance");

    printf("===================================================\n");

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

// Export accounts
void exportAccounts(FILE *fPtr)
{
    FILE *txtPtr;

    struct clientData client;

    txtPtr = fopen("accounts.txt", "w");

    if (txtPtr == NULL)
    {
        printf("Unable to create export file.\n");
        return;
    }

    rewind(fPtr);

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum != 0)
        {
            fprintf(txtPtr,
                    "%u %s %s %.2f\n",
                    client.acctNum,
                    client.firstName,
                    client.lastName,
                    client.balance);
        }
    }

    fclose(txtPtr);

    printf("Accounts exported successfully.\n");
}

// Admin panel
void adminPanel(FILE *fPtr)
{
    char password[20];

    printf("\nEnter Admin Password: ");
    scanf("%19s", password);

    if (strcmp(password, ADMIN_PASSWORD) != 0)
    {
        printf("Access denied.\n");
        return;
    }

    printf("\n========== ADMIN PANEL ==========\n");
    printf("1. Unlock All Accounts\n");

    int option;
    scanf("%d", &option);

    if (option == 1)
    {
        struct clientData client;

        rewind(fPtr);

        for (int i = 0; i < MAX_ACCOUNTS; i++)
        {
            fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr);

            client.locked = 0;
            client.failedAttempts = 0;

            fseek(fPtr,
                  -sizeof(struct clientData),
                  SEEK_CUR);

            fwrite(&client,
                   sizeof(struct clientData),
                   1,
                   fPtr);
        }

        printf("All accounts unlocked.\n");
    }
}
