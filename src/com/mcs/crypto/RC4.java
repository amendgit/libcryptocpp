package com.mcs.crypto;

import java.security.InvalidKeyException;

/*
 * Ron Rivest RC4 crypto algorithm.
 * 
 * In java, the built-in type are all signed. Therefore we used the little 8bit of char to 
 * store our key data. And in java the byte also cost 32bit in the memory. So there are no 
 * more memory waste.
 * 
 * see: http://www.darksleep.com/player/JavaAndUnsignedTypes.html
 */

public class RC4 {
	private short[] s;
	private int i, j;

	public RC4() {
		this.s = new short[256];
		this.i = this.j = 0;
	}

	/**
	 * Initialize the RC4 cipher.
	 * 
	 * @param key
	 *            The key argument should RC4 key, at least 1 byte and at most
	 *            256 bytes.
	 */
	public void init(byte[] key) throws InvalidKeyException {
		int keylen = key.length;
		short[] t = new short[256];
		if (keylen < 1 || keylen > 256) {
			/* Key Size Error */
			throw new InvalidKeyException("The key size is not fair.");
		}
		for (int i = 0; i < 256; i++) {
			this.s[i] = (short) i; // This is fine, No need to cast.
			t[i] = (short) ((key[i % keylen] + 256) % 256);
		}
		int j = 0;
		short tmp;
		for (int i = 0; i < 256; i++) {
			j = (j + (this.s[i] + t[i])) & 0xff;
			tmp = this.s[i];
			this.s[i] = this.s[j];
			this.s[j] = tmp;
		}
		return;
	}

	/**
	 * XorKeyStream sets dst to the result of XORing src with the key stream.
	 * Dst and src May be the same slice but otherwise should not overlap.
	 */
	public void xorKeyStream(byte[] src, byte[] dst) {
		short tmp;
		assert (src.length <= dst.length);
		for (int pos = 0; pos < src.length; pos++) {
			i = (i + 1) % 256;
			j = (j + s[i]) % 256;
			tmp = s[i];
			s[i] = s[j];
			s[j] = tmp;
			dst[pos] = (byte) (src[pos] ^ s[(s[i] + s[j]) % 256]);
		}
	}

	/**
	 * Reset zeros the key data so that it will no longer appear in the
	 * process's memory.
	 */
	public void reset() {
		for (int i = 0; i < s.length; i++) {
			s[i] = 0;
		}
		i = j = 0;
	}
}
