#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define PIR1 27 // PIR ���� 1�� GPIO �� ��ȣ
#define PIR2 28 // PIR ���� 2�� GPIO �� ��ȣ
#define PIR3 29 // PIR ���� 3�� GPIO �� ��ȣ
#define SWITCH_1 25 // ����ġ 1�� GPIO �� ��ȣ
#define SWITCH_2 24 // ����ġ 2�� GPIO �� ��ȣ (�Է� ����)

#define LED_1 23 // LED 1�� GPIO �� ��ȣ
#define LED_2 22 // LED 2�� GPIO �� ��ȣ   (��¼���)

int main(void) {
    if (wiringPiSetup() == -1) {
        printf("WiringPi setup error.\n");
        return -1;
    }

    // �� ��� ����
    pinMode(PIR1, INPUT);
    pinMode(PIR2, INPUT);
    pinMode(PIR3, INPUT);
    pinMode(SWITCH_1, INPUT);
    pinMode(SWITCH_2, INPUT); //�Է¼���
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT); //��¼���

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
		{ // ����ġ�� �ϳ��� on�Ǿ� ������ LED_2�� ������ ����
            digitalWrite(LED_1, HIGH);
            count = 0;
            flag = 0;
            printf("switch ON\n");
            delay(500);
            continue;
        }
        else {
            if (flag == 1) { // LED_2�� �����ٰ� ���������� ����Ǹ� ����ġ�� ���������� �ٽô� ������ ����
                continue;
            }
            if (digitalRead(PIR1) == LOW && digitalRead(PIR2) == LOW && digitalRead(PIR3) == LOW) 
			{ // PIR�������� ������ �������� ������
                if (count >= 600)
				{ // �����ð� �������� �������� ������
                    digitalWrite(LED_2, HIGH); // LED_2�� ����
                    printf("LED_2 ON\n");
                    if (digitalRead(LED_1) == HIGH) // LED_1�� ���� ������
                        digitalWrite(LED_1, LOW); // LED_1�� ����
                    led_count = 0; // LED_2�� �������� �ð� count
                    while (1) 
					{
                        if (digitalRead(SWITCH_1) == HIGH || digitalRead(SWITCH_2) == HIGH)
						{ // LED_2�� �����ִµ��� ����ġ�� on �Ǹ�
                            digitalWrite(LED_2, LOW); // LED_2�� ����
                            printf("switch ON during LED_2 is ON\n");
                            count = 0;
                            flag = 0;
                            break;
                        }
                        if (led_count >= 3600) { // LED_2�� �����ð� ������
                            printf("LED_2 OFF\n");
                            digitalWrite(LED_2, LOW); // LED_2 ����
                            count = 0;
                            flag = 1;
                            break;
                        }
                        else {
                            led_count++; // LED_2�� �����ִµ��� count ����
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