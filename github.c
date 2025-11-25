#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "accounts.dat"
#define MAX_NAME_LEN 100
#define MAX_TYPE_LEN 20

// ------------------ STRUCT DEFINITION ------------------
typedef struct {
    int accountID;
    char name[MAX_NAME_LEN];
    char depositType[MAX_TYPE_LEN];   // FD or RD
    double amount;
    float interestRate;
    int durationMonths;
} Account;

// ------------------ FUNCTION DECLARATIONS ------------------
void addAccount();
void viewAccounts();
void searchAccount();
void updateAccount();
void deleteAccount();
int idExists(int id);
void saveAccount(Account a);
void loadAccounts(Account accounts[], int *count);
void writeAllAccounts(Account accounts[], int count);

// ------------------ MAIN MENU ------------------
int main() {
    int choice;
    do {
        printf("\n==============================\n");
        printf("   FD / RD MANAGEMENT SYSTEM\n");
        printf("==============================\n");
        printf("1. Add New Deposit Account\n");
        printf("2. View All Accounts\n");
        printf("3. Search Account\n");
        printf("4. Update Account\n");
        printf("5. Delete Account\n");
        printf("6. Exit\n");
        printf("------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: addAccount(); break;
            case 2: viewAccounts(); break;
            case 3: searchAccount(); break;
            case 4: updateAccount(); break;
            case 5: deleteAccount(); break;
            case 6: printf("Exiting program. Goodbye!\n"); break;
            default: printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 6);

    return 0;
}

// ------------------ ADD ACCOUNT ------------------
void addAccount() {
    Account a;

    printf("\n--- Add New Account ---\n");
    printf("Enter Account ID: ");
    scanf("%d", &a.accountID);
    getchar();

    if (idExists(a.accountID)) {
        printf("Error: Account ID already exists!\n");
        return;
    }

    printf("Enter Full Name: ");
    fgets(a.name, sizeof(a.name), stdin);
    a.name[strcspn(a.name, "\n")] = '\0';

    printf("Enter Deposit Type (FD/RD): ");
    fgets(a.depositType, sizeof(a.depositType), stdin);
    a.depositType[strcspn(a.depositType, "\n")] = '\0';

    printf("Enter Deposit Amount: ");
    scanf("%lf", &a.amount);

    printf("Enter Interest Rate (%%): ");
    scanf("%f", &a.interestRate);

    printf("Enter Duration (in months): ");
    scanf("%d", &a.durationMonths);

    saveAccount(a);
    printf("Account added successfully!\n");
}

// ------------------ SAVE ACCOUNT ------------------
void saveAccount(Account a) {
    FILE *fp = fopen(FILE_NAME, "ab");
    if (!fp) {
        perror("Error opening file");
        return;
    }
    fwrite(&a, sizeof(Account), 1, fp);
    fclose(fp);
}

// ------------------ CHECK ID ------------------
int idExists(int id) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) return 0;

    Account a;
    while (fread(&a, sizeof(Account), 1, fp)) {
        if (a.accountID == id) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// ------------------ VIEW ALL ACCOUNTS ------------------
void viewAccounts() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) {
        printf("No records found.\n");
        return;
    }

    Account a;
    printf("\n%-10s %-20s %-10s %-10s %-10s %-10s\n",
           "ID", "Name", "Type", "Amount", "Rate", "Months");
    printf("-----------------------------------------------------------------------\n");

    while (fread(&a, sizeof(Account), 1, fp)) {
        printf("%-10d %-20s %-10s %-10.2f %-10.2f %-10d\n",
               a.accountID, a.name, a.depositType,
               a.amount, a.interestRate, a.durationMonths);
    }
    fclose(fp);
}

// ------------------ SEARCH ACCOUNT ------------------
void searchAccount() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) {
        printf("No records found.\n");
        return;
    }

    int id, found = 0;
    Account a;

    printf("\nEnter Account ID to search: ");
    scanf("%d", &id);

    while (fread(&a, sizeof(Account), 1, fp)) {
        if (a.accountID == id) {
            printf("\nAccount Found:\n");
            printf("ID: %d\nName: %s\nType: %s\nAmount: %.2f\nInterest: %.2f%%\nMonths: %d\n",
                   a.accountID, a.name, a.depositType,
                   a.amount, a.interestRate, a.durationMonths);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("No account found with ID %d.\n", id);

    fclose(fp);
}

// ------------------ UPDATE ACCOUNT ------------------
void updateAccount() {
    Account accounts[100];
    int count = 0, id, found = 0;

    loadAccounts(accounts, &count);

    printf("\nEnter Account ID to update: ");
    scanf("%d", &id);
    getchar();

    for (int i = 0; i < count; i++) {
        if (accounts[i].accountID == id) {
            found = 1;

            printf("Enter new Interest Rate: ");
            scanf("%f", &accounts[i].interestRate);

            printf("Enter new Duration (months): ");
            scanf("%d", &accounts[i].durationMonths);

            break;
        }
    }

    if (found) {
        writeAllAccounts(accounts, count);
        printf("Record updated successfully.\n");
    } else {
        printf("Account ID not found.\n");
    }
}

// ------------------ DELETE ACCOUNT ------------------
void deleteAccount() {
    Account accounts[100];
    int count = 0, id, found = 0;

    loadAccounts(accounts, &count);

    printf("\nEnter Account ID to delete: ");
    scanf("%d", &id);
    getchar();

    for (int i = 0; i < count; i++) {
        if (accounts[i].accountID == id) {
            found = 1;
            printf("Are you sure you want to delete account %s (ID %d)? (y/n): ",
                   accounts[i].name, id);

            char confirm = getchar();
            getchar();

            if (confirm == 'y' || confirm == 'Y') {
                for (int j = i; j < count - 1; j++)
                    accounts[j] = accounts[j + 1];

                count--;
                writeAllAccounts(accounts, count);
                printf("Record deleted successfully.\n");
            } else {
                printf("Deletion cancelled.\n");
            }
            break;
        }
    }

    if (!found)
        printf("Account ID not found.\n");
}

// ------------------ LOAD / WRITE ALL ------------------
void loadAccounts(Account accounts[], int *count) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) return;
    *count = 0;
    while (fread(&accounts[*count], sizeof(Account), 1, fp))
        (*count)++;
    fclose(fp);
}

void writeAllAccounts(Account accounts[], int count) {
    FILE *fp = fopen(FILE_NAME, "wb");
    if (!fp) {
        perror("Error opening file");
        return;
    }
    fwrite(accounts, sizeof(Account), count, fp);
    fclose(fp);
}
