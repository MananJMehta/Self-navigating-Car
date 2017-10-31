package sjsucmpe243.application.tatanano;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Build;
import android.os.ParcelUuid;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    AlertDialog.Builder alertBuilder4BT;
    AlertDialog dialog4BT;
    BluetoothAdapter BTadpt = BluetoothAdapter.getDefaultAdapter();
    BluetoothSocket gotSocket;
    TextView heading,tag;
    ImageView img,gifImg;
    MediaPlayer mp;
    Button pair;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (BTadpt == null){
            alertBuilder4BT = new AlertDialog.Builder(MainActivity.this);
            alertBuilder4BT.setTitle("Can't proceed.");
            alertBuilder4BT
                    .setMessage("Your device doesn't support bluetooth service. Use a device with a bluetooth facility.")
                    .setCancelable(false)
                    .setNegativeButton("Exit", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            MainActivity.this.finish();
                        }
                    })
                    .create();
        }
        IntentFilter filter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(mReceiver, filter);

        IntentFilter filter1 = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        registerReceiver(mReceiver1, filter1);

        IntentFilter filter2 = new IntentFilter(BluetoothAdapter.ACTION_CONNECTION_STATE_CHANGED);
        registerReceiver(mReceiver2, filter2);

        heading = (TextView) findViewById(R.id.textView);
        tag = (TextView) findViewById(R.id.textView2);
        img = (ImageView) findViewById(R.id.imageView);
        gifImg = (ImageView) findViewById(R.id.gifImageView);
        pair = (Button) findViewById(R.id.pairWithCar);


        alertBuilder4BT = new AlertDialog.Builder(MainActivity.this);
        alertBuilder4BT.setTitle("Unable to connect to car.");
        alertBuilder4BT
                .setMessage("Ignore if you don't want to connect to car.")
                .setCancelable(false)
                .setPositiveButton("Turn on",new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {

                        Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                        startActivityForResult(enableBtIntent, 5);

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
            setMainpage_4_Off();
            stopSound();
            dialog4BT.show();
        }
        else
        {
            setMainpage_4_On();
            playSound();
        }

        pair.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                if (connect2Car()){
                    Log.e("thunderBird Bluetooth","Connected");
                    heading.setText("Bluetooth : Connected");
                    tag.setVisibility(View.VISIBLE);
                } else {
                    Log.e("thunderBird Bluetooth","Unable to connect");
                    heading.setText("Bluetooth : Try again");
                    tag.setVisibility(View.INVISIBLE);
                }
            }
        });
    }

    private void stopSound() {
        mp.stop();
    }

    private void playSound() {
        if (mp != null)
            mp.release();

        mp = MediaPlayer.create(getApplicationContext(), R.raw.carstart);
        mp.start();
    }

    private void setMainpage_4_On() {
        heading.setText("Bluetooth : On");
        heading.setTextColor(Color.GREEN);
        gifImg.setVisibility(View.VISIBLE);
        img.setVisibility(View.INVISIBLE);
    }

    private void setMainpage_4_Off() {

        heading.setText("Bluetooth : Off");
        heading.setTextColor(Color.RED);
        gifImg.setVisibility(View.INVISIBLE);
        img.setVisibility(View.VISIBLE);
        tag.setVisibility(View.INVISIBLE);
    }

    private boolean connect2Car() {

        BluetoothDevice temp = findCarInPairedDevices();
        if (temp != null) {
            if (attempt2Connect(temp)) {
                return true;
            }else {
                return false;
            }
        }
        else {
            Log.e("thunderBird Bluetooth","device not found in paired devices list");
            if (!BTadpt.isDiscovering()){
                Log.e("thunderBird Bluetooth","started discovering.");
                BTadpt.startDiscovery();
            }
            return false;
        }
    }

    private BluetoothDevice findCarInPairedDevices(){

        Set<BluetoothDevice> pairedDevicesList = BTadpt.getBondedDevices();

        for (BluetoothDevice tempDevice : pairedDevicesList){
            if (tempDevice.getAddress().equals("B4:CE:F6:C3:36:B0") || tempDevice.getAddress().equals("DC:6D:CD:EE:92:36")){

                Log.e("thunderBird Bluetooth","Found device " +tempDevice.getName() + " in paired devices list");
                return tempDevice;
            }
        }
        return null;
    }

    private boolean attempt2Connect(BluetoothDevice bt){

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH_MR1) {
            for (ParcelUuid allUUIDS : bt.getUuids()){
                Log.e("thunderBird Bluetooth",allUUIDS.toString());
            }
        }

        ParcelUuid[] selectedOne = new ParcelUuid[0];
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.ICE_CREAM_SANDWICH_MR1) {
            selectedOne = bt.getUuids();
        }
        Log.e("thunderBird Bluetooth","Trying this. "+selectedOne[selectedOne.length-1].getUuid().toString());

        try {
            gotSocket = bt.createRfcommSocketToServiceRecord(selectedOne[selectedOne.length-1].getUuid());
        } catch (IOException e) {
            Log.e("thunderBird Bluetooth","Error creating Rfcomm Socket");
            return false;
        }

        if (BTadpt.isDiscovering())
            BTadpt.cancelDiscovery();

        try {

            if (gotSocket != null) {
                gotSocket.connect();
            }

            Log.e("thunderBird Bluetooth", "connected to " + bt.getName());

            return true;
        } catch (IOException e) {

            Log.e("thunderBird Bluetooth", "unable to connect to " + bt.getName());

            try {
                gotSocket.close();
                return false;
            } catch (IOException e1) {
                Log.e("thunderBird Bluetooth", "unable to close socket.");
                return false;
            }

        }
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
                        setMainpage_4_Off();
                        stopSound();
                        dialog4BT.show();
                        break;

                    case BluetoothAdapter.STATE_TURNING_OFF:
                        break;

                    case BluetoothAdapter.STATE_ON:
                        setMainpage_4_On();
                        playSound();

                        if (dialog4BT.isShowing())
                            dialog4BT.hide();
                        break;

                    case BluetoothAdapter.STATE_TURNING_ON:
                        break;
                }
            }
        }
    };

    public final BroadcastReceiver mReceiver1 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            Log.e("thunderBird Bluetooth",action);

            if (BluetoothDevice.ACTION_FOUND.equals(action)) {

                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                String deviceName = device.getName();
                String deviceHardwareAddress = device.getAddress(); // MAC address

                Log.e("thunderBird Bluetooth","found new device.- "+deviceName+" "+deviceHardwareAddress);

                if (deviceHardwareAddress.equals("B4:CE:F6:C3:36:B0") || deviceHardwareAddress.equals("DC:6D:CD:EE:92:36")) {

                    if (BTadpt.isDiscovering()) {
                        BTadpt.cancelDiscovery();
                    }

                    Log.e("thunderBird Bluetooth", "Stopped discovery.");

                    //attempt2Connect(device);
                }
            }
        }
    };

    private final BroadcastReceiver mReceiver2 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            Log.e("Bluetooth connection", action);
        }
    };

    public void openMap(View view){
        Intent openMap = new Intent(MainActivity.this,MapsActivity.class);
        startActivity(openMap);
    }

    @Override
    public void onBackPressed() {
        unregisterReceiver(mReceiver);
        unregisterReceiver(mReceiver1);
        this.finish();
    }

}