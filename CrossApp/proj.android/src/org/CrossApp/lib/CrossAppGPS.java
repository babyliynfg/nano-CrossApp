package org.CrossApp.lib;

import java.util.Iterator;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.location.Criteria;
import android.location.GpsSatellite;
import android.location.GpsStatus;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationProvider;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;

public class CrossAppGPS {
	
	private static final Context AndroidGPS = null;
	private static Activity s_pContext;
	public static LocationManager locationManager;
	public static String provider;
	private static final long serialVersionUID = -4582739827003032383L;
	protected static final String TAG = "location";   
	static Intent intent ;
	public static void Init( final Activity context )
	{
		s_pContext = context;
		openGPSSettings();	
	}
	
	public static void openGPSSettings()
	{
		locationManager = (LocationManager) s_pContext.getSystemService(Context.LOCATION_SERVICE);

        if (!locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
        	 intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
             s_pContext.startActivityForResult(intent,10);
            return;
        }    
    }
	
	public static  LocationListener locationListener = new LocationListener() { 

        /**
         * 位置信息变化时触发
         */
        public void onLocationChanged(Location location) {
            //updateView(location);
            Log.i(TAG, "时间："+location.getTime()); 
            Log.i(TAG, "经度："+location.getLongitude()); 
            Log.i(TAG, "纬度："+location.getLatitude()); 
            Log.i(TAG, "海拔："+location.getAltitude()); 
        }
        
        /**
         * GPS状态变化时触发
         */
        public void onStatusChanged(String provider, int status, Bundle extras) {
            switch (status) {
            //GPS状态为可见时
            case LocationProvider.AVAILABLE:
                break;
            //GPS状态为服务区外时
            case LocationProvider.OUT_OF_SERVICE:
                break;
            //GPS状态为暂停服务时
            case LocationProvider.TEMPORARILY_UNAVAILABLE:
                break;
            }
        }

		@Override
		public void onProviderDisabled(String arg0) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void onProviderEnabled(String arg0) {
			// TODO Auto-generated method stub
			
		}
 

	};
	
    
   static GpsStatus.Listener listener = new GpsStatus.Listener() {
        public void onGpsStatusChanged(int event) {
            switch (event) {

            case GpsStatus.GPS_EVENT_SATELLITE_STATUS:

                GpsStatus gpsStatus=locationManager.getGpsStatus(null);

                int maxSatellites = gpsStatus.getMaxSatellites();

                Iterator<GpsSatellite> iters = gpsStatus.getSatellites().iterator();
                int count = 0;     
                while (iters.hasNext() && count <= maxSatellites) {     
                    GpsSatellite s = iters.next();     
                    count++;     
                }   
                break;
            case GpsStatus.GPS_EVENT_STARTED:
                break;    
            case GpsStatus.GPS_EVENT_STOPPED:
                break;
            }
        };
    };
    
	public static void getLocation()
    {
		Criteria criteria= new Criteria();
        criteria.setAccuracy(Criteria.ACCURACY_FINE); 
        criteria.setAltitudeRequired(false);
        criteria.setBearingRequired(false);
        criteria.setCostAllowed(true);
        criteria.setBearingAccuracy(1);
        criteria.setPowerRequirement(Criteria.POWER_LOW);
        
        
		String serviceName = Context.LOCATION_SERVICE;
        locationManager = (LocationManager) s_pContext.getSystemService(serviceName);

        provider = locationManager.getBestProvider(criteria, true); 
        final Location location =  getLastKnownLocation();

		s_pContext.runOnUiThread(new Runnable()
		{
		
			@Override
			public void run() {
				locationManager.addGpsStatusListener(listener);
		        
				locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000,1, locationListener);			
			}
		});
		
	
		if(location != null)
		{
            returnLocationInfo(getLocationInfo(location));
		}
       
    }
	
	private static Location getLastKnownLocation()
	{
	    List<String> providers = locationManager.getProviders(true);
	    Location bestLocation = null;
	    for (String provider : providers)
	    {
	        Location l = locationManager.getLastKnownLocation(provider);
	        if (l == null)
	        {
	            continue;
	        }
	        if (bestLocation == null || l.getAccuracy() < bestLocation.getAccuracy())
	        {
	            // Found best last known location: %s", l);
	            bestLocation = l;
	            break;
	        }
	    }

	    return bestLocation;
	}

	private static void updateToNewLocation(final Location location)
	{
        if (location != null)
        {

            returnLocationInfo(getLocationInfo(location));
    
        }
    }
	
	
	private static CrossAppLocationInfo getLocationInfo(Location location)
	{
    	String longitude = Double.toString(location.getLongitude());
    	String latitude = Double.toString(location.getLatitude());
    	String altitude = Double.toString(location.getAltitude()); 	
    	String speed = Float.toString(location.getSpeed());
    	String bearing = Float.toString(location.getBearing());
    	
    	CrossAppLocationInfo mInfo = new CrossAppLocationInfo(longitude,latitude,altitude,speed,bearing);
   
    	return mInfo;
	}
	
	public static void stopUpdateLocation()
	{

	   if(locationManager != null)
	   {
	       locationManager.removeGpsStatusListener(listener);
	       locationManager.removeUpdates(locationListener);
	   }
	}
	
	public static native void returnLocationInfo(CrossAppLocationInfo info);
	
}
