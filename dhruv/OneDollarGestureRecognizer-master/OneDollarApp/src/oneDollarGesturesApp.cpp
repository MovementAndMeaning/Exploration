#include "oneDollarGesturesApp.h"

//--------------------------------------------------------------
void oneDollarGesturesApp::setup(){
	num_created_gestures = 0;
	ofBackground(0,0,0);
	createNewGesture();
	clear_line.setup(this, "clear_line", "Clear line", 785,10);
	create_gesture.setup(this, "create_gesture", "Create gesture", 785,45);
	find.setup(this,"find", "Find matching gesture",785,80);
	save_to_file.setup(this, "save_to_file", "Save to file", 785,115);
	load_from_file.setup(this, "load_from_file", "Load from file", 785,150);
	mode = 0;
}

//--------------------------------------------------------------
void oneDollarGesturesApp::update(){
	if(mode == 1 && ofGetElapsedTimeMillis() >= hide_message_on) {
		mode = 0;
		found_gesture.clear();
	}
}

//--------------------------------------------------------------
void oneDollarGesturesApp::draw(){
	save_to_file.draw();
	load_from_file.draw();
	clear_line.draw();
	create_gesture.draw();
	find.draw();

	if(mode == 0) {	
		glColor3f(1.0f, 1.0f, 0.0);
		glBegin(GL_LINE_STRIP);
		for(int i = 0; i < line.size(); ++i) {
			glVertex2fv(line[i].getPtr());
		}
		glEnd();
	}
	else if (mode == 1) {
		glColor3f(1.0f, 0.0f, 0.6f);
		ofDrawBitmapString(message, 10, ofGetHeight()-40);
	}
	
	glColor3f(0.0, 1.0, 0.2);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < found_gesture.size(); ++i) {
		glVertex2fv(found_gesture[i].getPtr());	
	}
	glEnd();
	
	glColor3f(1.0f, 1.0f, 0.0f);
	ofDrawBitmapString("Number of gestures: " +ofToString(dollar.gestures.size()), 10, ofGetHeight()-25);
	ofDrawBitmapString("Name of current gesture: " +gesture->name, 10, ofGetHeight()-10);
}

//--------------------------------------------------------------
void oneDollarGesturesApp::keyPressed(int key){

}

//--------------------------------------------------------------
void oneDollarGesturesApp::keyReleased(int key){

}

//--------------------------------------------------------------
void oneDollarGesturesApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void oneDollarGesturesApp::mouseDragged(int x, int y, int button){
	if(mode == 0) {
		line.push_back(ofVec2f(x,y));
	}
}

//--------------------------------------------------------------
void oneDollarGesturesApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void oneDollarGesturesApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void oneDollarGesturesApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void oneDollarGesturesApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void oneDollarGesturesApp::dragEvent(ofDragInfo dragInfo){
}

void oneDollarGesturesApp::onClick(string sName) {
	if(sName == "clear_line") {
		line.clear();
	}
	else if(sName == "create_gesture") {
		// Add all the point we've just drawn to the gesture. This creates a "gesture".
		gesture->reset(); 
		for(int i = 0; i < line.size(); ++i) {
			gesture->addPoint(line[i].x, line[i].y);
		}
		
		if(gesture->points.size() <= 10) {
			message = "Please add a line first";
		}
		else {
			dollar.addGesture(gesture);
			message = "Added gesture to recognizer";
			line.clear();
			createNewGesture();
		}
		showMessage(message, 800);
		line.clear();
	}
	else if(sName == "save_to_file") {
		dollar.save(ofToDataPath("gestures.txt",true));
		showMessage("Saved!");
	}
	else if(sName == "load_from_file") {
		dollar.load(ofToDataPath("gestures.txt",true));
		showMessage("Loaded! Note that saved gestures are rotated to the optimal position!",4000);
	}
	else if(sName == "find") {
		// find the gesture which matches the current line.
		ofxGesture* tmp = new ofxGesture();
		for(int i = 0; i < line.size(); ++i) {
			tmp->addPoint(line[i].x, line[i].y);
		}
		line.clear();
		double score = 0.0;
		ofxGesture* match = dollar.match(tmp, &score);
		string result = "Matching score: " +ofToString(score);
		if(match != NULL) {		
			result +=", which matches with gesture: " +match->name;
			found_gesture.clear();
			float dx = ofGetWidth()/2;
			float dy = ofGetHeight()/2;
			for(int i = 0; i < match->resampled_points.size(); ++i) {
				found_gesture.push_back(ofVec2f(dx+match->resampled_points[i].x, dy+match->resampled_points[i].y));
			}
		}
		showMessage(result, 6500);
		delete tmp;
	}	
}

void oneDollarGesturesApp::showMessage(string sMessage, int nDelay) {
	message = sMessage;
	mode = 1;
	hide_message_on = ofGetElapsedTimeMillis() + nDelay;
}

void oneDollarGesturesApp::createNewGesture() {
	++num_created_gestures;
	gesture = new ofxGesture();
	gesture->setName("Gesture#" +ofToString(num_created_gestures));
}