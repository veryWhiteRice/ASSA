package com.example.hello.memory;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;

import java.util.HashSet;
import java.util.Set;

public class SharedPreferencesMemory {

    private static final SharedPreferencesMemory sharedPreferencesMemory = new SharedPreferencesMemory();

    private static final String TOPICS = "topics";
    private static final String NOW_TOPIC = "nowTopic";

    private SharedPreferences sharedPreferences;
    private SharedPreferences.Editor editor;

    private SharedPreferencesMemory() {
    }

    public void setSharedPreferencesMemory(Context context) {
        this.sharedPreferences = PreferenceManager.getDefaultSharedPreferences(context);
        this.editor = sharedPreferences.edit();
    }

    public static SharedPreferencesMemory getInstance() {
        return sharedPreferencesMemory;
    }

    public Set<String> getTopicsAtSharedPreference() {
        return sharedPreferences.getStringSet(TOPICS, null);
    }

    public String getNowTopic() {
        return sharedPreferences.getString(NOW_TOPIC, null);
    }

    public void setNowTopic(String nowSubscribe) {
        editor.putString(NOW_TOPIC, nowSubscribe);
        editor.apply();
    }

    public SharedPreferences.Editor getEditor() {
        return editor;
    }

    public SharedPreferences getSharedPreferences() {
        return sharedPreferences;
    }

    public void addTopic(String topic) {
        Set<String> savedTopics = getTopicsAtSharedPreference();

        if (savedTopics == null) {
            savedTopics = new HashSet<>();
        }
        savedTopics.add(topic);
        editor.putStringSet(TOPICS, savedTopics);
        editor.apply();
    }

    public void removeTopic(String topic) {
        Set<String> savedTopics = getTopicsAtSharedPreference();
        SharedPreferences.Editor editor = getEditor();

        if (savedTopics.contains(topic)) {
            savedTopics.remove(topic);
            editor.putStringSet(TOPICS, savedTopics);
            editor.apply();
        }
    }

    public void checkNowSubscribe(String topic) {
        String nowTopic = getNowTopic();
        if (topic.equals(nowTopic)) {
            editor.putString(NOW_TOPIC, null);
            editor.apply();
        }
    }
}
