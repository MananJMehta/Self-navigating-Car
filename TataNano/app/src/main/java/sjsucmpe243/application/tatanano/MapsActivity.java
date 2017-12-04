    package sjsucmpe243.application.tatanano;

    import android.Manifest;
    import android.bluetooth.BluetoothAdapter;
    import android.bluetooth.BluetoothDevice;
    import android.content.BroadcastReceiver;
    import android.content.Context;
    import android.content.DialogInterface;
    import android.content.Intent;
    import android.content.IntentFilter;
    import android.content.pm.PackageManager;
    import android.graphics.Color;
    import android.location.Location;
    import android.location.LocationListener;
    import android.location.LocationManager;
    import android.provider.Settings;
    import android.support.design.widget.FloatingActionButton;
    import android.support.v4.app.ActivityCompat;
    import android.support.v4.app.FragmentActivity;
    import android.os.Bundle;
    import android.support.v7.app.AlertDialog;
    import android.text.InputType;
    import android.util.Log;
    import android.view.View;
    import android.widget.EditText;
    import android.widget.TextView;
    import android.widget.Toast;

    import com.google.android.gms.maps.CameraUpdateFactory;
    import com.google.android.gms.maps.GoogleMap;
    import com.google.android.gms.maps.OnMapReadyCallback;
    import com.google.android.gms.maps.SupportMapFragment;
    import com.google.android.gms.maps.model.BitmapDescriptorFactory;
    import com.google.android.gms.maps.model.CameraPosition;
    import com.google.android.gms.maps.model.LatLng;
    import com.google.android.gms.maps.model.Marker;
    import com.google.android.gms.maps.model.MarkerOptions;
    import com.google.android.gms.maps.model.PolylineOptions;

    import java.io.IOException;
    import java.io.InputStream;
    import java.io.OutputStream;
    import java.util.ArrayList;

    public class MapsActivity extends FragmentActivity implements OnMapReadyCallback {

        private GoogleMap mMap;
        FloatingActionButton SendLocation,SendStart,SendStop;//,bluetooth;
        LocationManager locMan;
        LocationListener locLis;
        LatLng myLatLng;
        static LatLng prevLatLng = new LatLng(0, 0);
        Marker me;
        MarkerOptions me_Option;
        ArrayList<LatLng> checkpoints = new ArrayList<>();
        static int Checkpoint_num = 0;
        boolean flag = true;
        String BTdata;
        TextView actualSpeed, currPosVal, headingVal, bearing, deflection, distance;
        InputStream rx_data;
        OutputStream tx_data;
        boolean Toggle_start_stop = false;
        Thread t;

        String dataType = "", latVal="0", lngVal="0", headVal="0", speedVal="0",bearingVal="0", deflectionVal="0",distanceVal="0";

        AlertDialog.Builder alertDialogBuilder;
        AlertDialog alertDialog;
        AlertDialog.Builder speedDialogBuilder;
        AlertDialog speedDialog;

        @Override
        protected void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_maps);

            actualSpeed = findViewById(R.id.speedText);
            currPosVal = findViewById(R.id.CurrPosText);
            headingVal = findViewById(R.id.HeadingText);
            bearing = findViewById(R.id.bearingText);
            deflection = findViewById(R.id.deflectionAngle);
            distance = findViewById(R.id.distance2Checkpoint);

            try {
                rx_data = MainActivity.gotSocket.getInputStream();
                tx_data = MainActivity.gotSocket.getOutputStream();
            } catch (IOException e) {
                Log.e("TataNano : Bluetooth", "Socket is not connected yet. ");
            }

            t = new Thread() {

                @Override
                public void run() {
                    try {
                        while (!isInterrupted()) {

                            Thread.sleep(1);
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    try {
                                        if(rx_data.available()>0) {
                                            StringBuilder test = new StringBuilder();

                                            while(rx_data.available()>0) {
                                                test.append((char) (rx_data.read())).append("");
                                            }
                                            if (test.indexOf("&")!=-1) //If we have a '&' symbol at the end
                                            {
                                                /**
                                                 * Remove all the elements after '&' if there are any
                                                 */
                                                test = new StringBuilder(test.substring(0, test.indexOf("&")+1));
                                            }
                                            else
                                            {
                                                Log.d("DATA", "Complete String not found");
                                            }

                                            Log.d("RX DATA VALUE", test.toString()); //Print Sanitized code

                                            if(test.indexOf("G")==0 && (test.indexOf("&")==test.length()-1)) {
                                                /**
                                                 * Enter only if the first Character is 'G' and
                                                 * the last element is '&'
                                                 */

                                                //test = new StringBuilder(test.substring(0, test.toString().indexOf('&') + 1));

                                                int pipePos = test.indexOf("|");        //Get the index of '|'
                                                if (pipePos != -1)
                                                    dataType = test.substring(0, 1);    //Get the first character

                                                if (dataType.equals("G")) {
                                                    if (test.length() > 1) {
                                                        test = new StringBuilder(test.substring(1));                //starts after 'G'
                                                        pipePos = test.indexOf("|");
                                                        try {
                                                            latVal = test.substring(0, pipePos);
                                                            test = new StringBuilder(test.substring(pipePos + 1));  //starts after first '|'
                                                            pipePos = test.toString().indexOf('H');
                                                            lngVal = test.substring(0, pipePos);
                                                            test = new StringBuilder(test.substring(pipePos));            //starts with 'H'
                                                        } catch (StringIndexOutOfBoundsException e) {
                                                            e.printStackTrace();
                                                        }

                                                        try {
                                                            currPosVal.setText(String.format("%s, %s", latVal, lngVal));
                                                            headingVal.setText(test.toString());
                                                        } catch (NullPointerException e) {
                                                            Log.e("TataNano : Bluetooth", "null pointer at setting value of Location.");
                                                        }

                                                        if (test.substring(0, 1).equals("H")) {
                                                            test = new StringBuilder(test.substring(1));            //starts after 'H'
                                                            pipePos = test.toString().indexOf('S');
                                                            headVal = test.substring(0, pipePos);
                                                            test = new StringBuilder(test.substring(pipePos));            //starts with 'S'

                                                            try {
                                                                headingVal.setText(headVal);
                                                            } catch (NullPointerException e) {
                                                                Log.e("TataNano : Bluetooth", "null pointer at setting value of heading.");
                                                            }
                                                        }

                                                        if (test.substring(0, 1).equals("S")) {
                                                            test = new StringBuilder(test.substring(1));            //starts after 'S'
                                                                pipePos = test.toString().indexOf('B');
                                                            speedVal = test.substring(0, pipePos);
                                                            test = new StringBuilder(test.substring(pipePos));             //starts with 'B'

                                                            try {
                                                                actualSpeed.setText(speedVal);
                                                            } catch (NullPointerException e) {
                                                                Log.e("TataNano : Bluetooth", "null pointer at setting value of speed ");
                                                            }
                                                        }

                                                        if (test.substring(0, 1).equals("B")) {
                                                            test = new StringBuilder(test.substring(1));            //starts after 'B'
                                                            pipePos = test.toString().indexOf('A');
                                                            bearingVal = test.substring(0, pipePos);
                                                            test = new StringBuilder(test.substring(pipePos));             //starts with 'A'

                                                            try {
                                                                bearing.setText(bearingVal);
                                                            } catch (NullPointerException e) {
                                                                Log.e("TataNano : Bluetooth", "null pointer at setting value of bearing ");
                                                            }
                                                        }

                                                        if (test.substring(0, 1).equals("A")) {
                                                            test = new StringBuilder(test.substring(1));
                                                            pipePos = test.toString().indexOf('D');
                                                            deflectionVal = test.substring(0, pipePos);
                                                            test = new StringBuilder(test.substring(pipePos));

                                                            try {
                                                                deflection.setText(deflectionVal);
                                                            } catch (NullPointerException e) {
                                                                Log.e("TataNano : Bluetooth", "null pointer at setting value of deflection " + deflectionVal);
                                                            }
                                                        }

                                                        if (test.substring(0, 1).equals("D")) {
                                                            test = new StringBuilder(test.substring(1));
                                                            pipePos = test.toString().indexOf('&');

                                                            distanceVal = test.substring(0, pipePos);
                                                            //test = new StringBuilder(test.substring(pipePos));

                                                            try {
                                                                distance.setText(distanceVal);
                                                            } catch (NullPointerException e) {
                                                                Log.e("TataNano : Bluetooth", "null pointer at setting value of distance " + distanceVal);
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                Log.e("RX ERROR", "Did not receive the correct data");
                                            }
                                        }
                                    } catch (IOException e) {
                                        e.printStackTrace();
                                    }
                                }
                            });
                        }
                    } catch (InterruptedException ignored) {
                    }
                }
            };

            SendLocation = findViewById(R.id.SendLocation);
            SendLocation.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    StringBuilder outputString = new StringBuilder();
                    //LatLng temp = new LatLng(0,0);

                        Log.e("TataNano : Bluetooth", " Total check points are "+ Checkpoint_num );
                        outputString.append("^").append(String.valueOf(Checkpoint_num));

                    for (LatLng temp : checkpoints) {
                            //Log.e("TataNano : Bluetooth", "(" + temp.latitude + "," + temp.longitude+")");
                            outputString.append("(").append((float) temp.latitude).append(",").append((float) temp.longitude).append(")");
                    }
                    outputString.append("\n");
                    BTdata = outputString.toString();
                    Log.e("TataNano : Bluetooth", BTdata);
                    try {
                        tx_data.write(BTdata.getBytes());
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            });

            SendStart = findViewById(R.id.SendStart);
            SendStart.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {

                    final EditText speed = new EditText(MapsActivity.this);
                    speed.setInputType(InputType.TYPE_CLASS_NUMBER);
                    speedDialogBuilder = new AlertDialog.Builder(MapsActivity.this);
                    speedDialogBuilder.setTitle("Enter desired speed");
                    speedDialog = speedDialogBuilder
                    .setMessage("Valid input is 1 to 10 km/h.\n(Type 55 for free run)\n\nIn free run GPS and Compass value will be ignored and default speed will be 3 km/h.")
                    .setCancelable(false)
                    .setView(speed)
                    .setPositiveButton("Set and Send", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                        if(!speed.getText().toString().equals("")) {

                            if (Integer.parseInt(speed.getText().toString()) > 10) {
                                if (Integer.parseInt(speed.getText().toString()) == 55) {
                                    speed.setText("3-");
                                    Toast.makeText(MapsActivity.this, "Free Run set.", Toast.LENGTH_SHORT).show();
                                }
                                else {
                                    speed.setText(R.string.max_speed);
                                    Toast.makeText(MapsActivity.this, "Maximum possible value is 10. Speed has been set to 10.", Toast.LENGTH_SHORT).show();
                                }
                            }
                            else if (Integer.parseInt(speed.getText().toString()) < 1) {
                                speed.setText(R.string.min_speed);
                                Toast.makeText(MapsActivity.this,"Minimum possible value is 1. Speed has been set to 1.",Toast.LENGTH_SHORT).show();
                            }


                            BTdata = "$" + speed.getText().toString()+"Start\n";


                            try {
                                tx_data.write(BTdata.getBytes());
                                Log.e("TataNano : Bluetooth", " Start command sent: " + BTdata );
                                Toggle_start_stop = true;
                            } catch (NullPointerException e) {
                                Log.e("TataNano : Bluetooth", " Could not sent. Null pointer exception" );
                            } catch (IOException e) {
                                Log.e("TataNano : Bluetooth", " Could not sent. IO exception" );
                                MapsActivity.this.finish();
                            }
                            speedDialog.cancel();
                        }
                        else{
                            Toast.makeText(MapsActivity.this, "Value cannot be empty.", Toast.LENGTH_SHORT).show();
                            Toggle_start_stop = false;
                        }
                            if (Toggle_start_stop) {
                                SendStart.setVisibility(View.INVISIBLE);
                                SendStop.setVisibility(View.VISIBLE);
                            }
                        }
                    })
                    .create();
                    speedDialog.show();
                }
            });


            SendStop = findViewById(R.id.SendStop);
            SendStop.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    SendStart.setVisibility(View.VISIBLE);
                    SendStop.setVisibility(View.INVISIBLE);
                    BTdata = "@Stop\n";
                    try {
                        tx_data.write(BTdata.getBytes());
                        Log.e("TataNano : Bluetooth", " Stop command sent" );
                    } catch (NullPointerException e) {
                        Log.e("TataNano : Bluetooth", " Could not sent. Null pointer exception" );
                    } catch (IOException e) {
                        Log.e("TataNano : Bluetooth", " Could not sent. IO exception" );
                    }
                }
            });

            IntentFilter filter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
            registerReceiver(mReceiver, filter);

            IntentFilter filter2 = new IntentFilter();
            filter2.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
            filter2.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
            registerReceiver(mReceiver2, filter2);

            alertDialogBuilder = new AlertDialog.Builder(MapsActivity.this);
            alertDialogBuilder.setTitle("Unable to locate you.");
            alertDialogBuilder
                    .setMessage("Click on Settings and turn it on.")
                    .setCancelable(false)
                    .setPositiveButton("Settings", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {

                            Intent startGPS = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                            startActivity(startGPS);
                        }
                    });

            alertDialog = alertDialogBuilder.create();

            locMan = (LocationManager) getSystemService(LOCATION_SERVICE);
            if (locMan != null && !locMan.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
                alertDialog.show();
            }

            locLis = new LocationListener() {
                @Override
                public void onLocationChanged(Location location) {

                    if (flag) {
                        LocationManager temp = (LocationManager) MapsActivity.this.getSystemService(Context.LOCATION_SERVICE);
                        if (ActivityCompat.checkSelfPermission(MapsActivity.this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED
                                && ActivityCompat.checkSelfPermission(MapsActivity.this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED){
                            // TODO: Consider calling
                            //    ActivityCompat#requestPermissions
                            // here to request the missing permissions, and then overriding
                            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                            //                                          int[] grantResults)
                            // to handle the case where the user grants the permission. See the documentation
                            // for ActivityCompat#requestPermissions for more details.
                            return;
                        }

                        if (temp != null && temp.isProviderEnabled(LocationManager.GPS_PROVIDER) && temp.getLastKnownLocation(LocationManager.GPS_PROVIDER) != null)
                            myLatLng = new LatLng(temp.getLastKnownLocation(LocationManager.GPS_PROVIDER).getLatitude()
                                    , temp.getLastKnownLocation(LocationManager.GPS_PROVIDER).getLongitude());

                        if (myLatLng != null) {
                            me_Option = new MarkerOptions()
                                    .icon(BitmapDescriptorFactory.fromResource(R.drawable.marker))
                                    .position(myLatLng)
                                    .rotation(0);

                            me = mMap.addMarker(me_Option);

                            CameraPosition cameraPosition = null;
                            if (temp != null) {
                                cameraPosition = CameraPosition.builder()
                                        .target(myLatLng)
                                        .zoom(20)
                                        .bearing(temp.getLastKnownLocation(LocationManager.GPS_PROVIDER).getBearing())
                                        .build();
                            }

                            mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition),
                                    500, null);
                            prevLatLng = myLatLng;
                        }

                        flag = false;
                        Toast.makeText(MapsActivity.this, "Let the marker get stable.\nThen tap on map to select destination.", Toast.LENGTH_LONG).show();

                    } else {

                        if (Float.parseFloat(speedVal) > 0.75 && prevLatLng != null && myLatLng != null) {

                            myLatLng = new LatLng(location.getLatitude(), location.getLongitude());
                            //myLatLng = new LatLng(Double.parseDouble(latVal),Double.parseDouble(lngVal));

                            mMap.addPolyline(new PolylineOptions().add(prevLatLng, myLatLng).width(5).color(Color.BLUE).geodesic(true));

                            me.setPosition(myLatLng);
                            me.setRotation(location.getBearing());

                            CameraPosition cameraPosition = CameraPosition.builder()
                                    .target(myLatLng)
                                    .zoom(mMap.getCameraPosition().zoom)
                                    .bearing(location.getBearing())
                                    .build();

                            mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition),500, null);

                            prevLatLng = myLatLng;
                        }
                    }
                }

                @Override
                public void onStatusChanged(String s, int i, Bundle bundle) {

                }

                @Override
                public void onProviderEnabled(String s) {

                }

                @Override
                public void onProviderDisabled(String s) {
                    alertDialog.show();
                }
            };

            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED
                    && ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                return;
            }
            locMan.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1500, 2, locLis);

            // Obtain the SupportMapFragment and get notified when the map is ready to be used.
            SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                    .findFragmentById(R.id.map);
            mapFragment.getMapAsync(this);
        }


        /**
         * Manipulates the map once available.
         * This callback is triggered when the map is ready to be used.
         * This is where we can add markers or lines, add listeners or move the camera. In this case,
         * we just add a marker near Sydney, Australia.
         * If Google Play services is not installed on the device, the user will be prompted to install
         * it inside the SupportMapFragment. This method will only be triggered once the user has
         * installed Google Play services and returned to the app.
         */
        @Override
        public void onMapReady(GoogleMap googleMap) {
            mMap = googleMap;

            mMap.setOnMapClickListener(new GoogleMap.OnMapClickListener() {

                @Override
                public void onMapClick(LatLng point) {

                    MarkerOptions marker = new MarkerOptions().position(new LatLng(point.latitude, point.longitude));
                    mMap.addMarker(marker);
                    //checkpoints[Checkpoint_num] = point;
                    checkpoints.add(Checkpoint_num,point);
                    Checkpoint_num ++;
                    //Toast.makeText(MapsActivity.this, "Next stop at " + point.latitude + "," + point.longitude, Toast.LENGTH_SHORT).show();
                    Log.e("TataNano : Bluetooth", "onMapClick: point number= "+ Checkpoint_num + " (" + point.latitude + "," + point.longitude+")");
                }
            });

            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED
                    && ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                return;
            }
            mMap.setMyLocationEnabled(true);
            t.start();
        }



        @Override
        public void onBackPressed() {
            locMan.removeUpdates(locLis);
            unregisterReceiver(mReceiver);
            unregisterReceiver(mReceiver2);
            //t.destroy();
            //this.finish();
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
                            onBackPressed();
                            break;

                        case BluetoothAdapter.STATE_TURNING_OFF:
                            break;

                        case BluetoothAdapter.STATE_ON:
                            break;

                        case BluetoothAdapter.STATE_TURNING_ON:
                            break;

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
                        break;
                    case BluetoothDevice.ACTION_ACL_DISCONNECTED:
                        Toast.makeText(MapsActivity.this,"Connection lost with car. Searching car.",Toast.LENGTH_LONG).show();
                        MapsActivity.this.finish();
                        break;
                }
            }
        };
    }
