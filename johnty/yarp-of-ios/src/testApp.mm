#include "testApp.h"


//yarp specific:



//--------------------------------------------------------------
void testApp::setup(){
    
    int status = 0;
    

    string name;
    
    test_str = "Hello World from oF\n\n***Testing ACE iOS \n   file access function***\n";
    printf("***Testing ACE iOS file access function***\n");
    status = ACE_OS::access(ACE_TEXT("missing.png"), F_OK);
    printf("ACE status for missing file = %i\n",status);
    
    test_str+= "ACE status for missing file = " + ofToString(status);

    
    string str = ofToDataPath("Icon.png");
    printf("\n\ntrying to load: %s....\n", str.c_str());
    status = ACE_OS::access(ACE_TEXT(str.c_str()), F_OK);
    printf("ACE status for found file = %i\n",status);
    
    test_str+= "\n\ntrying to load Icon.png...\n";
    test_str+= "ACE status for found file = " + ofToString(status);
    
    

    name = "/ofiOSYarpReader";
    test_str+= "\n\ntrying to register port \n     "+name;
    printf("opening port \n    %s\n",name.c_str());
    
	port.open(name.c_str());


}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(0, 0, 255);
    ofDrawBitmapString(test_str, 15, 55);
	
}

//--------------------------------------------------------------
void testApp::exit(){
    //port.close();

}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}

