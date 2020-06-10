package com.example.arc_app;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.app.Activity;
import android.app.Dialog;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.util.Pair;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.example.arc_app.Adapter.OptionsAdapter;
import com.example.arc_app.Adapter.RegisteredAdapter;
import com.example.arc_app.MQTT.MqttManager;

import java.util.ArrayList;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity {
    private static String TAG = "MainActivity";
    public static ArrayList<String> regDevices = new ArrayList<>();
    private ArrayList<String> optDevices = new ArrayList<>();
    private ArrayList<String> ID = new ArrayList<>();
    private Button regBtn;
    public static Dialog dialog;
    private Button dialogBtn;
    private RecyclerView recyclerView;
    private RegisteredAdapter regAdapter;
    private OptionsAdapter optAdapter;
    private RecyclerView.LayoutManager layoutManager;
    static public Pair<String,String> deviceChosed;
    //for data store
    private StoreData storeData;
    public static String filename = "datafile.txt";
    //for mqtt
    public static Handler handler_MQTT_message_receive;
    private final String URL="tcp://192.168.43.222:3000";
    private final String clientId="Android_App";
    private String topic = "arc";
    private JSONObject payload;
    //payload格式:
    //action: trig/reg/del
    //target: (device id)

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        regBtn = findViewById(R.id.btnregister);
        regBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                showDialog(MainActivity.this);
            }
        });
        showRegDevices();
        setTestdata();

        storeData = new StoreData(MainActivity.this);
        storeData.loadData();

        initMqttHandler();
        new Thread(new Runnable(){
            @Override
            public void run(){
                Log.d(TAG, "connecting to server");
                try {
                    MqttManager.getInstance().createConnection(URL, null, null, clientId);
                    MqttManager.getInstance().subscribe(clientId, 2);
                }catch (Exception e){
                    Log.d(TAG, e.toString());
                }
            }
        }).start();
    }

    @Override
    protected void onStop(){
        super.onStop();
        storeData.storeData();
    }

    private void showRegDevices(){
        recyclerView = findViewById(R.id.regRecycler);
        layoutManager = new LinearLayoutManager(MainActivity.this);
        regAdapter = new RegisteredAdapter(MainActivity.this,regDevices);
        recyclerView.setAdapter(regAdapter);
        recyclerView.setLayoutManager(layoutManager);
        regAdapter.setRegListener(new RegisteredAdapter.RegListener() {
            @Override
            public void onClick(String id) {
                sendMsg("trig",id);
            }

            @Override
            public void onDelete(String id) {
                sendMsg("del",id);
            }
        });
    }

    public void showDialog(Activity activity){
        dialog = new Dialog(activity);
        dialog.setCancelable(false);
        dialog.setContentView(R.layout.options_recycler);

        dialogBtn = dialog.findViewById(R.id.btndialog);
        dialogBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dialog.dismiss();
            }
        });

        RecyclerView recyclerView = dialog.findViewById(R.id.optRecycler);
        final EditText devicename = dialog.findViewById(R.id.device_name);
        optAdapter = new OptionsAdapter(MainActivity.this,optDevices);
        optAdapter.setOptListener(new OptionsAdapter.OptListener(){
            @Override
            public void onConfirm(int which){
                String name = devicename.getText().toString().trim();
                if(name == null || name.isEmpty()){
                    Toast toast = Toast.makeText(MainActivity.this, "請輸入裝置ID",Toast.LENGTH_SHORT);
                    toast.setGravity(Gravity.CENTER,0,0);
                    toast.show();
                }
                else {
                    deviceChosed = new Pair<>(name, optDevices.get(which));
                    updateDevice();
                    dialog.dismiss();
                }
            }
            @Override
            public void onCancel(){
            }
        });
        recyclerView.setAdapter(optAdapter);
        recyclerView.setLayoutManager(new LinearLayoutManager(MainActivity.this));
        dialog.show();
    }

    private void updateDevice(){
        Toast toast = Toast.makeText(this, "已新增裝置: "+deviceChosed.first+"/"+deviceChosed.second,Toast.LENGTH_SHORT);
        toast.setGravity(Gravity.CENTER,0,0);
        toast.show();
        regAdapter.addData(deviceChosed.first+"/"+deviceChosed.second);
        sendMsg("reg",deviceChosed.first);
    }

    private void initMqttHandler(){
        handler_MQTT_message_receive = new Handler(Looper.getMainLooper()){
            public void handleMessage(Message message) {
                super.handleMessage(message);
                if(message.what == 0){}
                else{}
            }
        };
    }

    private void sendMsg(String action, String id){
        //action: trig/reg/del
        //target: (device id)
        payload = new JSONObject();
        Log.d(TAG,"sendMsg: "+action+"/"+id);
        try {
            payload.put("action", action);
            payload.put("target", id);
            new Thread(new Runnable() {
                @Override
                public void run() {
                    MqttManager.getInstance().publish(topic, 2, payload.toString().getBytes());
                }
            }).start();
        }catch (Exception e){
            Log.d(TAG, e.toString());
        }
    }

    private  void setTestdata(){
//        regDevices.add("A");
//        regDevices.add("B");
//        regDevices.add("C");
//        regDevices.add("D");
//        regDevices.add("E");
//        regDevices.add("F");
//        regDevices.add("G");
//        regDevices.add("H");
//        regDevices.add("I");
//        regDevices.add("J");
//        regDevices.add("K");
//        regDevices.add("L");
//        regDevices.add("M");

        optDevices.add("廚房");
        optDevices.add("客廳");
        optDevices.add("大門");
        optDevices.add("書房");
        optDevices.add("玄關");
        optDevices.add("餐廳");

        ID.add("AEF1932d");
        ID.add("FAMS1242");
        ID.add("ZZF193as");
        ID.add("EIQF1O2d");
        ID.add("PQ129324");
        ID.add("AOQ12325");
        ID.add("YRF1N801");
    }
}
