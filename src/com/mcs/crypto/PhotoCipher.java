package com.mcs.crypto;

import android.graphics.Bitmap;

import com.mcs.util.PhotoBytesConvertor;

public class PhotoCipher {

	private CipherFormat cipherformat;

	public static final int CPT_HEADER_THUMB = 65;

	public PhotoCipher() {
		super();
		cipherformat = new CipherFormat();
	}

	public void encrypt(String plainfilename, String cipherfilename, byte[] key) {
		byte[] thumb = PhotoBytesConvertor.photoToBytes(plainfilename);
		if (thumb == null) {
			return;
		}

		cipherformat.RawSetHeader(CPT_HEADER_THUMB, thumb);

		cipherformat.RawEncrypt(plainfilename.getBytes(),
				cipherfilename.getBytes(), key);
	}

	public String decrypt(String cihperfilename) throws FileFormatException {
		byte[] filename = cipherformat.RawDecrypt(cihperfilename.getBytes());

		if (filename != null) {
			return new String(filename);
		}

		return null;
	}

	public static Bitmap loadThumb(String filename) {
		CipherFormat cipherformat = new CipherFormat();

		if (cipherformat.RawLoadHeaders(filename.getBytes()) != 0) {
			cipherformat.RawDestroy();
			return null;
		}

		byte[] thumb = cipherformat.RawGetHeader(CPT_HEADER_THUMB);
		cipherformat.RawDestroy();
		if (thumb == null) {
			return null;
		}

		return PhotoBytesConvertor.bytesToPhoto(thumb);
	}
}
