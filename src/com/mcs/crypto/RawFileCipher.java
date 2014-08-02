package com.mcs.crypto;

public class RawFileCipher {

	enum Direction {
		Encrypt, Decrypt
	};

	static {
		System.loadLibrary("cryptopg");
	}

	public void begin(Direction d) {
		return;
	}

	public void commit() {
		return;
	}

	/**
	 * 
	 * @param filename
	 *            Encode with utf-8.
	 * @param key
	 */
	public native static void RawEncrypt(byte[] plainFileName,
			byte[] cipherFileName, byte[] key);
}
