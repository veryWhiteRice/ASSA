package com.example.hello.notification;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.graphics.Color;
import android.os.Build;

import androidx.core.app.NotificationCompat;

import com.example.hello.R;

public class MqttNotification {

    private static final String ON_MESSAGE = "소독이 실행되었습니다.";
    private static final String OFF_MESSAGE = "소독이 꺼졌습니다.";

    private static final String AUTO_ON_MESSAGE = "소독이 자동 실행되었습니다.";
    private static final String AUTO_OFF_MESSAGE = "소독이 완료되었습니다.";

    public void createNotification(Context context, String message, String topic) {
        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, "default");

        builder.setSmallIcon(R.mipmap.ic_launcher)
                .setContentTitle("자동차 소독 알림 띵동 : " + topic)
                .setAutoCancel(true);

        if (message.equals("3:UV_ON")) {
            builder.setContentText(ON_MESSAGE).setColor(Color.GREEN);
        }else if (message.equals("3:UV_OFF")){
            builder.setContentText(OFF_MESSAGE).setColor(Color.RED);
        } else if (message.equals("3:AUTO_ON")) {
            builder.setContentText(AUTO_ON_MESSAGE).setColor(Color.GREEN);
        } else if (message.equals("3:AUTO_OFF")) {
            builder.setContentText(AUTO_OFF_MESSAGE).setColor(Color.RED);
        }



        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            notificationManager.createNotificationChannel(new NotificationChannel("default", "기본 채널", NotificationManager.IMPORTANCE_DEFAULT));
        }

        notificationManager.notify(1, builder.build());
    }

}
