package com.serviatech.hwapi;

import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;

import com.serviatech.hwapi.CanAPI;
public class MainActivity extends Activity {
	public CanAPI can0;
	public CanAPI can1;
	private final byte sendcan0[]={(byte)0xFA,(byte)0x00,(byte)0x00,(byte)0x00,(byte)0xFF};
	private final byte sendcan1[]={(byte)0xFA,(byte)0xAA,(byte)0xBB,(byte)0xCC,(byte)0xFA};
	public byte bufcan0[];
	public byte bufcan1[];
	public int revcan0;
	public int revcan1;
	public String TAG = "MainActivity";
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		can0 = new CanAPI("can0",125000);
		can1 = new CanAPI("can1",125000);
		revcan0=can0.send(0x123,sendcan0);
		revcan1=can1.send(0x123,sendcan1);
		Log.i(TAG, "revcan0 = "+revcan0);
		Log.i(TAG, "revcan1 = "+revcan1);

		bufcan0=can0.rev();
		bufcan1=can1.rev();
		for(int i=0;i<bufcan0.length;i++){
			
			Log.i(TAG, "can0 recive:" + bufcan0[i]);
			Log.i(TAG, "can1 recive:" + bufcan1[i]);
		}
		
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
