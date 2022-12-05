package com.knu.cclient;

import java.io.IOException;
import java.io.InputStreamReader;
import java.net.*;
import java.io.PrintWriter;
import java.io.BufferedReader;
import java.nio.CharBuffer;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    String SERVER_IP = "192.168.1.5";
    int SERVER_PORT = 1337;
    private static final String TAG = "Connect";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button button = findViewById(R.id.main_btnSetNew);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        EditText edtTemp = findViewById(R.id.main_editTemp);
                        EditText edtHum = findViewById(R.id.main_editHum);
                        String newTemp = edtTemp.getText().toString();
                        String newHum = edtHum.getText().toString();
                        Socket cli = connect();
                        if (cli != null) {
                            try {
                                PrintWriter out = new PrintWriter(cli.getOutputStream(), true);
                                out.write(newTemp);
                                out.flush();
                                out.write(newHum);
                            } catch (Exception ex) {

                            }
                        }
                    }
                }).start();
            }
        });

        new Thread(new Runnable() {
            @Override
            public void run() {
                while (true) {
                    try {
                        Socket cli = connect();
                        cli.setTcpNoDelay(true);
                        if (cli != null) {
                            TextView viewTemp = findViewById(R.id.main_viewTemp);
                            TextView viewHum = findViewById(R.id.main_viewHum);
                            String buf;
                            PrintWriter out = new PrintWriter(cli.getOutputStream(), true);
                            BufferedReader in = new BufferedReader(new InputStreamReader(cli.getInputStream()));
                            out.write("mobile\n");
                            out.flush();
                            out.write("get\n");
                            out.flush();
                            buf = in.readLine();
                            if (buf != null)
                                viewTemp.setText(buf.toString());
                            else
                                continue;
                            buf = in.readLine();
                            if (buf != null)
                                viewHum.setText(buf.toString());
                            else
                                continue;
                            cli.close();
                        }
                        Thread.sleep(5000);
                    } catch (Exception ex) {

                    }
                }
            }
        }).start();

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
            Toast.makeText(this, "connection error",
                    Toast.LENGTH_LONG).show();
        }
        return clientSocket;
    }
}