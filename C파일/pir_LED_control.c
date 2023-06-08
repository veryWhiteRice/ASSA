#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define PIR1 27 // PIR 센서 1의 GPIO 핀 번호
#define PIR2 28 // PIR 센서 2의 GPIO 핀 번호
#define PIR3 29 // PIR 센서 3의 GPIO 핀 번호
#define SWITCH_1 25 // 스위치 1의 GPIO 핀 번호
#define SWITCH_2 24 // 스위치 2의 GPIO 핀 번호 (입력 센서)

#define LED_1 23 // LED 1의 GPIO 핀 번호
#define LED_2 22 // LED 2의 GPIO 핀 번호   (출력센서)

int main(void) {
    if (wiringPiSetup() == -1) {
        printf("WiringPi setup error.\n");
        return -1;
    }

    // 핀 모드 설정
    pinMode(PIR1, INPUT);
    pinMode(PIR2, INPUT);
    pinMode(PIR3, INPUT);
    pinMode(SWITCH_1, INPUT);
    pinMode(SWITCH_2, INPUT); //입력센서
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT); //출력센서

    digitalWrite(PIR1, HIGH);
    digitalWrite(PIR2, HIGH);
    digitalWrite(PIR3, HIGH);
    digitalWrite(LED_1, LOW);
    digitalWrite(LED_2, LOW);
    delay(500);

    int count = 0;
    int flag = 0;
    int led_count = 0;
    while (1) {
        if (digitalRead(SWITCH_1) == HIGH || digitalRead(SWITCH_2) == HIGH)
		{ // 스위치가 하나라도 on되어 있으면 LED_2가 켜지지 않음
            digitalWrite(LED_1, HIGH);
            count = 0;
            flag = 0;
            printf("switch ON\n");
            delay(500);
            continue;
        }
        else {
            if (flag == 1) { // LED_2가 켜졌다가 정상적으로 종료되면 스위치가 눌릴때까지 다시는 켜지지 않음
                continue;
            }
            if (digitalRead(PIR1) == LOW && digitalRead(PIR2) == LOW && digitalRead(PIR3) == LOW) 
			{ // PIR센서에서 동작이 감지되지 않을때
                if (count >= 600)
				{ // 일정시간 움직임이 감지되지 않으면
                    digitalWrite(LED_2, HIGH); // LED_2가 켜짐
                    printf("LED_2 ON\n");
                    if (digitalRead(LED_1) == HIGH) // LED_1이 켜져 있으면
                        digitalWrite(LED_1, LOW); // LED_1이 꺼짐
                    led_count = 0; // LED_2가 켜져있을 시간 count
                    while (1) 
					{
                        if (digitalRead(SWITCH_1) == HIGH || digitalRead(SWITCH_2) == HIGH)
						{ // LED_2가 켜져있는동안 스위치가 on 되면
                            digitalWrite(LED_2, LOW); // LED_2가 꺼짐
                            printf("switch ON during LED_2 is ON\n");
                            count = 0;
                            flag = 0;
                            break;
                        }
                        if (led_count >= 3600) { // LED_2가 일정시간 켜지고
                            printf("LED_2 OFF\n");
                            digitalWrite(LED_2, LOW); // LED_2 끄기
                            count = 0;
                            flag = 1;
                            break;
                        }
                        else {
                            led_count++; // LED_2가 켜져있는동안 count 증기
                            printf("led_count %d\n", led_count);
                            delay(500);
                        }
                    }
                }
                else {
                    printf("No move, count = %d\n", count);
                    count++;
                    delay(500);
                }
            }
            else {
                printf("move\n");
                count = 0;
                delay(500);
            }
        }
    }
    return 0;
}