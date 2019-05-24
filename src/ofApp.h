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
    float X;
    float hue=0;
    float sat=0;
    float bri=0;
    ofVec3f A1, B1, C1, D1, A2, B2, C2, D2, v1, v2, v3, v4, A, B, C, D;
    
    ofCamera camera;
    ofImage background;
		
};
