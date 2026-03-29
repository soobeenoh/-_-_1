// guess_number.c
// 주간실습 #1 - 문제 1: 숫자 맞추기 게임
#include <stdio.h>
#include <stdlib.h>  // rand(), srand() 함수를 사용하기 위해 필요합니다.
#include <time.h>    // time() 함수를 사용하기 위해 필요합니다.

/*
 * get_random_secret() - 1~100 사이의 임의의 정수를 반환하는 함수
 *
 * [지금 당장 이해하지 않아도 됩니다]
 * 이 함수는 아직 배우지 않은 개념(함수, 표준 라이브러리)을 사용합니다.
 * 지금은 "실행할 때마다 다른 정답 숫자를 골라주는 기능"으로만 이해하세요.
 * 함수와 stdlib는 이후 수업에서 자세히 다룹니다.
 *
 * --- 내부 동작 설명 (궁금한 학생을 위해) ---
 *
 * 1. srand(time(NULL))
 *    - rand()가 숫자를 뽑는 방식은 사실 완전한 무작위가 아니라,
 *      특정 시작값(seed)을 기준으로 계산되는 규칙적인 수열입니다.
 *    - seed가 같으면 항상 같은 순서의 숫자가 나옵니다.
 *    - srand()는 이 seed를 설정하는 함수입니다.
 *    - time(NULL)은 현재 시각을 초(second) 단위 정수로 반환합니다.
 *    - 따라서 실행할 때마다 시각이 달라지므로, 매번 다른 수열이 만들어집니다.
 *    - srand()는 프로그램에서 딱 한 번만 호출하면 됩니다.
 *      (<stdlib.h>, <time.h> 필요)
 *
 * 2. rand() % 100 + 1
 *    - rand()는 0 이상의 임의의 정수를 반환합니다.
 *    - % 100 : 나머지 연산으로 0~99 범위로 좁힙니다.  (W02에서 배운 % 연산자)
 *    - + 1   : 1~100 범위로 이동합니다.
 *    - 예)  rand()가 283을 반환했다면 → 283 % 100 = 83 → 83 + 1 = 84
 */
int get_random_secret() {
    srand(time(NULL));
    return rand() % 100 + 1;
}

int main() {
    int secret = get_random_secret();  // 정답 숫자 (1~100 임의의 수)
    int guess = 0;                     // 사용자가 입력한 숫자
    int max_tries = 5;                 // 최대 시도 횟수

    printf("=== 숫자 맞추기 게임 ===\n");
    printf("1~100 사이의 숫자를 맞춰보세요! (기회: %d번)\n\n", max_tries);

    for (int tries = 1; tries <= max_tries; tries++) {
        printf("[%d/%d] 숫자 입력: ", tries, max_tries);

        // [TODO 1] 사용자로부터 정수를 입력받아 guess에 저장하세요.
        //          힌트: scanf("%d", ...)


        if (guess == secret) {
            printf("정답! %d번 만에 맞췄습니다!\n", tries);
            return 0;
        } else if (/* [TODO 2] guess가 secret보다 작은 조건을 작성하세요 */ 0) {
            printf("더 큰 숫자를 입력하세요.\n");
        } else {
            printf("더 작은 숫자를 입력하세요.\n");
        }
    }

    // [TODO 3] 기회를 모두 사용한 경우 아래 형식으로 출력하세요.
    //          출력 예시: "기회를 모두 사용했습니다. 정답은 42였습니다."
    //          힌트: secret 변수를 사용하세요.


    return 0;
}
