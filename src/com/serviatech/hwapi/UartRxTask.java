package com.serviatech.hwapi;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
//import java.io.OutputStream;

import android.content.Context;
//import android.os.Bundle;
import android.os.Handler;
//import android.os.Message;
import android.util.Log;

public class UartRxTask extends Thread
{
    public int mIsExit;
    
    Handler mHandler;
    String mPath;
    FileDescriptor mFD;
    Context mc;
    String TAG = "UartRxTask";
    int cmd[] = new int[6];
    int count = 0;
    private final byte data[]={(byte)0x01,(byte)0x02,(byte)0x03,(byte)0x04,(byte)0x05};
    private OutputStream fos;
	//byte cmdOut[] ={(byte)0xFA,(byte)0x00,(byte)0x00,(byte)0xFF};
    public UartRxTask(Context context,/*Handler h, */FileDescriptor fd, String path)
    {
//    	mHandler = h;
    	mc = context;
    	mPath = path;
    	mFD = new FileDescriptor();
    	mFD = fd;
    	mIsExit = 0;
		//Log.i("RxTxTask", String.format("mPath=%s mIsExit=%d\n", mPath, mIsExit));
    	
    }

    public void run()
    {
    	int buf[] = new int[1];
    	File f = new File(mPath);
    	try{
        	FileInputStream is = new FileInputStream(f);
        	InputStream fis = (InputStream)is;
        	
        	FileOutputStream os = new FileOutputStream(f);
        	fos = (OutputStream)os;
        	
			while(true){
				//Log.i("===","run");
				buf[0] = fis.read();
				//Log.i("===","read ok!");
				//Log.i("RxTxTask", String.format("nread=%c\n", buf[0]));
				Log.i("========","receive: " + Integer.toHexString(buf[0]));
				if(buf[0]==-1){
					continue;
				}
				if(mIsExit != 0){
					fis.close();			
					//fos.close();
					// FIXME in blocked IO, this might not work until next input
					Log.i("RxTxTask", "Exit......#################");
					break;
				}
			
				if(count==0){
					if(buf[0]==0xFA){//
						cmd[0]=buf[0];
					}else{
						continue;	
					}
				}else if(count>0&&count<5){
					cmd[count]=buf[0];
				}else if(count==5){
					count=0;
					if(buf[0]==0xFF){//
						//os.write(cmdOut);
			        	//os.flush();
						//Log.i("########"," one command!");
//						for(int i=0;i<5;i++){
//						
//							Log.i(TAG, "serial recive: " + Integer.toHexString(buf[i]));
//
//						}
						answer(data);
					}
					continue;
				}
				
				count++;
//				Message msg = mHandler.obtainMessage();
//				msg.what = MenuFrag.MI_UART;
//				Bundle b = new Bundle();
//				b.putString("in", new String(buf, 0, 1));
//				msg.setData(b);
//				if(mHandler != null){  // fix keyboard hot-plug problem
//					mHandler.sendMessage(msg);
//				}
			}
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
    /* 向下回应函数 */
	private void answer(byte ans[]) {// 
		try {
			fos.write(ans);
			fos.flush();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
