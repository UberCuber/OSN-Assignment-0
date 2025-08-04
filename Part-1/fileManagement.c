#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

char* to_string(int n) {
    if (n == 0) {
        char* zero = malloc(2);
        zero[0] = '0';
        zero[1] = '\0';
        return zero;
    }

    char* str = malloc(5); 
    int length = 0;
    while (n > 0) {
        str[length++] = (n % 10) + '0';
        n /= 10;
    }
    for (int i = 0; i < length / 2; i++) {
        char temp = str[i];
        str[i] = str[length - i - 1];
        str[length - i - 1] = temp;
    }
    str[length] = '\0'; 
    return str;
}

void writeToLogs(char* logsPath, char* log){
    int logs = open(logsPath, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (logs == -1) {
        perror("opening logs.txt failed");
        return;
    }
    dup2(logs, STDOUT_FILENO);
    close(logs);
    
    printf("%s\n", log);
    fflush(stdout);
}

void handleINPUT(char* logsPath, char* contentPath){
    printf("INPUT -> ");
    
    char line[1025];
    scanf(" %1024[^\n]", line);

    int original_stdout = dup(STDOUT_FILENO);

    writeToLogs(logsPath, "INPUT");

    int content = open(contentPath, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (content == -1) {
        perror("opening content.txt failed");
        return;
    }
    dup2(content, STDOUT_FILENO);
    close(content);
    printf("%s\n", line);

    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdout);
    return;
}

void handlePRINT(char* logsPath, char* contentPath){
    int original_stdout = dup(STDOUT_FILENO);
    writeToLogs(logsPath, "PRINT");
    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdout);

    int original_stdin = dup(STDIN_FILENO);

    int content = open(contentPath, O_RDONLY);
    if (content == -1) {
        perror("failed opening content.txt");
        return;
    }
    dup2(content, STDIN_FILENO);
    close(content);

    char line[1025];
    while (scanf(" %1024[^\n]%*c", line) == 1){
        printf("%s\n", line);
    }
    dup2(original_stdin, STDIN_FILENO);
    close(original_stdin);
    clearerr(stdin);
}

void handleFIRST(char* logsPath, char* contentPath){
    int n;
    scanf("%d", &n);

    // char dummy[1025];
    // scanf("%*[ ]"); // skip spaces
    // if (scanf("%1024[^\n]", dummy) == 1) {
    //     scanf("%*c"); // consume newline
    // }
    

    char* n_string = to_string(n);
    char operation[11] = "FIRST ";
    strcat(operation, n_string);

    int original_stdout = dup(STDOUT_FILENO);
    writeToLogs(logsPath, operation);
    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdout);

    int original_stdin = dup(STDIN_FILENO);
    int content = open(contentPath, O_RDONLY);
    if (content == -1) {
        perror("failed opening content.txt");
        return;
    }
    dup2(content, STDIN_FILENO);
    close(content);

    int count = 0;
    char line[1025];
    while (count < n && scanf("%1024[^\n]", line) == 1) {
        printf("%s\n", line);
        scanf("%*c");
        count++;
    }

    dup2(original_stdin, STDIN_FILENO);
    close(original_stdin);
    clearerr(stdin);
    free(n_string);
    return;
}

int main(){
    int pid = getpid();
    char* pid_string = to_string(pid);
    
    char dirName[15] = "folder_";
    strcat(dirName, pid_string);

    char logsPath[25];
    strcpy(logsPath, dirName);
    strcat(logsPath, "/logs.txt");

    char contentPath[28];
    strcpy(contentPath, dirName);
    strcat(contentPath, "/content.txt");

    if (mkdir(dirName, 0777) == -1) {
        perror("mkdir failed");
    }

    int lineCount = 0;
    while (1) {
        printf("Enter command: ");
        char op[6];
        scanf("%5s", op);

        if (!strcmp(op, "INPUT")) {
            lineCount++;
            handleINPUT(logsPath, contentPath);
        } 
        else if (!strcmp(op, "PRINT")) {
            handlePRINT(logsPath, contentPath);
        } 
        else if (!strcmp(op, "FIRST")) {
            handleFIRST(logsPath, contentPath);
        }
    
    }


}