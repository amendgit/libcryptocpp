package com.mcs.util;

public class StringBytesConvertor {

	public static byte[] stringToBytes(String string) {
		char[] data = string.toCharArray();
		byte[] bytes = new byte[data.length];

		for (int i = data.length - 1; i >= 0; i--) {
			bytes[i] = (byte) (data[i] - 128);
		}

		return bytes;
	}

	public static String bytesToString(byte[] bytes) {
		char[] data = new char[bytes.length];

		for (int i = bytes.length - 1; i >= 0; i--) {
			data[i] = (char) (bytes[i] + 128);
		}
		String a;
		return new String(data);
	}
}
