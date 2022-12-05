package com.knu.cclient;

import static com.google.android.material.internal.ContextUtils.getActivity;

import java.net.*;
import java.io.IOException;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.view.View;
import android.content.Intent;
import android.widget.Toast;

public class Connect extends AppCompatActivity {
    private static final String TAG = "Connect";
    String SERVER_IP = "192.168.1.5";
    int SERVER_PORT = 1337;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connect);

        setListeners();
    }

    private Socket connect() {
        Socket clientSocket = null;
        try {

            clientSocket = new Socket();
            clientSocket.connect(new InetSocketAddress(SERVER_IP, SERVER_PORT), 1000);
        } catch (Exception ex) {
            clientSocket = null;
            ex.printStackTrace();
            Log.e(TAG, "unknown socket error");
            showErr("connection error");
        }
        return clientSocket;
    }

    private void showErr(String errText) {
        runOnUiThread(new Runnable(){
            @Override
            public void run() {
                Toast.makeText(Connect.this,
                        errText, Toast.LENGTH_LONG).show();
            }
        });
    }

    private void setListeners() {
        Button button = findViewById(R.id.connect_Btn);

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        Socket cli = connect();
                        if (cli != null) {
                            try {
                                cli.close();
                                Intent intent = new Intent(view.getContext(), MainActivity.class);
                                view.getContext().startActivity(intent);
                            } catch (IOException ex) {

                            }
                        }
                    }
                }).start();
            }
        });
    }

}