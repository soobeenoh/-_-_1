// vending_machine.c
// 주간실습 #1 - 문제 2: 자판기
#include <stdio.h>

int main() {
    int balance = 0;    // 현재 잔액
    int insert;         // 투입 금액
    int choice;         // 메뉴 선택

    printf("=== 자판기 ===\n");
    printf("[메뉴] 1.콜라(1000원)  2.사이다(1200원)  3.물(800원)  0.잔액반환\n\n");

    while (1) {
        printf("현재 잔액: %d원\n", balance);
        printf("투입할 금액을 입력하세요 (0이면 메뉴로 이동): ");
        scanf("%d", &insert);

        // [TODO 1] insert 금액을 balance에 더하세요.
        //          힌트: balance = balance + insert; 와 같은 의미입니다.


        printf("메뉴를 선택하세요: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:  // 콜라 (1000원)
                // [TODO 2] balance가 1000 이상이면 콜라를 구매합니다.
                //          아래 if 조건식을 완성하세요.
                if (              ) {
                    balance -= 1000;
                    printf("콜라가 나왔습니다. 잔액: %d원\n\n", balance);
                } else {
                    printf("잔액 부족! %d원이 더 필요합니다.\n\n", 1000 - balance);
                }
                break;

            case 2:  // 사이다 (1200원)
                if (balance >= 1200) {
                    // [TODO 3] balance에서 사이다 가격(1200원)을 차감하세요.


                    printf("사이다가 나왔습니다. 잔액: %d원\n\n", balance);
                } else {
                    printf("잔액 부족! %d원이 더 필요합니다.\n\n", 1200 - balance);
                }
                break;

            case 3:  // 물 (800원)
                if (balance >= 800) {
                    balance -= 800;
                    printf("물이 나왔습니다. 잔액: %d원\n\n", balance);
                } else {
                    printf("잔액 부족! %d원이 더 필요합니다.\n\n", 800 - balance);
                }
                break;

            case 0:  // 잔액 반환 및 종료
                // [TODO 4] 잔액을 안내하는 메시지를 출력하세요.
                //          출력 예시: "잔액 500원을 반환합니다. 이용해 주셔서 감사합니다!"
                //          힌트: balance 변수를 사용하세요.


                return 0;

            default:
                printf("잘못된 입력입니다. 메뉴 번호를 확인하세요.\n\n");
        }
    }

    return 0;
}
