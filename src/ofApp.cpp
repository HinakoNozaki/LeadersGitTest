#include <vector>
#include "ofApp.h"

int trackerLength = 40;
float startTime = 0.035;
int stage_mode = 1;
ofSpherePrimitive box;

class Drawer
{
public:
    vector<const ofxBvhJoint*> joints;
    
    void add(const ofxBvhJoint *o){
        joints.push_back(o);
        cout << o->isSite();
    }
    
    void draw(){
        for(int i=0; i<joints.size(); i++){
            if(joints.at(i)->getName()=="Head"){
                ofSetColor(89, 197, 202);
                box.set(20, 20);
                box.setPosition(joints.at(i+1)->getPosition());
                box.draw();
            }else if(joints.at(i)->isSite()){
                ofSetColor(117, 249, 77);
                box.set(5, 20);
                box.setPosition(joints.at(i)->getPosition());
                box.draw();
            }else{
                ofSetColor(149, 42, 52);
                box.set(2, 20);
                box.setPosition(joints.at(i)->getPosition());
                box.draw();
            }
            
        }
    }
};

class Tracker
{
public:
    
    const ofxBvhJoint *joint;
    deque<ofVec3f> points;
    float trackerLength;
    
    void setup(const ofxBvhJoint *o){
        joint = o;
    }
    
    void update() {
        const ofVec3f &p = joint->getPosition();
        
        if (p.distance(points.front()) > 1)
            points.push_front(joint->getPosition());
        
        if (points.size() > trackerLength){
            points.pop_back();
        }
    }
    
    void draw()    {
        if (points.empty()) return;
        
        for (int i = 0; i < points.size() - 1; i++){
            //float a = ofMap(i, 0, points.size() - 1, 1, 0);
            
            ofVec3f &p0 = points[i];
            ofVec3f &p1 = points[i + 1];
            
            float dist = p0.distance(p1);
            
            if (dist < 40) {
                ofSetLineWidth(ofMap(dist, 0, 30, 0, 14));
                ofSetColor(dist*20, 127-dist*10, 255-dist*20);
                ofLine(p0.x, p0.y, p0.z, p1.x, p1.y, p1.z);
            }
        }
    }
    
    void clear() {
        points.clear();
    }
    
    void setTrackerLength(float _trackerLength) {
        trackerLength = _trackerLength;
    }
};

vector<Tracker*> trackers;
vector<Drawer*> drawers;

//--------------------------------------------------------------
void ofApp::setup(){
    //ofBackground(0,0,0);
    ofSetWindowShape(640, 480);
    ofSetFrameRate(60);
    
    ofSetVerticalSync(true);
    
    rotate = 0;
    
    // setup bvh
    bvh[0].load("bvhfiles/kashiyuka.bvh");
    bvh[1].load("bvhfiles/nocchi.bvh");
    //bvh[2].load("bvhfiles/aachan.bvh");
    
    for (int i = 0; i < 2; i++)    {
        bvh[i].play();
    }
    
    track.loadSound("Perfume_globalsite_sound.wav");
    track.play();
    track.setLoop(true);

    kinect.setup();
    kinect.setRegister(true);
    kinect.setMirror(true);
    kinect.addDepthGenerator();
    kinect.addImageGenerator();

    kinect.getDepthGenerator().GetAlternativeViewPointCap().SetViewPoint(kinect.getImageGenerator());
    kinect.addUserGenerator();
    kinect.start();

    //maskedImage.allocate(640, 480, OF_IMAGE_COLOR_ALPHA);
    //scenery.load("壁紙02.jpg");
    // setup tracker and drawer
    for (int i = 0; i < 2; i++)
    {
        ofxBvh &b = bvh[i];
        Drawer *d = new Drawer;
        
        for (int n = 0; n < b.getNumJoints(); n++) {
            const ofxBvhJoint *o = b.getJoint(n);
            Tracker *t = new Tracker;
            t->setup(o);
            t->setTrackerLength(trackerLength);
            trackers.push_back(t);
            d->add(o);
        }
        
        drawers.push_back(d);
    }
    
    ofPushMatrix();
    ofTranslate(0, -100);
    ofScale(1, 1, 1);
    ofRotateX(90);
    camera.setFov(90);
    camera.lookAt(ofVec3f(0,0,0));
    camera.setPosition(300, 30, 300);
    //camera.disableMouseInput();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    
    float t = (track.getPosition() * 64.28);
    t = t / bvh[0].getDuration();
    
    for (int i = 0; i < 2; i++)    {
        bvh[i].setPosition(t);
        bvh[i].update();
    }
    
    for (int i = 0; i < trackers.size(); i++) {
        if (t > startTime) {
            //trackers[i]->setTrackerLength(trackerLength);
            //trackers[i]->update();
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundHex(0x000000);
    ofSetHexColor(0xffffff);
    background.draw(0,0,ofGetWidth(),ofGetHeight());
    
    glEnable(GL_DEPTH_TEST);
    
    camera.begin();
    ofPushMatrix();
    
    {
        ofTranslate(0, -100);
        //ofRotate(rotate, 0, 1, 0);
        ofScale(1, 1, 1);
        
        ofPushMatrix();
        ofRotateX(90);
        
        camera.setFov(90);
        camera.lookAt(ofVec3f(0,0,0));
        //camera.setPosition(320*cos(ofMap(ofGetElapsedTimeMillis(), 0, 2000, 0, PI)), ofGetElapsedTimeMillis()/100, 320*sin(ofMap(ofGetElapsedTimeMillis(), 0, 2000, 0, PI)));
        //camera.setPosition(cos(ofGetElapsedTimeMillis()/300)*300, -ofGetElapsedTimeMillis()/10, sin(ofGetElapsedTimeMillis()/300)*300 );
        
        
        //draw stage
        ofNoFill();
        ofSetLineWidth(4);
        for (int i=0; i<=300; i=i+10){
            switch (stage_mode) {
                case 0:
                    ofSetColor(ofRandom(255),ofRandom(255),ofRandom(255));
                    break;
                case 1:
                    ofSetColor(255*pow(sin(ofMap((float)ofGetElapsedTimeMillis(), 0, 60000.0/132, 0, PI)+(double)i*PI/600),2));
                    break;
                case 2:
                    ofSetColor(0, 0, 255);
                    break;
                default:
                    break;
            }
            ofSetCircleResolution(50);
            ofCircle(0, 0, i);
        }
        
        //draw led pipes
        ofSetLineWidth(3);
        for (int i = 0; i < 3600; i = i + 2) {
            ofSetColor(ofColor::fromHsb(255*pow(sin((float)ofGetElapsedTimeMillis()/1000+((float)i*PI/600)),2),255,255));
            ofDrawLine(cos((float)i*PI/180)*300, sin((float)i*PI/180)*300, -(i/3) ,
                       cos((float)i*PI/180)*300, sin((float)i*PI/180)*300, -(i/3)-30 );
        }
        
        ofPopMatrix();
        
        // draw tracker
        glDisable(GL_DEPTH_TEST);
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        
        for (int i = 0; i < trackers.size(); i++){
            //trackers[i]->draw();
        }
        
        for (int i = 0; i < drawers.size(); i++){
            drawers[i]->draw();
        }
        
    }
    
    ofPopMatrix();
    camera.end();
    
    //ofBackground(0);
    ofSetColor(255);
//    ofSetColor(155, 155, 155);
//    kinect.drawDepth(0, 0, 640, 480);
//    kinect.drawImage(0, 0, 640, 480);
//
//    ofSetColor(0);
//    ofDrawBitmapString(distanceBuffer, 20, 40);
//    ofSetCircleResolution(8);
//    ofSetColor(0, 0, 255, ofRandom(255));
//    ofDrawCircle(mouseX, mouseY, 10);
    
//    kinect.drawDepth(0, 0, 640, 480);
    //kinect.drawSkeletons(0, 0, 640, 480);
    
    if(kinect.getNumTrackedUsers() > 0){
        ofxOpenNIUser user = kinect.getTrackedUser(0);
        
        ofSetColor(255, 255, 255, ofRandom(255));
        for(int i=0; i<user.getNumJoints(); i++){
            ofxOpenNIJoint joint = user.getJoint((enum Joint) i);
            if(joint.isFound()){
                float x = joint.getProjectivePosition().x;
                float y = joint.getProjectivePosition().y;
                switch ((enum Joint) i) {
                    case JOINT_HEAD:
                    case JOINT_LEFT_HAND:
                    case JOINT_RIGHT_HAND:
                    case JOINT_LEFT_FOOT:
                    case JOINT_RIGHT_FOOT:
                        ofSetColor(255, 0, 0);
                        ofDrawCircle(x, y, 20);
                        break;
                        
                    default:
                        ofSetColor(255);
                        ofDrawCircle(x, y, 10);
                        break;
                }
                
            }
        }
        ofSetLineWidth(2);
        ofSetColor(100, 200, 255);
        for(int i=0; i<user.getNumLimbs(); i++){
            ofxOpenNILimb limb = user.getLimb((enum Limb) i);
            if(limb.isFound()){
                float sX = limb.getStartJoint().getProjectivePosition().x;
                float sY = limb.getStartJoint().getProjectivePosition().y;
                float eX = limb.getEndJoint().getProjectivePosition().x;
                float eY = limb.getEndJoint().getProjectivePosition().y;
                ofDrawLine(sX, sY, eX, eY);
            }
        }
    }
    //scenery.draw(0, 0, 640, 480);
    //maskedImage.draw(0, 0, 640, 480);
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
//    unsigned short *depthData = kinect.getDepthRawPixels().getData();
//    unsigned short depthMM = depthData[y * 640 + x];
//    sprintf(distanceBuffer, "%d mm", depthMM);
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
