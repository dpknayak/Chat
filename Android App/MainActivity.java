package com.example.chat;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity implements OnClickListener {
    Socket socket;
    String ip = "192.168.1.12";
    String myId = "11111";
    String otherId = "11111";
    List<String> messageList;
    List<Integer> messageAlignList;
    MyAdapter myAdapter;
    TextView socketStatusTextView;
    ListView listView;
    Button sendButton;
    EditText enteredMessageEditText;
    Dialog dialog;
    Context context;

    private static HandlerThread senderThread = new HandlerThread("my sender thread");
    private static HandlerThread receiverThread = new HandlerThread("my receiver thread");
    static {
        senderThread.start();
        receiverThread.start();
    }

    private void init() {
        socketStatusTextView = (TextView) findViewById(R.id.socketStatus);
        listView = (ListView) findViewById(R.id.list);
        sendButton = (Button) findViewById(R.id.sendButton);
        enteredMessageEditText = (EditText) findViewById(R.id.message);

        context = this;
        socket = new Socket();
        messageList = new ArrayList<String>();
        messageAlignList = new ArrayList<Integer>();
        myAdapter = new MyAdapter(this, messageList, messageAlignList);

        sendButton.setOnClickListener(this);
        listView.setAdapter(myAdapter);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        init();
        showDialog();
    }

    @Override
    protected void onPause() {
        finish();
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        try {
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.sendButton) {
            if (enteredMessageEditText.getText().toString().isEmpty())
                return;

            senderHandler.sendMessageDelayed(senderHandler.obtainMessage(Constants.SEND_MESSAGE,
                            Utils.msgToTLV(enteredMessageEditText.getText().toString(), otherId)
                                            .getBytes()), 200);

            messageList.add(enteredMessageEditText.getText().toString());
            messageAlignList.add(Constants.ALIGN_RIGHT);
            enteredMessageEditText.setText("");
        }
    }

    private void showDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = getLayoutInflater();
        final View view = inflater.inflate(R.layout.dialog_signin, null);

        builder.setView(view).setPositiveButton(R.string.signin,
                        new DialogInterface.OnClickListener() {

                            @Override
                            public void onClick(DialogInterface dialog, int id) {
                                ip = ((EditText) view.findViewById(R.id.ip)).getText().toString();
                                myId = ((EditText) view.findViewById(R.id.myId)).getText()
                                                .toString();
                                otherId = ((EditText) view.findViewById(R.id.otherId)).getText()
                                                .toString();
                                if (!Utils.validIP(ip) || !Utils.validId(myId)
                                                || !Utils.validId(otherId))
                                    uiHandler.sendEmptyMessage(Constants.CONNECTION_REFUSED);
                                else
                                    senderHandler.sendEmptyMessage(Constants.REQUEST_CONNECTION);
                            }
                        });

        dialog = builder.create();
        dialog.setCancelable(false);
        dialog.show();
    }

    public Handler uiHandler = new Handler(Looper.getMainLooper()) {

        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {

            case Constants.CONNECTED:
                socketStatusTextView.setText(socket.toString());

                senderHandler.obtainMessage(Constants.SEND_MESSAGE, myId.getBytes()).sendToTarget();
                receiverHandler.sendEmptyMessage(Constants.READ_MESSAGE);
                break;

            case Constants.CONNECTION_REFUSED:
                socketStatusTextView.setText(socket.toString());
                Toast.makeText(context, R.string.connectionRefused, Toast.LENGTH_SHORT).show();
                showDialog();
                break;

            case Constants.REFRESH_UI_AFTER_RECEIVE:
                messageList.add(new String((byte[]) msg.obj));
                messageAlignList.add(Constants.ALIGN_LEFT);
                myAdapter.notifyDataSetChanged();
                listView.setSelection(myAdapter.getCount() - 1);
                break;

            case Constants.REFRESH_UI_AFTER_SEND:
                myAdapter.notifyDataSetChanged();
                listView.setSelection(myAdapter.getCount() - 1);
                break;
                
            default:
                break;
            }
        };

    };

    public Handler senderHandler = new Handler(senderThread.getLooper()) {

        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {

            case Constants.REQUEST_CONNECTION:
                try {
                    socket.connect(new InetSocketAddress(ip, 8080), 3000);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                if (!socket.isConnected())
                    uiHandler.sendEmptyMessage(Constants.CONNECTION_REFUSED);
                else
                    uiHandler.sendEmptyMessage(Constants.CONNECTED);
                break;

            case Constants.SEND_MESSAGE:

                try {
                    socket.getOutputStream().write((byte[]) msg.obj);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                uiHandler.sendEmptyMessage(Constants.REFRESH_UI_AFTER_SEND);
                break;

            default:
                break;
            }
        }
    };

    public Handler receiverHandler = new Handler(receiverThread.getLooper()) {
        int numReadBytes;

        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {

            case Constants.READ_MESSAGE:
                byte[] buffer = new byte[512];
                numReadBytes = 0;

                try {
                    numReadBytes = socket.getInputStream().read(buffer);
                } catch (IOException e) {
                    e.printStackTrace();
                }

                if (numReadBytes > 0)
                    uiHandler.sendMessage(uiHandler.obtainMessage(Constants.REFRESH_UI_AFTER_RECEIVE, buffer));

                sendEmptyMessageDelayed(Constants.READ_MESSAGE, 300);
                break;

            default:
                break;
            }
        }
    };
}
