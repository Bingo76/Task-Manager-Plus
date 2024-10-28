#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <ctype.h>
#include "cJSON.h"

// Constants
#define MAX_TASKS 100
#define TASK_FILE "tasks.json"
#define ANIMATION_DELAY 100000 // Microseconds

// Task Structure
typedef struct {
    char name[100];
    int completed;
    char last_completed[11]; // YYYY-MM-DD
    int streak;
} Task;

// Function Prototypes
void addTask(Task tasks[], int *task_count);
void markTaskAsDone(Task tasks[], int task_count);
void displayTasks(Task tasks[], int task_count);
void saveTasks(Task tasks[], int task_count);
int loadTasks(Task tasks[]);
int isSameDay(const char *date1, const char *date2);
void getCurrentDate(char *date_str);
void updateStreak(Task *task);
void animatedLoadingBar();
void getInput(char *input, int size);

// Main Menu
int main() {
    Task tasks[MAX_TASKS];
    int task_count = loadTasks(tasks);
    int choice;

    while (1) {
        printf("\n=== Task Manager Plus ===\n");
        printf("1. Add Task\n");
        printf("2. Mark Task as Done\n");
        printf("3. Display Tasks and Stats\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        
        char choiceInput[3];
        getInput(choiceInput, sizeof(choiceInput)); // Capture user input with backspace support
        choice = atoi(choiceInput); // Convert to integer

        switch (choice) {
            case 1:
                addTask(tasks, &task_count);
                break;
            case 2:
                markTaskAsDone(tasks, task_count);
                break;
            case 3:
                displayTasks(tasks, task_count);
                break;
            case 4:
                saveTasks(tasks, task_count);
                exit(0);
            default:
                printf("Invalid choice!\n");
        }

        animatedLoadingBar(); // Display animation at the bottom of the menu
    }

    return 0;
}

// Add a Task with backspace-supported input
void addTask(Task tasks[], int *task_count) {
    if (*task_count >= MAX_TASKS) {
        printf("Task limit reached.\n");
        return;
    }

    printf("Enter task name: ");
    getInput(tasks[*task_count].name, sizeof(tasks[*task_count].name)); // Capture input with backspace support
    tasks[*task_count].completed = 0;
    tasks[*task_count].streak = 0;
    strcpy(tasks[*task_count].last_completed, ""); // No completion yet
    (*task_count)++;

    printf("Task added successfully!\n");
}

// Mark Task as Done
void markTaskAsDone(Task tasks[], int task_count) {
    int i;
    char current_date[11];
    getCurrentDate(current_date);

    for (i = 0; i < task_count; i++) {
        printf("%d. %s [%s]\n", i + 1, tasks[i].name, tasks[i].completed ? "Done" : "Pending");
    }
    printf("Select task to mark as done (1-%d): ", task_count);
    
    char taskIndexInput[3];
    getInput(taskIndexInput, sizeof(taskIndexInput)); // Capture input with backspace support
    int task_index = atoi(taskIndexInput) - 1;

    if (task_index < 0 || task_index >= task_count) {
        printf("Invalid task.\n");
        return;
    }

    tasks[task_index].completed = 1;
    updateStreak(&tasks[task_index]);
    strcpy(tasks[task_index].last_completed, current_date);

    printf("Task marked as done!\n");
}

// Display Tasks and Stats
void displayTasks(Task tasks[], int task_count) {
    for (int i = 0; i < task_count; i++) {
        printf("Task: %s\n", tasks[i].name);
        printf("Status: %s\n", tasks[i].completed ? "Done" : "Pending");
        printf("Streak: %d days in a row\n", tasks[i].streak);
        printf("Last Completed: %s\n\n", tasks[i].last_completed);
    }
}

// Save tasks to JSON file
void saveTasks(Task tasks[], int task_count) {
    FILE *file = fopen(TASK_FILE, "w");
    if (!file) {
        printf("Could not open file for saving.\n");
        return;
    }

    cJSON *json = cJSON_CreateArray();
    for (int i = 0; i < task_count; i++) {
        cJSON *task_json = cJSON_CreateObject();
        cJSON_AddStringToObject(task_json, "name", tasks[i].name);
        cJSON_AddNumberToObject(task_json, "completed", tasks[i].completed);
        cJSON_AddStringToObject(task_json, "last_completed", tasks[i].last_completed);
        cJSON_AddNumberToObject(task_json, "streak", tasks[i].streak);
        cJSON_AddItemToArray(json, task_json);
    }

    char *json_str = cJSON_Print(json);
    fprintf(file, "%s", json_str);

    fclose(file);
    cJSON_Delete(json);
    free(json_str);
    printf("Tasks saved.\n");
}

// Load tasks from JSON file
int loadTasks(Task tasks[]) {
    FILE *file = fopen(TASK_FILE, "r");
    if (!file) {
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(len + 1);
    fread(data, 1, len, file);
    fclose(file);

    cJSON *json = cJSON_Parse(data);
    int task_count = 0;
    if (json) {
        int count = cJSON_GetArraySize(json);
        for (int i = 0; i < count; i++) {
            cJSON *task_json = cJSON_GetArrayItem(json, i);
            strcpy(tasks[task_count].name, cJSON_GetObjectItem(task_json, "name")->valuestring);
            tasks[task_count].completed = cJSON_GetObjectItem(task_json, "completed")->valueint;
            strcpy(tasks[task_count].last_completed, cJSON_GetObjectItem(task_json, "last_completed")->valuestring);
            tasks[task_count].streak = cJSON_GetObjectItem(task_json, "streak")->valueint;
            task_count++;
        }
        cJSON_Delete(json);
    }

    free(data);
    return task_count;
}

// Get Current Date in YYYY-MM-DD
void getCurrentDate(char *date_str) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date_str, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// Check if two dates are the same
int isSameDay(const char *date1, const char *date2) {
    return strcmp(date1, date2) == 0;
}

// Update Streak Count
void updateStreak(Task *task) {
    char current_date[11];
    getCurrentDate(current_date);

    if (isSameDay(current_date, task->last_completed)) {
        return;
    }

    struct tm prev_date = {0}, curr_date = {0};
    sscanf(task->last_completed, "%4d-%2d-%2d", &prev_date.tm_year, &prev_date.tm_mon, &prev_date.tm_mday);
    prev_date.tm_year -= 1900;
    prev_date.tm_mon -= 1;

    sscanf(current_date, "%4d-%2d-%2d", &curr_date.tm_year, &curr_date.tm_mon, &curr_date.tm_mday);
    curr_date.tm_year -= 1900;
    curr_date.tm_mon -= 1;

    double diff = difftime(mktime(&curr_date), mktime(&prev_date)) / (60 * 60 * 24);
    task->streak = (diff == 1) ? task->streak + 1 : 1;
}

// Animated Loading Bar
void animatedLoadingBar() {
    const char *boxes[] = {"[■ ■ ■ ■ ■]", "[□ ■ ■ ■ ■]", "[□ □ ■ ■ ■]", "[□ □ □ ■ ■]", "[□ □ □ □ ■]", "[□ □ □ □ □]"};
    for (int i = 0; i < 6; i++) {
        printf("\r%s", boxes[i]);
        fflush(stdout);
        usleep(ANIMATION_DELAY);
    }
    printf("\r               \r"); // Clear line after animation
}

// Custom Input with Backspace Handling
void getInput(char *input, int size) {
    struct termios oldt, newt;
    int ch, pos = 0;

    tcgetattr(STDIN_FILENO, &oldt); // Save current terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Disable buffering and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply new settings

    while ((ch = getchar()) != '\n' && pos < size - 1) {
        if (ch == 127) { // Handle backspace
            if (pos > 0) {
                printf("\b \b"); // Move back, print space, and move back again
                pos--;
            }
        } else if (isprint(ch)) { // Add printable character
            input[pos++] = ch;
            putchar(ch);
        }
    }
    input[pos] = '\0'; // Null-terminate input

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore terminal settings
    printf("\n");
}

