package com.example.arc_app;

import android.content.Context;
import android.util.Log;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import static com.example.arc_app.MainActivity.regDevices;
import static com.example.arc_app.MainActivity.filename;

public class StoreData {
    private FileInputStream fin;
    private FileOutputStream fout;
    private static String TAG = "StoreData";
    private Context mContext;

    public StoreData(Context context){
        mContext = context;
    }

    public void loadData(){
        //open file
        try {
            fin = mContext.openFileInput(filename);
        }catch (Exception e){
            Log.d(TAG,e.toString());
        }
        //get data
        try {
            int c;
            String temp="";
            while( (c = fin.read()) != -1){
                temp = temp + (char)c;
            }
            String[] array = temp.split(" ");
            for(String device : array)
                if(!device.trim().isEmpty())
                    regDevices.add(device);
        } catch (Exception e) {
            Log.d(TAG,e.toString());
        }
        //close file
        try {
            fin.close();
        }catch (Exception e){
            Log.d(TAG,e.toString());
        }
    }

    public void storeData(){
        //open file
        try {
            fout = mContext.openFileOutput(filename, Context.MODE_PRIVATE);
        }catch (Exception e){
            Log.d(TAG,e.toString());
        }
        //write data
        String data="";
        for(String device : regDevices){
            data+=device;
            data+=" ";
        }
        data.trim();
        try {
            fout.write(data.getBytes());
        }catch (Exception e){
            Log.d(TAG,e.toString());
        }
        //close file
        try {
            fout.close();
        }catch (Exception e){
            Log.d(TAG,e.toString());
        }
    }

}