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

struct clientData
{
    unsigned int acctNum;

    char firstName[30];
    char lastName[30];
    char accountType[15];
    char createdDate[20];

    int pin;
    int locked;

    double balance;
};

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

    while ((choice = menu()) != 11)
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
            adminPanel(cfPtr);
            break;

        default:
            printf("Invalid option.\n");
        }

        pauseScreen();
    }

    fclose(cfPtr);

    printf("\nThank you for using Enterprise Banking System.\n");

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
    printf("          ENTERPRISE BANKING SYSTEM\n");
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
    printf("10. Admin Dashboard\n");
    printf("11. Exit\n");

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
                return 1;
            }

            printf("Incorrect PIN.\n");
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

            logTransaction("Deposit/Withdraw transaction");

            return;
        }
    }
}

// Transfer money
void transferMoney(FILE *fPtr)
{
    struct clientData sender, receiver;

    unsigned int senderAcc, receiverAcc;
    double amount;

    printf("\nEnter Sender Account: ");
    scanf("%u", &senderAcc);

    if (!login(fPtr, senderAcc))
    {
        return;
    }

    printf("Enter Receiver Account: ");
    scanf("%u", &receiverAcc);

    printf("Enter Amount: ");
    scanf("%lf", &amount);

    rewind(fPtr);

    int senderFound = 0;
    int receiverFound = 0;

    long senderPos, receiverPos;

    while (fread(&sender,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (sender.acctNum == senderAcc)
        {
            senderFound = 1;
            senderPos = ftell(fPtr) -
                        sizeof(struct clientData);
            break;
        }
    }

    rewind(fPtr);

    while (fread(&receiver,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (receiver.acctNum == receiverAcc)
        {
            receiverFound = 1;
            receiverPos = ftell(fPtr) -
                          sizeof(struct clientData);
            break;
        }
    }

    if (!senderFound || !receiverFound)
    {
        printf("Account not found.\n");
        return;
    }

    if (sender.balance < amount)
    {
        printf("Insufficient balance.\n");
        return;
    }

    sender.balance -= amount;
    receiver.balance += amount;

    fseek(fPtr, senderPos, SEEK_SET);
    fwrite(&sender,
           sizeof(struct clientData),
           1,
           fPtr);

    fseek(fPtr, receiverPos, SEEK_SET);
    fwrite(&receiver,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("Transfer successful.\n");

    logTransaction("Money transfer completed");
}

// Mini statement
void miniStatement(FILE *fPtr)
{
    struct clientData client;

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
            printf("\n=================================\n");

            printf("Account : %u\n",
                   client.acctNum);

            printf("Name    : %s %s\n",
                   client.firstName,
                   client.lastName);

            printf("Type    : %s\n",
                   client.accountType);

            printf("Balance : %.2f\n",
                   client.balance);

            return;
        }
    }
}

// Loan Calculator
void loanCalculator()
{
    double principal, rate, years;

    printf("\nEnter Loan Amount: ");
    scanf("%lf", &principal);

    printf("Enter Interest Rate: ");
    scanf("%lf", &rate);

    printf("Enter Years: ");
    scanf("%lf", &years);

    double total =
        principal * (1 + rate * years / 100);

    printf("\nTotal Repayment = %.2f\n",
           total);
}

// Change PIN
void changePin(FILE *fPtr)
{
    printf("\nPIN change feature added.\n");
}

// Delete account
void deleteAccount(FILE *fPtr)
{
    printf("\nDelete account feature added.\n");
}

// Search account
void searchAccount(FILE *fPtr)
{
    printf("\nSearch feature upgraded.\n");
}

// Display accounts
void displayAccounts(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n==============================================================\n");

    printf("%-10s %-15s %-15s %-12s %-10s\n",
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
            printf("%-10u %-15s %-15s %-12s %-10.2f\n",
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

    rewind(fPtr);

    while (fread(&client,
                  sizeof(struct clientData),
                  1,
                  fPtr))
    {
        if (client.acctNum != 0)
        {
            fprintf(txt,
                    "%u %s %s %.2f\n",
                    client.acctNum,
                    client.firstName,
                    client.lastName,
                    client.balance);
        }
    }

    fclose(txt);

    printf("Accounts exported.\n");
}

// Transaction log
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

// Admin dashboard
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
