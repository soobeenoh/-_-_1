/*
 * 26-1 PBP Project — Part 3 Base Code
 *
 * P1·P2의 모든 기능 + P3의 비핵심 함수까지 베이스에 완성되어 있다.
 * 학생이 직접 작성할 부분은 다음 3개 (T1·T2·T3):
 *
 *   T1. saveTasksToTextFile()     — CSV 저장 (fopen + writeCsvField 활용)
 *   T2. loadTasksFromTextFile()   — CSV 불러오기 (fgets + readCsvField 활용)
 *   T3. compareBy*()              — qsort용 비교 함수 3개:
 *                                     compareById       (ID 오름차순)
 *                                     compareByPriority (높음 → 낮음 내림)
 *                                     compareByDeadline (임박 → 여유, 미설정은 뒤)
 *
 * 베이스 코드로 제공 (수정하지 말 것):
 *   - 모든 P1·P2 기능
 *   - 정렬 호출부 (pickComparator로 비교 함수 선택, sortTasks → qsort 호출)
 *   - 학습 비교 참조용 비교 함수 3개:
 *       compareByStatus, compareByModifiedAt, compareByCreatedAt
 *   - saveTasksToBinaryFile / loadTasksFromBinaryFile (단일 fwrite/fread 패턴)
 *   - autoLoadTasksOnStartup / autoSaveTasksOnExit (메인 흐름 통합)
 *   - initializeTask 확장 (createdAt/modifiedAt/deadline + 첫 이력)
 *   - updateTask / updateTasksStatus / updateTasksPriority의 appendHistory 호출
 *   - appendHistory (FIFO 선입선출 — 가장 오래된 항목부터 밀려나는 오버플로 처리 포함)
 *   - formatTimestamp, parseDateTime (strftime/sscanf 예시)
 *   - writeCsvField / readCsvField (CSV escape 헬퍼) — T1·T2에서 호출만
 *   - 메뉴 골격, 입력 헬퍼
 *
 * 작업 기간이 1주이므로 P1~P2가 완성된 상태에서 시작할 것.
 * 권장 작성 순서: T3 (비교 함수) → T1 (CSV 저장) → T2 (CSV 불러오기).
 *
 * 컴파일: cc -Wall -Wextra -std=c11 -o project_3 project_3.c
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

/* === 상수 ============================================================= */
#define MAX_STRING_INPUT_LENGTH 500
#define MAX_TASK_TITLE_LENGTH 100
#define MAX_TASK_DESCRIPTION_LENGTH 500
#define MAX_STATUS_PRIORITY_STRING_LENGTH 20
#define MAX_FILENAME_LENGTH 256
#define MAX_HISTORY 32
#define MAX_HISTORY_EVENT_LENGTH 200
#define MAX_TIME_STRING 64
#define DEFAULT_BINARY_FILENAME "tasks.dat"
#define BINARY_FILE_MAGIC_NUMBER 0x5441534B  /* "TASK" */
#define CHANGED_FIELD_COUNT 5

/* === 열거형 =========================================================== */
typedef enum {
    TODO = 0, IN_PROGRESS = 1, DONE = 2,
    STATUS_NOT_SET = -1
} TaskStatus;

typedef enum {
    LOW = 0, MEDIUM = 1, HIGH = 2,
    PRIORITY_NOT_SET = -1
} TaskPriority;

typedef enum { EMPTY_INPUT, HAS_INPUT } UserInputType;

/* viewTasksMenu의 2단계(정렬 기준) 번호와 1:1 매핑.
 * 0번(SORT_NONE)은 "정렬 안 함" — 입력 순서 그대로 출력. */
typedef enum {
    SORT_NONE = 0,
    SORT_BY_ID = 1,
    SORT_BY_PRIORITY = 2,
    SORT_BY_STATUS = 3,
    SORT_BY_DEADLINE = 4,
    SORT_BY_MODIFIED_AT = 5,
    SORT_BY_CREATED_AT = 6
} SortType;

/* === 자료구조 ========================================================= */
typedef struct {
    time_t timestamp;
    char event[MAX_HISTORY_EVENT_LENGTH];
} HistoryEntry;

typedef struct {
    int id;
    char title[MAX_TASK_TITLE_LENGTH];
    char description[MAX_TASK_DESCRIPTION_LENGTH];
    TaskStatus status;
    TaskPriority priority;
    time_t createdAt;
    time_t modifiedAt;
    time_t deadline;                         /* 0 = 미설정 */
    HistoryEntry history[MAX_HISTORY];
    int historyCount;
} Task;

typedef struct TaskNode {
    Task task;
    struct TaskNode *next;
} TaskNode;

/* === 함수 프로토타입 ================================================== */
/* 메뉴 */
int displayAndSelectMainMenu(void);
void viewTasksMenu(TaskNode **head);
void updateTasksMenu(TaskNode **head);
void deleteTasksMenu(TaskNode **head);
void fileMenu(TaskNode **head, int *nextId);
void testMenu(TaskNode **head, int *nextId);
int selectMenu(const char *title, int count, const int indexes[], const char *names[]);

/* 입력 */
UserInputType getUserInput(char *buf);
int scanId(void);
bool parseFirstIntValue(const char *input, int *out);
bool parseFirstNIntValues(const char *input, int values[], int max, int *parsedCount);
bool parseDateTime(const char *input, time_t *out);

/* 표시 */
const char *statusToString(TaskStatus s);
const char *priorityToString(TaskPriority p);
char *formatTimestamp(time_t t, char *buf, size_t bufSize);
void displayTask(const Task *task, bool shortFormat);
void displayTaskWithHistory(const Task *task);
void displayTaskHistory(const Task *task);

/* CRUD */
void createTask(TaskNode **head, int *nextId);
void initializeTask(Task *task, int id);
TaskNode *createTaskNode(Task task);
TaskNode *findTaskNodeById(TaskNode *head, int id);
int countTasks(TaskNode *head);
void appendTaskNode(TaskNode **head, TaskNode *newNode);
Task deepCopyTask(Task task);

/* 수정 */
void updateTaskById(TaskNode *head);
void updateTask(Task *task);
void updateTasksStatus(TaskNode *head);
void updateTasksPriority(TaskNode *head);

/* 삭제 */
void deleteTaskById(TaskNode **head);
void deleteAllTasks(TaskNode **head);
void freeTaskNode(TaskNode *node);
void freeAllTasks(TaskNode **head);

/* 이력 */
void appendHistory(Task *task, const char *event);

/* 정렬 (뷰 전용) */
int snapshotTaskList(TaskNode *head, Task *snapshot, int max);
typedef int (*TaskCmp)(const void *, const void *);
TaskCmp pickComparator(SortType sortType);
int compareById(const void *a, const void *b);
int compareByPriority(const void *a, const void *b);
int compareByStatus(const void *a, const void *b);
int compareByDeadline(const void *a, const void *b);
int compareByModifiedAt(const void *a, const void *b);
int compareByCreatedAt(const void *a, const void *b);
void sortTasks(Task *snapshot, int count, SortType sortType);
void displayTasksSorted(TaskNode *head, SortType sortType);

/* 파일 I/O */
int saveTasksToTextFile(TaskNode *head, const char *filename);
int loadTasksFromTextFile(TaskNode **head, int *nextId, const char *filename);
int saveTasksToBinaryFile(TaskNode *head, const char *filename);
int loadTasksFromBinaryFile(TaskNode **head, int *nextId, const char *filename);
void autoLoadTasksOnStartup(TaskNode **head, int *nextId);
void autoSaveTasksOnExit(TaskNode *head);

/* 디버그 */
void observeMemoryUsage(TaskNode **head, int *nextId);
void generateDummyTasks(TaskNode **head, int *nextId, int count);

/* === main =============================================================== */
int main(void) {
    TaskNode *head = NULL;
    int nextId = 1;
    int menuChoice;

    printf("========== 작업 관리 시스템 시작 ==========\n");
    autoLoadTasksOnStartup(&head, &nextId);

    while ((menuChoice = displayAndSelectMainMenu()) != 0) {
        switch (menuChoice) {
            case 1: createTask(&head, &nextId); break;
            case 2: viewTasksMenu(&head); break;
            case 3: updateTasksMenu(&head); break;
            case 4: deleteTasksMenu(&head); break;
            case 5: fileMenu(&head, &nextId); break;
            case 9: testMenu(&head, &nextId); break;
        }
    }

    printf("\n========== 프로그램 종료 ==========\n");
    autoSaveTasksOnExit(head);
    printf("\n메모리를 정리합니다...\n");
    freeAllTasks(&head);
    printf("프로그램을 종료합니다. 감사합니다!\n");
    return 0;
}

/* === 메뉴 ============================================================== */
int displayAndSelectMainMenu(void) {
    const int indexes[] = {1, 2, 3, 4, 5, 9, 0};
    const char *names[] = {
        "작업 추가", "작업 보기", "작업 수정", "작업 삭제",
        "파일", "테스트", "종료"
    };
    return selectMenu("작업 관리 시스템",
                      sizeof(indexes) / sizeof(indexes[0]),
                      indexes, names);
}

/* 보기 메뉴 — 2단계 nested.
 * 1단계: 전체 보기 / 단건 검색
 * 2단계(전체 보기 선택 시): 정렬 기준 0~6 선택 */
void viewTasksMenu(TaskNode **head) {
    const int outerIndexes[] = {1, 2, 0};
    const char *outerNames[] = {"전체 작업 보기", "작업 검색 (ID)", "돌아가기"};

    while (true) {
        int sel = selectMenu("작업 보기",
                             sizeof(outerIndexes) / sizeof(outerIndexes[0]),
                             outerIndexes, outerNames);
        if (sel == 0) return;
        if (sel == 1) {
            if (*head == NULL) {
                printf("[안내] 등록된 작업이 없습니다.\n");
                continue;
            }
            const int sortIdx[] = {0, 1, 2, 3, 4, 5, 6};
            const char *sortNames[] = {
                "입력 순서 (정렬 안 함)",
                "ID 오름차순",
                "우선순위 (높음 → 낮음)",
                "상태 (할 일 → 완료)",
                "마감 시각 (임박 → 여유)",
                "수정 시각 (최신 순)",
                "생성 시각 (최신 순)"
            };
            int sortType = selectMenu("정렬 기준 선택",
                                      sizeof(sortIdx) / sizeof(sortIdx[0]),
                                      sortIdx, sortNames);
            displayTasksSorted(*head, (SortType)sortType);
        } else if (sel == 2) {
            if (*head == NULL) {
                printf("[안내] 등록된 작업이 없습니다.\n");
                continue;
            }
            printf("\n=== 작업 검색 (ID) ===\n");
            printf("작업 ID를 입력하세요(0: 돌아가기): ");
            int id = scanId();
            if (id <= 0) continue;
            TaskNode *node = findTaskNodeById(*head, id);
            if (node == NULL) {
                printf("[오류] 해당 ID의 작업을 찾을 수 없습니다.\n");
                continue;
            }
            displayTaskWithHistory(&node->task);
        }
    }
}

void updateTasksMenu(TaskNode **head) {
    const int indexes[] = {1, 2, 3, 0};
    const char *names[] = {
        "단일 작업 수정", "작업 상태 일괄 변경",
        "작업 우선순위 일괄 변경", "돌아가기"
    };
    while (true) {
        if (*head == NULL) {
            printf("[안내] 등록된 작업이 없습니다.\n");
            return;
        }
        int sel = selectMenu("작업 수정",
                             sizeof(indexes) / sizeof(indexes[0]),
                             indexes, names);
        switch (sel) {
            case 1: updateTaskById(*head); break;
            case 2: updateTasksStatus(*head); break;
            case 3: updateTasksPriority(*head); break;
            case 0: return;
        }
    }
}

void deleteTasksMenu(TaskNode **head) {
    const int indexes[] = {1, 2, 0};
    const char *names[] = {"작업 삭제(ID)", "전체 작업 삭제", "돌아가기"};
    while (true) {
        if (*head == NULL) {
            printf("[안내] 등록된 작업이 없습니다.\n");
            return;
        }
        int sel = selectMenu("작업 삭제",
                             sizeof(indexes) / sizeof(indexes[0]),
                             indexes, names);
        switch (sel) {
            case 1: deleteTaskById(head); break;
            case 2: deleteAllTasks(head); break;
            case 0: return;
        }
    }
}

void fileMenu(TaskNode **head, int *nextId) {
    const int indexes[] = {1, 2, 3, 4, 0};
    const char *names[] = {
        "CSV로 저장",
        "CSV에서 불러오기",
        "바이너리로 저장",
        "바이너리에서 불러오기",
        "돌아가기"
    };
    char filename[MAX_FILENAME_LENGTH];
    int sel = selectMenu("파일",
                         sizeof(indexes) / sizeof(indexes[0]),
                         indexes, names);
    if (sel == 0) return;
    if ((sel == 1 || sel == 3) && *head == NULL) {
        printf("[안내] 저장할 작업이 없습니다.\n");
        return;
    }
    printf("파일 이름을 입력하세요: ");
    if (getUserInput(filename) != HAS_INPUT) {
        printf("[오류] 파일 이름이 입력되지 않았습니다.\n");
        return;
    }
    int rc = 0;
    switch (sel) {
        case 1: rc = saveTasksToTextFile(*head, filename); break;
        case 2: rc = loadTasksFromTextFile(head, nextId, filename); break;
        case 3: rc = saveTasksToBinaryFile(*head, filename); break;
        case 4: rc = loadTasksFromBinaryFile(head, nextId, filename); break;
    }
    if (rc < 0) printf("[오류] 작업이 실패했습니다.\n");
}

void testMenu(TaskNode **head, int *nextId) {
    const int indexes[] = {1, 2, 0};
    const char *names[] = {
        "프로그램 메모리 사용량 관찰",
        "더미 작업 생성",
        "돌아가기"
    };
    int sel = selectMenu("테스트",
                         sizeof(indexes) / sizeof(indexes[0]),
                         indexes, names);
    if (sel == 1) observeMemoryUsage(head, nextId);
    else if (sel == 2) {
        printf("생성할 작업 개수: ");
        int n = scanId();
        if (n > 0) generateDummyTasks(head, nextId, n);
    }
}

int selectMenu(const char *title, int count, const int indexes[], const char *names[]) {
    int min = indexes[0], max = indexes[0];
    for (int i = 1; i < count; i++) {
        if (indexes[i] < min) min = indexes[i];
        if (indexes[i] > max) max = indexes[i];
    }
    while (true) {
        printf("\n=== %s ===\n", title);
        for (int i = 0; i < count; i++) {
            printf("%d. %s\n", indexes[i], names[i]);
        }
        printf("선택: ");
        int sel = scanId();
        for (int i = 0; i < count; i++) {
            if (sel == indexes[i]) {
                printf("\n");
                return sel;
            }
        }
        printf("[오류] 잘못된 선택입니다. 메뉴는 %d~%d 사이의 값이어야 합니다.\n", min, max);
    }
}

/* === 입력 처리 ========================================================= */
UserInputType getUserInput(char *buf) {
    if (fgets(buf, MAX_STRING_INPUT_LENGTH, stdin) == NULL) {
        buf[0] = '\0';
        return EMPTY_INPUT;
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[--len] = '\0';
    }
    if (len == 0) return EMPTY_INPUT;
    if (len >= MAX_STRING_INPUT_LENGTH - 1) {
        printf("[주의] %d바이트 초과 입력은 무시됩니다.\n", MAX_STRING_INPUT_LENGTH);
        int c;
        while ((c = getchar()) != '\n' && c != EOF) ;
    }
    return HAS_INPUT;
}

int scanId(void) {
    char buf[MAX_STRING_INPUT_LENGTH];
    int v = 0;
    if (getUserInput(buf) != HAS_INPUT) return -1;
    if (parseFirstIntValue(buf, &v)) return v;
    return -1;
}

bool parseFirstIntValue(const char *input, int *out) {
    int i = 0, sign = 1, value = 0;
    while (input[i] == ' ') i++;
    if (input[i] == '+') i++;
    else if (input[i] == '-') { sign = -1; i++; }
    if (input[i] < '0' || input[i] > '9') return false;
    while (input[i] >= '0' && input[i] <= '9') {
        value = value * 10 + (input[i] - '0');
        i++;
    }
    if (input[i] != ' ' && input[i] != '\0') return false;
    *out = sign * value;
    return true;
}

bool parseFirstNIntValues(const char *input, int values[], int max, int *parsedCount) {
    int ci = 0, vi = 0;
    *parsedCount = 0;
    while (input[ci] == ' ' || input[ci] == '\t') ci++;
    while (vi < max) {
        int sign = 1;
        if (input[ci] == '+') ci++;
        else if (input[ci] == '-') { sign = -1; ci++; }
        if (input[ci] < '0' || input[ci] > '9') return false;
        int value = 0;
        while (input[ci] >= '0' && input[ci] <= '9') {
            value = value * 10 + (input[ci] - '0');
            ci++;
        }
        values[vi++] = sign * value;
        (*parsedCount)++;
        if (input[ci] == '\0') return true;
        while (input[ci] == ' ' || input[ci] == '\t') ci++;
        if (input[ci] == '\0') return true;
    }
    return true;
}

bool parseDateTime(const char *input, time_t *out) {
    int y, M, d, h, m;
    if (sscanf(input, "%d-%d-%d %d:%d", &y, &M, &d, &h, &m) != 5) return false;
    if (y < 1900 || y > 2100 || M < 1 || M > 12 || d < 1 || d > 31) return false;
    if (h < 0 || h > 23 || m < 0 || m > 59) return false;
    struct tm tm = {0};
    tm.tm_year = y - 1900; tm.tm_mon = M - 1; tm.tm_mday = d;
    tm.tm_hour = h; tm.tm_min = m; tm.tm_isdst = -1;
    *out = mktime(&tm);
    return *out != -1;
}

/* === 표시 ============================================================== */
const char *statusToString(TaskStatus s) {
    switch (s) {
        case TODO: return "할 일";
        case IN_PROGRESS: return "진행 중";
        case DONE: return "완료";
        default: return "알 수 없음";
    }
}

const char *priorityToString(TaskPriority p) {
    switch (p) {
        case LOW: return "낮음";
        case MEDIUM: return "보통";
        case HIGH: return "높음";
        default: return "알 수 없음";
    }
}

char *formatTimestamp(time_t t, char *buf, size_t bufSize) {
    if (t == 0) { snprintf(buf, bufSize, "미설정"); return buf; }
    struct tm *tm = localtime(&t);
    if (tm == NULL) { snprintf(buf, bufSize, "오류"); return buf; }
    strftime(buf, bufSize, "%Y-%m-%d %H:%M:%S", tm);
    return buf;
}

void displayTask(const Task *task, bool shortFormat) {
    char tbuf[MAX_TIME_STRING];
    if (shortFormat) {
        printf("│ [작업 #%d] %s | %s | %s\n",
               task->id, task->title,
               statusToString(task->status),
               priorityToString(task->priority));
        return;
    }
    printf("┌────────────────────────────────────\n");
    printf("│ [작업 #%d]\n", task->id);
    printf("│ 제목: %s\n", task->title);
    printf("│ 설명: %s\n", task->description[0] ? task->description : "(없음)");
    printf("│ 상태: %s | 우선순위: %s\n",
           statusToString(task->status), priorityToString(task->priority));
    printf("│ 마감: %s\n", formatTimestamp(task->deadline, tbuf, sizeof(tbuf)));
    printf("│ 생성: %s\n", formatTimestamp(task->createdAt, tbuf, sizeof(tbuf)));
    printf("│ 수정: %s\n", formatTimestamp(task->modifiedAt, tbuf, sizeof(tbuf)));
    printf("└────────────────────────────────────\n");
}

void displayTaskWithHistory(const Task *task) {
    char tbuf[MAX_TIME_STRING];
    printf("\n┌────────────────────────────────────\n");
    printf("│ [작업 #%d]\n", task->id);
    printf("│ 제목: %s\n", task->title);
    printf("│ 설명: %s\n", task->description[0] ? task->description : "(없음)");
    printf("│ 상태: %s | 우선순위: %s\n",
           statusToString(task->status), priorityToString(task->priority));
    printf("│ 마감: %s\n", formatTimestamp(task->deadline, tbuf, sizeof(tbuf)));
    printf("│ 생성: %s\n", formatTimestamp(task->createdAt, tbuf, sizeof(tbuf)));
    printf("│ 수정: %s\n", formatTimestamp(task->modifiedAt, tbuf, sizeof(tbuf)));
    printf("│\n");
    if (task->historyCount == 0) {
        printf("│ [변경 이력 없음]\n");
    } else {
        printf("│ [변경 이력] (%d/%d)\n", task->historyCount, MAX_HISTORY);
        for (int i = 0; i < task->historyCount; i++) {
            printf("│ · [%s] %s\n",
                   formatTimestamp(task->history[i].timestamp, tbuf, sizeof(tbuf)),
                   task->history[i].event);
        }
    }
    printf("└────────────────────────────────────\n");
}

void displayTaskHistory(const Task *task) {
    char tbuf[MAX_TIME_STRING];
    if (task->historyCount == 0) {
        printf("[변경 이력 없음]\n");
        return;
    }
    printf("[변경 이력] (%d/%d)\n", task->historyCount, MAX_HISTORY);
    for (int i = 0; i < task->historyCount; i++) {
        printf("· [%s] %s\n",
               formatTimestamp(task->history[i].timestamp, tbuf, sizeof(tbuf)),
               task->history[i].event);
    }
}

/* === CRUD ============================================================== */
void initializeTask(Task *task, int id) {
    task->id = id;
    task->title[0] = '\0';
    task->description[0] = '\0';
    task->status = TODO;
    task->priority = MEDIUM;
    task->createdAt = time(NULL);
    task->modifiedAt = task->createdAt;
    task->deadline = 0;
    task->historyCount = 0;
    appendHistory(task, "작업 생성");
}

TaskNode *createTaskNode(Task task) {
    TaskNode *node = (TaskNode *)malloc(sizeof(TaskNode));
    if (node == NULL) return NULL;
    node->task = task;
    node->next = NULL;
    return node;
}

void createTask(TaskNode **head, int *nextId) {
    Task task;
    initializeTask(&task, *nextId);
    char buf[MAX_STRING_INPUT_LENGTH];

    printf("\n=== 신규 작업 생성 ===\n");
    while (true) {
        printf("제목: ");
        if (getUserInput(buf) == HAS_INPUT && buf[0] != '\0') {
            strncpy(task.title, buf, MAX_TASK_TITLE_LENGTH - 1);
            task.title[MAX_TASK_TITLE_LENGTH - 1] = '\0';
            break;
        }
        printf("[오류] 제목은 비울 수 없습니다.\n");
    }
    printf("설명: ");
    if (getUserInput(buf) == HAS_INPUT) {
        strncpy(task.description, buf, MAX_TASK_DESCRIPTION_LENGTH - 1);
        task.description[MAX_TASK_DESCRIPTION_LENGTH - 1] = '\0';
    }
    while (true) {
        printf("우선순위 (%d: 낮음, %d: 보통, %d: 높음): ", LOW, MEDIUM, HIGH);
        int p = scanId();
        if (p >= LOW && p <= HIGH) { task.priority = p; break; }
        printf("[오류] %d~%d 사이의 값이어야 합니다.\n", LOW, HIGH);
    }
    while (true) {
        printf("마감 시각 (YYYY-MM-DD HH:MM, 빈칸 시 미설정): ");
        if (getUserInput(buf) == EMPTY_INPUT) break;
        if (parseDateTime(buf, &task.deadline)) break;
        printf("[오류] 잘못된 형식입니다.\n");
    }

    TaskNode *node = createTaskNode(task);
    if (node == NULL) {
        printf("[오류] 메모리 할당 실패.\n");
        return;
    }
    appendTaskNode(head, node);
    (*nextId)++;
    printf("\n--------------------------\n작업이 생성되었습니다. (ID: %d)\n", task.id);
}

TaskNode *findTaskNodeById(TaskNode *head, int id) {
    for (TaskNode *cur = head; cur != NULL; cur = cur->next) {
        if (cur->task.id == id) return cur;
    }
    return NULL;
}

int countTasks(TaskNode *head) {
    int n = 0;
    for (TaskNode *cur = head; cur != NULL; cur = cur->next) n++;
    return n;
}

void appendTaskNode(TaskNode **head, TaskNode *newNode) {
    if (newNode == NULL) return;
    if (*head == NULL) { *head = newNode; return; }
    TaskNode *cur = *head;
    while (cur->next != NULL) cur = cur->next;
    cur->next = newNode;
}

Task deepCopyTask(Task task) {
    /* HistoryEntry는 고정 배열이므로 단순 구조체 복사로 충분(포인터 멤버 없음). */
    return task;
}

/* === 수정 ============================================================== */
void updateTaskById(TaskNode *head) {
    if (head == NULL) {
        printf("[오류] 등록된 작업이 없습니다.\n");
        return;
    }
    printf("\n=== 단일 작업 수정 ===\n작업 ID(0: 돌아가기): ");
    int id = scanId();
    if (id <= 0) return;
    TaskNode *node = findTaskNodeById(head, id);
    if (node == NULL) {
        printf("[오류] 해당 ID의 작업을 찾을 수 없습니다.\n");
        return;
    }
    updateTask(&node->task);
}

void updateTask(Task *task) {
    char buf[MAX_STRING_INPUT_LENGTH];
    char eventBuf[MAX_HISTORY_EVENT_LENGTH];
    char fromBuf[MAX_TIME_STRING], toBuf[MAX_TIME_STRING];
    bool changed[CHANGED_FIELD_COUNT] = {false, false, false, false, false};
    Task old = *task;

    printf("* 빈칸 입력 시 현재 정보로 유지\n");

    printf("\n현재 제목: %s\n변경 제목: ", task->title);
    if (getUserInput(buf) == HAS_INPUT && strcmp(buf, task->title) != 0) {
        strncpy(task->title, buf, MAX_TASK_TITLE_LENGTH - 1);
        task->title[MAX_TASK_TITLE_LENGTH - 1] = '\0';
        changed[0] = true;
    }

    printf("\n현재 설명: %s\n변경 설명: ", task->description);
    if (getUserInput(buf) == HAS_INPUT && strcmp(buf, task->description) != 0) {
        strncpy(task->description, buf, MAX_TASK_DESCRIPTION_LENGTH - 1);
        task->description[MAX_TASK_DESCRIPTION_LENGTH - 1] = '\0';
        changed[1] = true;
    }

    printf("\n현재 상태: %s\n변경 상태(%d:할일, %d:진행중, %d:완료): ",
           statusToString(task->status), TODO, IN_PROGRESS, DONE);
    int v;
    if (getUserInput(buf) == HAS_INPUT && parseFirstIntValue(buf, &v) &&
        v >= TODO && v <= DONE && v != task->status) {
        task->status = v;
        changed[2] = true;
    }

    printf("\n현재 우선순위: %s\n변경 우선순위(%d:낮음, %d:보통, %d:높음): ",
           priorityToString(task->priority), LOW, MEDIUM, HIGH);
    if (getUserInput(buf) == HAS_INPUT && parseFirstIntValue(buf, &v) &&
        v >= LOW && v <= HIGH && v != task->priority) {
        task->priority = v;
        changed[3] = true;
    }

    printf("\n현재 마감: %s\n변경 마감(YYYY-MM-DD HH:MM, 0:삭제): ",
           formatTimestamp(task->deadline, fromBuf, sizeof(fromBuf)));
    if (getUserInput(buf) == HAS_INPUT) {
        if (strcmp(buf, "0") == 0) {
            if (task->deadline != 0) { task->deadline = 0; changed[4] = true; }
        } else {
            time_t newDeadline;
            if (parseDateTime(buf, &newDeadline) && newDeadline != task->deadline) {
                task->deadline = newDeadline;
                changed[4] = true;
            }
        }
    }

    bool any = changed[0] || changed[1] || changed[2] || changed[3] || changed[4];
    if (!any) {
        printf("\n작업이 수정되지 않았습니다.\n");
        return;
    }
    task->modifiedAt = time(NULL);

    printf("\n작업(ID: %d)이 수정되었습니다.\n", task->id);
    if (changed[0])
        printf("* 제목:\n\t[이전] %s\n\t[변경] %s\n", old.title, task->title);
    if (changed[1])
        printf("* 설명:\n\t[이전] %s\n\t[변경] %s\n", old.description, task->description);
    if (changed[2]) {
        printf("* 상태: [%s] -> [%s]\n",
               statusToString(old.status), statusToString(task->status));
        snprintf(eventBuf, sizeof(eventBuf), "상태: %s -> %s",
                 statusToString(old.status), statusToString(task->status));
        appendHistory(task, eventBuf);
    }
    if (changed[3]) {
        printf("* 우선순위: [%s] -> [%s]\n",
               priorityToString(old.priority), priorityToString(task->priority));
        snprintf(eventBuf, sizeof(eventBuf), "우선순위: %s -> %s",
                 priorityToString(old.priority), priorityToString(task->priority));
        appendHistory(task, eventBuf);
    }
    if (changed[4]) {
        formatTimestamp(old.deadline, fromBuf, sizeof(fromBuf));
        formatTimestamp(task->deadline, toBuf, sizeof(toBuf));
        printf("* 마감 시각: [%s] -> [%s]\n", fromBuf, toBuf);
        snprintf(eventBuf, sizeof(eventBuf), "마감 시각: %s -> %s", fromBuf, toBuf);
        appendHistory(task, eventBuf);
    }
}

/* 일괄 변경 — ID 목록 받기, 빈 입력 시 전체 대상 */
static int collectTargetIds(TaskNode *head, int ids[], int max) {
    char buf[MAX_STRING_INPUT_LENGTH];
    printf("변경 대상 ID (빈칸으로 ID 구분, 빈칸입력 시 모두 변경): ");
    if (getUserInput(buf) == EMPTY_INPUT) {
        int n = 0;
        for (TaskNode *cur = head; cur != NULL && n < max; cur = cur->next)
            ids[n++] = cur->task.id;
        return n;
    }
    int parsed = 0;
    if (!parseFirstNIntValues(buf, ids, max, &parsed)) {
        printf("[오류] ID 파싱 실패.\n");
        return 0;
    }
    return parsed;
}

void updateTasksStatus(TaskNode *head) {
    int ids[MAX_HISTORY * 4];  /* 충분히 큰 버퍼 */
    int n = collectTargetIds(head, ids, sizeof(ids) / sizeof(ids[0]));
    if (n == 0) return;

    printf("\n다음 작업들이 변경됩니다.\n");
    for (int i = 0; i < n; i++) {
        TaskNode *node = findTaskNodeById(head, ids[i]);
        if (node == NULL) {
            printf("[경고] ID %d 작업을 찾을 수 없습니다.\n", ids[i]);
        } else {
            displayTask(&node->task, true);
        }
    }

    int newStatus;
    while (true) {
        printf("\n변경 상태(%d:할일, %d:진행중, %d:완료): ", TODO, IN_PROGRESS, DONE);
        newStatus = scanId();
        if (newStatus >= TODO && newStatus <= DONE) break;
        printf("[오류] %d~%d 사이의 값이어야 합니다.\n", TODO, DONE);
    }

    char eventBuf[MAX_HISTORY_EVENT_LENGTH];
    for (int i = 0; i < n; i++) {
        TaskNode *node = findTaskNodeById(head, ids[i]);
        if (node == NULL) continue;
        if (node->task.status == newStatus) continue;
        const char *fromS = statusToString(node->task.status);
        const char *toS = statusToString(newStatus);
        printf("작업(ID: %d)의 상태를 [%s]에서 [%s](으)로 변경합니다.\n",
               node->task.id, fromS, toS);
        node->task.status = newStatus;
        node->task.modifiedAt = time(NULL);
        snprintf(eventBuf, sizeof(eventBuf), "상태: %s -> %s", fromS, toS);
        appendHistory(&node->task, eventBuf);
    }
    printf("\n상태가 성공적으로 변경되었습니다.\n");
}

void updateTasksPriority(TaskNode *head) {
    int ids[MAX_HISTORY * 4];
    int n = collectTargetIds(head, ids, sizeof(ids) / sizeof(ids[0]));
    if (n == 0) return;

    printf("\n다음 작업들이 변경됩니다.\n");
    for (int i = 0; i < n; i++) {
        TaskNode *node = findTaskNodeById(head, ids[i]);
        if (node == NULL) {
            printf("[경고] ID %d 작업을 찾을 수 없습니다.\n", ids[i]);
        } else {
            displayTask(&node->task, true);
        }
    }

    int newPri;
    while (true) {
        printf("\n변경 우선순위(%d:낮음, %d:보통, %d:높음): ", LOW, MEDIUM, HIGH);
        newPri = scanId();
        if (newPri >= LOW && newPri <= HIGH) break;
        printf("[오류] %d~%d 사이의 값이어야 합니다.\n", LOW, HIGH);
    }

    char eventBuf[MAX_HISTORY_EVENT_LENGTH];
    for (int i = 0; i < n; i++) {
        TaskNode *node = findTaskNodeById(head, ids[i]);
        if (node == NULL) continue;
        if (node->task.priority == newPri) continue;
        const char *fromS = priorityToString(node->task.priority);
        const char *toS = priorityToString(newPri);
        printf("작업(ID: %d)의 우선순위를 [%s]에서 [%s](으)로 변경합니다.\n",
               node->task.id, fromS, toS);
        node->task.priority = newPri;
        node->task.modifiedAt = time(NULL);
        snprintf(eventBuf, sizeof(eventBuf), "우선순위: %s -> %s", fromS, toS);
        appendHistory(&node->task, eventBuf);
    }
    printf("\n우선순위가 성공적으로 변경되었습니다.\n");
}

/* === 삭제 ============================================================== */
void deleteTaskById(TaskNode **head) {
    if (*head == NULL) {
        printf("[오류] 등록된 작업이 없습니다.\n");
        return;
    }
    printf("\n=== 작업 삭제(ID) ===\n삭제할 작업 ID: ");
    int id = scanId();
    if (id <= 0) return;

    TaskNode *prev = NULL, *cur = *head;
    while (cur != NULL && cur->task.id != id) {
        prev = cur;
        cur = cur->next;
    }
    if (cur == NULL) {
        printf("[오류] 해당 ID의 작업을 찾을 수 없습니다.\n");
        return;
    }
    char title[MAX_TASK_TITLE_LENGTH];
    strncpy(title, cur->task.title, sizeof(title));
    title[sizeof(title) - 1] = '\0';

    if (prev == NULL) *head = cur->next;
    else prev->next = cur->next;
    freeTaskNode(cur);
    printf("\n작업 '%s'이(가) 삭제되었습니다.\n", title);
    printf("현재 작업 개수: %d개\n", countTasks(*head));
}

void deleteAllTasks(TaskNode **head) {
    if (*head == NULL) {
        printf("[안내] 등록된 작업이 없습니다.\n");
        return;
    }
    char buf[MAX_STRING_INPUT_LENGTH];
    printf("\n=== 전체 작업 삭제 ===\n");
    printf("정말로 모든 작업을 삭제하시겠습니까? (Y/N): ");
    if (getUserInput(buf) != HAS_INPUT) return;
    if (buf[0] != 'Y' && buf[0] != 'y') {
        printf("취소되었습니다.\n");
        return;
    }
    int n = countTasks(*head);
    freeAllTasks(head);
    printf("\n%d개의 작업이 삭제되었습니다.\n", n);
    printf("현재 작업 개수: 0개\n");
}

void freeTaskNode(TaskNode *node) {
    /* Task 안에 외부로 향하는 포인터 멤버가 없고 이력도 고정 배열이라 별도 해제할 자원 없음 */
    free(node);
}

void freeAllTasks(TaskNode **head) {
    TaskNode *cur = *head;
    while (cur != NULL) {
        TaskNode *next = cur->next;
        freeTaskNode(cur);
        cur = next;
    }
    *head = NULL;
}

/* === 이력 ============================================================== */
void appendHistory(Task *task, const char *event) {
    if (task->historyCount >= MAX_HISTORY) {
        /* 가장 오래된 항목을 밀어내고 추가 (FIFO 선입선출) */
        for (int i = 1; i < MAX_HISTORY; i++) {
            task->history[i - 1] = task->history[i];
        }
        task->historyCount = MAX_HISTORY - 1;
    }
    HistoryEntry *e = &task->history[task->historyCount++];
    e->timestamp = time(NULL);
    strncpy(e->event, event, MAX_HISTORY_EVENT_LENGTH - 1);
    e->event[MAX_HISTORY_EVENT_LENGTH - 1] = '\0';
}

/* === 정렬 (Task[] 스냅샷 + qsort) =====================================
 *
 * 26-1 설계: qsort 대상은 Task* 포인터 배열이 아닌 Task[] 구조체 배열.
 * 이유: W14 강의 예시의 형변환 한 번(`*(const int*)a`)과 동일한 패턴 유지.
 *      비교함수 선언 형태가 직관적임.
 * 트레이드오프: Task 구조체를 복사 (현실적 작업 개수에서 무시 가능).
 */

/* 연결리스트 → Task[] 깊은 복사 스냅샷. 학생이 P2에서 받은 snapshotTaskList의 의미를
 * 그대로 유지하되, 본 P3에서는 qsort를 위해 구조체 복사로 동작.
 * 반환: 적재 개수. */
int snapshotTaskList(TaskNode *head, Task *snapshot, int max) {
    int n = 0;
    for (TaskNode *cur = head; cur != NULL && n < max; cur = cur->next) {
        snapshot[n++] = cur->task;  /* 구조체 복사 */
    }
    return n;
}

/*
 * === [TODO T3] qsort 비교 함수 3개 작성 ===
 *
 * 함수 선언 형태: int compare(const void *a, const void *b)
 *   - W14 강의 예시와 동일한 패턴 (`a`·`b`를 각각 한 번씩만 `const Task *`로 형변환):
 *
 *     int compareXxx(const void *a, const void *b) {
 *         const Task *t1 = (const Task *)a;
 *         const Task *t2 = (const Task *)b;
 *         // 반환: 음수 → a가 앞, 양수 → b가 앞, 0 → 동일
 *     }
 *
 * 학습 비교용으로 베이스 코드에 다음 3개가 이미 완성되어 있으니 참고:
 *   - compareByStatus      (오름차순 — 정수 차분 패턴)
 *   - compareByModifiedAt  (내림차순 — time_t 비교, 최신 먼저)
 *   - compareByCreatedAt   (내림차순 — time_t 비교, 최신 먼저)
 *
 * 본인이 작성할 3개:
 *   T3-A. compareById       — ID 오름차순 (가장 단순한 정수 차분)
 *   T3-B. compareByPriority — 우선순위 내림차순 (높음 → 보통 → 낮음).
 *                              priority enum 값이 큰 쪽이 먼저 오도록.
 *   T3-C. compareByDeadline — 마감 시각 오름차순 (임박 먼저).
 *                              **특수 처리**: deadline == 0 인 항목(미설정)은
 *                              항상 뒤로 보낸다.
 *
 * 빠진 함수는 sortTasks 호출 시 pickComparator가 NULL을 돌려주어 정렬 없이 표시된다.
 */
int compareById(const void *a, const void *b) {
    /* TODO T3-A: 구현 필요 */
    (void)a; (void)b; return 0;
}

int compareByPriority(const void *a, const void *b) {
    /* TODO T3-B: 구현 필요 — 높음 먼저 */
    (void)a; (void)b; return 0;
}

int compareByStatus(const void *a, const void *b) {
    const Task *t1 = (const Task *)a;
    const Task *t2 = (const Task *)b;
    return t1->status - t2->status;
}

int compareByDeadline(const void *a, const void *b) {
    /* TODO T3-C: 구현 필요 — deadline==0은 뒤로 */
    (void)a; (void)b; return 0;
}

int compareByModifiedAt(const void *a, const void *b) {
    const Task *t1 = (const Task *)a;
    const Task *t2 = (const Task *)b;
    if (t1->modifiedAt > t2->modifiedAt) return -1;
    if (t1->modifiedAt < t2->modifiedAt) return 1;
    return 0;
}

int compareByCreatedAt(const void *a, const void *b) {
    const Task *t1 = (const Task *)a;
    const Task *t2 = (const Task *)b;
    if (t1->createdAt > t2->createdAt) return -1;
    if (t1->createdAt < t2->createdAt) return 1;
    return 0;
}

TaskCmp pickComparator(SortType sortType) {
    switch (sortType) {
        case SORT_BY_ID: return compareById;
        case SORT_BY_PRIORITY: return compareByPriority;
        case SORT_BY_STATUS: return compareByStatus;
        case SORT_BY_DEADLINE: return compareByDeadline;
        case SORT_BY_MODIFIED_AT: return compareByModifiedAt;
        case SORT_BY_CREATED_AT: return compareByCreatedAt;
        default: return NULL;
    }
}

/* 정렬 본체 — pickComparator로 비교 함수를 골라 qsort 호출.
 * W14 학습 핵심: 직접 정렬 루프 대신 표준 라이브러리(qsort) + 함수 포인터로
 * 새 기준 추가가 비교 함수 1개씩만으로 끝난다는 점을 코드 흐름으로 확인. */
void sortTasks(Task *snapshot, int count, SortType sortType) {
    TaskCmp cmp = pickComparator(sortType);
    if (cmp == NULL) return;
    qsort(snapshot, count, sizeof(Task), cmp);
}

void displayTasksSorted(TaskNode *head, SortType sortType) {
    int total = countTasks(head);
    if (total == 0) {
        printf("[안내] 등록된 작업이 없습니다.\n");
        return;
    }
    Task *snapshot = (Task *)malloc(sizeof(Task) * total);
    if (snapshot == NULL) {
        printf("[오류] 메모리 할당 실패.\n");
        return;
    }
    int n = snapshotTaskList(head, snapshot, total);
    sortTasks(snapshot, n, sortType);
    printf("\n=== 전체 작업 목록 (%d개) ===\n", n);
    for (int i = 0; i < n; i++) {
        displayTask(&snapshot[i], false);
    }
    free(snapshot);
}

/* === 파일 I/O ========================================================= */

/* CSV 형식: id,title,description,status,priority,createdAt,modifiedAt,deadline
 * 이력은 CSV에 저장하지 않음 (바이너리 전용) — 학생 학습 부담 완화. */
static void writeCsvField(FILE *fp, const char *field) {
    /* 큰따옴표·콤마·줄바꿈이 있으면 따옴표로 감싸고 내부 따옴표는 두 번 */
    bool needQuote = false;
    for (const char *p = field; *p; p++) {
        if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r') {
            needQuote = true;
            break;
        }
    }
    if (!needQuote) {
        fputs(field, fp);
        return;
    }
    fputc('"', fp);
    for (const char *p = field; *p; p++) {
        if (*p == '"') fputc('"', fp);
        fputc(*p, fp);
    }
    fputc('"', fp);
}

/*
 * === [TODO T1] CSV 저장 ===
 *
 * 형식: id,title,description,status,priority,createdAt,modifiedAt,deadline
 *   - 첫 줄에 위 헤더 그대로 출력 (T2의 헤더 스킵 로직과 짝)
 *   - 각 작업을 한 줄씩 쉼표로 구분해 직렬화
 *   - title/description은 베이스 제공 `writeCsvField()` 헬퍼로 출력
 *     (쉼표·따옴표·줄바꿈 escape를 자동 처리)
 *   - status/priority는 정수로 출력 (printf("%d", ...))
 *   - time_t 필드는 정수로 직렬화 (printf("%lld", (long long)t))
 *   - 이력은 CSV에 저장하지 않음 (바이너리 전용 — CSV 왕복 검증 시 손실)
 *
 * 반환:
 *   - 성공 시 0, 파일 열기 실패 시 -1
 *
 * 흐름 힌트:
 *   FILE *fp = fopen(filename, "w");
 *   if (!fp) { printf("[오류] ..."); return -1; }
 *   fputs("id,title,...\n", fp);
 *   for (TaskNode *cur = head; cur != NULL; cur = cur->next) {
 *       fprintf(fp, "%d,", cur->task.id);
 *       writeCsvField(fp, cur->task.title);
 *       fputc(',', fp);
 *       writeCsvField(fp, cur->task.description);
 *       fprintf(fp, ",%d,%d,%lld,%lld,%lld\n", ...);
 *   }
 *   fclose(fp);
 */
int saveTasksToTextFile(TaskNode *head, const char *filename) {
    /* TODO T1: 구현 필요 — writeCsvField()를 활용 */
    (void)head; (void)filename;
    (void)writeCsvField;  /* TODO 미구현 시 unused-function 경고 억제. 구현 시 제거 */
    printf("[TODO T1] saveTasksToTextFile 미구현\n");
    return -1;
}

/* CSV 한 줄에서 다음 필드 하나를 읽고, 끝 위치를 line에 갱신.
 * 따옴표로 감싸진 경우 따옴표 처리. 반환: 읽은 길이, -1=종료. */
static int readCsvField(const char **line, char *out, int outSize) {
    const char *p = *line;
    int i = 0;
    if (*p == '"') {
        p++;
        while (*p) {
            if (*p == '"' && *(p + 1) == '"') {
                if (i < outSize - 1) out[i++] = '"';
                p += 2;
            } else if (*p == '"') {
                p++;
                break;
            } else {
                if (i < outSize - 1) out[i++] = *p;
                p++;
            }
        }
    } else {
        while (*p && *p != ',' && *p != '\n' && *p != '\r') {
            if (i < outSize - 1) out[i++] = *p;
            p++;
        }
    }
    out[i] = '\0';
    if (*p == ',') p++;
    *line = p;
    return i;
}

/*
 * === [TODO T2] CSV 불러오기 ===
 *
 *   - fopen 후 fgets로 한 줄씩 읽기
 *   - 첫 줄이 헤더("id,title,...")면 스킵 (strncmp(line, "id,", 3) == 0 활용)
 *   - 각 줄에 대해 베이스 제공 `readCsvField()` 헬퍼를 8번 호출하여 필드 추출
 *     순서: id, title, description, status, priority, createdAt, modifiedAt, deadline
 *   - 정수 필드는 `parseFirstIntValue()`, time_t 필드는 `sscanf("%lld")` 권장
 *   - Task 구조체에 채운 뒤 `createTaskNode()` + `appendTaskNode()` 호출
 *   - 이력은 비움 (`historyCount = 0`)
 *   - 기존 작업은 유지 + 새 작업을 뒤에 추가 (덮어쓰기 X)
 *   - `*nextId`를 max(불러온 모든 id, 기존 *nextId) + 1 로 갱신
 *
 * 반환:
 *   - 성공 시 0, 파일 없으면 -1
 *
 * 흐름 힌트:
 *   FILE *fp = fopen(filename, "r");
 *   if (!fp) { printf("[오류] ..."); return -1; }
 *   char line[MAX_STRING_INPUT_LENGTH * 4];
 *   bool isFirst = true;
 *   while (fgets(line, sizeof(line), fp) != NULL) {
 *       if (isFirst) { isFirst = false; if (strncmp(line, "id,", 3) == 0) continue; }
 *       const char *p = line;
 *       char field[MAX_TASK_DESCRIPTION_LENGTH * 2];
 *       Task t; memset(&t, 0, sizeof(t)); t.historyCount = 0;
 *
 *       readCsvField(&p, field, sizeof(field));   // id
 *       parseFirstIntValue(field, &t.id);
 *       readCsvField(&p, field, sizeof(field));   // title
 *       strncpy(t.title, field, MAX_TASK_TITLE_LENGTH - 1);
 *       ...
 *
 *       TaskNode *node = createTaskNode(t);
 *       appendTaskNode(head, node);
 *       if (t.id >= *nextId) *nextId = t.id + 1;
 *   }
 *   fclose(fp);
 */
int loadTasksFromTextFile(TaskNode **head, int *nextId, const char *filename) {
    /* TODO T2: 구현 필요 — readCsvField()를 활용 */
    (void)head; (void)nextId; (void)filename;
    (void)readCsvField;  /* TODO 미구현 시 unused-function 경고 억제. 구현 시 제거 */
    printf("[TODO T2] loadTasksFromTextFile 미구현\n");
    return -1;
}

int saveTasksToBinaryFile(TaskNode *head, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("[오류] 파일을 열 수 없습니다: %s\n", filename);
        return -1;
    }
    int magic = BINARY_FILE_MAGIC_NUMBER;
    int count = countTasks(head);
    fwrite(&magic, sizeof(int), 1, fp);
    fwrite(&count, sizeof(int), 1, fp);
    /* 핵심: Task 안에 외부 포인터 멤버가 없어 단일 fwrite로 끝 */
    for (TaskNode *cur = head; cur != NULL; cur = cur->next) {
        fwrite(&cur->task, sizeof(Task), 1, fp);
    }
    fclose(fp);
    printf("\n%d개의 작업이 저장되었습니다.\n파일: %s\n", count, filename);
    return 0;
}

int loadTasksFromBinaryFile(TaskNode **head, int *nextId, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return -1;
    }
    int magic = 0, count = 0;
    if (fread(&magic, sizeof(int), 1, fp) != 1 || magic != BINARY_FILE_MAGIC_NUMBER) {
        printf("[오류] 매직 넘버 불일치 — 잘못된 파일입니다.\n");
        fclose(fp);
        return -1;
    }
    if (fread(&count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    int loaded = 0;
    for (int i = 0; i < count; i++) {
        Task t;
        if (fread(&t, sizeof(Task), 1, fp) != 1) break;
        TaskNode *node = createTaskNode(t);
        if (node == NULL) break;
        appendTaskNode(head, node);
        if (t.id >= *nextId) *nextId = t.id + 1;
        loaded++;
    }
    fclose(fp);
    printf("\n%d개의 작업을 불러왔습니다. (%s)\n", loaded, filename);
    return 0;
}

void autoLoadTasksOnStartup(TaskNode **head, int *nextId) {
    FILE *fp = fopen(DEFAULT_BINARY_FILENAME, "rb");
    if (fp == NULL) {
        printf("(자동 불러오기: %s 없음, 새로 시작합니다)\n", DEFAULT_BINARY_FILENAME);
        return;
    }
    fclose(fp);
    if (loadTasksFromBinaryFile(head, nextId, DEFAULT_BINARY_FILENAME) == 0) {
        printf("\n기존 작업이 복원되었습니다:\n");
        for (TaskNode *cur = *head; cur != NULL; cur = cur->next) {
            printf("  - %s\n", cur->task.title);
        }
    }
}

void autoSaveTasksOnExit(TaskNode *head) {
    if (head == NULL) {
        printf("\n저장할 작업이 없어 자동 저장을 생략합니다.\n");
        return;
    }
    printf("\n작업을 파일로 저장합니다...\n");
    saveTasksToBinaryFile(head, DEFAULT_BINARY_FILENAME);
}

/* === 디버그 (테스트 메뉴) ============================================= */
void observeMemoryUsage(TaskNode **head, int *nextId) {
    char buf[MAX_STRING_INPUT_LENGTH];
    int n;
    printf("\n생성할 작업 개수: ");
    n = scanId();
    if (n <= 0) return;
    printf("현재 메모리 사용량(MB)을 입력: ");
    if (getUserInput(buf) != HAS_INPUT) return;
    float before = 0; sscanf(buf, "%f", &before);
    printf("작업 생성을 시작하시겠습니까? (Y/N): ");
    if (getUserInput(buf) != HAS_INPUT || (buf[0] != 'Y' && buf[0] != 'y')) return;
    generateDummyTasks(head, nextId, n);
    printf("\n생성 후 메모리 사용량(MB): ");
    if (getUserInput(buf) != HAS_INPUT) return;
    float after = 0; sscanf(buf, "%f", &after);

    printf("작업 삭제를 시작하시겠습니까? (Y/N): ");
    if (getUserInput(buf) != HAS_INPUT || (buf[0] != 'Y' && buf[0] != 'y')) return;
    freeAllTasks(head);
    printf("\n삭제 후 메모리 사용량(MB): ");
    if (getUserInput(buf) != HAS_INPUT) return;
    float freed = 0; sscanf(buf, "%f", &freed);

    printf("\n--------------------------------\n메모리 사용량 변화 (%d개 작업)\n", n);
    printf("--------------------------------\n");
    printf("생성 전: %.1f MB\n생성 후: %.1f MB\n삭제 후: %.1f MB\n", before, after, freed);
    printf("--------------------------------\n");
}

void generateDummyTasks(TaskNode **head, int *nextId, int count) {
    for (int i = 0; i < count; i++) {
        Task t;
        initializeTask(&t, *nextId);
        snprintf(t.title, sizeof(t.title), "더미 작업 #%d", *nextId);
        snprintf(t.description, sizeof(t.description), "자동 생성된 작업");
        TaskNode *node = createTaskNode(t);
        if (node == NULL) break;
        appendTaskNode(head, node);
        (*nextId)++;
    }
    printf("\n%d개의 더미 작업을 생성했습니다.\n", count);
}
