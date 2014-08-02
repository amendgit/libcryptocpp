package com.mcs.util;

import java.io.ByteArrayOutputStream;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;

public class PhotoBytesConvertor {

	public static byte[] photoToBytes(String filename) {
		Bitmap rawbmp = BitmapFactory.decodeFile(filename);

		int width = rawbmp.getWidth();
		int height = rawbmp.getHeight();
		final int MAX_WIDTH = 160, MAX_HEIGHT = 120;
		int X = 0, Y = 0;

		if (width * 3 > height * 4) {
			width = 4 * height / 3;
			X = (rawbmp.getWidth() - width) / 2;
			Y = 0;
		} else {
			height = width * 3 / 4;
			X = 0;
			Y = (rawbmp.getHeight() - height) / 2;
		}

		Bitmap thumb = Bitmap.createBitmap(rawbmp, X, Y, width, height);

		Matrix matrix = new Matrix();

		float scaleX = ((float) MAX_WIDTH / width);
		float scaleY = ((float) MAX_HEIGHT / height);

		matrix.postScale(scaleX, scaleY);

		thumb = Bitmap.createBitmap(thumb, 0, 0, width, height, matrix, true);

		ByteArrayOutputStream BAOS = new ByteArrayOutputStream();
		thumb.compress(Bitmap.CompressFormat.JPEG, 40, BAOS);

		return BAOS.toByteArray();
	}

	public static Bitmap bytesToPhoto(byte[] data) {
		return BitmapFactory.decodeByteArray(data, 0, data.length);
	}
}
