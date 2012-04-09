/*
 *  ofxTurboJpeg.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 20/03/12.
 *  Copyright 2012 uri.cat. All rights reserved.
 *
 */


#pragma once

#include "../libs/turbo-jpeg/include/turbojpeg.h"
#include "ofMain.h"

#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	#include <fstream.h>
#else
	#include <fstream>
#endif

#define STATIC_BUFFER_SIZE 10000000

class ofxTurboJpeg{
	
	public:
	
		ofxTurboJpeg();
		~ofxTurboJpeg();

		void setBufferSize(int bufferSize);
	
		void save( ofImage * img, string path, int jpegQuality = 95 );	
		ofImage* load(string path);
		bool load(string path, ofImage* dstImg);
	
	private:	

		tjhandle handleCompress;
		tjhandle handleDecompress;

		//speed increased roughly 10% by not re-allocating on every load!
		int fileBufferSize;
		unsigned char * fileBuffer;	
		int imageBufferSize;
		unsigned char * imageBuffer;	
};
	
