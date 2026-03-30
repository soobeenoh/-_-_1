// dungeon_battle.c
// 주간실습 #1 - 문제 3: 던전 배틀
#include <stdio.h>
#include <stdlib.h>  // rand(), srand() 함수를 사용하기 위해 필요합니다.
#include <time.h>    // time() 함수를 사용하기 위해 필요합니다.

/*
 * get_random_damage(min, max) - min~max 사이의 임의의 데미지를 반환하는 함수
 *
 * [지금 당장 이해하지 않아도 됩니다]
 * 이 함수는 아직 배우지 않은 개념(함수의 매개변수)을 사용합니다.
 * 지금은 "min 이상 max 이하의 임의의 데미지 값을 골라주는 기능"으로만 이해하세요.
 * 함수와 stdlib는 이후 수업에서 자세히 다룹니다.
 *
 * --- 내부 동작 설명 (궁금한 학생을 위해) ---
 * rand() % (max - min + 1) + min
 *   - max - min + 1 : 가능한 값의 개수  (예: min=3, max=7 이면 5가지: 3,4,5,6,7)
 *   - % (max-min+1) : 0 ~ (max-min) 범위로 좁힘
 *   - + min         : min ~ max 범위로 이동
 *   - 예) min=3, max=7 → rand()%5+3 → 3,4,5,6,7 중 하나
 */
int get_random_damage(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int main() {
    srand(time(NULL));  // 프로그램 시작 시 한 번만 호출 (수정하지 마세요)

    int player_hp = 20;       // 플레이어 HP (몬스터 전투 내내 유지됩니다)
    int monster_hp;           // 현재 몬스터 HP
    int monster_dmg_min;      // 현재 몬스터 최소 데미지
    int monster_dmg_max;      // 현재 몬스터 최대 데미지
    int choice;               // 플레이어 선택 (1.공격 / 2.방어)

    printf("=== 던전에 입장합니다! (몬스터 3마리) ===\n");
    printf("플레이어 HP: %d\n", player_hp);

    // 외부 for: 몬스터 3마리를 순서대로 상대합니다.
    for (int i = 1; i <= 3; i++) {

        // 몬스터별 스탯 설정 (수정하지 마세요)
        switch (i) {
            case 1:
                printf("\n=============================\n");
                printf("[%d/3] 슬라임 출현! (HP: 8)\n", i);
                monster_hp = 8;
                monster_dmg_min = 1;
                monster_dmg_max = 3;
                break;
            case 2:
                printf("\n=============================\n");
                printf("[%d/3] 고블린 출현! (HP: 12)\n", i);
                monster_hp = 12;
                monster_dmg_min = 2;
                monster_dmg_max = 4;
                break;
            case 3:
                printf("\n=============================\n");
                printf("[%d/3] 드래곤 출현! (HP: 20) ★보스★\n", i);
                monster_hp = 20;
                monster_dmg_min = 3;
                monster_dmg_max = 6;
                break;
        }

        // [TODO 1] 내부 while: 몬스터 HP와 플레이어 HP가 모두 0보다 클 때 반복합니다.
        //          힌트: while (조건1 && 조건2) 형태로 작성합니다.
        while ( monster_hp >0 && player_hp >0 ) {

            printf("\n[내 HP: %d] [몬스터 HP: %d]\n", player_hp, monster_hp);
            printf("1.공격  2.방어: ");
            scanf("%d", &choice);

            // 플레이어의 이번 턴 공격력 계산 (3~7 사이의 임의의 값)
            int player_dmg = get_random_damage(3, 7);

            if (choice == 1) {  // 공격
                // [TODO 2] 플레이어가 monster_dmg만큼 몬스터에게 피해를 줍니다.
                //          힌트: monster_hp에서 player_dmg를 빼세요.
                monster_hp -= player_dmg ;

                if (monster_hp < 0) monster_hp = 0;
                printf("내가 %d 데미지! 몬스터 HP: %d\n", player_dmg, monster_hp);
            } else if (choice == 2) {  // 방어
                printf("방어 태세! 이번 턴 받는 피해가 절반으로 줄어듭니다.\n");
            } else {
                printf("잘못된 입력입니다. 공격으로 처리합니다.\n");
                monster_hp -= player_dmg;
                if (monster_hp < 0) monster_hp = 0;
                printf("내가 %d 데미지! 몬스터 HP: %d\n", player_dmg, monster_hp);
            }

            // 몬스터 반격 (몬스터가 살아있을 때만)
            if (monster_hp > 0) {
                int monster_dmg = get_random_damage(monster_dmg_min, monster_dmg_max);
                if (choice == 2) {
                    monster_dmg /= 2;
                    if (monster_dmg < 1) monster_dmg = 1;
                }
                // [TODO 3] 몬스터가 monster_dmg만큼 플레이어에게 피해를 줍니다.
                //          힌트: player_hp에서 monster_dmg를 빼세요.
                player_hp -= monster_dmg ;

                if (player_hp < 0) player_hp = 0;
                printf("몬스터가 %d 데미지! 내 HP: %d\n", monster_dmg, player_hp);
            }

            // [TODO 4] 플레이어 HP가 0 이하이면 "게임 오버" 메시지를 출력하고 종료하세요.
            //          출력 예시: "*** 게임 오버! 2번째 몬스터에게 쓰러졌습니다... ***"
            //          힌트: i 변수로 몇 번째 몬스터인지 나타낼 수 있습니다.
           if (player_hp <= 0)
              printf(" *** 게임 오버! %d번째 몬스터에게 쓰러졌습니다... ***",i);
              break;

        }

        printf("몬스터를 처치했습니다! (남은 HP: %d)\n", player_hp);
    }

    printf("\n=============================\n");
    printf("=== 던전 클리어! 최종 HP: %d/%d ===\n", player_hp, 20);

    return 0;
}

