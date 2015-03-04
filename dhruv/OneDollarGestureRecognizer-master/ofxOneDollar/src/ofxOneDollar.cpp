//
//  ofxOneDollar.cpp
//  emptyExample
//
//  Created by M+M on 2014-04-02.
//
//

#include "ofxOneDollar.h"

//----------------------------------------------------
void ofxOneDollar :: addGesture(ofxGesture* pGesture) {
    // finalize and add.
    pGesture->angle_precision = angle_precision;
    pGesture->normalize(num_samples);
    gestures.push_back(pGesture);
}

//----------------------------------------------------
bool ofxOneDollar:: save(string sFile) {
    std::ofstream out_file(sFile.c_str(), ios::out | ios::trunc);
    if(!out_file.is_open()) {
        return false;;
    }
    
    std::vector<ofxGesture*>::const_iterator it = gestures.begin();
    while(it != gestures.end()) {
        out_file << *(*it) << std::endl;
        ++it;
    }
    out_file.close();
}

//----------------------------------------------------
void ofxOneDollar:: load(string sFile) {
    std::ifstream in_file(sFile.c_str());
    if(!in_file) {
        ofLog(OF_LOG_ERROR, "Error while loading gesture file: '%s'", sFile.c_str());
        return;
    }
    // TODO: reset first.
    stringstream ss;
    ss << in_file.rdbuf();
    in_file.close();
    string line;
    while(getline(ss, line)) {
        stringstream iss;
        iss << line;
        ofxGesture* gesture = new ofxGesture();
        iss >> *(gesture);
        gestures.push_back(gesture);
        ofLog(OF_LOG_VERBOSE, "Loaded gesture: '%s' with '%d' samples", gesture->name.c_str(), gesture->resampled_points.size());
    }
}

ofxGesture* ofxOneDollar:: match(ofxGesture* pGesture, double* pScore) {
    double min_dist = FLT_MAX;
    ofxGesture* found_gesture = NULL;
    pGesture->normalize(num_samples);
    std::vector<ofxGesture*>::const_iterator it = gestures.begin();
    while(it != gestures.end()) {
        double dist = (*it)->distanceAtBestAngle(pGesture);
        if(dist > 0 && dist < min_dist) {
            min_dist = dist;
            found_gesture = (*it);
        }
        ++it;
    }
    *pScore = 1.0 - (min_dist/half_diagonal);
    return found_gesture;
}