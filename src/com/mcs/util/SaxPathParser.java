package com.mcs.util;

import java.io.InputStream;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Result;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.sax.SAXTransformerFactory;
import javax.xml.transform.sax.TransformerHandler;
import javax.xml.transform.stream.StreamResult;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

public class SaxPathParser implements PathParser {

	@Override
	public List<Path> parse(InputStream is) throws Exception {
		SAXParserFactory factory = SAXParserFactory.newInstance();
		SAXParser parser = factory.newSAXParser();
		MyHandler handler = new MyHandler();
		parser.parse(is, handler);
		return handler.getPaths();
	}

	@Override
	public String serialize(List<Path> Paths) throws Exception {
		SAXTransformerFactory factory = (SAXTransformerFactory) TransformerFactory
				.newInstance();
		TransformerHandler handler = factory.newTransformerHandler();
		Transformer transformer = handler.getTransformer();
		transformer.setOutputProperty(OutputKeys.ENCODING, "UTF-8");
		transformer.setOutputProperty(OutputKeys.INDENT, "yes");
		transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "no");
		StringWriter writer = new StringWriter();
		Result result = new StreamResult(writer);
		handler.setResult(result);

		String uri = "";
		String localName = "";
		handler.startDocument();
		handler.startElement(uri, localName, "paths", null);

		char[] ch1 = null;
		char[] ch2 = null;
		for (Path mPath : Paths) {
			handler.startElement(uri, localName, "path", null);
			handler.startElement(uri, localName, "id", null);
			ch1 = String.valueOf(mPath.getId()).toCharArray();
			handler.characters(ch1, 0, ch1.length);
			handler.endElement(uri, localName, "id");
			handler.startElement(uri, localName, "name", null);
			ch2 = String.valueOf(mPath.getName()).toCharArray();
			handler.characters(ch2, 0, ch2.length);
			handler.endElement(uri, localName, "name");
			handler.endElement(uri, localName, "path");
		}
		handler.endElement(uri, localName, "paths");
		handler.endDocument();
		return writer.toString();

	}

	class MyHandler extends DefaultHandler {
		private List<Path> paths;
		private Path mPath;
		private StringBuilder builder;

		public List<Path> getPaths() {
			return paths;
		}

		@Override
		public void characters(char[] ch, int start, int length)
				throws SAXException {
			super.characters(ch, start, length);
			builder.append(ch, start, length);
		}

		@Override
		public void endElement(String uri, String localName, String qName)
				throws SAXException {
			super.endElement(uri, localName, qName);
			if (localName.equals("id")) {
				mPath.setId(Integer.parseInt(builder.toString()));
			} else if (localName.equals("name")) {
				mPath.setName(builder.toString());
			} else if (localName.equals("path")) {
				paths.add(mPath);
			}
		}

		@Override
		public void startDocument() throws SAXException {
			super.startDocument();
			paths = new ArrayList<Path>();
			builder = new StringBuilder();
		}

		@Override
		public void startElement(String uri, String localName, String qName,
				Attributes attributes) throws SAXException {
			super.startElement(uri, localName, qName, attributes);
			if (localName.equals("path")) {
				mPath = new Path();
			}
			builder.setLength(0);

		}

	}

}
