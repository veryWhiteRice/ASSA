package com.example.hello.layout;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.example.hello.MainActivity;
import com.example.hello.R;
import com.example.hello.memory.SharedPreferencesMemory;
import com.example.hello.mqtt.MqttUtil;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public class SettingActivity extends AppCompatActivity {

    private MqttUtil mqttUtil;
    private SharedPreferencesMemory sharedPreferencesMemory;

    private Map<Integer, String> buttonTopicRepository;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);

        mqttUtil = MqttUtil.getMqttUtilInstance();
        sharedPreferencesMemory = SharedPreferencesMemory.getInstance();
        buttonTopicRepository = new HashMap<>();

        makeTableLayout();
    }

    public void backButtonOnclick(View view) {
        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
    }


    public void onClickAddButton(View view) {
        String topic = getTopic();

        if (checkDuplicateTopic(topic)) {
            EditText editText = findViewById(R.id.editText);
            editText.setText("");

            sharedPreferencesMemory.addTopic(topic);
            addTableRow(findViewById(R.id.tableLayout), topic);
        } else {
            Toast.makeText(this, "중복되었습니다.", Toast.LENGTH_SHORT).show();
        }
    }

    private String getTopic() {
        EditText editText = findViewById(R.id.editText);
        return editText.getText().toString();
    }

    private boolean checkDuplicateTopic(String topic) {
        Set<String> savedTopics = sharedPreferencesMemory.getTopicsAtSharedPreference();
        if (savedTopics == null) {
            return true;
        }

        return savedTopics.contains(topic) == false;
    }

    private void subscribeTopic(int buttonId) {
        String topic = buttonTopicRepository.get(buttonId);
        if (mqttUtil.setSubscribe(topic, this)) {
            sharedPreferencesMemory.setNowTopic(topic);
            Toast.makeText(this, topic + "구독되었습니다.", Toast.LENGTH_SHORT);
        }
    }

    private void unSubscribeTopic(int buttonId) {
        String topic = buttonTopicRepository.get(buttonId);
        mqttUtil.unSubscribe(topic, this);
    }

    private void removeTopic(int buttonId) {
        String topic = buttonTopicRepository.get(buttonId);
        buttonTopicRepository.remove(buttonId);
        if (mqttUtil.unSubscribe(topic, this)) {
            sharedPreferencesMemory.removeTopic(topic);
            sharedPreferencesMemory.checkNowSubscribe(topic);
            Toast.makeText(this, "구독이 삭제되었습니다.",  Toast.LENGTH_SHORT).show();
        }else{
            Toast.makeText(this, "구독삭제에 실패했습니다.",  Toast.LENGTH_SHORT).show();
        }

        removeTableRow();
        makeTableLayout();
    }

    private void removeTableRow() {
        TableLayout tableLayout = findViewById(R.id.tableLayout);
        tableLayout.removeAllViews();
    }

    private void makeTableLayout() {
        TableLayout tableLayout = findViewById(R.id.tableLayout);


        Set<String> topics = sharedPreferencesMemory.getTopicsAtSharedPreference();
        if (topics == null) {
            return;
        }
        for (String topic : topics) {
            addTableRow(tableLayout, topic);
        }
    }

    private void addTableRow(TableLayout tableLayout, String topic) {
        TableRow tableRow = new TableRow(this);

        TextView textView = getTextView(topic);
        Button subscribeButton = getSubscribeButton(topic);
        Button cancelButton = getCancelButton(topic);
        Button removeButton = getRemoveButton(topic);

        tableRow.addView(textView);
        tableRow.addView(subscribeButton);
        tableRow.addView(cancelButton);
        tableRow.addView(removeButton);

        tableLayout.addView(tableRow);
    }

    @NonNull
    private TextView getTextView(String topic) {
        TextView textView = new TextView(this);
        textView.setText(topic);
        return textView;
    }

    @NonNull
    private Button getSubscribeButton(String topic) {
        return makeButton(topic, ButtonContent.SUBSCRIBE);
    }

    @NonNull
    private Button getRemoveButton(String topic) {
        return makeButton(topic, ButtonContent.REMOVE);
    }

    @NonNull
    private Button getCancelButton(String topic) {
        return makeButton(topic, ButtonContent.UNSUBSCRIBE);
    }

    private Button makeButton(String topic, ButtonContent buttonContent) {
        Button button = new Button(this);
        int buttonId = View.generateViewId();
        button.setId(buttonId);
        button.setText(buttonContent.getContent());
        buttonTopicRepository.put(buttonId, topic);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                choiceOnclickMethod(buttonContent, buttonId);
            }
        });

        TableRow.LayoutParams params = new TableRow.LayoutParams(
                TableRow.LayoutParams.WRAP_CONTENT, // 너비
                TableRow.LayoutParams.WRAP_CONTENT // 높이
        );
        params.width = 250; // 너비 설정
        params.height = 200; // 높이 설정
        button.setLayoutParams(params);

        return button;
    }

    private void choiceOnclickMethod(ButtonContent buttonContent, int buttonId) {
        if (buttonContent == ButtonContent.SUBSCRIBE) {
            subscribeTopic(buttonId);
        } else if (buttonContent == ButtonContent.UNSUBSCRIBE) {
            unSubscribeTopic(buttonId);
        }else{
            removeTopic(buttonId);
        }
    }

}



