package com.example.chat;

import java.util.List;

import android.content.Context;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

public class MyAdapter extends BaseAdapter {
    List<String> messageList;
    List<Integer> messageAlignList;
    Context context;

    public MyAdapter(Context activityContext, List<String> msgList, List<Integer> msgAlignList) {
        context = activityContext;
        messageList = msgList;
        messageAlignList = msgAlignList;
    }

    @Override
    public int getCount() {
        return messageList.size();
    }

    @Override
    public String getItem(int position) {
        return messageList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        LayoutInflater inflater = (LayoutInflater) context
                        .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        convertView = inflater.inflate(R.layout.list_item, null);

        TextView messageTextView = (TextView) convertView.findViewById(R.id.message);
        messageTextView.setText(getItem(position));

        if (messageAlignList.get(position) == Constants.ALIGN_RIGHT)
            messageTextView.setGravity(Gravity.END);
        else
            messageTextView.setGravity(Gravity.START);
        return convertView;
    }

}
