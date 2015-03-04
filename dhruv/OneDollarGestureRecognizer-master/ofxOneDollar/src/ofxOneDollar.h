//Fixed for 008 OF, here is the link followed
//http://depts.washington.edu/aimgroup/proj/dollar/
//Dhruv Adhia, H+ Technologies Ltd.
// light weight gesture training module for that works very similar to Gesture Variation Follower.

#pragma once
#include <vector>
#include <fstream>
#include "ofMain.h" 
#include "ofxGesture.h"

class ofxOneDollar {
public:
    
    // Need to change the resampling of distance in order to give less priority for scaling factor so that bigger circle and smaller circle could be identified as the intended movement gesture
    
	ofxOneDollar() 
		:num_samples(64)
		,square_size(250.0)
		,angle_precision(1.0)
	{
		half_diagonal = 0.5 * sqrt((square_size*square_size) + (square_size*square_size));
	}
    
    // Functions defining gestures
    void addGesture(ofxGesture* pGesture);
	bool save(string sFile);
	void load(string sFile);
	ofxGesture* match(ofxGesture* pGesture, double* pScore);
    
	std::vector<ofxGesture*> gestures;
	double square_size;
	double half_diagonal;
	int num_samples;
	double angle_precision;
    
};