//
//  ofxGesture.cpp
//  emptyExample
//
//  Created by M+M on 2014-04-02.
//
//

#pragma once
#include <vector>
#include <fstream>
#include "ofMain.h"
#include "ofxOneDollarRect.h"

class ofxGesture {
public:
    
	ofxGesture() {
		golden_ratio = 0.5 * (-1.0 + sqrt(5.0));
		angle_precision = 1.0;
	}
    
	void setName(string sName);
	void addPoint(double nX, double nY);
	void draw();
	void resample(int n);
	ofVec2f centroid();
	double length();
	double indicativeAngle();
	void rotateToZero();
	vector<ofVec2f> rotateBy(vector<ofVec2f> oPoints, double nRad);
	void scaleTo(double nSize);
	void translate();
	ofxOneDollarRect boundingBox();
	double distanceAtBestAngle(ofxGesture* pGesture);
	double distanceAtAngle(double nAngle, ofxGesture* pGesture);
	double pathDistance(vector<ofVec2f> oPoints, ofxGesture* pGesture);
	void normalize(int nNumSamples);
	void reset();
    
	// serialize and deserialize.
	friend ostream& operator <<(ostream& os, const ofxGesture& rGesture) {
		os << rGesture.name.c_str() << ' ';
		std::vector<ofVec2f>::const_iterator it = rGesture.resampled_points.begin();
		while(it != rGesture.resampled_points.end()) {
			os << (*it).x << ';' << (*it).y << ';';
			++it;
		};
		return os;
	}
    
	friend istream& operator >>(istream& is, ofxGesture& rGesture) {
		double x,y;
		istringstream ss;
		is >> rGesture.name;
		while(is) {
			is >> x; is.ignore(1); is >> y; is.ignore(1);
			if(is) {
				rGesture.resampled_points.push_back(ofVec2f(x,y));
			}
		}
		return is;
	}
    
	std::string name;
	ofVec2f center;
	std::vector<ofVec2f> points;
	std::vector<ofVec2f> resampled_points;
	double golden_ratio;
	double angle_precision;
};

