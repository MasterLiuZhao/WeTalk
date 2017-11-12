package com.liuzhao.wetalk.entity;

public class MessageEntity {
	private String name;
	private String message;
	private String time;
	private boolean messageType;

	public MessageEntity() {
		super();
	}

	public MessageEntity(String name, String message, String time, boolean messageType) {
		super();
		this.name = name;
		this.message = message;
		this.time = time;
		this.messageType = messageType;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getMessage() {
		return message;
	}

	public void setMessage(String message) {
		this.message = message;
	}

	public String getTime() {
		return time;
	}

	public void setTime(String time) {
		this.time = time;
	}

	public boolean isMessageType() {
		return messageType;
	}

	public void setMessageType(boolean messageType) {
		this.messageType = messageType;
	}

}
