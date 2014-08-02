package com.mcs.crypto;

import java.security.SecureRandom;
import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;

public class AES {

	public static String encrypt(String seed, String plainText)
			throws Exception {
		byte[] rawKey = getRawKey(seed.getBytes());
		byte[] result = encrypt(rawKey, plainText.getBytes());
		return toHex(result);
	}

	public static String decrypt(String seed, String cipherText)
			throws Exception {
		byte[] rawKey = getRawKey(seed.getBytes());
		byte[] cipherTextBytes = toByte(cipherText);
		byte[] result = decrypt(rawKey, cipherTextBytes);
		return new String(result);
	}

	private static byte[] getRawKey(byte[] seed) throws Exception {
		// Get key generator.
		KeyGenerator keyGenerator = KeyGenerator.getInstance("AES");

		// Get random generator and set the seed.
		SecureRandom secureRandom = SecureRandom.getInstance("SHA1PRNG");
		secureRandom.setSeed(seed);

		// Ready to generate the raw key.
		keyGenerator.init(128, secureRandom);
		SecretKey secretKey = keyGenerator.generateKey();

		// Default Encoding.
		byte[] rawKey = secretKey.getEncoded();
		return rawKey;
	}

	private static byte[] encrypt(byte[] rawKey, byte[] plainTextBytes)
			throws Exception {
		//
		SecretKeySpec secretKeySpec = new SecretKeySpec(rawKey, "AES");
		Cipher cipher = Cipher.getInstance("AES");
		cipher.init(Cipher.ENCRYPT_MODE, secretKeySpec);
		byte[] cipherTextBytes = cipher.doFinal(plainTextBytes);
		return cipherTextBytes;
	}

	private static byte[] decrypt(byte[] rawKey, byte[] cipherTextBytes)
			throws Exception {
		SecretKeySpec secretKeySpec = new SecretKeySpec(rawKey, "AES");
		Cipher cipher = Cipher.getInstance("AES");
		byte[] plainTextBytes = cipher.doFinal(cipherTextBytes);
		return plainTextBytes;
	}

	public static String toHex(String string) {
		return toHex(string.getBytes());
	}

	public static String fromHex(String hex) {
		return new String(toByte(hex));
	}

	public static byte[] toByte(String hexString) {
		int length = hexString.length() / 2;
		byte[] result = new byte[length];
		for (int i = 0; i < length; i++) {
			result[i] = Integer.valueOf(hexString.substring(2 * i, 2 * i + 2),
					16).byteValue();
		}
		return result;
	}

	private static String toHex(byte[] bytes) {
		if (bytes == null) {
			return new String("");
		}
		StringBuffer stringBuffer = new StringBuffer(2 * bytes.length);
		for (int i = 0; i < bytes.length; i++) {
			appendHex(stringBuffer, bytes[i]);
		}
		return stringBuffer.toString();
	}

	private final static String HEX = "0123456789abcdef";

	private static void appendHex(StringBuffer stringbuf, byte b) {
		stringbuf.append(HEX.charAt((b >> 4) & 0x0f)).append(
				HEX.charAt(b & 0x0f));
	}
}
