package com.example.arc_app.Adapter;

import android.content.Context;
import android.content.DialogInterface;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.RecyclerView;

import com.example.arc_app.R;
import static com.example.arc_app.MainActivity.regDevicesMac;

import java.util.ArrayList;

public class RegisteredAdapter extends RecyclerView.Adapter<RegisteredAdapter.RegisteredHolder> {
    private LayoutInflater inflater;
    private ArrayList<String> regDevices;
    private Context mContext;
    private RegListener mRegListener;
    private String TAG = "RegisteredAdapter";

    public RegisteredAdapter(Context ctx, ArrayList<String> regDevices){
        mContext = ctx;
        inflater = LayoutInflater.from(mContext);
        this.regDevices = regDevices;
    }

    @Override
    public RegisteredAdapter.RegisteredHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = inflater.inflate(R.layout.registered_devices_layout, parent, false);
        RegisteredHolder holder = new RegisteredHolder(view);
        return holder;
    }

    @Override
    public void onBindViewHolder(RegisteredAdapter.RegisteredHolder holder, int position) {
        holder.device.setText(regDevices.get(position));
    }

    @Override
    public int getItemCount() {
        return regDevices.size();
    }

    public void addData(String name) {
        regDevices.add(regDevices.size(), name);
        notifyItemInserted(regDevices.size());
    }
    public void removeData(int position) {
        regDevices.remove(position);
        regDevicesMac.remove(position);
        notifyItemRemoved(position);
        notifyDataSetChanged();
    }

    class RegisteredHolder extends RecyclerView.ViewHolder{
        TextView device;

        public RegisteredHolder(View itemView) {
            super(itemView);
            device = (TextView) itemView.findViewById(R.id.device);
            itemView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    String deviceName = regDevices.get(getAdapterPosition());
                    Toast toast = Toast.makeText(mContext, deviceName,Toast.LENGTH_SHORT);
                    toast.setGravity(Gravity.CENTER,0,0);
                    toast.show();
                    String array[] = deviceName.split("/");
                    mRegListener.onClick(array[0], getAdapterPosition());
                }
            });
            itemView.setOnLongClickListener(new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View view) {
                    final int index = getAdapterPosition();
                    new AlertDialog.Builder(mContext)
                            .setTitle("刪除裝置 "+regDevices.get(index)+" ?")
                            .setPositiveButton("確認", new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    Toast toast = Toast.makeText(mContext, "已刪除裝置: "+regDevices.get(index),Toast.LENGTH_SHORT);
                                    toast.setGravity(Gravity.CENTER,0,0);
                                    toast.show();
                                    String array[] = regDevices.get(index).split("/");
                                    try {
                                        mRegListener.onDelete(array[0],index);
                                    }catch (Exception e){
                                        Log.d(TAG, e.toString());
                                    }
                                    removeData(index);
                                }
                            }).setNegativeButton("取消", new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialogInterface, int i) {

                                }
                            })
                            .create()
                            .show();
                    return false;
                }
            });
        }
    }

    public void setRegListener(RegListener regListener){
        mRegListener = regListener;
    }
    public interface RegListener{
        void onClick(String id, int index);
        void onDelete(String id, int index);
    }

}