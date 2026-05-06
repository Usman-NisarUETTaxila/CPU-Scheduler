#include <windows.h>
#include <stdlib.h>  
#include <stdio.h>
#include<stdbool.h> 
// structs
struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int completion_time;
};

struct Node {
    struct Process p;
    struct Node* next;
};

// Global Variables
int n;
struct Process process_arr[100]; // 100 max processess

HWND processes, arrival_t, burst_t, algorithms, table, gantt_chart, avg_values;

// helper functions
void AppendText(HWND hDisplay, const char *text) {
    int len = GetWindowTextLength(hDisplay);
    SendMessage(hDisplay, EM_SETSEL, len, len);
    SendMessage(hDisplay, EM_REPLACESEL, 0, (LPARAM)text);
}

void clearOutput() {
    SetWindowText(table, "");
    SetWindowText(gantt_chart, "");
    SetWindowText(avg_values, "");
}

void clearAll() {
    SetWindowText(processes, "");
    SetWindowText(arrival_t, "");
    SetWindowText(burst_t, "");
    SendMessage(algorithms, CB_SETCURSEL, 0, 0);
    SetWindowText(table, "");
    SetWindowText(gantt_chart, "");
    SetWindowText(avg_values, "");
}

void displayAvgValues(int t_wt, int t_tat){
    char temp[264];
    double avg_wt = (double) t_wt/n;
    snprintf(temp, sizeof(temp), "Average Waiting Time: %.2f \n", avg_wt);
    AppendText(avg_values,temp);
    memset(temp, 0, sizeof(temp));

    double avg_tat = (double) t_tat/n;
    snprintf(temp, sizeof(temp), "Average Turnaround Time: %.2f", avg_tat);
    AppendText(avg_values,temp);
    memset(temp, 0, sizeof(temp));
}

// Window Procedure (handles messages)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == 105) {  
                clearOutput();
                char buffer[256];
                char temp[20];
                int j=0;
                int k=0;

                GetWindowText(processes, buffer, 256);
                n = atoi(buffer);

                if(n>100){
                    MessageBox(
                        hwnd, 
                        "Processes cannot be more than 100!", 
                        "Error", 
                        MB_OK | MB_ICONERROR
                    );
                    clearAll();
                    return DefWindowProc(hwnd, msg, wParam, lParam);
                }

                memset(buffer, 0, sizeof(buffer));

                GetWindowText(arrival_t, buffer, 256);
                for(int i=0; buffer[i] != '\0';i++){
                    if(buffer[i] != ','){
                        temp[j] = buffer[i];
                        j++;
                    } else {
                        process_arr[k].arrival_time = atoi(temp);
                        process_arr[k].pid = k+1;
                        k++;
                        memset(temp, 0, sizeof(temp));
                        j=0;
                    }
                }

                process_arr[k].arrival_time = atoi(temp);
                process_arr[k].pid = k+1;
                if(k!=n-1){
                    MessageBox(
                        hwnd, 
                        "Invalid Input of Arrival Time!", 
                        "Error", 
                        MB_OK | MB_ICONERROR
                    );
                    clearAll();
                    return DefWindowProc(hwnd, msg, wParam, lParam);
                }
                memset(buffer, 0, sizeof(buffer));
                memset(temp, 0, sizeof(temp));
                j=0;
                k=0;

                GetWindowText(burst_t, buffer, 256);
                for(int i=0; buffer[i] != '\0';i++){
                    if(buffer[i] != ','){
                        temp[j] = buffer[i];
                        j++;
                    } else {
                        process_arr[k].burst_time = atoi(temp);
                        k++;
                        memset(temp, 0, sizeof(temp));
                        j=0;
                    }
                }

                process_arr[k].burst_time = atoi(temp);
                memset(buffer, 0, sizeof(buffer));
                memset(temp, 0, sizeof(temp));
                if(k!=n-1){
                    MessageBox(
                        hwnd, 
                        "Invalid Input of Burst Time!", 
                        "Error", 
                        MB_OK | MB_ICONERROR
                    );
                    clearAll();
                    return DefWindowProc(hwnd, msg, wParam, lParam);
                }
                j=0;
                k=0;

                int index = SendMessage(algorithms, CB_GETCURSEL, 0, 0);

                if (index == CB_ERR) {
                    MessageBox(hwnd, "Please select an option!", "Error", MB_OK);
                    break;
                }

                SendMessage(algorithms, CB_GETLBTEXT, index, (LPARAM)buffer);

                

                // Use index to run different logic
                switch (index) {
                    case 0:{
                        // FCFS Algo

                        // sorting 
                        for(int i=0;i<n;i++){
                            for(int j=0;j<n-1;j++){
                                if(process_arr[j+1].arrival_time < process_arr[j].arrival_time){
                                    struct Process temp = process_arr[j+1];
                                    process_arr[j+1] = process_arr[j];
                                    process_arr[j] = temp;

                                }
                            }
                        }


                        // execution
                        // gantt chart
                        char temp[256];
                        snprintf(temp, sizeof(temp), "Gantt Chart: \n");
                        AppendText(gantt_chart,temp);
                        memset(temp, 0, sizeof(temp));

                        int i=0;
                        int time=0;
                        while(i<n){
                            snprintf(temp, sizeof(temp), " P%d | ", process_arr[i].pid);
                            AppendText(gantt_chart,temp);
                            memset(temp, 0, sizeof(temp));
                            time++;

                            if(time == process_arr[i].burst_time){
                                i++;
                                time=0;
                            }
                        }

                        // table
                        snprintf(temp, sizeof(temp), "FCFS Table: \n");
                        AppendText(table,temp);
                        memset(temp, 0, sizeof(temp));

                        snprintf(temp, sizeof(temp), "P     AT    BT    WT   TAT \n");
                        AppendText(table,temp);
                        memset(temp, 0, sizeof(temp));
                        int ct=0;
                        int t_tat=0;
                        int t_wt=0;
                        for(int i=0;i<n;i++) {
                            ct+=process_arr[i].burst_time;
                            int tat=ct;
                            int wt = tat - process_arr[i].burst_time;
                            t_tat+=tat;
                            t_wt+=wt;
                            snprintf(temp, sizeof(temp), "P%d    %d     %d       %d       %d \n", process_arr[i].pid, process_arr[i].arrival_time, process_arr[i].burst_time, wt, tat);
                            AppendText(table,temp);
                            memset(temp, 0, sizeof(temp));
                        }

                        displayAvgValues(t_wt,t_tat);

                        break;
                    }
                    case 1:{
                        // SJF Non premptive
                        // sorting
                        for(int i=0;i<n;i++){
                            for(int j=0;j<n-1;j++){
                                if(process_arr[j+1].arrival_time < process_arr[j].arrival_time){
                                    struct Process temp = process_arr[j+1];
                                    process_arr[j+1] = process_arr[j];
                                    process_arr[j] = temp;

                                }
                            }
                        }

                        // SJF execution
                        struct Node dummy;
                        struct Node first = {process_arr[0],NULL};
                        dummy.next = &first;
                        struct Node* head = dummy.next;
                        int time=0;
                        char temp[256];
                        snprintf(temp, sizeof(temp), "Gantt Chart: \n");
                        AppendText(gantt_chart,temp);
                        memset(temp, 0, sizeof(temp));

                        while(head!=NULL){
                            if(time != 0 && time <= process_arr[n-1].arrival_time){
                                for(int i=1;i<n;i++){
                                    if(process_arr[i].arrival_time == time){
                                        // node creation
                                        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
                                        
                                        newNode->p = process_arr[i];
                                        newNode->next = NULL;
                                        
                                        // node insertion
                                        struct Node* curr = head->next;
                                        struct Node* prev = head;
                                        bool has_inserted = false;
                                        while(curr!=NULL){
                                            if(newNode->p.burst_time < curr->p.burst_time){
                                                prev->next = newNode;
                                                newNode->next = curr;
                                                has_inserted = true;
                                            }
                                            prev = curr;
                                            curr = curr->next;
                                        }

                                        if(!has_inserted){
                                            prev->next = newNode;
                                            newNode->next = NULL;
                                        }
                                        
                                    }
                                }
                            }

                            snprintf(temp, sizeof(temp), " P%d | ", head->p.pid);
                            AppendText(gantt_chart,temp);
                            memset(temp, 0, sizeof(temp));
                            head->p.burst_time--;
                            if(head->p.burst_time == 0){
                                for(int i=0; i<n;i++){
                                    if(head->p.pid == process_arr[i].pid){
                                        process_arr[i].completion_time = time+1;
                                    }
                                }
                                head = head->next;
                            }
                            
                            time++;
                        }

                        // table
                        snprintf(temp, sizeof(temp), "SJF Table: \n");
                        AppendText(table,temp);
                        memset(temp, 0, sizeof(temp));

                        snprintf(temp, sizeof(temp), "P     AT    BT    WT   TAT \n");
                        AppendText(table,temp);
                        memset(temp, 0, sizeof(temp));
                        int ct=0;
                        int t_tat=0;
                        int t_wt=0;
                        for(int i=0;i<n;i++) {
                            ct+=process_arr[i].burst_time;
                            int tat= process_arr[i].completion_time - process_arr[i].arrival_time;
                            int wt = tat - process_arr[i].burst_time;
                            t_tat+=tat;
                            t_wt+=wt;
                            snprintf(temp, sizeof(temp), "P%d    %d     %d       %d       %d \n", process_arr[i].pid, process_arr[i].arrival_time, process_arr[i].burst_time, wt, tat);
                            AppendText(table,temp);
                            memset(temp, 0, sizeof(temp));
                        }

                        displayAvgValues(t_wt,t_tat);

                        break;
                    }
                    case 2:{
                        // SRTF Premptive
                        // sorting
                        for(int i=0;i<n;i++){
                            for(int j=0;j<n-1;j++){
                                if(process_arr[j+1].arrival_time < process_arr[j].arrival_time){
                                    struct Process temp = process_arr[j+1];
                                    process_arr[j+1] = process_arr[j];
                                    process_arr[j] = temp;

                                }
                            }
                        }

                        // SRTF execution
                        struct Node dummy;
                        struct Node first = {process_arr[0],NULL};
                        dummy.next = &first;
                        struct Node* head = dummy.next;
                        int time=0;
                        char temp[256];
                        snprintf(temp, sizeof(temp), "Gantt Chart: \n");
                        AppendText(gantt_chart,temp);
                        memset(temp, 0, sizeof(temp));

                        while(head!=NULL){
                            if(time != 0 && time <= process_arr[n-1].arrival_time){
                                for(int i=1;i<n;i++){
                                    if(process_arr[i].arrival_time == time){
                                        // node creation
                                        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
                                        
                                        newNode->p = process_arr[i];
                                        newNode->next = NULL;
                                        
                                        // node insertion
                                        if(head == NULL || newNode->p.burst_time < head->p.burst_time){
                                            newNode->next = head;
                                            head = newNode;
                                        } else {
                                            struct Node* curr = head->next;
                                            struct Node* prev = head;
                                            bool has_inserted = false;
                                            while(curr!=NULL){
                                                if(newNode->p.burst_time < curr->p.burst_time){
                                                    prev->next = newNode;
                                                    newNode->next = curr;
                                                    has_inserted = true;
                                                }
                                                prev = curr;
                                                curr = curr->next;
                                            }

                                            if(!has_inserted){
                                                prev->next = newNode;
                                                newNode->next = NULL;
                                            }
                                        }
                                    }
                                }
                            }

                            snprintf(temp, sizeof(temp), " P%d | ", head->p.pid);
                            AppendText(gantt_chart,temp);
                            memset(temp, 0, sizeof(temp));
                            head->p.burst_time--;
                            if(head->p.burst_time == 0){
                                for(int i=0; i<n;i++){
                                    if(head->p.pid == process_arr[i].pid){
                                        process_arr[i].completion_time = time+1;
                                    }
                                }
                                head = head->next;
                            }
                            
                            time++;
                        }

                        // table
                        snprintf(temp, sizeof(temp), "SRTF Table: \n");
                        AppendText(table,temp);
                        memset(temp, 0, sizeof(temp));

                        snprintf(temp, sizeof(temp), "P     AT    BT    WT   TAT \n");
                        AppendText(table,temp);
                        memset(temp, 0, sizeof(temp));
                        int ct=0;
                        int t_tat=0;
                        int t_wt=0;
                        for(int i=0;i<n;i++) {
                            ct+=process_arr[i].burst_time;
                            int tat= process_arr[i].completion_time - process_arr[i].arrival_time;
                            int wt = tat - process_arr[i].burst_time;
                            t_tat+=tat;
                            t_wt+=wt;
                            snprintf(temp, sizeof(temp), "P%d    %d     %d       %d       %d \n", process_arr[i].pid, process_arr[i].arrival_time, process_arr[i].burst_time, wt, tat);
                            AppendText(table,temp);
                            memset(temp, 0, sizeof(temp));
                        }

                        displayAvgValues(t_wt,t_tat);

                        break;
                    }
                }

                
            } else if (LOWORD(wParam) == 106) {
                clearOutput();
            } else if(LOWORD(wParam) == 107) {
                clearAll();
            } else if(LOWORD(wParam) == 108) {
                DestroyWindow(hwnd);
            } 
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Entry point for Windows GUI apps
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {

    // 1. Register Window Class
    WNDCLASS wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = "MyWindowClass";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    // 2. Create the Window
    HWND hwnd = CreateWindow(
        "MyWindowClass", "CPU Scheduler",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
        NULL, NULL, hInstance, NULL
    );

    CreateWindow(
        "STATIC", "Processes:",
        WS_VISIBLE | WS_CHILD,
        25, 25, 100, 25,
        hwnd, NULL, hInstance, NULL
    );

    CreateWindow(
        "STATIC", "Arrival_T:",
        WS_VISIBLE | WS_CHILD,
        25, 75, 100, 25,
        hwnd, NULL, hInstance, NULL
    );

    CreateWindow(
        "STATIC", "Burst_T:",
        WS_VISIBLE | WS_CHILD,
        25, 125, 100, 25,
        hwnd, NULL, hInstance, NULL
    );

    CreateWindow(
        "STATIC", "Algorithm:",
        WS_VISIBLE | WS_CHILD,
        25, 175, 100, 25,
        hwnd, NULL, hInstance, NULL
    );

    processes = CreateWindow("EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
        125, 25, 200, 25,
        hwnd, (HMENU)101, hInstance, NULL);
    
    arrival_t = CreateWindow("EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        125, 75, 200, 25,
        hwnd, (HMENU)102, hInstance, NULL);
    
    burst_t = CreateWindow("EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        125, 125, 200, 25,
        hwnd, (HMENU)103, hInstance, NULL);
    
    algorithms = CreateWindow(
        "COMBOBOX", "",
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        125, 175, 200, 200,   
        hwnd, (HMENU)104, hInstance, NULL
    );

    SendMessage(algorithms, CB_ADDSTRING, 0, (LPARAM)"FCFS");
    SendMessage(algorithms, CB_ADDSTRING, 0, (LPARAM)"SJF");
    SendMessage(algorithms, CB_ADDSTRING, 0, (LPARAM)"SRTF");
    SendMessage(algorithms, CB_SETCURSEL, 0, 0);

    CreateWindow(
    "BUTTON", "Run",        
    WS_VISIBLE | WS_CHILD,      
    25, 225, 120, 35,            
    hwnd, (HMENU)105, hInstance, NULL  
    );

    CreateWindow(
    "BUTTON", "Clear",        
    WS_VISIBLE | WS_CHILD,      
    170, 225, 120, 35,            
    hwnd, (HMENU)106, hInstance, NULL  
    );

    CreateWindow(
    "BUTTON", "Reset",        
    WS_VISIBLE | WS_CHILD,      
    315, 225, 120, 35,            
    hwnd, (HMENU)107, hInstance, NULL  
    );

    CreateWindow(
    "BUTTON", "Exit",        
    WS_VISIBLE | WS_CHILD,      
    460, 225, 120, 35,            
    hwnd, (HMENU)108, hInstance, NULL  
    );

    table = CreateWindow("EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL |
        ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        25, 275, 700, 150,
        hwnd, (HMENU)110, hInstance, NULL);
    
    gantt_chart = CreateWindow("EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL |
        ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        25, 475, 700, 50,
        hwnd, (HMENU)111, hInstance, NULL);

    avg_values = CreateWindow("EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL |
        ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        25, 575, 700, 50,
        hwnd, (HMENU)112, hInstance, NULL);


    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 3. Message Loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}