//
//  ofxGesture.cpp
//  emptyExample
//
//  Created by M+M on 2014-04-02.
//
//

#include "ofxGesture.h"

void ofxGesture :: setName(string sName) {
    name = sName;
}

void ofxGesture :: addPoint(double nX, double nY) {
    points.push_back(ofVec2f(nX, nY));
}

void ofxGesture :: draw() {
    std::vector<ofVec2f>::iterator it = points.begin();
    glBegin(GL_LINE_STRIP);
    while(it != points.end()) {
        glVertex2f((*it).x, (*it).y);
        ++it;
    }
    glEnd();
    
    glPointSize(4);
    std::vector<ofVec2f>::iterator it_res = resampled_points.begin();
    glBegin(GL_POINTS);
    while(it_res != resampled_points.end()) {
        glVertex2f((*it_res).x, (*it_res).y);
        ++it_res;
    }
    glEnd();
    ofCircle(center.x, center.y, 5);
}

void ofxGesture :: resample(int n) {
    double I = length()/(n - 1);
    double D = 0;
    
    for(int i = 1; i < points.size(); ++i) {
        ofVec2f curr = points[i];
        ofVec2f prev = points[i-1];
        ofVec2f dir = prev - curr;
        double d = dir.length();
        if( (D + d) >= I) {
            double qx = prev.x + ((I-D)/d) * (curr.x - prev.x);
            double qy = prev.y + ((I-D)/d) * (curr.y - prev.y);
            ofVec2f resampled(qx, qy);
            resampled_points.push_back(resampled);
            points.insert(points.begin() + i, resampled);
            D = 0.0;
        }
        else {
            D += d;
        }
    }
    // Had to do some freaky resizing because of rounding issues.
    while(resampled_points.size() <= (n - 1)) {
        resampled_points.push_back(points.back());
    }
    if(resampled_points.size() > n) {
        resampled_points.erase(resampled_points.begin(), resampled_points.begin()+n);
    }
}

ofVec2f ofxGesture :: centroid() {
    double x = 0;
    double y = 0;
    std::vector<ofVec2f>::iterator it = resampled_points.begin();
    while(it != resampled_points.end()) {
        x += (*it).x;
        y += (*it).y;
        ++it;
    }
    x /= resampled_points.size();
    y /= resampled_points.size();
    ofVec2f tmp(x,y);
    return tmp;
}

double ofxGesture :: length() {
    double len = 0;
    for(int i = 1; i < points.size(); ++i) {
        len += (points[i-1] - points[i]).length();
    }
    return len;
}

double ofxGesture :: indicativeAngle() {
    ofVec2f c = centroid(); // TODO: optimize
    double angle = (c.y - resampled_points[0].y, c.x - resampled_points[0].x);
    return angle;
}

void ofxGesture :: rotateToZero() {
    double angle = indicativeAngle();
    resampled_points = rotateBy(resampled_points, -angle);
}

vector<ofVec2f> ofxGesture :: rotateBy(vector<ofVec2f> oPoints, double nRad) {
    vector<ofVec2f> rotated;
    ofVec2f c = centroid(); // TODO: optimize
    center = c; // TODO optimize
    double cosa = cos(nRad);
    double sina = sin(nRad);
    vector<ofVec2f>::iterator it = oPoints.begin();
    while(it != oPoints.end()) {
        ofVec2f v = (*it);
        double dx = v.x - c.x;
        double dy = v.y - c.y;
        v.x = dx * cosa - dy * sina + c.x;
        v.y = dx * sina + dy * cosa + c.y;
        rotated.push_back(v);
        ++it;
    }
    return rotated;
}

void ofxGesture :: scaleTo(double nSize = 250.0) {
    ofxOneDollarRect rect = boundingBox();
    std::vector<ofVec2f>::iterator it = resampled_points.begin();
    while(it != resampled_points.end()) {
        ofVec2f* v = &(*it);
        v->x = v->x * (nSize/rect.w);
        v->y = v->y * (nSize/rect.h);
        ++it;
    };
}

// translates to origin.
void ofxGesture :: translate() {
    ofVec2f c = centroid(); //TODO: optimize
    std::vector<ofVec2f>::iterator it = resampled_points.begin();
    while(it != resampled_points.end()) {
        ofVec2f* v = &(*it);
        v->x = v->x - c.x;
        v->y = v->y - c.y;
        ++it;
    };
}

ofxOneDollarRect ofxGesture :: boundingBox() {
    double min_x = FLT_MAX, min_y = FLT_MAX, max_x = FLT_MIN, max_y = FLT_MIN;
    std::vector<ofVec2f>::const_iterator it = resampled_points.begin();
    while(it != resampled_points.end()) {
        ofVec2f v = (*it);
        if(v.x < min_x) min_x = v.x;
        if(v.x > max_x) max_x = v.x;
        if(v.y < min_y) min_y = v.y;
        if(v.y > max_y) max_y = v.y;
        ++it;
    }
    
    ofxOneDollarRect rect;
    rect.x = min_x;
    rect.y = min_y;
    rect.w = (max_x - min_x);
    rect.h = (max_y - min_y);
    return rect;
}

double ofxGesture :: distanceAtBestAngle(ofxGesture* pGesture) {
    double angle_range = PI;
    double start_range = -angle_range;
    double end_range = angle_range;
    double x1 = golden_ratio * start_range + (1.0 - golden_ratio) * end_range;
    double f1 = distanceAtAngle(x1, pGesture);
    double x2 = (1.0 - golden_ratio) * start_range + golden_ratio * end_range;
    double f2 = distanceAtAngle(x2, pGesture);
    while(abs(end_range - start_range) > angle_precision) {
        if(f1 < f2) {
            end_range = x2;
            x2 = x1;
            f2 = f1;
            x1 =  golden_ratio * start_range + (1.0 - golden_ratio) * end_range;
            f1 = distanceAtAngle(x1, pGesture);
        }
        else {
            start_range = x1;
            x1 = x2;
            f1 = f2;
            x2 = (1.0 - golden_ratio) * start_range + golden_ratio * end_range;
            f2 = distanceAtAngle(x2, pGesture);
        }
    }
    return min(f1, f2);
}

double ofxGesture :: distanceAtAngle(double nAngle, ofxGesture* pGesture) {
    vector<ofVec2f> points_tmp = resampled_points;
    points_tmp = rotateBy(points_tmp, nAngle);
    return pathDistance(points_tmp, pGesture);
}

// distance between two paths.
double ofxGesture :: pathDistance(vector<ofVec2f> oPoints, ofxGesture* pGesture) {
    // sizes are not equal (?)
    if(oPoints.size() != pGesture->resampled_points.size()) {
        return -1.0;
    }
    double d = 0;
    for(int i = 0; i < resampled_points.size(); ++i) {
        d += (oPoints[i] - pGesture->resampled_points[i]).length();
    }
    return d/oPoints.size();
}

void ofxGesture :: normalize(int nNumSamples) {
    resample(nNumSamples);
    rotateToZero();
    scaleTo();
    translate();
}

void ofxGesture :: reset() {
    resampled_points.erase(resampled_points.begin(), resampled_points.end());
    points.erase(points.begin(), points.end());
    center.set(0,0);
}



