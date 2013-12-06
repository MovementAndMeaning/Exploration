#pragma once

#include "ofMain.h"
#include "ofxFaceTracker.h"
#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 12345

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
	ExpressionClassifier classifier;
    
    ofxOscSender sender;
};
