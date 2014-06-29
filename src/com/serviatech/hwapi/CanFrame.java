package com.serviatech.hwapi;

public class CanFrame {
	public int id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
	public byte dlc; /* data length code: 0 .. 8 */
	public int [] data;
	
	public CanFrame(int id,byte dlc,int [] data){
		this.id = id;
		this.dlc = dlc;
		this.data = data;
	}
}
