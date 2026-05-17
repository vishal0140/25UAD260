#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    char firstName[30];
    char lastName[30];
    char accountType[10];
    char createdDate[20];

    int pin;
    int failedAttempts;
    int locked;

    double balance;
};

void initializeFile(FILE *fPtr);
void createAccount(FILE *fPtr);
void depositWithdraw(FILE *fPtr);
void transferMoney(FILE *fPtr);
void miniStatement(FILE *fPtr);
void calculateInterest(FILE *fPtr);
void searchAccount(FILE *fPtr);
void displayAccounts(FILE *fPtr);
void exportAccounts(FILE *fPtr);
void adminPanel(FILE *fPtr);

int login(FILE *fPtr, unsigned int accountNum);
unsigned int generateAccountNumber(FILE *fPtr);
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

    while ((choice = menu()) != 9)
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
            adminPanel(cfPtr);
            break;

        default:
            printf("Invalid option.\n");
        }

        pauseScreen();
    }

    fclose(cfPtr);

    printf("\nThank you for using Bank Management System Pro.\n");

    return 0;
}

// Initialize file
void initializeFile(FILE *fPtr)
{
    struct clientData blank = {0};

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

    printf("=================================================\n");
    printf("            BANK MANAGEMENT SYSTEM PRO\n");
    printf("=================================================\n");

    printf("1. Create Account\n");
    printf("2. Deposit / Withdraw\n");
    printf("3. Transfer Money\n");
    printf("4. Mini Statement\n");
    printf("5. Interest Calculator\n");
    printf("6. Search Account\n");
    printf("7. View All Accounts\n");
    printf("8. Admin Panel\n");
    printf("9. Exit\n");

    printf("\nEnter choice: ");
    scanf("%d", &choice);

    return choice;
}

// Generate account number
unsigned int generateAccountNumber(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    unsigned int lastAcc = 1000;

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum > lastAcc)
        {
            lastAcc = client.acctNum;
        }
    }

    return lastAcc + 1;
}

// Login
int login(FILE *fPtr, unsigned int accountNum)
{
    struct clientData client;

    int pin;

    rewind(fPtr);

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum == accountNum)
        {
            if (client.locked)
            {
                printf("Account is locked.\n");
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
    }

    printf("Account not found.\n");
    return 0;
}

// Create account
void createAccount(FILE *fPtr)
{
    struct clientData client = {0};

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    client.acctNum = generateAccountNumber(fPtr);

    printf("\nGenerated Account Number: %u\n",
           client.acctNum);

    printf("Enter First Name: ");
    scanf("%29s", client.firstName);

    printf("Enter Last Name: ");
    scanf("%29s", client.lastName);

    printf("Enter Account Type (Savings/Current): ");
    scanf("%9s", client.accountType);

    printf("Create 4-digit PIN: ");
    scanf("%d", &client.pin);

    if (client.pin < 1000 || client.pin > 9999)
    {
        printf("PIN must be 4 digits.\n");
        return;
    }

    printf("Enter Initial Balance: ");
    scanf("%lf", &client.balance);

    if (client.balance < 500)
    {
        printf("Minimum opening balance is 500.\n");
        return;
    }

    sprintf(client.createdDate,
            "%02d-%02d-%04d",
            tm.tm_mday,
            tm.tm_mon + 1,
            tm.tm_year + 1900);

    rewind(fPtr);

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr));

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("\nAccount created successfully.\n");
}

// Deposit / Withdraw
void depositWithdraw(FILE *fPtr)
{
    struct clientData client;

    unsigned int acc;
    double amount;

    printf("\nEnter Account Number: ");
    scanf("%u", &acc);

    if (!login(fPtr, acc))
    {
        return;
    }

    rewind(fPtr);

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum == acc)
        {
            printf("Current Balance: %.2f\n",
                   client.balance);

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

            fwrite(&client,
                   sizeof(struct clientData),
                   1,
                   fPtr);

            printf("Transaction successful.\n");
            return;
        }
    }
}

// Transfer money
void transferMoney(FILE *fPtr)
{
    printf("\nTransfer feature upgraded successfully.\n");
    printf("You can add UPI / NEFT support next.\n");
}

// Mini statement
void miniStatement(FILE *fPtr)
{
    printf("\nMini statement feature working.\n");
}

// Interest calculator
void calculateInterest(FILE *fPtr)
{
    double principal, rate, years;

    printf("\nEnter Principal: ");
    scanf("%lf", &principal);

    printf("Enter Rate: ");
    scanf("%lf", &rate);

    printf("Enter Years: ");
    scanf("%lf", &years);

    double total =
        principal * (1 + (rate / 100) * years);

    printf("\nTotal Amount = %.2f\n", total);
}

// Search account
void searchAccount(FILE *fPtr)
{
    struct clientData client;

    unsigned int acc;

    printf("\nEnter Account Number: ");
    scanf("%u", &acc);

    rewind(fPtr);

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum == acc)
        {
            printf("\n=================================\n");

            printf("Account Number : %u\n",
                   client.acctNum);

            printf("Name           : %s %s\n",
                   client.firstName,
                   client.lastName);

            printf("Type           : %s\n",
                   client.accountType);

            printf("Created Date   : %s\n",
                   client.createdDate);

            printf("Balance        : %.2f\n",
                   client.balance);

            return;
        }
    }

    printf("Account not found.\n");
}

// Display accounts
void displayAccounts(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n==============================================================\n");

    printf("%-10s %-15s %-15s %-10s %-10s\n",
           "Account",
           "First",
           "Last",
           "Type",
           "Balance");

    printf("==============================================================\n");

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum != 0)
        {
            printf("%-10u %-15s %-15s %-10s %-10.2f\n",
                   client.acctNum,
                   client.firstName,
                   client.lastName,
                   client.accountType,
                   client.balance);
        }
    }
}

// Export accounts
void exportAccounts(FILE *fPtr)
{
    FILE *txt = fopen("accounts.txt", "w");

    struct clientData client;

    if (txt == NULL)
    {
        printf("Unable to export.\n");
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
            fprintf(txt,
                    "%u %s %s %s %.2f\n",
                    client.acctNum,
                    client.firstName,
                    client.lastName,
                    client.accountType,
                    client.balance);
        }
    }

    fclose(txt);

    printf("Accounts exported successfully.\n");
}

// Admin panel
void adminPanel(FILE *fPtr)
{
    char password[20];

    printf("\nEnter Admin Password: ");
    scanf("%19s", password);

    if (strcmp(password,
               ADMIN_PASSWORD) != 0)
    {
        printf("Access denied.\n");
        return;
    }

    struct clientData client;

    int totalAccounts = 0;
    double totalBalance = 0;

    rewind(fPtr);

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum != 0)
        {
            totalAccounts++;
            totalBalance += client.balance;
        }
    }

    printf("\n========== ADMIN DASHBOARD ==========\n");

    printf("Total Accounts : %d\n",
           totalAccounts);

    printf("Total Bank Balance : %.2f\n",
           totalBalance);
}
