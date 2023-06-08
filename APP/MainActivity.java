package com.example.hello;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.example.hello.layout.MainButton;
import com.example.hello.layout.SettingActivity;
import com.example.hello.memory.SharedPreferencesMemory;
import com.example.hello.mqtt.MqttUtil;

import org.eclipse.paho.client.mqttv3.MqttException;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getSimpleName();

    private final String OFF = "UV OFF";
    private final String ON = "UV ON";


    private MqttUtil mqttUtil;
    private SharedPreferencesMemory sharedPreferencesMemory;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        sharedPreferencesMemory = SharedPreferencesMemory.getInstance();
        sharedPreferencesMemory.setSharedPreferencesMemory(this);

        mqttUtil = MqttUtil.getMqttUtilInstance();

        MainButton.makeInstance(findViewById(R.id.toggleButton));

        makeSubscribeList();
    }

    public void intentSettingLayout(View view) {
        Intent intent = new Intent(this, SettingActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(intent);
    }

    public void uvButtonClick(View view) throws MqttException {
        CompoundButton toggleButton = findViewById(R.id.toggleButton);

        // uvBtn의 텍스트 값이 기본값이 있으면 안된다. 현재 내 자동차 상태를 보고 text를 결정해야한다.

        if (toggleButton.isChecked()) {
            toggleButton.setBackgroundResource(R.drawable.shape_for_circle_button);
            mqttUtil.publishMessage("UV ON");
        }else {
            toggleButton.setBackgroundResource(R.drawable.shape_for_circle_button_red);
            mqttUtil.publishMessage("UV OFF");
        }

    }

    private void makeSubscribeList() {
        TextView textView = findViewById(R.id.nowTopic);
        String nowTopic = sharedPreferencesMemory.getNowTopic();

        if (nowTopic != null) {
            textView.setText("현재 자동차 : " + nowTopic);
        }

        /*
        * 앱이 켜질 때 현재 구독 중인 자동차에게 check 메시지를 보내고
        * 해당 자동차는 check 메시지를 받으면 현재 상태(ON/OFF)를 보낸다.
        * 해당 메시지에 따라 스위치의 상태를 바꾼다.
        * */
    }

}