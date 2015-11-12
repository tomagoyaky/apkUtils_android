package com.tomagoyaky.apkutils;

import android.app.Application;

public class TomagoyakyApplication extends Application{

	static{
		try {
			System.loadLibrary("ApkUtils");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	@Override
	public void onCreate() {
		super.onCreate();
	}
}