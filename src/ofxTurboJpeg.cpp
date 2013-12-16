/*
 *  ofxTurboJpeg.cpp
 *  emptyExample
 *
 *  Created by Oriol Ferrer Mesià on 20/03/12.
 *  Copyright 2012 uri.cat. All rights reserved.
 *
 */

#include "ofxTurboJpeg.h"
#include <sys/stat.h>

ofxTurboJpeg::ofxTurboJpeg(int defaultSize){
	
	handleCompress = NULL;//tjInitCompress();	

	handleDecompress = NULL;//tjInitDecompress();
	
	fileBufferSize = defaultSize;
	imageBufferSize = fileBufferSize;
	fileBuffer = (unsigned char*) malloc ( fileBufferSize ); 
	imageBuffer = (unsigned char*) malloc ( imageBufferSize ); 
}

ofxTurboJpeg::~ofxTurboJpeg(){
	if (handleCompress){
		tjDestroy(handleCompress);
	}
	if (handleDecompress){
		tjDestroy(handleDecompress);
	}

	free(fileBuffer);
	free(imageBuffer);
}

void ofxTurboJpeg::setBufferSize(int bufferSize){	
	
	free(fileBuffer);
	free(imageBuffer);

	if(bufferSize > 0){
		fileBufferSize = bufferSize;
		fileBuffer = (unsigned char*) malloc ( fileBufferSize ); 
		imageBufferSize = bufferSize;
		imageBuffer = (unsigned char*) malloc ( imageBufferSize ); 

	}else{
		fileBufferSize = 1000; //Even when we're being stingy I'm sure we can spare 1K
		fileBuffer = (unsigned char*) malloc ( fileBufferSize ); 
		imageBufferSize = fileBufferSize;
		imageBuffer = (unsigned char*) malloc ( imageBufferSize ); 
	}
}

void ofxTurboJpeg::save( ofImage * img, string saveFileName, int jpegQuality ){
	
	if (img == NULL){
		return;
	}

	save(img->getPixels(), saveFileName, img->width, img->height, jpegQuality);

}

void ofxTurboJpeg::save( ofFbo * fbo, string saveFileName, int jpegQuality ){
	
	if (fbo == NULL){
		return;
	}

	savePixelBuffer.allocate(fbo->getWidth(), fbo->getHeight(), OF_IMAGE_COLOR);
	fbo->readToPixels(savePixelBuffer);
	save(savePixelBuffer.getPixels(), saveFileName, fbo->getWidth(), fbo->getHeight(), jpegQuality);

}

//rgb only for now...
void ofxTurboJpeg::save( unsigned char * pixels, string saveFileName, int width, int height, int jpegQuality ){
	
	if (pixels == NULL) return;

	int pitch = 0, flags = 0, jpegsubsamp = 0;
	int bpp = 3;	//rgb only for now...

	unsigned char * output;
	unsigned long size = sizeof(char) * width * height * bpp;
	if(size <= fileBufferSize){
		output = fileBuffer;
	}else{
		output = (unsigned char *)malloc(size);
	}

	if(handleCompress == NULL)
		handleCompress = tjInitCompress();	
	
	if (handleCompress == NULL){
		printf("Error in tjInitCompress():\n%s\n", tjGetErrorStr());
	}
	
	tjCompress(handleCompress, pixels , width, pitch, height, bpp, output, &size, jpegsubsamp, jpegQuality, flags);

	string filePath = ofToDataPath( saveFileName, false);
	FILE * file = fopen( filePath.c_str(), "wb");
	fwrite ( output , 1 , size , file );
	fclose( file);
	
	if(output != fileBuffer){
		free (output);	
	}	
}

//rgb only for now...
ofImage* ofxTurboJpeg::load(string fileName){
	
	ofImage * img = new ofImage();

	load(fileName,img);

	return img;

}

bool ofxTurboJpeg::load(string fileName, ofImage* dstImg){

	string filePath = ofToDataPath( fileName, false);
	FILE * file = fopen( filePath.c_str(), "rb");

	if ( file != NULL){

		int flags = 0;
		flags = 0; //TJ_FASTUPSAMPLE would be faster but worse!
	
		ofImage* img = dstImg;
		struct stat info;
		stat(filePath.c_str(), &info);	

		unsigned char * pixels;
		int size = info.st_size * sizeof(char);
		if(size <= fileBufferSize){
			pixels = fileBuffer;
		}else{
			pixels = (unsigned char *)malloc(size);
		}

		fread(pixels, info.st_size, 1, file);
		fclose(file);
		
		int w; int h; int subsamp;
		
		if(handleDecompress==NULL)
			handleDecompress = tjInitDecompress();
		
		if (handleDecompress == NULL){
			printf("Error in tjInitDeCompress():\n%s\n", tjGetErrorStr());
		}

		int ok = tjDecompressHeader2( handleDecompress, pixels, size, &w, &h, &subsamp );
		if (ok!=0) {printf("Error in tjDecompressHeader2():\n%s\n", tjGetErrorStr());}
		
		if(w<0 || h<0 || size<0){
			ofLog(OF_LOG_ERROR, "ofxTurboJpeg::load() >> Image %s error decompressing image", fileName.c_str());
			return false;
		}

		unsigned char* rgbData;
		int useImageSize = 3 * w * h * sizeof(char);
		if(useImageSize <= imageBufferSize){
			rgbData = imageBuffer;
		}else{
			rgbData = (unsigned char *)malloc(useImageSize);
		}

		//printf("jpeg is %d x %d\n", w, h);
		// 3 >> rgb only for now
		tjDecompress( handleDecompress, pixels, size, rgbData, w, w*3, h, 3 /* rgb only for now*/, 0 );
		
		img->setFromPixels( rgbData, w, h, OF_IMAGE_COLOR);
		
		if(rgbData != imageBuffer){
			delete rgbData;
		}
		if(pixels != fileBuffer){
			delete pixels;
		}

		return true;

	}else {
		ofLog(OF_LOG_ERROR, "ofxTurboJpeg::load() >> Image %s not found", fileName.c_str());
		return false;
	}
	
}

