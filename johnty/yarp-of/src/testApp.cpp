#include "testApp.h"
//using namespace yarp::os;

string name;
string printLine;

//--------------------------------------------------------------
void testApp::setup(){
	name = "/ofYarpReader";
	port.open(name.c_str());
	printLine = "";

}

//--------------------------------------------------------------
void testApp::update(){
	if (port.getPendingReads()) {
		printf("port read!\n");
		yarp::os::Bottle *input = port.read();
		printLine+=input->toString()+"\n";
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	string str;
	str = "Registered address: "+name;
	ofSetColor(255, 0, 255);
	ofDrawBitmapString(str, 25, 25, 0);
	ofSetColor(0, 0, 255);
	ofDrawBitmapString(printLine, 35, 75, 0);
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}