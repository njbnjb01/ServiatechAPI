package com.serviatech.hwapi;

public class GPIO {
	public GPIO(){
		
	}
	
	public native static int write(int gpio, int value);
	public native static int read(int gpio);
	static{
		System.loadLibrary("GPIO-jni");
	}
}
