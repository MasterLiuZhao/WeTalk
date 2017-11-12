package com.liuzhao.wetalk.adapter;

import java.util.List;

import android.content.Context;
import android.media.MediaPlayer;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.liuzhao.wetalk.R;
import com.liuzhao.wetalk.ViewHolder;
import com.liuzhao.wetalk.entity.MessageEntity;

public class MessageViewAdapter extends BaseAdapter {
	private Context context;
	private List<MessageEntity> messageList;
	private LayoutInflater mInflater;
	private MediaPlayer mMediaPlayer = new MediaPlayer();
	
	public MessageViewAdapter(Context context, List<MessageEntity> messageList) {
		super();
		this.messageList = messageList;
		this.context = context;
		this.mInflater = LayoutInflater.from(context);
	}

	@Override
	public int getCount() {
		return messageList.size();
	}

	@Override
	public Object getItem(int position) {
		return messageList.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}
	
	public int getViewTypeCount() {
		return 2;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		MessageEntity entity = messageList.get(position);
		boolean isOwnerMessage = entity.isMessageType();
		
		ViewHolder viewHolder = null;
		if(convertView == null){
			if(isOwnerMessage){
				convertView = mInflater.inflate(R.layout.chatting_item_msg_text_right, null);
			}else{
				convertView = mInflater.inflate(R.layout.chatting_item_msg_text_left, null);
			}
			
			viewHolder = new ViewHolder();
			viewHolder.tvSendTime = (TextView) convertView.findViewById(R.id.tv_sendtime);
			viewHolder.tvUserName = (TextView) convertView.findViewById(R.id.tv_username);
			viewHolder.tvContent = (TextView) convertView.findViewById(R.id.tv_chatcontent);
			viewHolder.tvTime = (TextView) convertView.findViewById(R.id.tv_time);
			viewHolder.ownerMessage = isOwnerMessage;
			
			convertView.setTag(viewHolder);
		}else{
			viewHolder = (ViewHolder) convertView.getTag();
		}
		
		viewHolder.tvSendTime.setText(entity.getTime());
		viewHolder.tvUserName.setText(entity.getName());
		if(entity.getMessage().contains(".amr")){
			viewHolder.tvContent.setText("");
			viewHolder.tvContent.setCompoundDrawablesWithIntrinsicBounds(0, 0, R.drawable.chatto_voice_playing, 0);
		}else{
			viewHolder.tvContent.setText(entity.getMessage());
			viewHolder.tvContent.setCompoundDrawablesWithIntrinsicBounds(0, 0, 0, 0);
		}
		viewHolder.tvContent.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				
			}
		});
		
		
		return convertView;
	}

	public Context getContext() {
		return context;
	}

	public void setContext(Context context) {
		this.context = context;
	}

	public List<MessageEntity> getMessageList() {
		return messageList;
	}

	public void setMessageList(List<MessageEntity> messageList) {
		this.messageList = messageList;
	}

}
