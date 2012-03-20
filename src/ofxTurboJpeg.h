/*
 *  ofxTurboJpeg.h
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 20/03/12.
 *  Copyright 2012 uri.cat. All rights reserved.
 *
 */


#pragma once

#include "turbojpeg.h"
#include "ofMain.h"
#include <fstream.h>

class ofxTurboJpeg{
	
	public:
	
		ofxTurboJpeg();
		void save( ofImage * img, string path, int jpegQuality );	
		ofImage* load(string path);
	
	private:	

		tjhandle handleCompress;
		tjhandle handleDecompress;
	
};
	