package com.example.shivam.cmpe243;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
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
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.FragmentActivity;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.util.Log;
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

import java.util.Set;

import static com.example.shivam.cmpe243.R.id.map;

public class MapsActivity extends FragmentActivity implements OnMapReadyCallback {

    private GoogleMap mMap;
    LocationManager locMan;
    LocationListener locLis;
    LatLng myLatLng;
    static LatLng prevLatLng;
    boolean flag = true;
    Marker me;
    MarkerOptions me_Option;
    AlertDialog.Builder alertDialogBuilder, alertBuilder4BT;
    AlertDialog alertDialog,dialog4BT;
    BluetoothAdapter BTadpt = BluetoothAdapter.getDefaultAdapter();
    BluetoothDevice BT4car;
    BluetoothServerSocket serverSocket;
    Set<BluetoothDevice> pairedDevices;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);

        alertDialogBuilder = new AlertDialog.Builder(MapsActivity.this);
        alertDialogBuilder.setTitle("Unable to locate you.");
        alertDialogBuilder
                .setMessage("Click on Settings and turn it on.")
                .setCancelable(false)
                .setPositiveButton("Settings",new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {

                        Intent startGPS = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                        startActivity(startGPS);
                    }
                });

        alertDialog = alertDialogBuilder.create();

        alertBuilder4BT = new AlertDialog.Builder(MapsActivity.this);
        alertBuilder4BT.setTitle("Unable to connect to car.");
        alertBuilder4BT
                .setMessage("Click on Settings and turn it on. Ignore if okay.")
                .setCancelable(false)
                .setPositiveButton("Settings",new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {

                        Intent startBT = new Intent(Settings.ACTION_BLUETOOTH_SETTINGS);
                        startActivity(startBT);
                    }
                })
                .setNegativeButton("Cancle", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        dialog4BT.cancel();
                    }
                });

        dialog4BT = alertBuilder4BT.create();

        locMan = (LocationManager) getSystemService(LOCATION_SERVICE);
        if (!locMan.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
            alertDialog.show();
        }

        if (!BTadpt.isEnabled()){
            dialog4BT.show();
        }
        else
        {
            attempt2Connect();
        }

        IntentFilter filter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(mReceiver, filter);

        locLis = new LocationListener() {
            @Override
            public void onLocationChanged(Location location) {

                if (flag){
                    if (ActivityCompat.checkSelfPermission(MapsActivity.this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED
                            && ActivityCompat.checkSelfPermission(MapsActivity.this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                        // TODO: Consider calling
                        //    ActivityCompat#requestPermissions
                        // here to request the missing permissions, and then overriding
                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                        //                                          int[] grantResults)
                        // to handle the case where the user grants the permission. See the documentation
                        // for ActivityCompat#requestPermissions for more details.
                        return;
                    }


                    LocationManager temp = (LocationManager) MapsActivity.this.getSystemService(Context.LOCATION_SERVICE);
                    if (temp.isProviderEnabled(LocationManager.GPS_PROVIDER) && temp.getLastKnownLocation(LocationManager.GPS_PROVIDER)!= null)
                        myLatLng = new LatLng(temp.getLastKnownLocation(LocationManager.GPS_PROVIDER).getLatitude()
                                ,temp.getLastKnownLocation(LocationManager.GPS_PROVIDER).getLongitude());

                    if (myLatLng != null) {
                        me_Option = new MarkerOptions()
                                .icon(BitmapDescriptorFactory.fromResource(R.drawable.marker))
                                .position(myLatLng)
                                .rotation(0);

                        me = mMap.addMarker(me_Option);

                        CameraPosition cameraPosition = CameraPosition.builder()
                                .target(myLatLng)
                                .zoom(20)
                                .bearing(temp.getLastKnownLocation(LocationManager.GPS_PROVIDER).getBearing())
                                .build();

                        mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition),
                                500, null);
                        prevLatLng = myLatLng;
                    }
                    flag = false;
                    Toast.makeText(MapsActivity.this,"Let the marker get stable.\nThen tap on map to select destination.",Toast.LENGTH_LONG).show();
                }
                else {

                    //Toast.makeText(MapsActivity.this, "accuracy: " + location.getAccuracy() + ", Speed: " + location.getSpeed(), Toast.LENGTH_SHORT).show();

                    if (location.getSpeed() > 1 && location != null && prevLatLng != null && myLatLng != null) {

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

        locMan.requestLocationUpdates(LocationManager.GPS_PROVIDER, 3000, 2, locLis);

        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(map);
        mapFragment.getMapAsync(this);

    }

    private void attempt2Connect() {

    }

    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;

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

        mMap.setOnMapClickListener(new GoogleMap.OnMapClickListener() {

            @Override
            public void onMapClick(LatLng point) {

                MarkerOptions marker = new MarkerOptions().position(new LatLng(point.latitude, point.longitude));
                mMap.addMarker(marker);

                Toast.makeText(MapsActivity.this,"Next stop at "+point.latitude+","+point.longitude,Toast.LENGTH_SHORT).show();
            }
        });

        mMap.setMyLocationEnabled(true);

    }

    @Override
    public void onBackPressed() {
        locMan.removeUpdates(locLis);
        this.finish();
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
                        dialog4BT.show();
                        break;

                    case BluetoothAdapter.STATE_TURNING_OFF:
                        break;

                    case BluetoothAdapter.STATE_ON:
                        if (dialog4BT.isShowing())
                            dialog4BT.hide();
                        attempt2Connect();
                        break;

                    case BluetoothAdapter.STATE_TURNING_ON:
                        break;

                }
            }
        }
    };
}
