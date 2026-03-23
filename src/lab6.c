#include <stdio.h>
#include <stdbool.h>
#include "lab6.h"

#define MAX_P 10
#define MAX_R 10

typedef enum RequestResult {
    REQUEST_GRANTED = 0,
    REQUEST_EXCEEDS_NEED,
    REQUEST_EXCEEDS_AVAILABLE,
    REQUEST_UNSAFE
} RequestResult;

static void compute_need(int p, int r,
                         int allocation[MAX_P][MAX_R],
                         int max[MAX_P][MAX_R],
                         int need[MAX_P][MAX_R]) {
    for (int i = 0; i < p; i++) {
        for (int j = 0; j < r; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }
}

static bool is_safe_state(int p, int r,
                          int available[MAX_R],
                          int allocation[MAX_P][MAX_R],
                          int need[MAX_P][MAX_R],
                          int safe_sequence[MAX_P]) {
    bool finished[MAX_P] = { false };
    int work[MAX_R];
    int completed = 0;

    for (int i = 0; i < r; i++) {
        work[i] = available[i];
    }

    while (completed < p) {
        bool found_process = false;

        for (int i = 0; i < p; i++) {
            if (finished[i]) {
                continue;
            }

            bool can_run = true;
            for (int j = 0; j < r; j++) {
                if (need[i][j] > work[j]) {
                    can_run = false;
                    break;
                }
            }

            if (can_run) {
                for (int j = 0; j < r; j++) {
                    work[j] += allocation[i][j];
                }
                safe_sequence[completed++] = i;
                finished[i] = true;
                found_process = true;
            }
        }

        if (!found_process) {
            return false;
        }
    }

    return true;
}

static RequestResult request_resources(int p, int r, int pid,
                                       int request[MAX_R],
                                       int available[MAX_R],
                                       int allocation[MAX_P][MAX_R],
                                       int need[MAX_P][MAX_R],
                                       int safe_sequence[MAX_P]) {
    for (int j = 0; j < r; j++) {
        if (request[j] > need[pid][j]) {
            return REQUEST_EXCEEDS_NEED;
        }
        if (request[j] > available[j]) {
            return REQUEST_EXCEEDS_AVAILABLE;
        }
    }

    for (int j = 0; j < r; j++) {
        available[j] -= request[j];
        allocation[pid][j] += request[j];
        need[pid][j] -= request[j];
    }

    if (!is_safe_state(p, r, available, allocation, need, safe_sequence)) {
        for (int j = 0; j < r; j++) {
            available[j] += request[j];
            allocation[pid][j] -= request[j];
            need[pid][j] += request[j];
        }
        return REQUEST_UNSAFE;
    }

    return REQUEST_GRANTED;
}

static void print_safe_sequence(int p, int safe_sequence[MAX_P]) {
    printf("Safe sequence: ");
    for (int i = 0; i < p; i++) {
        printf("P%d", safe_sequence[i]);
        if (i < p - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}

static bool load_default_case(int *p, int *r,
                              int allocation[MAX_P][MAX_R],
                              int max[MAX_P][MAX_R],
                              int available[MAX_R]) {
    *p = 5;
    *r = 3;

    int default_allocation[5][3] = {
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };

    int default_max[5][3] = {
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };

    int default_available[3] = {3, 3, 2};

    for (int i = 0; i < *p; i++) {
        for (int j = 0; j < *r; j++) {
            allocation[i][j] = default_allocation[i][j];
            max[i][j] = default_max[i][j];
        }
    }

    for (int j = 0; j < *r; j++) {
        available[j] = default_available[j];
    }

    return true;
}

static bool read_custom_case(int *p, int *r,
                             int allocation[MAX_P][MAX_R],
                             int max[MAX_P][MAX_R],
                             int available[MAX_R]) {
    printf("Enter number of processes (1-%d): ", MAX_P);
    if (scanf("%d", p) != 1 || *p < 1 || *p > MAX_P) {
        printf("Invalid process count.\n");
        return false;
    }

    printf("Enter number of resource types (1-%d): ", MAX_R);
    if (scanf("%d", r) != 1 || *r < 1 || *r > MAX_R) {
        printf("Invalid resource type count.\n");
        return false;
    }

    printf("Enter allocation matrix (%d x %d):\n", *p, *r);
    for (int i = 0; i < *p; i++) {
        for (int j = 0; j < *r; j++) {
            if (scanf("%d", &allocation[i][j]) != 1 || allocation[i][j] < 0) {
                printf("Invalid allocation value.\n");
                return false;
            }
        }
    }

    printf("Enter max matrix (%d x %d):\n", *p, *r);
    for (int i = 0; i < *p; i++) {
        for (int j = 0; j < *r; j++) {
            if (scanf("%d", &max[i][j]) != 1 || max[i][j] < 0) {
                printf("Invalid max value.\n");
                return false;
            }
            if (max[i][j] < allocation[i][j]) {
                printf("Invalid state: max[%d][%d] < allocation[%d][%d].\n", i, j, i, j);
                return false;
            }
        }
    }

    printf("Enter available vector (%d values):\n", *r);
    for (int j = 0; j < *r; j++) {
        if (scanf("%d", &available[j]) != 1 || available[j] < 0) {
            printf("Invalid available value.\n");
            return false;
        }
    }

    return true;
}

void run_lab6_demo(void) {
    int p = 0;
    int r = 0;
    int allocation[MAX_P][MAX_R] = { {0} };
    int max[MAX_P][MAX_R] = { {0} };
    int need[MAX_P][MAX_R] = { {0} };
    int available[MAX_R] = {0};
    int safe_sequence[MAX_P] = {0};

    int choice = 0;
    printf("Banker's Algorithm Lab\n");
    printf("------------------------\n");
    printf("1) Use built-in sample\n");
    printf("2) Enter custom input\n");
    printf("Choose option: ");

    if (scanf("%d", &choice) != 1) {
        printf("Invalid option.\n");
        return;
    }

    if (choice == 1) {
        if (!load_default_case(&p, &r, allocation, max, available)) {
            printf("Failed to load built-in case.\n");
            return;
        }
    } else if (choice == 2) {
        if (!read_custom_case(&p, &r, allocation, max, available)) {
            return;
        }
    } else {
        printf("Invalid option.\n");
        return;
    }

    compute_need(p, r, allocation, max, need);

    if (is_safe_state(p, r, available, allocation, need, safe_sequence)) {
        printf("System is in a SAFE state.\n");
        print_safe_sequence(p, safe_sequence);
    } else {
        printf("System is in an UNSAFE state.\n");
    }

    char simulate = 'n';
    printf("Simulate one resource request? (y/n): ");
    if (scanf(" %c", &simulate) != 1 || (simulate != 'y' && simulate != 'Y')) {
        return;
    }

    int pid = 0;
    int request[MAX_R] = {0};
    printf("Enter process id (0-%d): ", p - 1);
    if (scanf("%d", &pid) != 1 || pid < 0 || pid >= p) {
        printf("Invalid process id.\n");
        return;
    }

    printf("Enter request vector (%d values):\n", r);
    for (int j = 0; j < r; j++) {
        if (scanf("%d", &request[j]) != 1 || request[j] < 0) {
            printf("Invalid request value.\n");
            return;
        }
    }

    RequestResult result = request_resources(
        p, r, pid, request, available, allocation, need, safe_sequence
    );

    if (result == REQUEST_GRANTED) {
        printf("Request GRANTED. System remains SAFE.\n");
        print_safe_sequence(p, safe_sequence);
        return;
    }

    if (result == REQUEST_EXCEEDS_NEED) {
        printf("Request DENIED: exceeds process need.\n");
    } else if (result == REQUEST_EXCEEDS_AVAILABLE) {
        printf("Request DENIED: exceeds available resources.\n");
    } else {
        printf("Request DENIED: would make the system UNSAFE.\n");
    }
}
