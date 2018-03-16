#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofSetCircleResolution(80);
    ofBackground(54, 54, 54);    
    
    soundStream.printDeviceList();
    
    bufferSize = 256; // 512;
    sampleRate = 44100;
    winSize = 256;
    order = 5;
    threshold = -6;
    uSize = 4;
    vad = new VAD(winSize, bufferSize, order, threshold);
//    soundStream.setup(this, 0, 1, 44100, buffSize, 4);
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    volHistory.assign(400, 0.0);
    
    bufferCounter    = 0;
    drawCounter        = 0;
    smoothedVol     = 0.0;
    scaledVol        = 0.0;
    
    
    ofSoundStreamSettings settings;
    
    // if you want to set the device id to be different than the default
    // auto devices = soundStream.getDeviceList();
    // settings.device = devices[4];
    
    // you can also get devices for an specific api
    // auto devices = soundStream.getDevicesByApi(ofSoundDevice::Api::PULSE);
    // settings.device = devices[0];
    
    // or get the default device for an specific api:
    // settings.api = ofSoundDevice::Api::PULSE;
    
    // or by name
    auto devices = soundStream.getMatchingDevices("default");
    if(!devices.empty()){
        settings.setInDevice(devices[0]);
    }
    
    settings.setInListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    settings.numBuffers = 4;
    soundStream.setup(settings);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
    //lets record the volume into an array
    volHistory.push_back( scaledVol );
    
    //if we are bigger the the size we want to record - lets drop the oldest value
    if( volHistory.size() >= 400 ){
        volHistory.erase(volHistory.begin(), volHistory.begin()+1);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
        //----from audioInputExample example
    
    ofSetColor(225);
    ofDrawBitmapString("AUDIO INPUT EXAMPLE", 32, 32);
    ofDrawBitmapString("press 's' to unpause the audio\n'e' to pause the audio", 31, 92);
    
    ofNoFill();
    
    // draw the left channel:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(32, 170, 0);
    
    ofSetColor(225);
    ofDrawBitmapString("Left Channel", 4, 18);
    
    ofSetLineWidth(1);    
    ofDrawRectangle(0, 0, 512, 200);
    
    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);
    
    ofBeginShape();
    for (unsigned int i = 0; i < left.size(); i++){
        ofVertex(i*2, 100 -left[i]*180.0f);
    }
    ofEndShape(false);
    
    ofPopMatrix();
    ofPopStyle();
    
    // draw the right channel:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(32, 370, 0);
    
    ofSetColor(225);
    ofDrawBitmapString("Right Channel", 4, 18);
    
    ofSetLineWidth(1);    
    ofDrawRectangle(0, 0, 512, 200);
    
    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);
    
    ofBeginShape();
    for (unsigned int i = 0; i < right.size(); i++){
        ofVertex(i*2, 100 -right[i]*180.0f);
    }
    ofEndShape(false);
    
    ofPopMatrix();
    ofPopStyle();
    
    // draw the average volume:
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(565, 170, 0);
    
    ofSetColor(225);
    ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(scaledVol * 100.0, 0), 4, 18);
    ofDrawRectangle(0, 0, 400, 400);
    
    ofSetColor(245, 58, 135);
    ofFill();        
    ofDrawCircle(200, 200, scaledVol * 190.0f);
    
    //lets draw the volume history as a graph
    ofBeginShape();
    for (unsigned int i = 0; i < volHistory.size(); i++){
        if( i == 0 ) ofVertex(i, 400);
        
        ofVertex(i, 400 - volHistory[i] * 70);
        
        if( i == volHistory.size() -1 ) ofVertex(i, 400);
    }
    ofEndShape(false);        
    
    ofPopMatrix();
    ofPopStyle();
    
    drawCounter++;
    
    ofSetColor(225);
    string reportString = "buffers received: "+ofToString(bufferCounter)+"\ndraw routines called: "+ofToString(drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
    ofDrawBitmapString(reportString, 32, 589);
    
    //-----plot VAD sound wave here 
    if(VAD_begin.size() > 0){
        cout << enFrame.row(0) << endl;
        for(int i = 0; i < VAD_begin.size(); i++){
            double x_start = enFrame(0,VAD_begin[i]+1) + 0.5*order*winSize;
            double x_end = enFrame(enFrame.rows()-1,VAD_end[i]+1) + 0.5*order*winSize;
            
            cout<<x_start<<" , "<<x_end<<endl;
        }
    }
}

//--------------------------------------------------------------
//void ofApp::audioIn(float * input, int bufferSize, int nChannels){
void ofApp::audioIn(ofSoundBuffer & input){
    //https://github.com/dmngu9/Voice-Activity-Detection/blob/master/voice_detection.cpp
    
    
    double * dInput = (double*)malloc(bufferSize * sizeof(double));
//    for (int i = 0; i < bufferSize; i++) {
     for(int i = 0; i < input.getNumFrames(); i++){
        dInput[i] = input[i];
    }
    vector <double> outcome = vad->compute(dInput);
     enFrame = vad->getNormalizedEnFrame();
    
      ofLog()<<"outcome.size() "<<outcome.size();
    
    double maxLevel = 0;
//    ofLog()<<"input.getNumFrames() "<<input.getNumFrames();
    for(int i = 0; i < input.getNumFrames(); i++){
        float temp_left = input[i];
        if(maxLevel < abs(temp_left))  maxLevel = abs(temp_left);
    }
//    ofLog()<<"maxLevel "<<maxLevel;
    
    maxLevel += 0.01*maxLevel;
    vector<int> idx(outcome.size(),0);
    for(int i = 0; i < outcome.size(); i++){
        ofLog()<<i<<" , outcome[i] "<<outcome[i]; 
        idx[i] = (outcome[i] > 2.5) ? 1 : 0;
        ofLog()<<i<<" ,  idx[i] "<< idx[i];
    }
  
    
    VectorXd d = VectorXd::Zero(idx.size()-1);
    VectorXd vadStart, vadEnd;
    
    for(int i = 0; i < outcome.size()-1; i++){
        d(i) = idx[i+1] - idx[i];
        ofLog()<<i<<" , d(i) "<<d(i);
        if(d(i) == 1){
            vadStart.conservativeResize(vadStart.size()+1);
            vadStart(vadStart.size()-1) = i;
        }
        else if (d(i) == -1){ 
            vadEnd.conservativeResize(vadEnd.size()+1);
            vadEnd(vadEnd.size()-1) = i;
        }
    }
    
    VAD_begin.clear();
    VAD_end.clear();
    double q = (double) winSize/float(sampleRate);
    ofLog()<<"q "<<q;
    VectorXd temp = vadEnd - vadStart;
//    ofLog()<<"temp "<<temp[0]; //<<" , "<<temp[1]; //<<" , "<<temp[2]<<" , "<<temp[3];
    VectorXd len = temp*q;
     ofLog()<<"len.size() "<<len.size();
//    vector<int> VAD_begin, VAD_end;
    for(int i = 0; i < len.size(); i++){
        if(len(i) >= (uSize*winSize/float(sampleRate))){
            VAD_begin.push_back(vadStart(i));
            VAD_end.push_back(vadEnd(i));
            ofLog()<<i<<" , VAD_begin "<<VAD_begin.back();
             ofLog()<<i<<" , VAD_end "<<VAD_end.back();
        }
    }
    
    
    //----from audioInputExample example
    float curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;    
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume    
    for (int i = 0; i < input.getNumFrames(); i++){
        left[i]        = input[i*2]*0.5;
        right[i]    = input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    //this is how we get the mean of rms :) 
    curVol /= (float)numCounted;
    
    // this is how we get the root of rms :) 
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    bufferCounter++;
    
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
