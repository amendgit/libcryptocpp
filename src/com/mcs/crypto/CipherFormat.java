package com.mcs.crypto;

public class CipherFormat {

	static {
		System.loadLibrary("cryptopg");
	}

	private long instance;

	public CipherFormat() {
		RawNew();
	}

	protected void finalize() {
		if (this.instance != 0) {
			this.RawDestroy();
		}
	}

	public void setKeyEncryptKey(byte[] kek) {
		return;
	}

	// 从文件格式头中读取并解密文件密钥。
	public void RevealKeyEncKey() {
		byte[] encKey, key;
		// encKey = GetFileKey();
		// key = DecryptFileKey(encKey);
		// SetFileKey(key);
	}

	public native int RawNew();

	public native int RawSetHeader(int type, byte[] bytes);

	public native byte[] RawGetHeader(int type);

	public native int RawSetFileKey(byte[] key);

	public native byte[] RawGetFileKey();

	public native int RawEncrypt(byte[] plainfilename, byte[] cipherfilename,
			byte[] key);

	public native byte[] RawDecrypt(byte[] cipherfilename);

	public native void RawDestroy();

	public native int RawLoadHeaders(byte[] filename);

}
