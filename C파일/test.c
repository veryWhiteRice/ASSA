// 최종
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define _TIMESPEC_DEFINED
#include <pthread.h>
#include <wiringPi.h>
#include "MQTTClient.h"
#define HAVE_STRUCT_TIMESPEC

#define PIR1 27 // PIR 센서 1의 GPIO 핀 번호
#define PIR2 28 // PIR 센서 2의 GPIO 핀 번호
#define PIR3 29 // PIR 센서 3의 GPIO 핀 번호
#define SWITCH_1 25 // 스위치 1의 GPIO 핀 번호
#define SWITCH_2 24 // 스위치 2의 GPIO 핀 번호 (입력 센서)

#define LED_1 23 // LED 1의 GPIO 핀 번호
#define LED_2 22 // LED 2의 GPIO 핀 번호   (출력센서)

#define ADDRESS     "tcp://broker.hivemq.com:1883"
#define CLIENTID    "Han"
#define TOPIC_SUB       "topic"
#define TOPIC_PUB     "topicCar"
#define PAYLOAD_ON  "UV_ON"
#define PAYLOAD_OFF "UV_OFF"
#define PAYLOAD_AUTO_ON "AUTO_ON"
#define PAYLOAD_AUTO_OFF "AUTO_OFF"
#define ME          "3"
#define QOS         1
#define TIMEOUT     100000L

MQTTClient client;
int UVflag = -1; // 0:OFF  1:ON  2:AUTO_OFF  3:AUTO_ON
MQTTClient_message* msg = NULL;

void* publisher_thread(void* arg)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    pubmsg.payload = NULL;
    pubmsg.payloadlen = 0;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    while (1)
    {
        if (UVflag == -1)//아직 receive가 없다면 계속 -1
        {
            delay(1000);
            continue;//while 루프 탄다 
        }
        if (UVflag == 1)
        {
            char payload_with_client_id[50];
            snprintf(payload_with_client_id, sizeof(payload_with_client_id), "%s:%s", ME, PAYLOAD_ON);

            pubmsg.payload = payload_with_client_id;
            pubmsg.payloadlen = strlen(payload_with_client_id);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;
            MQTTClient_publishMessage(client, TOPIC_PUB, &pubmsg, &token);
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
            UVflag = -1;//메세지 보내고 난 후 flag -1
        }
        else if (UVflag == 0)//OFF
        {
            char payload_with_client_id[50];
            snprintf(payload_with_client_id, sizeof(payload_with_client_id), "%s:%s", ME, PAYLOAD_OFF);

            pubmsg.payload = payload_with_client_id;
            pubmsg.payloadlen = strlen(payload_with_client_id);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;
            MQTTClient_publishMessage(client, TOPIC_PUB, &pubmsg, &token);
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
            UVflag = -1;//메세지 보내고 난 후 flag -1
        }
        else if (UVflag == 2)//AUTO_OFF
        {
            char payload_with_client_id[50];
            snprintf(payload_with_client_id, sizeof(payload_with_client_id), "%s:%s", ME, PAYLOAD_AUTO_OFF);

            pubmsg.payload = payload_with_client_id;
            pubmsg.payloadlen = strlen(payload_with_client_id);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;
            MQTTClient_publishMessage(client, TOPIC_PUB, &pubmsg, &token);
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
            UVflag = -1;//메세지 보내고 난 후 flag -1
        }
        else if (UVflag == 3)//AUTO_ON
        {
            char payload_with_client_id[50];
            snprintf(payload_with_client_id, sizeof(payload_with_client_id), "%s:%s", ME, PAYLOAD_AUTO_ON);

            pubmsg.payload = payload_with_client_id;
            pubmsg.payloadlen = strlen(payload_with_client_id);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;
            MQTTClient_publishMessage(client, TOPIC_PUB, &pubmsg, &token);
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
            UVflag = -1;//메세지 보내고 난 후 flag -1
        }
        //MQTTClient_freeMessage(&msg);//msg 비우기
        delay(500);
    }
}

void* subscriber_thread(void* arg)
{
    MQTTClient_message* msg = NULL;
    char* topic = NULL;
    int topicLen;
    int rc;

    while (1)
    {
        rc = MQTTClient_receive(client, &topic, &topicLen, &msg, TIMEOUT);

        if (rc == MQTTCLIENT_SUCCESS)
        {
            char clientId[2];
            strncpy(clientId, (char*)msg->payload, 1);
            clientId[1] = '\0';

            if (strcmp(clientId, ME) != 0)
            {
                printf("Subscribed to topic '%s'\n", TOPIC_SUB);
                printf("Waiting for messages...\n");
                printf("Received message: %.*s\n", msg->payloadlen, (char*)msg->payload);

                // 메시지를 받았을 때 동작을 여기에 추가하세요.
                // 예를 들어, 게시자 스레드에 메시지를 전달하여 처리하도록 할 수 있습니다.
                if (!strcmp((char*)msg->payload, "UV ON"))
                {
                    UVflag = 1;
                }
                else if (!strcmp((char*)msg->payload, "UV OFF"))
                {
                    UVflag = 0;
                }
            }

            MQTTClient_freeMessage(&msg);
        }
        delay(500);
    }
}
void* car_sensor_thread()
{
    int PIR_count = 0;
    int flag = 0;
    int UV_ON_count = 0;
    while (1) {
        if (digitalRead(SWITCH_1) == HIGH || digitalRead(SWITCH_2) == HIGH) { // 스위치가 하나라도 켜져 있으면 LED_2가 켜지지 않음
            digitalWrite(LED_1, HIGH); // LED_1 켜기
            PIR_count = 0; flag = 0; UV_ON_count = 0;
            delay(500);
            continue;
        }
        else {
            if (UVflag == 1) { // 앱에서 LED_2를 키라는 message가 오면
                digitalWrite(LED_2, HIGH); // LED_2가 켜짐
                printf("LED_2 ON\n");
                UV_ON_count = 0;

                while (1) {
                    if ((digitalRead(SWITCH_1) == HIGH || digitalRead(SWITCH_2) == HIGH) || 
                        ((digitalRead(PIR1) == HIGH || digitalRead(PIR2) == HIGH || digitalRead(PIR3) == HIGH)))
                    { // LED_2가 켜져있는동안 스위치가 켜지거나 움직임이 감지되면
                        digitalWrite(LED_2, LOW); // LED_2가 꺼짐
                        UVflag = 0; // LED_2가 꺼졌다는 message 보내기
                        flag = 0; PIR_count = 0; UV_ON_count = 0;
                        break;
                    }
                    
                    if (UVflag == 0) { // LED_2를 끄라는 message가 오면
                        digitalWrite(LED_2, LOW); // LED_2 끄기
                        delay(200);
                        break;
                    }
                    if (UV_ON_count >= 10) { // LED_2가 일정시간 켜지고
                        printf("LED_2 AUTO_OFF\n");
                        digitalWrite(LED_2, LOW); // LED_2 끄기
                        UVflag = 2; // AUTO_OFF 라는 message 보내기
                        UV_ON_count = 0; flag = 1; PIR_count = 0;
                        break;
                    }
                    else {
                        UV_ON_count++; // LED_2가 켜져있는동안 UV_ON_count 증가
                        printf("UV_ON_count %d\n", UV_ON_count);
                        delay(500);
                    }
                }
            }

            if (flag == 1) { // LED_2가 켜졌다가 일정시간 이후 종료되면 스위치가 켜지거나 앱에서 message가 오지 않으면 다시 켜지지 않음
                if (UVflag == 1) { // 앱에서 LED_2를 켜라는 message가 오면
                    digitalWrite(LED_2, HIGH); // LED_2 켜기 
                    printf("LED_2 ON\n");
                    PIR_count = 0; UV_ON_count = 0; flag = 0;
                }
                delay(100);
                continue;
            }

            if (digitalRead(PIR1) == LOW && digitalRead(PIR2) == LOW && digitalRead(PIR3) == LOW) { // PIR센서에서 동작이 감지되지 않을때
                if (PIR_count >= 10) { // 일정시간 움직임이 감지되지 않으면
                    digitalWrite(LED_2, HIGH); // LED_2가 켜짐
                    printf("LED_2 AUTO_ON\n");
                    UVflag = 3; // AUTO_ON 이라는 message 보냄
                    if (digitalRead(LED_1) == HIGH) // LED_1이 켜져 있으면
                        digitalWrite(LED_1, LOW); // LED_1이 꺼짐
                    UV_ON_count = 0; // UV_ON_count 시작
                    while (1) {
                        if ((digitalRead(SWITCH_1) == HIGH || digitalRead(SWITCH_2) == HIGH) ||
                            ((digitalRead(PIR1) == HIGH || digitalRead(PIR2) == HIGH || digitalRead(PIR3) == HIGH)))
                        { // LED_2가 켜져있는동안 스위치가 켜지거나 움직임이 감지되면
                            digitalWrite(LED_2, LOW); // LED_2가 꺼짐
                            UVflag = 0; // LED_2가 꺼졌다는 message 보냄
                            PIR_count = 0; flag = 0; UV_ON_count = 0;
                            break;
                        }
                        if (UVflag == 0) { // LED_2를 끄라는 message가 오면
                            digitalWrite(LED_2, LOW); // LED_2 끄기
                            PIR_count = 0; flag = 0; UV_ON_count = 0;
                            delay(100);
                            break;
                        }
                        if (UV_ON_count >= 10) { // LED_2가 일정시간 켜지고
                            printf("LED_2 AUTO_OFF\n");
                            digitalWrite(LED_2, LOW); // LED_2 끄기
                            UVflag = 2; // AUTO_OFF 라는 message 보냄
                            UV_ON_count = 0; PIR_count = 0; flag = 1;
                            break;
                        }
                        else {
                            UV_ON_count++; // LED_2가 켜져있는동안 UV_ON_count 증가
                            printf("UV_ON_count %d\n", UV_ON_count);
                            delay(500);
                        }
                        delay(500);
                    }
                }
                else {
                    printf("No move, PIR_count = %d\n", PIR_count);
                    PIR_count++;
                    delay(500);
                }
            }
            else { // 움직임이 감지되면
                printf("move\n");
                PIR_count = 0; flag = 0; UV_ON_count = 0;
                delay(100);
            }
        }
        delay(500);
    }
}

int main(void)
{
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
    delay(100);

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    else {
        printf("Connection Success!!!\n");
    }

    MQTTClient_subscribe(client, TOPIC_SUB, QOS);

    pthread_t pub_thread, sub_thread, car_thread;
    pthread_create(&pub_thread, NULL, publisher_thread, NULL);
    pthread_create(&sub_thread, NULL, subscriber_thread, NULL);
    pthread_create(&car_thread, NULL, car_sensor_thread, NULL);

    pthread_join(pub_thread, NULL);
    pthread_join(sub_thread, NULL);
    pthread_join(car_thread, NULL);


    MQTTClient_disconnect(client, 100000);
    MQTTClient_destroy(&client);
    return rc;
}