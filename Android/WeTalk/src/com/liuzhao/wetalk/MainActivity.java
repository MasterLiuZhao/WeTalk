package com.liuzhao.wetalk;

import android.support.v7.app.ActionBarActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends ActionBarActivity {
	
	private String username;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		Button button = (Button) this.findViewById(R.id.userNameButton);
        button.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
				
				TextView textView = (TextView) MainActivity.this.findViewById(R.id.userNameText);
				username = textView.getText().toString();
				
				if("".equalsIgnoreCase(username)){
					//DisplayMessage.toastDisplayMessage(MainActivity.this, "Please enter your name first!");
					Toast toast = Toast.makeText(MainActivity.this, "Please enter your name first!", Toast.LENGTH_LONG);
			        toast.show();
				}else{
					Intent intent = new Intent();
					intent.setClass(MainActivity.this, GroupChatActivity.class);
					intent.putExtra("username", username);
					MainActivity.this.startActivity(intent);
				}
			}
        	
        });
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	public String getUsername() {
		return username;
	}

	public void setUsername(String username) {
		this.username = username;
	}
	
}
