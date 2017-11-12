package com.liuzhao.wetalk;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.widget.EditText;
import android.widget.ListView;

import com.liuzhao.wetalk.adapter.MessageViewAdapter;
import com.liuzhao.wetalk.entity.MessageEntity;

public class GroupChatActivity extends ActionBarActivity {
	private String host = "192.168.0.209";
	private int post = 2222;
	
	private Socket socket;
	private OutputStreamWriter outputStreamWriter = null;
	private BufferedReader bufferReader = null;
	
	private Handler myHandler = new Handler();
	
	private Runnable updateListView = new Runnable() {
		public void run() {
			
			List<MessageEntity> list = messageViewAdapter.getMessageList();
			list.add(entity);
			
			messageViewAdapter.notifyDataSetChanged();
			messageListView.setSelection(messageListView.getCount() - 1);
			
		}
	};
	
	private Intent requestIntent;
	private String username;
	private String message;
	
	private MessageEntity entity;
	
	private ListView messageListView;
	private MessageViewAdapter messageViewAdapter;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		//this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		this.setContentView(R.layout.group_chat);
		
		requestIntent = this.getIntent();
		Bundle bundle = requestIntent.getExtras();
		username = bundle.getString("username");
		//Toast toast = Toast.makeText(GroupChatActivity.this, username, Toast.LENGTH_LONG);
        //toast.show();
        
		messageListView = (ListView) this.findViewById(R.id.group_chat_listview);
        messageViewAdapter = new MessageViewAdapter(this, new ArrayList<MessageEntity>());
        messageListView.setAdapter(messageViewAdapter);
        
        
        
        new Thread(new Runnable(){

			@Override
			public void run() {
				try {
					socket = new Socket(host, post);
					
					if(bufferReader == null){
						bufferReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
					}
					
					while(true){
						String temp = bufferReader.readLine();
						
						Log.i("MyLog", "temp = " + temp);
						
						if(temp != null && !"".equals(temp)){
							String[] messageSplit = temp.split(" : ");
							
							if(messageSplit != null && messageSplit.length >= 2){
								entity = new MessageEntity();
								entity.setTime(GroupChatActivity.this.getTime());
								entity.setName(messageSplit[0]);
								
								Log.i("MyLog", "messageSplit[0] = " + messageSplit[0]);
								Log.i("MyLog", "username = " + username);
								
								if(messageSplit[0].equalsIgnoreCase(username)){
									entity.setMessageType(true);
								}else{
									entity.setMessageType(false);
								}
								entity.setMessage(messageSplit[1]);
								
								myHandler.post(updateListView);
							}else{
								break;
							}
							
						}else{
							break;
						}
						
					}
					
				} catch (UnknownHostException e) {
					e.printStackTrace();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
			
		}).start();
	}
	
	public void sendMessage(View view) throws IOException {
		EditText editText = (EditText) this.findViewById(R.id.group_chat_edit_sendmessage);
		message = editText.getText().toString();
		
		if(message != null && !"".equals(message)){
			
			editText.setText("");
			
			String sendMessage = username;
			sendMessage += "#";
			sendMessage += message;
			
			if(outputStreamWriter == null){
				outputStreamWriter = new OutputStreamWriter(socket.getOutputStream());
			}
			
			outputStreamWriter.write(sendMessage, 0, sendMessage.length());
			outputStreamWriter.flush();
			
		}
	}
	
	private String getTime() {
		Calendar c = Calendar.getInstance();

		String year = String.valueOf(c.get(Calendar.YEAR));
		String month = String.valueOf(c.get(Calendar.MONTH));
		String day = String.valueOf(c.get(Calendar.DAY_OF_MONTH) + 1);
		String hour = String.valueOf(c.get(Calendar.HOUR_OF_DAY));
		String mins = String.valueOf(c.get(Calendar.MINUTE));

		StringBuffer sbBuffer = new StringBuffer();
		sbBuffer.append(year + "-" + month + "-" + day + " " + hour + ":" + mins);

		return sbBuffer.toString();
	}

	@Override
	protected void onDestroy() {
		Log.i("MyLog", "socket.close().");
		
		try {
			if(bufferReader != null){
				bufferReader.close();
			}
			if(outputStreamWriter != null){
				outputStreamWriter.close();
			}
			
			socket.getInputStream().close();
			socket.getOutputStream().close();
			
			socket.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		super.onDestroy();
	}
	
}
