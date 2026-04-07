/*
 * 주간실습 #2 - 함수 활용: 구구단, 알파벳 카운터, 피보나치
 *
 * [TODO] 표시된 부분을 채워 프로그램을 완성하세요.
 * [수정 금지] 표시된 코드는 수정하지 마세요.
 *
 * Ctrl+F (Mac: Cmd+F) → "TODO" 검색으로 빈칸 위치를 확인하세요.
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* ===== 함수 프로토타입 [수정 금지] ===== */
int gugudan_var(int from, int to);
int gugudan_arr(int from, int to);
int alpha_counter();
int fibonacci(int n);

int main() {
  int choice;
  int from, to, n;
  int result;

  while (1) {
    /* [수정 금지] 메뉴 출력 */
    printf("\n=== 주간실습 #2: 함수 활용 ===\n");
    printf("[메뉴] 1.변수 구구단  2.배열 구구단  3.알파벳 카운터  4.피보나치  0.종료\n");
    printf("선택: ");
    scanf("%d", &choice);

    switch (choice) {
    /* ===== [TODO 1] case 1, 2: from, to 입력 + 함수 호출 + 결과 출력 ===== */
    case 1:
      printf("시작 단 입력: ");
      scanf("%d", &from);
      printf("끝 단 입력: ");
      scanf("%d", &to);
      /* 힌트: gugudan_var 함수 호출 후 result에 저장 */
      result = gugudan_var(from, to, result);
        if (result != -1) {
        printf("\n[결과] 총 %d개의 단을 출력했습니다.\n", result);
      }
      break;
    case 2:
      printf("시작 단 입력: ");
      scanf("%d", &from);
      printf("끝 단 입력: ");
      scanf("%d", &to);
      /* 힌트: gugudan_arr 함수 호출 후 result에 저장 */
      result = gugudan_arr(from,to,result);
      if (result != -1) {
        printf("\n[결과] 총 %d개의 단을 출력했습니다.\n", result);
      }
      break;

    /* ===== [TODO 2] case 3: alpha_counter 호출 + 결과 출력 ===== */
    case 3:
      /* 힌트: alpha_counter 함수 호출 후 result에 저장 */
      result = alpha_counter(from,to,result);
      if (result != -1) {
        /* 힌트: "[결과] %d개의 알파벳이 사용되었습니다." 출력 */
        /* TODO */
        printf("[결과] %d개의 알파벳이 사용되었습니다.") ;
      }
      break;

    /* ===== [TODO 3] case 4: n 입력 + F(0)~F(n) 출력 루프 + 결과 출력 ===== */
    case 4:
      printf("N 입력: ");
      scanf("%d", &n);
      result = fibonacci(n);
      if (result != -1) {
        printf("\n--- 피보나치 수열 ---\n");
        int fib_val;
        /* 힌트: for문으로 i=0부터 n까지,
         *       fib_val = fibonacci(i) 호출 후
         *       "F(%d) = %d" 형식으로 출력 */
        /* TODO */
        for (int i = 0; i <= n; i++) {
          int fib_val = fibonacci(i);

        printf("\n[결과] F(%d) = %d\n", n, fib_val);
      }
      break;

    /* [수정 금지] */
    case 0:
      printf("프로그램을 종료합니다.\n");
      return 0;
    default:
      printf("잘못된 입력입니다. 메뉴 번호를 확인하세요.\n");
      break;
    }
  }

  return 0;
}

/* ===== F1: 변수기반 구구단 ===== */
int gugudan_var(int from, int to) {
  int dan, i;
  int count;

  /* ===== [TODO 4] 가드 조건: from > to → 오류 메시지 + return -1 ===== */
  /* 힌트: if (from > to) 이면 "[오류] 시작 단이 끝 단보다 클 수 없습니다." 출력
   * 후 return -1 */
  /* TODO */
  if (from>to) {
    printf("[오류] 시작 단이 끝 단보다 클 수 없습니다.") ;
  }
  return -1;

  count = to - from + 1;

  /* ===== [TODO 5] 이중 for문으로 구구단 가로 출력 (변수 사용) + 단 수 반환
   * ===== */
  printf("\n--- 변수기반 구구단 ---\n");
  for (i = 1; i <= 9; i++) {
    for (dan = from; dan <= to; dan++) {
      /* 힌트: printf("%d x %d = %2d    ", dan, i, ???) 에서 ???를 채우세요 */
      printf("%d x %d = %2d    ", dan, i, result[d]);
    }
    printf("\n");
  }

  return count;
}

/* ===== F2: 배열기반 구구단 ===== */
int gugudan_arr(int from, int to) {
  int count;

  /* ===== [TODO 6] 가드 조건 + 배열 선언 ===== */
  /* 힌트: if (from > to) 이면 "[오류] 시작 단이 끝 단보다 클 수 없습니다." 출력
   * 후 return -1 */
  /* TODO */
  if (from>to) {
    printf("[오류] 시작 단이 끝 단보다 클 수 없습니다.");
  }

  count = to - from + 1;
  /* 힌트: 각 행의 결과를 저장할 int 배열을 count 크기로 선언하세요 */
  /* TODO */
  int result[count];

  /* ===== [TODO 7] 이중 for문으로 배열에 저장 후 가로 출력 + 단 수 반환 =====
   */
  printf("\n--- 배열기반 구구단 ---\n");
  for (int i = 1; i <= 9; i++) {
    /* 힌트 (저장): for문으로 d=0~count-1, result[d] = (from + d) * i */
    /* TODO */
    for (int d = 0; d < count; d++) {
    result[d] = (from + d) * i;
    }

    /* 힌트 (출력): for문으로 d=0~count-1, printf("%d x %d = %2d    ", from+d,
     * i, result[d]) */
    /* TODO */
    for (int d = 0; d < count; d++) {
     printf("%d x %d = %2d    ", from+d,i,result[d]);
    }
  

    printf("\n");
  }

  return count;
}

/* ===== F3: 알파벳 카운터 ===== */
int alpha_counter() {
  char str[100];
  int count[26] = {0};
  int non_zero = 0;
  int total_alpha = 0;

  /* [수정 금지] fgets로 문자열 입력
   * fgets는 공백을 포함한 문자열을 입력받을 수 있다.
   * scanf는 공백을 포함한 문자열을 입력받을 수 없다.
   * fgets에 대한 자세한 내용은 추후 강의에서 다룹니다.
   */
  printf("\n--- 알파벳 카운터 ---\n");
  printf("문자열 입력: ");
  getchar(); /* 이전 scanf 후 남은 개행 문자 제거 */
  fgets(str, sizeof(str), stdin);
  str[strcspn(str, "\n")] = '\0'; /* fgets 개행 제거 */

  /* ===== [TODO 8] 문자열 순회 + 소문자 변환 + 알파벳 카운트 ===== */
  for (int i = 0; str[i] != '\0'; i++) {
    char c = str[i];

    /* 힌트: c가 대문자('A'~'Z')이면 소문자로 변환 → c = c + ('a' - 'A') */
    /* TODO */
    if (c >= 'A' && c <= 'Z') {
        c = c + ('a' - 'A');
    }

    /* 힌트: c가 소문자('a'~'z')이면 문자별로 계수(count)하고,
     *       확인된 전체 알파벳 수를 갱신한다. (알파벳 아닌 문자는 무시) */
    /* TODO */
    if (c >= 'a' && c <= 'z') {
      alphabet_counts[c - 'a']++; 
      total_alpha ++;
  }

  /* ===== [TODO 9] 가드 조건 + non-zero 수 계산 후 반환 ===== */
  /* 힌트: total_alpha가 0이면 "[오류] 알파벳이 하나도 없습니다." 출력 후 return
   * -1 */
  /* TODO */
    if (total_alpha == 0) {
      printf("[오류] 알파벳이 하나도 없습니다.")
        return -1 ;

  printf("\n=== 알파벳 출현 횟수 ===\n");
  for (int i = 0; i < 26; i++) {
    if (count[i] > 0) {
      printf("%c: %d\n", 'a' + i, count[i]);
      non_zero++;
    }
  }

  return non_zero;
}

/* ===== F4: 피보나치 수열 (재귀함수) ===== */
int fibonacci(int n) {
  /* ===== [TODO 10] 가드 조건: n < 0 → 오류 메시지 + return -1 ===== */
  /* 힌트: if (n < 0) 이면 "[오류] N은 0 이상이어야 합니다." 출력 후 return -1
   */
  /* TODO */
  if (n < 0) {
    printf("[오류] N은 0 이상이어야 합니다.\n");
    return -1;

  /* ===== [TODO 11] base case + 재귀 호출 반환 ===== */
  /* 힌트:
   * - n이 0이면 return 0
   * - n이 1이면 return 1
   * - 그 외: return fibonacci(n-1) + fibonacci(n-2)
   */
  /* TODO */
    if (n== 0) {
      return 0;
    }
    if (n==-1) {
      return 1;
    }
    return fibonacci(n-1) + fibonacci(n-2) ;

  return 0; /* ← 위 TODO를 완성하면 이 줄은 실행되지 않습니다 */
}
