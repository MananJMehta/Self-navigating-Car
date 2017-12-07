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
//import android.media.MediaPlayer;
import android.os.Build;
import android.os.ParcelUuid;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.util.Set;

public class MainActivity extends AppCompatActivity {

    AlertDialog.Builder alertBuilder4BT;
    AlertDialog dialog4BT;
    BluetoothAdapter BTadpt = BluetoothAdapter.getDefaultAdapter();
    static BluetoothSocket gotSocket;
    TextView heading,tag;
    ImageView gifImg;
    //MediaPlayer mp = MediaPlayer.create(getApplicationContext(), R.raw.startcar);
    static BluetoothDevice tatanano;

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

        IntentFilter filter2 = new IntentFilter();
        filter2.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        filter2.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        registerReceiver(mReceiver2, filter2);

        heading = findViewById(R.id.textView);
        tag = findViewById(R.id.textView2);
        gifImg = findViewById(R.id.gifImageView);

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
            //stopSound();
            dialog4BT.show();
        }
        else
        {
            setMainpage_4_On();
            //playSound();
        }

        gifImg.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (gotSocket != null && gotSocket.isConnected())
                    openMap();
                else
                    Toast.makeText(MainActivity.this, "Wait until we connect with car.", Toast.LENGTH_SHORT).show();
            }
        });

        connectivityThread.setDaemon(true);
        connectivityThread.start();
    }


    /*private void stopSound() {
        if (mp.isPlaying() && mp!=null)
            mp.stop();
    }

    private void playSound() {
        if (mp != null)
        {
            mp.release();
            mp.start();
        }
    }*/

    Thread connectivityThread = new Thread() {

        @Override
        public void run() {
                if (!connect2Car()) ;
        }
    };

    private boolean connect2Car() {

        //tatanano = findCarInPairedDevices();
        if (tatanano != null) {
            return (attempt2Connect(tatanano));
        }
        else {
            Log.e("TataNano : Bluetooth","device not found in paired devices list");
            if (!BTadpt.isDiscovering()){
                Log.e("TataNano : Bluetooth","started discovering.");
                if (!BTadpt.isDiscovering())
                BTadpt.startDiscovery();
            }
            return false;
        }
    }

    private BluetoothDevice findCarInPairedDevices(){

        Set<BluetoothDevice> pairedDevicesList = BTadpt.getBondedDevices();

        for (BluetoothDevice tempDevice : pairedDevicesList){
            if (tempDevice.getAddress().equals("98:D3:33:80:67:F9")){

                Log.e("TataNano : Bluetooth","Found device " +tempDevice.getName() + " in paired devices list");
                return tempDevice;
            }
        }
        return null;
    }

    private boolean attempt2Connect(BluetoothDevice bt) {


        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH_MR1) {
            for (ParcelUuid allUUIDS : bt.getUuids()){
                Log.e("TataNano : Bluetooth",allUUIDS.toString());
            }
        }

        ParcelUuid[] selectedOne = new ParcelUuid[0];
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.ICE_CREAM_SANDWICH_MR1) {
            selectedOne = bt.getUuids();
        }

            Log.e("TataNano : Bluetooth", "Trying this. " + selectedOne[0].getUuid().toString());
            try {
                gotSocket = bt.createRfcommSocketToServiceRecord(selectedOne[0].getUuid());
            } catch (IOException e) {
                //Log.e("TataNano : Bluetooth", "Error creating Rfcomm Socket");
                return false;
            }

            if (BTadpt.isDiscovering())
                BTadpt.cancelDiscovery();

            try {

                if (gotSocket != null) {
                    gotSocket.connect();
                }

                //Log.e("TataNano : Bluetooth", "connected to " + bt.getName());

                return true;
            } catch (IOException e) {

                //Log.e("TataNano : Bluetooth", "unable to connect to " + bt.getName());

                try {
                    gotSocket.close();
                    return false;
                } catch (IOException e1) {
                    //Log.e("TataNano : Bluetooth", "unable to close socket.");
                    return false;
                }

            }
    }

    private void setMainpage_4_On() {
        heading.setText(R.string.BTSearching);
        heading.setTextColor(Color.GREEN);
        gifImg.setVisibility(View.VISIBLE);
    }

    private void setMainpage_4_Off() {

        heading.setText(R.string.BTOff);
        heading.setTextColor(Color.RED);
        gifImg.setVisibility(View.INVISIBLE);
        tag.setVisibility(View.INVISIBLE);
    }

    private void setMainpage_4_Connected() {

        heading.setText(R.string.BTConnected);
        heading.setTextColor(Color.GREEN);
        gifImg.setVisibility(View.VISIBLE);
        gifImg.setClickable(true);
        tag.setVisibility(View.VISIBLE);
    }

    private void setMainpage_4_Disconnected() {

        heading.setText(R.string.BTSearching);
        heading.setTextColor(Color.RED);
        gifImg.setVisibility(View.VISIBLE);
        gifImg.setClickable(false);
        tag.setVisibility(View.INVISIBLE);
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (action != null && action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
                        BluetoothAdapter.ERROR);
                switch (state) {

                    case BluetoothAdapter.STATE_OFF:
                        setMainpage_4_Off();
                        //stopSound();
                        dialog4BT.show();
                        break;

                    case BluetoothAdapter.STATE_TURNING_OFF:
                        break;

                    case BluetoothAdapter.STATE_ON:
                        if (!(connectivityThread.isAlive()))
                            connectivityThread.run();
                        else
                            connectivityThread.start();
                        setMainpage_4_On();
                        //playSound();

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

            Log.e("TataNano : BT discovery",action);

            if (BluetoothDevice.ACTION_FOUND.equals(action)) {

                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                String deviceName = device.getName();
                String deviceHardwareAddress = device.getAddress(); // MAC address

                Log.e("TataNano : Bluetooth","found new device.- "+deviceName+" "+deviceHardwareAddress);

                if (deviceHardwareAddress.equals("98:D3:33:80:67:F9")) {

                    if (BTadpt.isDiscovering()) {
                        BTadpt.cancelDiscovery();
                        Log.e("TataNano : Bluetooth", "Stopped discovery.");
                    }

                    Log.e("TataNano : Bluetooth",device.getBondState()+"");
                    tatanano = device;
                    attempt2Connect(device);
                }
            }
        }
    };

    private final BroadcastReceiver mReceiver2 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            assert action != null;

            switch (action){
                case BluetoothDevice.ACTION_ACL_CONNECTED:
                    Log.e("TataNano : Bluetooth","Connected");
                    setMainpage_4_Connected();
                    BTadpt.cancelDiscovery();
                    break;
                case BluetoothDevice.ACTION_ACL_DISCONNECTED:
                    Log.e("TataNano : Bluetooth","Trying to connect");
                    setMainpage_4_Disconnected();
                    if (!BTadpt.isDiscovering())
                        BTadpt.startDiscovery();
                    break;
            }
        }
    };

    public void openMap(){
        Intent openMap = new Intent(MainActivity.this,MapsActivity.class);
        startActivity(openMap);
    }

    @Override
    public void onBackPressed() {
        unregisterReceiver(mReceiver);
        unregisterReceiver(mReceiver1);
        unregisterReceiver(mReceiver2);
        BTadpt.disable();
        this.finish();
    }

}