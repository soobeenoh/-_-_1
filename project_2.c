/*
 * 26-1 PBP Project — Part 2 Base Code
 *
 * P1 기능은 베이스 코드에 완성된 채 포함되어 있다.
 * P2에서 학생이 추가/변경할 부분 (7개):
 *
 *   T1. deepCopyTask()           — Task 깊은 복사
 *   T2. updateTask()             — 변경 전/후 비교 출력 강화
 *   T3. updateTasksStatus()      — ID 입력·검증 + 일괄 상태 변경
 *   T4. updateTasksPriority()    — ID 입력·검증 + 일괄 우선순위 변경
 *   T5. deleteAllTasks()         — 전체 작업 삭제 (Y/N 확인)
 *   T6. freeAllTasks()           — 전체 노드 메모리 해제 (순회 중 next 보관 패턴)
 *   T7. buildTaskView()          — 연결리스트 → 포인터 배열 어댑터
 *
 * 베이스 코드로 제공 (수정하지 말 것):
 *   - 모든 P1 기능 (createTask, displayTask, findTaskNodeById, statusToString 등)
 *   - selectMenu, getUserInput, scanId, parseFirstIntValue, parseFirstNIntValues
 *   - buildPointerView, swapTaskPointers, sortTaskPointers
 *   - 메뉴 골격(viewTasksMenu, updateTasksMenu, deleteTasksMenu, testMenu)
 *   - **deleteTaskById()** — ID별 작업 삭제 (head/middle/tail prev 포인터 패턴)
 *   - **freeTaskNode()**   — 단일 노드 메모리 해제
 *   - **main() 종료 시 freeAllTasks 호출**
 *
 * 힌트: T3·T4 일괄 변경 구현 시 `collectTargetIds()` 헬퍼를 호출해
 *       대상 ID 입력 + 빈칸=전체 처리 로직을 재사용하세요. 호출하지 않으면
 *       빌드 시 "unused function 'collectTargetIds'" 경고가 남아 있습니다.
 *
 * 컴파일: cc -Wall -Wextra -std=c11 -o project_2 project_2.c
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* === 상수 ============================================================= */
#define MAX_STRING_INPUT_LENGTH 500
#define MAX_TASK_TITLE_LENGTH 100
#define MAX_TASK_DESCRIPTION_LENGTH 500
#define MAX_BULK_IDS 256

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

typedef enum {
    SORT_NONE = 0, SORT_BY_ID = 1, SORT_BY_PRIORITY = 2, SORT_BY_STATUS = 3
} SortType;

/* === 자료구조 ========================================================= */
typedef struct {
    int id;
    char title[MAX_TASK_TITLE_LENGTH];
    char description[MAX_TASK_DESCRIPTION_LENGTH];
    TaskStatus status;
    TaskPriority priority;
} Task;

typedef struct TaskNode {
    Task task;
    struct TaskNode *next;
} TaskNode;

/* === 함수 프로토타입 ================================================== */
int displayAndSelectMainMenu(void);
void viewTasksMenu(TaskNode **head);
void updateTasksMenu(TaskNode **head);
void deleteTasksMenu(TaskNode **head);
void testMenu(TaskNode **head, int *nextId);
int selectMenu(const char *title, int count, const int indexes[], const char *names[]);

UserInputType getUserInput(char *buf);
int scanId(void);
bool parseFirstIntValue(const char *input, int *out);
bool parseFirstNIntValues(const char *input, int values[], int max, int *parsedCount);

const char *statusToString(TaskStatus s);
const char *priorityToString(TaskPriority p);
void displayTask(const Task *task, bool shortFormat);

void createTask(TaskNode **head, int *nextId);
TaskNode *createTaskNode(Task task);
TaskNode *findTaskNodeById(TaskNode *head, int id);
int countTasks(TaskNode *head);
void appendTaskNode(TaskNode **head, TaskNode *newNode);
Task deepCopyTask(Task task);

void updateTaskById(TaskNode *head);
void updateTask(Task *task);
void updateTasksStatus(TaskNode *head);
void updateTasksPriority(TaskNode *head);

void deleteTaskById(TaskNode **head);
void deleteAllTasks(TaskNode **head);
void freeTaskNode(TaskNode *node);
void freeAllTasks(TaskNode **head);

int buildTaskView(TaskNode *head, Task **view, int max);
void buildPointerView(Task *src[], int count, Task *view[]);
void swapTaskPointers(Task **a, Task **b);
void sortTaskPointers(Task *view[], int count, SortType sortType);
void displayTasksSorted(TaskNode *head, SortType sortType);

void observeMemoryUsage(TaskNode **head, int *nextId);
void generateDummyTasks(TaskNode **head, int *nextId, int count);

/* === main =============================================================== */
int main(void) {
    TaskNode *head = NULL;
    int nextId = 1;
    int menuChoice;

    while ((menuChoice = displayAndSelectMainMenu()) != 0) {
        switch (menuChoice) {
            case 1: createTask(&head, &nextId); break;
            case 2: viewTasksMenu(&head); break;
            case 3: updateTasksMenu(&head); break;
            case 4: deleteTasksMenu(&head); break;
            case 9: testMenu(&head, &nextId); break;
        }
    }

    printf("\n========== 프로그램 종료 ==========\n");
    printf("\n메모리를 정리합니다...\n");
    freeAllTasks(&head);
    printf("프로그램을 종료합니다. 감사합니다!\n");
    return 0;
}

/* === 메뉴 — 베이스 코드 ============================================== */
int displayAndSelectMainMenu(void) {
    const int indexes[] = {1, 2, 3, 4, 9, 0};
    const char *names[] = {
        "작업 추가", "작업 보기", "작업 수정", "작업 삭제",
        "테스트", "종료"
    };
    return selectMenu("작업 관리 시스템",
                      sizeof(indexes) / sizeof(indexes[0]),
                      indexes, names);
}

void viewTasksMenu(TaskNode **head) {
    const int outerIndexes[] = {1, 2, 0};
    const char *outerNames[] = {"전체 작업 보기", "작업 검색 (ID)", "돌아가기"};

    while (true) {
        int sel = selectMenu("작업 보기",
                             sizeof(outerIndexes) / sizeof(outerIndexes[0]),
                             outerIndexes, outerNames);
        if (sel == 0) return;
        if (sel == 1) {
            if (*head == NULL) { printf("[안내] 등록된 작업이 없습니다.\n"); continue; }
            const int sortIdx[] = {0, 1, 2, 3};
            const char *sortNames[] = {
                "입력 순서 (정렬 안 함)", "ID 오름차순",
                "우선순위 (높음 → 낮음)", "상태 (할 일 → 완료)"
            };
            int sortType = selectMenu("정렬 기준 선택",
                                      sizeof(sortIdx) / sizeof(sortIdx[0]),
                                      sortIdx, sortNames);
            displayTasksSorted(*head, (SortType)sortType);
        } else if (sel == 2) {
            if (*head == NULL) { printf("[안내] 등록된 작업이 없습니다.\n"); continue; }
            printf("\n=== 작업 검색 (ID) ===\n");
            printf("작업 ID를 입력하세요(0: 돌아가기): ");
            int id = scanId();
            if (id <= 0) continue;
            TaskNode *node = findTaskNodeById(*head, id);
            if (node == NULL) {
                printf("[오류] 해당 ID의 작업을 찾을 수 없습니다.\n");
                continue;
            }
            displayTask(&node->task, false);
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
        if (*head == NULL) { printf("[안내] 등록된 작업이 없습니다.\n"); return; }
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
        if (*head == NULL) { printf("[안내] 등록된 작업이 없습니다.\n"); return; }
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

void testMenu(TaskNode **head, int *nextId) {
    const int indexes[] = {1, 0};
    const char *names[] = {
        "프로그램 메모리 사용량 관찰", "돌아가기"
    };
    int sel = selectMenu("테스트",
                         sizeof(indexes) / sizeof(indexes[0]),
                         indexes, names);
    if (sel == 1) observeMemoryUsage(head, nextId);
}

int selectMenu(const char *title, int count, const int indexes[], const char *names[]) {
    int min = indexes[0], max = indexes[0];
    for (int i = 1; i < count; i++) {
        if (indexes[i] < min) min = indexes[i];
        if (indexes[i] > max) max = indexes[i];
    }
    while (true) {
        printf("\n=== %s ===\n", title);
        for (int i = 0; i < count; i++) printf("%d. %s\n", indexes[i], names[i]);
        printf("선택: ");
        int sel = scanId();
        for (int i = 0; i < count; i++) {
            if (sel == indexes[i]) { printf("\n"); return sel; }
        }
        printf("[오류] 잘못된 선택입니다. 메뉴는 %d~%d 사이의 값이어야 합니다.\n", min, max);
    }
}

/* === 입력 처리 — 베이스 코드 ========================================= */
UserInputType getUserInput(char *buf) {
    if (fgets(buf, MAX_STRING_INPUT_LENGTH, stdin) == NULL) {
        buf[0] = '\0'; return EMPTY_INPUT;
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') buf[--len] = '\0';
    if (len == 0) return EMPTY_INPUT;
    if (len >= MAX_STRING_INPUT_LENGTH - 1) {
        printf("[주의] %d바이트 초과 입력은 무시됩니다.\n", MAX_STRING_INPUT_LENGTH);
        int c; while ((c = getchar()) != '\n' && c != EOF) ;
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
        value = value * 10 + (input[i] - '0'); i++;
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
            value = value * 10 + (input[ci] - '0'); ci++;
        }
        values[vi++] = sign * value;
        (*parsedCount)++;
        if (input[ci] == '\0') return true;
        while (input[ci] == ' ' || input[ci] == '\t') ci++;
        if (input[ci] == '\0') return true;
    }
    return true;
}

/* === 표시 — 베이스 코드 (P1 완성 분) ================================= */
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

void displayTask(const Task *task, bool shortFormat) {
    if (shortFormat) {
        printf("│ [작업 #%d] %s | %s | %s\n",
               task->id, task->title,
               statusToString(task->status), priorityToString(task->priority));
        return;
    }
    printf("┌────────────────────────────────────\n");
    printf("│ [작업 #%d]\n", task->id);
    printf("│ 제목: %s\n", task->title);
    printf("│ 설명: %s\n", task->description[0] ? task->description : "(없음)");
    printf("│ 상태: %s | 우선순위: %s\n",
           statusToString(task->status), priorityToString(task->priority));
    printf("└────────────────────────────────────\n");
}

/* === CRUD — 베이스 코드 (P1 + P2 노드 헬퍼) ========================== */
TaskNode *createTaskNode(Task task) {
    TaskNode *node = (TaskNode *)malloc(sizeof(TaskNode));
    if (node == NULL) return NULL;
    node->task = task;
    node->next = NULL;
    return node;
}

void createTask(TaskNode **head, int *nextId) {
    Task task;
    memset(&task, 0, sizeof(task));
    task.id = *nextId;
    task.status = TODO;
    task.priority = MEDIUM;

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

/*
 * === [TODO T1] Task 깊은 복사 ===
 * Task 구조체를 안전하게 복사. 26-1 P2의 Task는 포인터 멤버가 없으므로
 * 단순한 구조체 복사(`return task;`)로 충분하다.
 *
 * 이 함수는 T2(updateTask 변경 전/후 비교)에서 oldTask 백업용으로 사용된다.
 */
Task deepCopyTask(Task task) {
    /* TODO T1: 구현 필요 */
    Task empty;
    memset(&empty, 0, sizeof(empty));
    (void)task;
    return empty;
}

/* === 수정 ============================================================== */
void updateTaskById(TaskNode *head) {
    if (head == NULL) {
        printf("[안내] 등록된 작업이 없습니다.\n");
        return;
    }
    printf("\n=== 단일 작업 수정 ===\n작업 ID를 입력하세요(0: 메뉴로 돌아가기): ");
    int id = scanId();
    if (id <= 0) return;
    TaskNode *node = findTaskNodeById(head, id);
    if (node == NULL) {
        printf("[오류] 해당 ID의 작업을 찾을 수 없습니다.\n");
        return;
    }
    updateTask(&node->task);
}

/*
 * === [TODO T2] 작업 수정 + 변경 전/후 비교 출력 ===
 *
 * 1) deepCopyTask로 oldTask 백업
 * 2) 각 필드(제목, 설명, 상태, 우선순위) 변경 입력 받기 (빈 입력은 유지)
 * 3) 변경된 필드만 추적 (changed[4] 플래그 활용)
 * 4) 모든 변경이 끝난 후, 변경된 필드만 "* 필드: [이전] -> [변경]" 형식으로 출력
 *    - 제목/설명: 두 줄로 [이전]/[변경] 표시
 *    - 상태/우선순위: 한 줄로 [from] -> [to]
 * 5) 아무것도 변경 안 됐으면 "작업이 수정되지 않았습니다." 출력
 */
void updateTask(Task *task) {
    /* TODO T2: 구현 필요 */
    (void)task;
    printf("[TODO T2] updateTask 미구현\n");
}

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

/*
 * === [TODO T3] 일괄 상태 변경 ===
 *
 * 1) collectTargetIds로 대상 ID 목록 수집
 * 2) 각 ID에 대해 displayTask(short)로 변경 대상 미리보기
 * 3) 새 상태 값 입력 받기 (0~2 범위 검증)
 * 4) 각 ID에 대해 findTaskNodeById → 상태 변경
 *    - 이미 같은 상태면 메시지 생략
 *    - 변경 시 "작업(ID: N)의 상태를 [from]에서 [to](으)로 변경합니다." 출력
 * 5) "상태가 성공적으로 변경되었습니다." 출력
 */
void updateTasksStatus(TaskNode *head) {
    /* TODO T3: 구현 필요 */
    (void)head;
    printf("[TODO T3] updateTasksStatus 미구현\n");
}

/*
 * === [TODO T4] 일괄 우선순위 변경 ===
 * T3와 동일한 패턴. 우선순위(0~2)에 적용.
 */
void updateTasksPriority(TaskNode *head) {
    /* TODO T4: 구현 필요 */
    (void)head;
    printf("[TODO T4] updateTasksPriority 미구현\n");
}

/* === 삭제 ============================================================== */

/*
 * deleteTaskById — **베이스 코드 제공** (head/middle/tail prev 포인터 패턴).
 *
 * head 노드: *head = cur->next
 * 중간/마지막 노드: prev->next = cur->next
 * 어느 경우든 cur는 freeTaskNode로 해제.
 */
void deleteTaskById(TaskNode **head) {
    if (*head == NULL) {
        printf("[안내] 등록된 작업이 없습니다.\n");
        return;
    }
    printf("\n=== 작업 삭제(ID) ===\n삭제할 작업 ID를 입력하세요: ");
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

    if (prev == NULL) *head = cur->next;     /* head 케이스 */
    else prev->next = cur->next;             /* middle/tail 케이스 */
    freeTaskNode(cur);
    printf("\n작업 '%s'이(가) 삭제되었습니다.\n", title);
    printf("현재 작업 개수: %d개\n", countTasks(*head));
}

/*
 * === [TODO T5] 전체 삭제 ===
 *
 * 1) Y/N 확인 받기
 * 2) Y면 freeAllTasks 호출, N이면 취소 메시지
 * 3) "N개의 작업이 삭제되었습니다." 출력
 */
void deleteAllTasks(TaskNode **head) {
    /* TODO T5: 구현 필요 */
    (void)head;
    printf("[TODO T5] deleteAllTasks 미구현\n");
}

/* freeTaskNode — **베이스 코드 제공**.
 * malloc으로 할당된 노드를 free. */
void freeTaskNode(TaskNode *node) {
    free(node);
}

/*
 * === [TODO T6] 전체 노드 해제 ===
 *
 * head 부터 순회하며 모든 노드를 freeTaskNode 로 해제하고, 마지막에 *head 를
 * NULL 로 초기화하세요.
 *
 * [주의] 노드를 해제하기 전에 다음 노드 주소를 미리 보관해야 합니다. 해제된
 * 노드의 멤버(cur->next 등) 를 다시 읽으면 use-after-free 가 되어 정의되지
 * 않은 동작이 발생합니다.
 */
void freeAllTasks(TaskNode **head) {
    /* TODO T6: 구현 필요 */
    (void)head;
}

/* === 정렬 — 베이스 코드 (P1에서 받았던 것 + P2 어댑터 TODO) ========== */

/*
 * === [TODO T7] buildTaskView — 연결리스트로부터 정렬용 view 배열 만들기 ===
 *
 * [왜 필요한가 — 어댑터(adapter)의 개념]
 *   P1 베이스의 정렬 함수 sortTaskPointers 는 `Task* view[]` (포인터 배열) 을
 *   받습니다:
 *
 *     void sortTaskPointers(Task *view[], int count, SortType sortType);
 *
 *   그런데 P2 는 데이터를 연결리스트(TaskNode) 로 보관합니다. 자료구조가
 *   달라 그대로는 정렬 함수에 넘길 수 없습니다.
 *
 *   두 자료구조 사이를 잇기 위해, **연결리스트를 한 번 훑어 각 노드 안의
 *   Task 주소를 임시 포인터 배열에 모으는** 작은 함수가 필요합니다. 이것이
 *   본 함수 buildTaskView 이며, 서로 다른 인터페이스를 맞춰주는 이런 패턴을
 *   일반적으로 *어댑터(adapter)* 라고 부릅니다.
 *
 *     [연결리스트]  →  buildTaskView  →  [Task* view[]]  →  sortTaskPointers
 *
 * [구현 방법]
 *   - head 부터 cur = cur->next 순회
 *   - 각 노드에서 &cur->task (해당 Task 의 주소) 를 view[n] 에 저장하고 n 증가
 *   - n 이 max 를 초과하지 않도록 가드
 *
 * [반환값]
 *   실제 적재한 작업 수.
 *
 * [주의]
 *   view[] 에 담는 것은 *주소* 입니다 — 복사본이 아닙니다. 따라서 정렬 후
 *   호출자가 view[i]->title 등을 읽으면 원본 노드의 데이터가 그대로 보입니다.
 *   원본 연결리스트의 노드 순서는 변하지 않으며, 변하는 것은 view 안의 포인터
 *   순서뿐입니다.
 */
int buildTaskView(TaskNode *head, Task **view, int max) {
    /* TODO T7: 구현 필요 */
    (void)head; (void)view; (void)max;
    return 0;
}

void buildPointerView(Task *src[], int count, Task *view[]) {
    for (int i = 0; i < count; i++) view[i] = src[i];
}

void swapTaskPointers(Task **a, Task **b) {
    Task *tmp = *a; *a = *b; *b = tmp;
}

void sortTaskPointers(Task *view[], int count, SortType sortType) {
    if (sortType == SORT_NONE) return;
    for (int i = 0; i < count - 1; i++) {
        int sel = i;
        for (int j = i + 1; j < count; j++) {
            bool prefer = false;
            switch (sortType) {
                case SORT_BY_ID:
                    prefer = view[j]->id < view[sel]->id; break;
                case SORT_BY_PRIORITY:
                    prefer = view[j]->priority > view[sel]->priority; break;
                case SORT_BY_STATUS:
                    prefer = view[j]->status < view[sel]->status; break;
                default: break;
            }
            if (prefer) sel = j;
        }
        if (sel != i) swapTaskPointers(&view[i], &view[sel]);
    }
}

void displayTasksSorted(TaskNode *head, SortType sortType) {
    int total = countTasks(head);
    if (total == 0) { printf("[안내] 등록된 작업이 없습니다.\n"); return; }
    Task **view = (Task **)malloc(sizeof(Task *) * total);
    if (view == NULL) { printf("[오류] 메모리 할당 실패.\n"); return; }
    int n = buildTaskView(head, view, total);
    sortTaskPointers(view, n, sortType);
    printf("\n=== 전체 작업 목록 (%d개) ===\n", n);
    for (int i = 0; i < n; i++) displayTask(view[i], false);
    free(view);
}

/* === 디버그 (테스트 메뉴) — 베이스 코드 =============================== */
void observeMemoryUsage(TaskNode **head, int *nextId) {
    char buf[MAX_STRING_INPUT_LENGTH];
    printf("\n생성할 작업 개수를 입력하세요: ");
    int n = scanId();
    if (n <= 0) return;
    printf("\n********************\n");
    printf("- 작업 생성/삭제를 각각 시작하기 전/후에 현재 프로그램 메모리 사용량을 관찰하세요.\n");
    printf("- 환경별 메모리 사용량 관찰 방법은 프로젝트-2 게시물 하단의 가이드를 참고하세요.\n");
    printf("********************\n");
    printf("\n현재 메모리 사용량을 입력하세요(MB): ");
    if (getUserInput(buf) != HAS_INPUT) return;
    float before = 0; sscanf(buf, "%f", &before);
    printf("작업 생성을 시작하시겠습니까? (Y/N): ");
    if (getUserInput(buf) != HAS_INPUT) return;
    if (buf[0] != 'Y' && buf[0] != 'y') {
        printf("\n작업 생성이 취소되었습니다.\n");
        return;
    }
    generateDummyTasks(head, nextId, n);
    printf("\n작업 생성이 완료되었습니다.\n");
    printf("현재 메모리 사용량을 입력하세요(MB): ");
    if (getUserInput(buf) != HAS_INPUT) return;
    float after = 0; sscanf(buf, "%f", &after);
    printf("\n작업 삭제를 시작하시겠습니까? (Y/N): ");
    if (getUserInput(buf) != HAS_INPUT) return;
    if (buf[0] != 'Y' && buf[0] != 'y') {
        printf("\n작업 삭제가 취소되었습니다.\n");
        return;
    }
    freeAllTasks(head);
    printf("\n작업 삭제가 완료되었습니다.\n");
    printf("현재 메모리 사용량을 입력하세요(MB): ");
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
        memset(&t, 0, sizeof(t));
        t.id = *nextId;
        t.status = TODO; t.priority = MEDIUM;
        snprintf(t.title, sizeof(t.title), "더미 작업 #%d", *nextId);
        snprintf(t.description, sizeof(t.description), "자동 생성된 작업");
        TaskNode *node = createTaskNode(t);
        if (node == NULL) break;
        appendTaskNode(head, node);
        (*nextId)++;
    }
}
