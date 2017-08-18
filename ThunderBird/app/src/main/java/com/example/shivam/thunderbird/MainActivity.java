package com.example.shivam.thunderbird;

import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.provider.Settings;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.FileDescriptor;

public class MainActivity extends AppCompatActivity {

    AlertDialog.Builder alertBuilder4BT;
    AlertDialog dialog4BT;
    BluetoothAdapter BTadpt = BluetoothAdapter.getDefaultAdapter();
    TextView heading,tag;
    ImageView img,gifImg;
    MediaPlayer mp;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        IntentFilter filter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(mReceiver, filter);

        heading = (TextView) findViewById(R.id.textView);
        tag = (TextView) findViewById(R.id.textView2);
        img = (ImageView) findViewById(R.id.imageView);
        gifImg = (ImageView) findViewById(R.id.gifImageView);

        mp = MediaPlayer.create(getApplicationContext(), R.raw.carstart);

        alertBuilder4BT = new AlertDialog.Builder(MainActivity.this);
        alertBuilder4BT.setTitle("Unable to connect to car.");
        alertBuilder4BT
                .setMessage("Ignore if you don't want to connect to car.")
                .setCancelable(false)
                .setPositiveButton("Turn on",new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        BTadpt.enable();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        dialog4BT.cancel();
                    }
                });

        dialog4BT = alertBuilder4BT.create();

        if (!BTadpt.isEnabled()){
            heading.setText("Bluetooth : Disconnected");
            heading.setTextColor(Color.RED);
            gifImg.setVisibility(View.INVISIBLE);
            img.setVisibility(View.VISIBLE);
            mp.stop();
            tag.setVisibility(View.INVISIBLE);
            dialog4BT.show();
        }
        else
        {
            heading.setText("Bluetooth : On");
            heading.setTextColor(Color.GREEN);
            gifImg.setVisibility(View.VISIBLE);
            img.setVisibility(View.INVISIBLE);
            mp.start();
            tag.setVisibility(View.VISIBLE);
            if(attempt2Connect()){

            }
        }

    }

    private boolean attempt2Connect() {
        return true;
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
                        BluetoothAdapter.ERROR);
                switch (state) {

                    case BluetoothAdapter.STATE_OFF:
                        heading.setText("Bluetooth : Disconnected");
                        heading.setTextColor(Color.RED);
                        gifImg.setVisibility(View.INVISIBLE);
                        img.setVisibility(View.VISIBLE);
                        dialog4BT.show();
                        break;

                    case BluetoothAdapter.STATE_TURNING_OFF:
                        tag.setVisibility(View.INVISIBLE);
                        mp.stop();
                        break;

                    case BluetoothAdapter.STATE_ON:
                        heading.setText("Bluetooth : On");
                        heading.setTextColor(Color.GREEN);
                        gifImg.setVisibility(View.VISIBLE);
                        img.setVisibility(View.INVISIBLE);
                        if (dialog4BT.isShowing())
                            dialog4BT.hide();
                        break;

                    case BluetoothAdapter.STATE_TURNING_ON:
                        mp.release();
                        mp = MediaPlayer.create(getApplicationContext(), R.raw.carstart);
                        mp.start();
                        tag.setVisibility(View.VISIBLE);
                        break;
                }
            }
        }
    };

    public void openMap(View view){
        Intent openMap = new Intent(MainActivity.this,MapsActivity.class);
        startActivity(openMap);
    }
}