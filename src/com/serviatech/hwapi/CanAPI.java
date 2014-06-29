package com.serviatech.hwapi;

import java.io.DataOutputStream;

import android.util.Log;

public class CanAPI {
	
	private static final String TAG = "CanAPI";
	
	public int mFd;
	public String cmd1;
	public String cmd2;
	public String cmd0;
	public final byte a[]={(byte)0x00};
	private String mycan;
	
	
	public CanAPI(String dev, int bitrate)
	{
		mycan=dev;
		mFd = opencan(mycan);
		if (mFd == -1) {
	    		Log.e(TAG, "open "+mycan+" failed!");
	    		return ;
	   	}
		cmd0 = "ifconfig "+mycan+" down\n";
		cmd1 = "ip link set "+mycan+" type can bitrate "+bitrate+"\n";
		cmd2 = "ifconfig "+mycan+" up\n";
		exec_cmd(cmd0);
		exec_cmd(cmd1);
		exec_cmd(cmd2);
				
		send(123,a);
		
//		exec_cmd("ip link set can0 type can bitrate 125000\n");
//		exec_cmd("ifconfig can0 up \n");
	}
	
	public void exec_cmd(String cmd){		
		  try {
			Process p;
			p = Runtime.getRuntime().exec("su");
			DataOutputStream os = new DataOutputStream(p.getOutputStream());
			os.writeBytes(cmd);
			os.writeBytes("exit\n");
			
			os.flush();

			Log.e(TAG,cmd);

		} catch (Exception e) {
			e.printStackTrace();
			//throw new SecurityException();
		}
	}
	public int send(int canid ,byte[] data){
		int ret;
		ret=writecan(canid, data);
		return ret;
	}
//	public int[] receive(){
//		
//		byte data[];
//		
//		data = readcan();
//		Log.d(TAG, "datalength ="+data.length);
//		int ret[]=new int[data.length];
//		for(int i=0;i<data.length;i++){
//			ret[i] = (int)(data[i]&0xFF);
//			Log.i(TAG, "can1 recive:" + ret[i]);
//		}
//		return ret;
//	}
	
	public void close(){
		
		closecan();
		exec_cmd("ifconfig "+mycan+" down\n");
		
	}
	//JNI
	private native int opencan(String dev);
	public native void closecan();
	public native int writecan(int canid, byte[] data);//返回成功写入字节数
	public native CanFrame readcan();
	public native int setFilter(int canid, int mask);//返回零成功
	static{
		System.loadLibrary("CanAPI-jni");
	}
}
