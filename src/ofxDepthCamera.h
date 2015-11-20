/*
*  ofxBaseDepthCamera.h
*  ofxDepthCamera
*
*  Created by Jim George on 3/13/12 for ofxDepthKit
*  Modified by Matt Felsen 10/2015
*
*/

#pragma once

#define OFX_DEPTH_CAMERA_ORBBEC_ASTRA

#include "ofMain.h"
#include "ofxBaseDepthCamera.h"
#include "ofxDepthCameraUtils.h"

#include "ofxDepthCameraKinect.h"
#include "ofxDepthCameraOrbbecAstra.h"

class ofxDepthCamera {
public:

	enum Type {
		OrbbecAstra,
		Kinect
	};

	ofxDepthCamera();
	~ofxDepthCamera();

	void setup();
	void setType(Type type);
	void setPointer(shared_ptr<ofxBaseDepthCamera> pointer);

	void update();

	ofShortPixels& getRawDepth();
	ofImage& getDepthImage();
	ofImage& getColorImage();

protected:
	shared_ptr<ofxBaseDepthCamera> camera;
};

//class ofxKinect : public ofxBaseDepthCamera {
//public:
//	ofxKinect();
//	~ofxKinect();
//
//	void setup();
//	void close();
//	void update();
//
//	int maxDepth();
//	ofVec3f getWorldCoordinateAt(int x, int y);
//
//};