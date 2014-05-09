package com.serviatech.hwapi;

import java.io.DataOutputStream;
import java.io.FileDescriptor;

import android.util.Log;

public class CanAPI {
	
	private static final String TAG = "CanAPI";
	
	public int mFd;

	public CanAPI(String dev, int bitrate)
	{

		mFd = open(dev);
		if (mFd == -1) {
	    		Log.e(TAG, "open "+dev+" failed!");
	    		return ;
	   	}
		exec_cmd("ip link set "+dev+" type can bitrate "+bitrate+"\n");
		exec_cmd("ifconfig "+dev+" up\n");
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
		ret=write(canid, data, mFd);
		return ret;
	}
	public byte[] rev(){
		return read(mFd);
	}
	//JNI
	private native int open(String dev);
	public native void close();
	public native int write(int canid, byte[] data, int mfd);
	public native byte[] read(int mfd);
	
	static{
		System.loadLibrary("CanAPI-jni");
	}
}
