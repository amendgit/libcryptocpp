package com.mcs.crypto;

public class RawRC4 {

	static {
		System.loadLibrary("cryptopg");
	}

	private long instance;

	public native int RawNew(byte[] key);

	public native int RawXORStream(byte[] buf);

	public native void RawDestroy();

	public native void RawReset();

	protected void finalize() {
		if (this.instance != 0) {
			this.RawDestroy();
		}
	}

	public RawRC4(byte[] key) {
		RawNew(key);
	}
}
