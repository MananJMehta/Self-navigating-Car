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
import android.util.Log;
import android.view.View;
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

    AlertDialog.Builder alertDialogBuilder;
    AlertDialog alertDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);

        SendLocation = findViewById(R.id.SendLocation);
        SendLocation.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //LatLng temp = new LatLng(0,0);

                try {
                    OutputStream rx_data = MainActivity.gotSocket.getOutputStream();
                    Log.e("TataNano : Bluetooth", " Total check points are "+ Checkpoint_num );

                    for (LatLng temp : checkpoints) {
                        Log.e("TataNano : Bluetooth", "(" + temp.latitude + "," + temp.longitude+")");
                        BTdata = "(" + temp.latitude + "," + temp.longitude+")\n";
                        rx_data.write(BTdata.getBytes());
                    }

                } catch (IOException e) {
                    Log.e("TataNano : Bluetooth", "Socket is not connected yet. ");
                }
            }
        });

        SendStart = findViewById(R.id.SendStart);
        SendStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SendStart.setVisibility(View.INVISIBLE);
                SendStop.setVisibility(View.VISIBLE);

                try {
                    OutputStream rx_data = MainActivity.gotSocket.getOutputStream();
                    Log.e("TataNano : Bluetooth", " Start command sent" );
                        BTdata = "start\n";
                        rx_data.write(BTdata.getBytes());
                } catch (IOException e) {
                    Log.e("TataNano : Bluetooth", "Socket is not connected yet. ");
                }
            }
        });


        SendStop = findViewById(R.id.SendStop);
        SendStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                SendStart.setVisibility(View.VISIBLE);
                SendStop.setVisibility(View.INVISIBLE);

                try {
                    OutputStream rx_data = MainActivity.gotSocket.getOutputStream();
                    Log.e("TataNano : Bluetooth", " Stop command sent" );
                    BTdata = "stop\n";
                    rx_data.write(BTdata.getBytes());
                } catch (IOException e) {
                    Log.e("TataNano : Bluetooth", "Socket is not connected yet. ");
                }
            }
        });

        IntentFilter filter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(mReceiver, filter);

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

                    //Toast.makeText(MapsActivity.this, "accuracy: " + location.getAccuracy() + ", Speed: " + location.getSpeed(), Toast.LENGTH_SHORT).show();

                    if (location.getSpeed() > 1 && prevLatLng != null && myLatLng != null) {

                        myLatLng = new LatLng(location.getLatitude(), location.getLongitude());

                        mMap.addPolyline(new PolylineOptions().add(prevLatLng, myLatLng).width(5).color(Color.BLUE).geodesic(true));

                    /*me_Option = new MarkerOptions()
                            .icon(BitmapDescriptorFactory.fromResource(R.drawable.marker))
                            .position(myLatLng)
                            .rotation(location.getBearing());

                    me = mMap.addMarker(me_Option);*/

                        me.setPosition(myLatLng);
                        me.setRotation(location.getBearing());

                        CameraPosition cameraPosition = CameraPosition.builder()
                                .target(myLatLng)
                                .zoom(mMap.getCameraPosition().zoom)
                                .bearing(location.getBearing())
                                .build();

                        mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition),
                                500, null);

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

    }



    @Override
    public void onBackPressed() {
        locMan.removeUpdates(locLis);
        unregisterReceiver(mReceiver);
        this.finish();
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
}
