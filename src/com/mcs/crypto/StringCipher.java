package com.mcs.crypto;

import java.io.UnsupportedEncodingException;
import java.security.InvalidKeyException;

import com.mcs.util.StringBytesConvertor;

import android.util.Log;

public class StringCipher {

	public static byte[] easyStringWrap(String key, byte[] data) {

		byte[] work = new byte[data.length];
		work = data.clone();

		RawRC4 rc4 = new RawRC4(key.getBytes());
		rc4.RawXORStream(work);

		return work;
	}

	public static String easyStringEncWrap(String key, String data) {
		byte[] work = null;
		try {
			work = data.getBytes("UTF-8");
		} catch (UnsupportedEncodingException e1) {
			return null;
		}

		RC4 rc4 = new RC4();
		try {
			rc4.init(key.getBytes());
		} catch (InvalidKeyException e) {
			return null;
		}

		rc4.xorKeyStream(work, work);

		return StringBytesConvertor.bytesToString(work);
	}

	public static String easyStringDecWrap(String key, String data) {
		byte[] work = StringBytesConvertor.stringToBytes(data);

		RC4 rc4 = new RC4();
		try {
			rc4.init(key.getBytes());
		} catch (InvalidKeyException e) {
			return null;
		}

		rc4.xorKeyStream(work, work);

		try {
			return new String(work, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			return null;
		}
	}

	RawAES rawAES;

	public void aesSetupEncKey(String key) {
		rawAES = new RawAES();
		rawAES.RawSetupEncKey(key.getBytes());
	}

	public String aesEncBuf(String buf) {
		byte[] cipher = rawAES.RawEncBuf(StringBytesConvertor
				.stringToBytes(buf));
		return StringBytesConvertor.bytesToString(cipher);
	}

	public void aesSetupDecKey(String key) {
		rawAES = new RawAES();
		rawAES.RawSetupDecKey(key.getBytes());
	}

	public String aesDecBuf(String buf) {
		byte[] plain = rawAES
				.RawEncBuf(StringBytesConvertor.stringToBytes(buf));
		return StringBytesConvertor.bytesToString(plain);
	}

	public void aesEnd() {
		rawAES.RawDestroy();
		rawAES = null;
	}
}
