package com.mcs.util;

import java.io.InputStream;
import java.util.List;

public interface PathParser {
	public List<Path> parse(InputStream is) throws Exception;

	public String serialize(List<Path> Paths) throws Exception;

}
