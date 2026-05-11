/*
 * 26-1 PBP Project — Part 1 Base Code
 *
 * 학생 작업 TODO (6개, 라벨은 T1~T6):
 *   T1. createTask()                — 작업 생성
 *   T2. displayTask()               — 작업 출력
 *   T3. statusToString/priorityToString — 열거형 → 문자열 변환
 *   T4. findTaskById()              — ID로 작업 검색 (포인터 반환)
 *   T5. viewTasksMenu()의 정렬 호출부 — 베이스 정렬 함수 호출 + 출력
 *   T6. updateTask()                — 포인터 기반 작업 수정
 *
 * 베이스 코드로 제공 (수정하지 말 것):
 *   - selectMenu, getUserInput, scanId, parseFirstIntValue
 *   - buildPointerView, swapTaskPointers, sortTaskPointers (정렬 알고리즘)
 *   - displayTasksSorted (포인터 뷰 → 정렬 → 출력 통합)
 *   - main(), displayAndSelectMainMenu()
 *
 * 컴파일: cc -Wall -Wextra -std=c11 -o project_1 project_1.c
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* === 상수 ============================================================= */
#define MAX_STRING_INPUT_LENGTH 500
#define MAX_TASK_TITLE_LENGTH 100
#define MAX_TASK_DESCRIPTION_LENGTH 500
#define MAX_TASKS 100

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
    SORT_NONE = 0,
    SORT_BY_ID = 1,
    SORT_BY_PRIORITY = 2,
    SORT_BY_STATUS = 3
} SortType;

/* === 자료구조 ========================================================= */
typedef struct {
    int id;
    char title[MAX_TASK_TITLE_LENGTH];
    char description[MAX_TASK_DESCRIPTION_LENGTH];
    TaskStatus status;
    TaskPriority priority;
} Task;

/* === 함수 프로토타입 ================================================== */
int displayAndSelectMainMenu(void);
void viewTasksMenu(Task tasks[], int count);
int selectMenu(const char *title, int count, const int indexes[], const char *names[]);

UserInputType getUserInput(char *buf);
int scanId(void);
bool parseFirstIntValue(const char *input, int *out);

const char *statusToString(TaskStatus s);
const char *priorityToString(TaskPriority p);
void displayTask(const Task *task, bool shortFormat);

void createTask(Task tasks[], int *count, int *nextId);
Task *findTaskById(Task tasks[], int count, int id);
void updateTaskById(Task tasks[], int count);
void updateTask(Task *task);

void buildPointerView(Task tasks[], int count, Task *view[]);
void swapTaskPointers(Task **a, Task **b);
void sortTaskPointers(Task *view[], int count, SortType sortType);
void displayTasksSorted(Task tasks[], int count, SortType sortType);

/* === main =============================================================== */
int main(void) {
    Task tasks[MAX_TASKS];
    int count = 0;
    int nextId = 1;
    int menuChoice;

    while ((menuChoice = displayAndSelectMainMenu()) != 0) {
        switch (menuChoice) {
            case 1:
                if (count >= MAX_TASKS) {
                    printf("[오류] 최대 작업 개수(%d)에 도달했습니다.\n", MAX_TASKS);
                    break;
                }
                createTask(tasks, &count, &nextId);
                break;
            case 2: viewTasksMenu(tasks, count); break;
            case 3: updateTaskById(tasks, count); break;
        }
    }

    printf("\n========== 프로그램 종료 ==========\n");
    printf("프로그램을 종료합니다. 감사합니다!\n");
    return 0;
}

/* === 메뉴 — 베이스 코드 (수정 X) ====================================== */
int displayAndSelectMainMenu(void) {
    const int indexes[] = {1, 2, 3, 0};
    const char *names[] = {"작업 추가", "작업 보기", "작업 수정", "종료"};
    return selectMenu("작업 관리 시스템",
                      sizeof(indexes) / sizeof(indexes[0]),
                      indexes, names);
}

/*
 * === [TODO T5] viewTasksMenu의 정렬 호출부 구현 ===
 *
 * 1단계 메뉴: 1.전체보기 / 2.검색 / 0.돌아가기  ← 이미 구현됨
 * 2단계: "전체보기" 선택 시 정렬 기준(0~3)을 받아 displayTasksSorted() 호출
 *
 * 학생 작업: 아래 sel == 1 분기에서
 *   1) 정렬 기준 메뉴(selectMenu)로 sortType 입력 받기
 *   2) displayTasksSorted(tasks, count, sortType) 호출
 *
 * 정렬 함수는 모두 베이스 코드로 제공됨 — 호출만 하면 됨.
 */
void viewTasksMenu(Task tasks[], int count) {
    const int outerIndexes[] = {1, 2, 0};
    const char *outerNames[] = {"전체 작업 보기", "작업 검색 (ID)", "돌아가기"};

    while (true) {
        int sel = selectMenu("작업 보기",
                             sizeof(outerIndexes) / sizeof(outerIndexes[0]),
                             outerIndexes, outerNames);
        if (sel == 0) return;
        if (sel == 1) {
            if (count == 0) {
                printf("[안내] 등록된 작업이 없습니다.\n");
                continue;
            }
            /* === [TODO T5] 정렬 기준 선택 + 정렬된 출력 호출 ===
             * 힌트:
             *   const int sortIdx[] = {0, 1, 2, 3};
             *   const char *sortNames[] = {"입력 순서", "ID", "우선순위", "상태"};
             *   int sortType = selectMenu(...);
             *   displayTasksSorted(tasks, count, (SortType)sortType);
             */
            printf("[TODO T5] 정렬 보기 미구현\n");
        } else if (sel == 2) {
            if (count == 0) {
                printf("[안내] 등록된 작업이 없습니다.\n");
                continue;
            }
            printf("\n=== 작업 검색 (ID) ===\n");
            printf("작업 ID를 입력하세요(0: 돌아가기): ");
            int id = scanId();
            if (id <= 0) continue;
            Task *task = findTaskById(tasks, count, id);
            if (task == NULL) {
                printf("[오류] 해당 ID의 작업을 찾을 수 없습니다.\n");
                continue;
            }
            displayTask(task, false);
        }
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

/* === 입력 처리 — 베이스 코드 (수정 X) ================================= */
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

/* === 표시 ============================================================== */

/*
 * === [TODO T3] 열거형 → 문자열 변환 ===
 * 각 enum 값에 맞는 한글 문자열 반환:
 *   TODO → "할 일", IN_PROGRESS → "진행 중", DONE → "완료"
 *   LOW → "낮음", MEDIUM → "보통", HIGH → "높음"
 *   알 수 없는 값 → "알 수 없음"
 */
const char *statusToString(TaskStatus s) {
    /* TODO T3: 구현 필요 */
    (void)s;
    return "알 수 없음";
}

const char *priorityToString(TaskPriority p) {
    /* TODO T3: 구현 필요 */
    (void)p;
    return "알 수 없음";
}

/*
 * === [TODO T2] 작업 출력 ===
 * shortFormat == true:
 *   "│ [작업 #1] 제목 | 상태 | 우선순위" 한 줄로
 * shortFormat == false:
 *   ┌─...─┐ 박스 안에 ID, 제목, 설명, 상태, 우선순위 출력
 */
void displayTask(const Task *task, bool shortFormat) {
    /* TODO T2: 구현 필요 */
    (void)task; (void)shortFormat;
    printf("[TODO T2] displayTask 미구현\n");
}

/* === CRUD ============================================================== */

/*
 * === [TODO T1] 작업 생성 ===
 * 1) tasks[*count]에 새 Task 초기화 (id=*nextId, status=TODO, priority=MEDIUM)
 * 2) 사용자 입력으로 title, description, priority 받기
 *    - title은 빈 문자열 거부 (재입력)
 *    - priority는 0~2 범위 검증 (재입력)
 * 3) (*count)++, (*nextId)++
 * 4) "작업이 생성되었습니다. (ID: N)" 출력
 */
void createTask(Task tasks[], int *count, int *nextId) {
    /* TODO T1: 구현 필요 */
    (void)tasks; (void)count; (void)nextId;
    printf("[TODO T1] createTask 미구현\n");
}

/*
 * === [TODO T4] ID로 작업 검색 ===
 * tasks 배열을 순회하며 id가 일치하는 첫 작업의 포인터 반환.
 * 없으면 NULL 반환.
 *
 * 핵심: 포인터를 반환해야 호출자가 원본을 수정할 수 있음 (`->` 연산자 사용)
 */
Task *findTaskById(Task tasks[], int count, int id) {
    /* TODO T4: 구현 필요 */
    (void)tasks; (void)count; (void)id;
    return NULL;
}

/* === 수정 ============================================================== */
void updateTaskById(Task tasks[], int count) {
    if (count == 0) {
        printf("[안내] 등록된 작업이 없습니다.\n");
        return;
    }
    printf("\n=== 작업 수정 ===\n작업 ID(0: 돌아가기): ");
    int id = scanId();
    if (id <= 0) return;
    Task *task = findTaskById(tasks, count, id);
    if (task == NULL) {
        printf("[오류] 해당 ID의 작업을 찾을 수 없습니다.\n");
        return;
    }
    updateTask(task);
}

/*
 * === [TODO T6] 작업 수정 ===
 * 각 필드(제목, 설명, 상태, 우선순위)에 대해:
 *   1) 현재 값 출력
 *   2) 변경 값 입력 받기 (빈 입력은 유지)
 *   3) 변경되었으면 task의 해당 필드 갱신 (`->` 연산자)
 *
 * 변경된 필드만 요약 출력. 아무것도 변경되지 않았으면 "작업이 수정되지 않았습니다." 출력.
 *
 * 출력 형식:
 *   - 제목/설명: 두 줄 — "[이전] {값}\n\t[변경] {값}"
 *   - 상태/우선순위: 한 줄 — "[{from}] -> [{to}]"
 *   ※ 정확한 출력은 P1.md의 "실행 결과 예시 — 작업 수정" 섹션을 참고.
 */
void updateTask(Task *task) {
    /* TODO T6: 구현 필요 */
    (void)task;
    printf("[TODO T6] updateTask 미구현\n");
}

/* === 정렬 — 베이스 코드 (수정 X) ====================================== */

void buildPointerView(Task tasks[], int count, Task *view[]) {
    for (int i = 0; i < count; i++) view[i] = &tasks[i];
}

void swapTaskPointers(Task **a, Task **b) {
    Task *tmp = *a;
    *a = *b;
    *b = tmp;
}

/* 선택 정렬 — P3에서 본인이 qsort로 본문을 리팩터링하게 됨.
 * sortType: 0=정렬 안 함, 1=ID 오름, 2=우선순위 내림(높음 먼저), 3=상태 오름. */
void sortTaskPointers(Task *view[], int count, SortType sortType) {
    if (sortType == SORT_NONE) return;
    for (int i = 0; i < count - 1; i++) {
        int sel = i;
        for (int j = i + 1; j < count; j++) {
            bool prefer = false;
            switch (sortType) {
                case SORT_BY_ID:
                    prefer = view[j]->id < view[sel]->id;
                    break;
                case SORT_BY_PRIORITY:
                    prefer = view[j]->priority > view[sel]->priority;
                    break;
                case SORT_BY_STATUS:
                    prefer = view[j]->status < view[sel]->status;
                    break;
                default:
                    break;
            }
            if (prefer) sel = j;
        }
        if (sel != i) swapTaskPointers(&view[i], &view[sel]);
    }
}

void displayTasksSorted(Task tasks[], int count, SortType sortType) {
    if (count == 0) {
        printf("[안내] 등록된 작업이 없습니다.\n");
        return;
    }
    Task *view[MAX_TASKS];
    buildPointerView(tasks, count, view);
    sortTaskPointers(view, count, sortType);
    printf("\n=== 전체 작업 목록 (%d개) ===\n", count);
    for (int i = 0; i < count; i++) {
        displayTask(view[i], false);
    }
}
