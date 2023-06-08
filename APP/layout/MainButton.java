package com.example.hello.layout;

import android.widget.CompoundButton;

import com.example.hello.R;

public class MainButton {

    private static MainButton mainButton;
    private static CompoundButton toggleButton;

    private MainButton(CompoundButton toggleButton) {
        this.toggleButton = toggleButton;
    }

    public static void changeButtonState(ButtonContent buttonContent) {
        if (buttonContent==ButtonContent.OFF) {
            toggleButton.setChecked(false);
            toggleButton.setBackgroundResource(R.drawable.shape_for_circle_button_red);
        } else{
            toggleButton.setChecked(true);
            toggleButton.setBackgroundResource(R.drawable.shape_for_circle_button);
        }
    }

    public static MainButton getInstance() {
        if (mainButton == null) {
            throw new IllegalArgumentException();
        }
        return mainButton;
    }

    public static void makeInstance(CompoundButton toggleButton) {
        mainButton = new MainButton(toggleButton);
    }
}
