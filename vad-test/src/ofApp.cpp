#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    int buffSize = 512;
    winSize = 256;
    order = 5;
    threshold = -6;
    uSize = 4;
    vad = new VAD(winSize, buffSize, order, threshold);
    soundStream.setup(this, 0, 1, 44100, buffSize, 4);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    double * dInput = (double*)malloc(bufferSize * sizeof(double));
    for (int i = 0; i < bufferSize; i++) {
        dInput[i] = input[i];
    }
    vector <double> outcome = vad->compute(dInput);
    MatrixXd enFrame = vad->getNormalizedEnFrame();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
