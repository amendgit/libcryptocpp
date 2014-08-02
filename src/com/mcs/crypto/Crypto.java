package com.mcs.crypto;

public class Crypto {
	static {
		System.loadLibrary("cryptopg");
	}
	
	/**
	 * Native file encrypt function. Example: image.jpg encrypted to image.mxx.
	 * @param filename Must be utf8 encode. 
	 * @param key Must be utf8 encode.
	 * @return 0 for SUCCESS, -1 for FAILED.
	 */
	public static native byte[] easyEncrypt(byte[] filename, byte[] key);
	
	/**
	 * Native file encrypt function. 
	 * @param inFilename Must be utf8 encode. 
	 * @param outFilename Must be utf8 encode. 
	 * @param key Must be utf8 encode. 
	 * @return 0 for SUCCESS, -1 for FAILED.
	 */
	public static native int easyEncrypt(byte[] inFilename, byte[] outFilename, byte[] key);
	
	/**
	 * Native file decrypt function. E.G. image.mxx be decrypted to image.jpg.
	 * @param filename Filename must be utf8 encode. 
	 * @param key KEY must be utf8 encode.
	 * @return 0 for SUCCESS, -1 for FAILED.
	 */
	public static native byte[] easyDecrypt(byte[] filename, byte[] key);
	
	/**
	 * Native file decrypt function. 
	 * @param inFilename inFilename must be utf8 encode.
	 * @param outFilename outFilename must be utf8 encode.
	 * @param key Must be utf8 encode. 
	 * @return 0 for SUCCESS, -1 for FAILED.
	 */
	public static native int easyDecrypt(byte[] inFilename, byte[] outFilename, byte[] key);
	
	/**
	 * Decrypt file and return the content in memory.
	 * @param filename Filename Must be utf8 encode.
	 * @param key KEY must be utf8 encode.
	 * @return NULL for FAILED.
	 */
	public static native byte[] easyDecryptInMemory(byte[] filename, byte[] key);
	
	public static native byte[] easyEncryptVideo(byte[] filename, byte[] key);
	
	public static native int easyEncryptVideo(byte[] inFilename, byte[] outFilename, byte[] key);
	
	public static native byte[] easyDecryptVideo(byte[] filename, byte[] key);
	
	public static native int easyDecryptVideo(byte[] inFilename, byte[] outFilename, byte[] key);
}