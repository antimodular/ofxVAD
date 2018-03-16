#pragma once

#include "ofMain.h"
#include "VAD.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
       

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

//     void audioIn(float * input, int bufferSize, int nChannels);
    void audioIn(ofSoundBuffer & input);

    int bufferSize;
    
        int winSize;
    int sampleRate;
        int order;
        double threshold;
        int uSize = 4;
        VAD * vad;
   
      vector<int> VAD_begin, VAD_end;
    MatrixXd enFrame;
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int     bufferCounter;
    int     drawCounter;
    
    float smoothedVol;
    float scaledVol;
    
    ofSoundStream soundStream;
};
