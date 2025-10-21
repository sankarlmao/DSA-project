#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct History {
    char date[20];
    char reason[256];
    int admitted;
    char medicines[256];
    struct History *next;
} History;

typedef struct Patient {
    int id;
    char name[50];
    int age;
    char gender[10];
    char phone[20];
    History *historyHead;
    struct Patient *next;
} Patient;

Patient *patientList = NULL;
int nextPatientId = 1001;

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void getString(char *buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

Patient* find_patient(int id) {
    Patient *p = patientList;
    while (p) {
        if (p->id == id) return p;
        p = p->next;
    }
    return NULL;
}

void add_patient() {
    Patient *p = malloc(sizeof(Patient));
    if (!p) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    p->id = nextPatientId++;

    printf("\n--- New Patient Details ---\n");
    printf("Enter Patient Name: ");
    getString(p->name, 50);

    printf("Enter Patient Age: ");
    scanf("%d", &p->age);
    clearInputBuffer();

    printf("Enter Patient Gender: ");
    getString(p->gender, 10);

    printf("Enter Patient Phone: ");
    getString(p->phone, 20);

    printf("\n--- First Visit Details ---\n");
    History *h = malloc(sizeof(History));
    if (!h) {
        printf("Error: Memory allocation failed for history.\n");
        free(p);
        return;
    }

    printf("Enter Visit Date (YYYY-MM-DD): ");
    getString(h->date, 20);

    printf("Enter Reason for Visit: ");
    getString(h->reason, 256);

    printf("Enter Initial Medicines: ");
    getString(h->medicines, 256);

    printf("Was the patient admitted? (1 for Yes, 0 for No): ");
    scanf("%d", &h->admitted);
    clearInputBuffer();

    h->next = NULL;
    p->historyHead = h;

    p->next = patientList;
    patientList = p;

    printf("\nSuccess: Added Patient %s with ID %d\n", p->name, p->id);
}

void add_history() {
    int pid;
    printf("Enter Patient ID to add history: ");
    scanf("%d", &pid);
    clearInputBuffer();

    Patient *p = find_patient(pid);
    if (!p) {
        printf("Error: Patient with ID %d not found.\n", pid);
        return;
    }

    History *h = malloc(sizeof(History));
    if (!h) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    printf("\n--- New Visit Details for %s ---\n", p->name);
    printf("Enter Date (YYYY-MM-DD): ");
    getString(h->date, 20);

    printf("Enter Reason for Visit: ");
    getString(h->reason, 256);

    printf("Enter Medicines: ");
    getString(h->medicines, 256);

    printf("Was the patient admitted? (1 for Yes, 0 for No): ");
    scanf("%d", &h->admitted);
    clearInputBuffer();

    h->next = NULL;

    if (!p->historyHead) {
        p->historyHead = h;
    } else {
        History *t = p->historyHead;
        while (t->next) {
            t = t->next;
        }
        t->next = h;
    }

    printf("\nSuccess: History added for Patient %s (ID %d)\n", p->name, p->id);
}

void view_history() {
    int pid;
    printf("Enter Patient ID to view history: ");
    scanf("%d", &pid);
    clearInputBuffer();

    Patient *p = find_patient(pid);
    if (!p) {
        printf("Error: Patient with ID %d not found.\n", pid);
        return;
    }

    printf("\n--- Visit History for %s (ID %d) ---\n", p->name, p->id);

    if (!p->historyHead) {
        printf("No history found for this patient.\n");
        return;
    }

    History *t = p->historyHead;
    while (t) {
        printf("--------------------------------------\n");
        printf("  Date:       %s\n", t->date);
        printf("  Reason:     %s\n", t->reason);
        printf("  Admitted:   %s\n", t->admitted ? "Yes" : "No");
        printf("  Medicines:  %s\n", t->medicines);
        t = t->next;
    }
    printf("--------------------------------------\n");
}

void list_all_patients() {
    printf("\n======================================\n");
    printf("         LIST OF ALL PATIENTS         \n");
    printf("======================================\n");

    if (!patientList) {
        printf("No patients in the system.\n");
        return;
    }

    Patient *p = patientList;
    while (p) {
        printf("\n--- PATIENT --- \n");
        printf("  ID:       %d\n", p->id);
        printf("  Name:     %s\n", p->name);
        printf("  Age:      %d\n", p->age);
        printf("  Gender:   %s\n", p->gender);
        printf("  Phone:    %s\n", p->phone);

        printf("  --- Visit History ---\n");
        if (!p->historyHead) {
            printf("    No history found.\n");
        } else {
            History *h = p->historyHead;
            while (h) {
                printf("    ------------------\n");
                printf("    Date:       %s\n", h->date);
                printf("    Reason:     %s\n", h->reason);
                printf("    Admitted:   %s\n", h->admitted ? "Yes" : "No");
                printf("    Medicines:  %s\n", h->medicines);
                h = h->next;
            }
            printf("    ------------------\n");
        }
        
        printf("======================================\n");
        p = p->next;
    }
}

void free_all() {
    Patient *p = patientList;
    while (p) {
        History *h = p->historyHead;
        while (h) {
            History *tempH = h;
            h = h->next;
            free(tempH);
        }

        Patient *tempP = p;
        p = p->next;
        free(tempP);
    }
    patientList = NULL;
}

int main() {
    int choice = 0;

    while (1) {
        printf("\n--- Hospital Management System ---\n");
        printf("1. Add New Patient (and first visit)\n");
        printf("2. Add Subsequent Visit History\n");
        printf("3. View a Patient's History\n");
        printf("4. List All Patients (Full Details)\n");
        printf("5. Exit\n");
        printf("----------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                add_patient();
                break;
            case 2:
                add_history();
                break;
            case 3:
                view_history();
                break;
            case 4:
                list_all_patients();
                break;
            case 5:
                printf("Cleaning up and exiting...\n");
                free_all();
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}
