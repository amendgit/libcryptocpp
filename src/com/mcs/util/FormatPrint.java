package com.mcs.util;

public class FormatPrint {

	public static void out(byte byteArray[]) {
		System.out.print("[");

		for (int i = 0; i < byteArray.length; i++) {
			System.out.print("0x" + Integer.toHexString(byteArray[i]) + ",");
		}

		System.out.print("]\n");
	}
}
