package com.mcs.util;

import java.io.File;
import java.io.FileOutputStream;

public class Utility {
	public static void WriteToFile(byte[] octets, String filename) {
		File fd = new File(filename);
		FileOutputStream out;

		try {
			out = new FileOutputStream(fd);
			out.write(octets);
			out.close();
		} catch (Exception e) {
			e.printStackTrace();
		}

		return;
	}
}
