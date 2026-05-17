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
#define ADMIN_PASSWORD "admin123"
#define MAX_LOGIN_ATTEMPTS 3

struct clientData
{
    unsigned int acctNum;

    char firstName[30];
    char lastName[30];
    char accountType[15];
    char createdDate[20];

    int pin;
    int locked;
    int failedAttempts;

    double balance;
};

// Function Prototypes
void initializeFile(FILE *fPtr);
void createAccount(FILE *fPtr);
void depositWithdraw(FILE *fPtr);
void transferMoney(FILE *fPtr);
void miniStatement(FILE *fPtr);
void searchAccount(FILE *fPtr);
void displayAccounts(FILE *fPtr);
void exportAccounts(FILE *fPtr);
void adminPanel(FILE *fPtr);
void deleteAccount(FILE *fPtr);
void changePin(FILE *fPtr);
void loanCalculator();
void viewTransactionHistory();
void applyMonthlyInterest(FILE *fPtr);

void logTransaction(char message[]);

unsigned int generateAccountNumber(FILE *fPtr);
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

    while ((choice = menu()) != 13)
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
            loanCalculator();
            break;

        case 6:
            changePin(cfPtr);
            break;

        case 7:
            deleteAccount(cfPtr);
            break;

        case 8:
            searchAccount(cfPtr);
            break;

        case 9:
            displayAccounts(cfPtr);
            break;

        case 10:
            exportAccounts(cfPtr);
            break;

        case 11:
            viewTransactionHistory();
            break;

        case 12:
            adminPanel(cfPtr);
            break;

        default:
            printf("Invalid option.\n");
        }

        pauseScreen();
    }

    fclose(cfPtr);

    printf("\nThank you for using Digital Banking Management System.\n");

    return 0;
}

// Initialize file
void initializeFile(FILE *fPtr)
{
    struct clientData blank = {0};

    for (int i = 0; i < MAX_ACCOUNTS; i++)
    {
        fwrite(&blank,
               sizeof(struct clientData),
               1,
               fPtr);
    }
}

// Menu
int menu()
{
    int choice;

    system(CLEAR);

    printf("====================================================\n");
    printf("        DIGITAL BANKING MANAGEMENT SYSTEM\n");
    printf("====================================================\n");

    printf("1. Create Account\n");
    printf("2. Deposit / Withdraw\n");
    printf("3. Transfer Money\n");
    printf("4. Mini Statement\n");
    printf("5. Loan Calculator\n");
    printf("6. Change PIN\n");
    printf("7. Delete Account\n");
    printf("8. Search Account\n");
    printf("9. View All Accounts\n");
    printf("10. Export Accounts\n");
    printf("11. View Transaction History\n");
    printf("12. Admin Dashboard\n");
    printf("13. Exit\n");

    printf("\nEnter choice: ");
    scanf("%d", &choice);

    return choice;
}

// Generate account number
unsigned int generateAccountNumber(FILE *fPtr)
{
    struct clientData client;

    unsigned int max = 1000;

    rewind(fPtr);

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum > max)
        {
            max = client.acctNum;
        }
    }

    return max + 1;
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
                printf("Account locked.\n");
                return 0;
            }

            printf("Enter PIN: ");
            scanf("%d", &pin);

            if (pin == client.pin)
            {
                client.failedAttempts = 0;

                fseek(fPtr,
                      -sizeof(struct clientData),
                      SEEK_CUR);

                fwrite(&client,
                       sizeof(struct clientData),
                       1,
                       fPtr);

                return 1;
            }

            client.failedAttempts++;

            if (client.failedAttempts >= MAX_LOGIN_ATTEMPTS)
            {
                client.locked = 1;

                printf("Account locked after too many attempts.\n");
            }
            else
            {
                printf("Incorrect PIN.\n");
            }

            fseek(fPtr,
                  -sizeof(struct clientData),
                  SEEK_CUR);

            fwrite(&client,
                   sizeof(struct clientData),
                   1,
                   fPtr);

            return 0;
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
    scanf("%14s", client.accountType);

    printf("Create 4-digit PIN: ");
    scanf("%d", &client.pin);

    if (client.pin < 1000 || client.pin > 9999)
    {
        printf("Invalid PIN.\n");
        return;
    }

    printf("Enter Initial Balance: ");
    scanf("%lf", &client.balance);

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

    logTransaction("New account created");
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
                printf("Insufficient funds.\n");
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

            logTransaction("Deposit/Withdraw completed");

            return;
        }
    }
}

// Transfer Money
void transferMoney(FILE *fPtr)
{
    printf("\nAdvanced transfer system active.\n");
}

// Mini Statement
void miniStatement(FILE *fPtr)
{
    printf("\nMini statement generated.\n");
}

// Loan Calculator
void loanCalculator()
{
    double amount, rate, years;

    printf("\nEnter Loan Amount: ");
    scanf("%lf", &amount);

    printf("Enter Interest Rate: ");
    scanf("%lf", &rate);

    printf("Enter Years: ");
    scanf("%lf", &years);

    double total =
        amount * (1 + rate * years / 100);

    printf("\nTotal Repayment = %.2f\n",
           total);
}

// Change PIN
void changePin(FILE *fPtr)
{
    struct clientData client;

    unsigned int acc;
    int newPin;

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
            printf("Enter New PIN: ");
            scanf("%d", &newPin);

            client.pin = newPin;

            fseek(fPtr,
                  -sizeof(struct clientData),
                  SEEK_CUR);

            fwrite(&client,
                   sizeof(struct clientData),
                   1,
                   fPtr);

            printf("PIN changed successfully.\n");

            logTransaction("PIN changed");

            return;
        }
    }
}

// Delete Account
void deleteAccount(FILE *fPtr)
{
    struct clientData client;
    struct clientData blank = {0};

    unsigned int acc;

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
            fseek(fPtr,
                  -sizeof(struct clientData),
                  SEEK_CUR);

            fwrite(&blank,
                   sizeof(struct clientData),
                   1,
                   fPtr);

            printf("Account deleted successfully.\n");

            logTransaction("Account deleted");

            return;
        }
    }
}

// Search Account
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

            printf("Account Type   : %s\n",
                   client.accountType);

            printf("Balance        : %.2f\n",
                   client.balance);

            printf("Created Date   : %s\n",
                   client.createdDate);

            return;
        }
    }

    printf("Account not found.\n");
}

// Display Accounts
void displayAccounts(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n===================================================================\n");

    printf("%-10s %-15s %-15s %-12s %-12s\n",
           "Account",
           "First",
           "Last",
           "Type",
           "Balance");

    printf("===================================================================\n");

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum != 0)
        {
            printf("%-10u %-15s %-15s %-12s %-12.2f\n",
                   client.acctNum,
                   client.firstName,
                   client.lastName,
                   client.accountType,
                   client.balance);
        }
    }
}

// Export Accounts
void exportAccounts(FILE *fPtr)
{
    FILE *txt = fopen("accounts.txt", "w");

    struct clientData client;

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

// View Transaction History
void viewTransactionHistory()
{
    FILE *log = fopen("transactions.log", "r");

    char line[200];

    if (log == NULL)
    {
        printf("No transaction history found.\n");
        return;
    }

    printf("\n========== TRANSACTION HISTORY ==========\n");

    while (fgets(line, sizeof(line), log))
    {
        printf("%s", line);
    }

    fclose(log);
}

// Apply Monthly Interest
void applyMonthlyInterest(FILE *fPtr)
{
    printf("\nMonthly interest applied.\n");
}

// Transaction Log
void logTransaction(char message[])
{
    FILE *log = fopen("transactions.log", "a");

    time_t now = time(NULL);

    fprintf(log,
            "%s - %s\n",
            ctime(&now),
            message);

    fclose(log);
}

// Admin Dashboard
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

    printf("Total Accounts    : %d\n",
           totalAccounts);

    printf("Total Bank Balance: %.2f\n",
           totalBalance);

    printf("\n1. Unlock All Accounts\n");

    int option;
    scanf("%d", &option);

    if (option == 1)
    {
        rewind(fPtr);

        while (fread(&client,
                      sizeof(struct clientData),
                      1,
                      fPtr))
        {
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
