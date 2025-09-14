#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct History {
    char date[20];
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
    struct Patient *left, *right;
    History *historyHead;
} Patient;

typedef struct Doctor {
    int id;
    char name[50];
    char speciality[50];
    struct Doctor *next;
} Doctor;

typedef struct Appointment {
    int patient_id, doctor_id, priority;
    char reason[100];
    struct Appointment *next;
} Appointment;

Patient *patientRoot = NULL;
Doctor *doctorHead = NULL;
Appointment *normalQueue = NULL, *emergencyQueue = NULL;
int nextPatientId = 1001, nextDoctorId = 501;

Patient* newPatientNode(int id, char *name, int age, char *gender, char *phone) {
    Patient *p = (Patient*)malloc(sizeof(Patient));
    p->id = id;
    strcpy(p->name, name);
    p->age = age;
    strcpy(p->gender, gender);
    strcpy(p->phone, phone);
    p->left = p->right = NULL;
    p->historyHead = NULL;
    return p;
}

Patient* insertPatient(Patient* root, Patient* p) {
    if (!root) return p;
    if (p->id < root->id)
        root->left = insertPatient(root->left, p);
    else if (p->id > root->id)
        root->right = insertPatient(root->right, p);
    return root;
}

void inorderPatients(Patient* root) {
    if (!root) return;
    inorderPatients(root->left);
    printf("ID: %d | Name: %s | Age: %d | Gender: %s | Phone: %s\n",
           root->id, root->name, root->age, root->gender, root->phone);
    inorderPatients(root->right);
}

Patient* findPatientNode(Patient* root, int id) {
    if (!root) return NULL;
    if (id == root->id) return root;
    if (id < root->id) return findPatientNode(root->left, id);
    return findPatientNode(root->right, id);
}

void addHistoryToPatient(int pid, char *date, int admitted, char *meds) {
    Patient *p = findPatientNode(patientRoot, pid);
    if (!p) {
        printf("Patient ID %d not found.\n", pid);
        return;
    }
    History *h = (History*)malloc(sizeof(History));
    strncpy(h->date, date, sizeof(h->date)-1);
    h->date[sizeof(h->date)-1] = '\0';
    h->admitted = admitted;
    strncpy(h->medicines, meds, sizeof(h->medicines)-1);
    h->medicines[sizeof(h->medicines)-1] = '\0';
    h->next = NULL;
    if (!p->historyHead) p->historyHead = h;
    else {
        History *t = p->historyHead;
        while (t->next) t = t->next;
        t->next = h;
    }
    printf("History added for patient %d.\n", pid);
}

void viewPatientHistory(int pid) {
    Patient *p = findPatientNode(patientRoot, pid);
    if (!p) {
        printf("Patient ID %d not found.\n", pid);
        return;
    }
    if (!p->historyHead) {
        printf("No history records for patient %d.\n", pid);
        return;
    }
    printf("History for %s (ID %d):\n", p->name, p->id);
    History *t = p->historyHead;
    int idx = 1;
    while (t) {
        printf("[%d] Date: %s | Admitted: %s\n", idx, t->date, t->admitted ? "Yes" : "No");
        printf("    Medicines: %s\n", t->medicines);
        t = t->next;
        idx++;
    }
}

void addDoctor(char *name, char *speciality) {
    Doctor *d = (Doctor*)malloc(sizeof(Doctor));
    d->id = nextDoctorId++;
    strcpy(d->name, name);
    strcpy(d->speciality, speciality);
    d->next = doctorHead;
    doctorHead = d;
    printf("Doctor added with ID %d\n", d->id);
}

void listDoctors() {
    Doctor *d = doctorHead;
    while (d) {
        printf("ID: %d | %s (%s)\n", d->id, d->name, d->speciality);
        d = d->next;
    }
}

void enqueueNormal(int pid, int did, char *reason) {
    Appointment *a = (Appointment*)malloc(sizeof(Appointment));
    a->patient_id = pid;
    a->doctor_id = did;
    a->priority = 999;
    strcpy(a->reason, reason);
    a->next = NULL;
    if (!normalQueue) normalQueue = a;
    else {
        Appointment *t = normalQueue;
        while (t->next) t = t->next;
        t->next = a;
    }
    printf("Normal appointment scheduled.\n");
}

void enqueueEmergency(int pid, int did, char *reason, int priority) {
    Appointment *a = (Appointment*)malloc(sizeof(Appointment));
    a->patient_id = pid;
    a->doctor_id = did;
    a->priority = priority;
    strcpy(a->reason, reason);
    a->next = NULL;
    if (!emergencyQueue || priority < emergencyQueue->priority) {
        a->next = emergencyQueue;
        emergencyQueue = a;
    } else {
        Appointment *t = emergencyQueue;
        while (t->next && t->next->priority <= priority) t = t->next;
        a->next = t->next;
        t->next = a;
    }
    printf("Emergency appointment scheduled.\n");
}

void serveNext() {
    if (emergencyQueue) {
        Appointment *a = emergencyQueue;
        emergencyQueue = emergencyQueue->next;
        printf("Serving EMERGENCY patient %d with doctor %d\n", a->patient_id, a->doctor_id);
        free(a);
    } else if (normalQueue) {
        Appointment *a = normalQueue;
        normalQueue = normalQueue->next;
        printf("Serving patient %d with doctor %d\n", a->patient_id, a->doctor_id);
        free(a);
    } else {
        printf("No appointments.\n");
    }
}

int main() {
    int ch;
    while (1) {
        printf("\n--- Hospital Management ---\n");
        printf("1. Add Patient\n2. Add Doctor\n3. List Patients\n4. List Doctors\n");
        printf("5. Schedule Normal Appointment\n6. Schedule Emergency Appointment\n");
        printf("7. Serve Next\n8. Add Patient History\n9. View Patient History\n0. Exit\nChoice: ");
        scanf("%d", &ch);
        getchar();
        if (ch == 1) {
            char name[50], gender[10], phone[20];
            int age;
            printf("Enter name: "); fgets(name, 50, stdin); name[strcspn(name,"\n")]=0;
            printf("Age: "); scanf("%d", &age); getchar();
            printf("Gender: "); fgets(gender, 10, stdin); gender[strcspn(gender,"\n")]=0;
            printf("Phone: "); fgets(phone, 20, stdin); phone[strcspn(phone,"\n")]=0;
            Patient *p = newPatientNode(nextPatientId++, name, age, gender, phone);
            patientRoot = insertPatient(patientRoot, p);
            printf("Patient added with ID %d\n", p->id);
        }
        else if (ch == 2) {
            char name[50], spec[50];
            printf("Doctor name: "); fgets(name, 50, stdin); name[strcspn(name,"\n")]=0;
            printf("Speciality: "); fgets(spec, 50, stdin); spec[strcspn(spec,"\n")]=0;
            addDoctor(name, spec);
        }
        else if (ch == 3) inorderPatients(patientRoot);
        else if (ch == 4) listDoctors();
        else if (ch == 5) {
            int pid, did;
            char reason[100];
            printf("Patient ID: "); scanf("%d", &pid); getchar();
            printf("Doctor ID: "); scanf("%d", &did); getchar();
            printf("Reason: "); fgets(reason, 100, stdin); reason[strcspn(reason,"\n")]=0;
            enqueueNormal(pid, did, reason);
        }
        else if (ch == 6) {
            int pid, did, pr;
            char reason[100];
            printf("Patient ID: "); scanf("%d", &pid); getchar();
            printf("Doctor ID: "); scanf("%d", &did); getchar();
            printf("Reason: "); fgets(reason, 100, stdin); reason[strcspn(reason,"\n")]=0;
            printf("Priority (1 = highest): "); scanf("%d", &pr); getchar();
            enqueueEmergency(pid, did, reason, pr);
        }
        else if (ch == 7) serveNext();
        else if (ch == 8) {
            int pid, admitted;
            char date[20], meds[256], admc;
            printf("Patient ID: "); scanf("%d", &pid); getchar();
            printf("Date (YYYY-MM-DD): "); fgets(date, 20, stdin); date[strcspn(date,"\n")]=0;
            printf("Admitted? (y/n): "); scanf("%c", &admc); getchar();
            admitted = (admc == 'y' || admc == 'Y') ? 1 : 0;
            printf("List medicines (comma separated): "); fgets(meds, 256, stdin); meds[strcspn(meds,"\n")]=0;
            addHistoryToPatient(pid, date, admitted, meds);
        }
        else if (ch == 9) {
            int pid;
            printf("Patient ID: "); scanf("%d", &pid); getchar();
            viewPatientHistory(pid);
        }
        else if (ch == 0) break;
        else printf("Invalid choice.\n");
    }
    return 0;
}

