#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxBvh.h"

class ofApp : public ofBaseApp{
private: //＊追加
    ofxOpenNI kinect;
//    char distanceBuffer[100];
    ofImage maskedImage, scenery;

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
    
    ofSoundPlayer track;
    ofxBvh bvh[3];
    
    float rotate;
    float play_rate, play_rate_t;
    
    ofCamera camera;
    ofImage background;
		
};
