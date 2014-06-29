package com.serviatech.hwapi;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.IOException;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import android.widget.Toast;

import com.serviatech.hwapi.CanAPI;
import com.serviatech.hwapi.SerialPort;
import com.serviatech.hwapi.UartRxTask;
import com.serviatech.hwapi.CanFrame;
public class MainActivity extends Activity {
	public CanAPI can0;
	public CanAPI can1;
	private final byte sendcan0[]={(byte)0xFA,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xFF};

	public int bufcan0[]={1,2,3};
	public int sendbytes0;

	public CanFrame mCanFrame;
	
	public String TAG = "MainActivity";
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		can0 = new CanAPI("can0",50000);
//		can1 = new CanAPI("can1",125000);
		
//		exec_cmd("ifconfig can1 down\n");
//		exec_cmd("ip link set can1 type can bitrate 125000\n");
//		exec_cmd("ifconfig can1 up\n");

		//sendbytes0=can0.send(0x123,sendcan0);
		
		CanFrame frame = new CanFrame(0,(byte)0,bufcan0);
		Log.i(TAG, "sendbytes0 = "+sendbytes0);

		//sendbytes1=can0.write(0x123,sendcan1);
		//Log.i(TAG, "sendbytes1 = "+sendbytes1);
//		can0.closecan();
		
		can0.setFilter(0x124,0x124);
		int m=0;
		while(true){
			mCanFrame=can0.readcan();
//			bufcan1=can1.read();
			Log.i(TAG, "canid="+Integer.toHexString(mCanFrame.id)+" dlc="+mCanFrame.dlc);
			for(int i=0;i<mCanFrame.data.length;i++){
				
				Log.i(TAG, "can0 recive["+i+"]= " + Integer.toHexString(mCanFrame.data[i]));
//				Log.i(TAG, "can1 recive:" + bufcan1[i]);
			}
			int a=0x123;
			sendbytes0=can0.send(a,sendcan0);
		}
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

//	@Override
//	public boolean onCreateOptionsMenu(Menu menu) {
//		// Inflate the menu; this adds items to the action bar if it is present.
//		getMenuInflater().inflate(R.menu.main, menu);
//		return true;
//	}
//	
//	private UartRxTask mRxTask;
//	public String TAG = "MainActivity";
//	private final byte data[]={(byte)0xFA,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xFF};
//	protected void onCreate(Bundle savedInstanceState) {
//		super.onCreate(savedInstanceState);
//		//uartConnect();
//		exec_cmd("ifconfig eth0 192.168.1.244\n");
//		
//	}
//		
//	private void uartConnect() {
//		// TODO Auto-generated method stub
//		String notePath = "/dev/ttymxc1";
//		int baudrate = 9600;
//
//		if (mRxTask != null) {
//			Toast.makeText(this, "Please disconnect !!", Toast.LENGTH_LONG)
//					.show();
//			return;
//		}
//
//		try {
//			SerialPort mSerialPort = new SerialPort(new File(notePath),
//					baudrate, 0);
//			FileDescriptor fd = ((FileInputStream) mSerialPort.getInputStream())
//					.getFD();
//			mRxTask = new UartRxTask(getApplicationContext(), /*mHandler,*/ fd,
//					notePath); // open thread to read
//			// Log.i("===","开启接收线程");
//			mRxTask.start(); // run thread
//		} catch (SecurityException e) {
//			e.printStackTrace();
//		} catch (IOException e) {
//			e.printStackTrace();
//		}
//	}
}
	
