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
    //ofSetWindowShape(640, 480);
    ofSetFrameRate(60);
    
    ofSetVerticalSync(true);
    
    rotate = 0;
    
    // setup bvh
    bvh[0].load("bvhfiles/kashiyuka.bvh");
    bvh[1].load("bvhfiles/nocchi.bvh");
    bvh[2].load("bvhfiles/aachan.bvh");
    
    for (int i = 0; i < 3; i++)    {
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
    for (int i = 0; i < 3; i++)
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
    
    ofVec3f v0(0,0,0);
    A = v0;
    B = v0;
    C = v0;
    D = v0;
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(to_string(ofGetFrameRate()));
    kinect.update();
    
    float t = (track.getPosition() * 64.28);
    t = t / bvh[0].getDuration();
    
    for (int i = 0; i < 3; i++)    {
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
            ofSetColor(ofColor::fromHsb(hue*pow(sin((float)ofGetElapsedTimeMillis()/1000+((float)i*PI/600)),2),sat,bri));
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
        
        for (int i = 0; i < drawers.size()-1; i++){
            drawers[i]->draw();
        }
        ofVec3f a2(drawers[2]->joints.at(17)->getPosition());
        ofVec3f b2(drawers[2]->joints.at(15)->getPosition());
        ofVec3f c2(drawers[2]->joints.at(12)->getPosition());
        ofVec3f d2(drawers[2]->joints.at(10)->getPosition());
        A2=a2;
        B2=b2;
        C2=c2;
        D2=d2;
        v3 = A2-B2;
        v4 = C2-D2;
    }
    
    ofPopMatrix();
    camera.end();
    
    //ofBackground(0);
    ofSetColor(255);
    ofFill();
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
                float z = joint.getProjectivePosition().z;
                switch ((enum Joint) i) {
                    case JOINT_HEAD:
                    {
                        ofSetColor(255, 0, 0);
                        ofDrawCircle(x/5*4+450, y/2+250, z/18, 20);
                        break;
                    }
                    case JOINT_LEFT_HAND:
                    {
                        ofVec3f a1(x,y,z);
                        A1 = a1;
                        //cout << a1 << endl;
                        ofSetColor(255, 0, 0);
                        ofDrawCircle(x/5*4+450, y/2+250, z/18, 11);
                        break;
                    }
                    case JOINT_RIGHT_HAND:
                    {
                        ofVec3f c1(x,y,z);
                        C1 = c1;
                        ofSetColor(255, 0, 0);
                        ofDrawCircle(x/5*4+450, y/2+250, z/18, 11);
                        break;
                    }
                    case JOINT_LEFT_FOOT:
                    case JOINT_RIGHT_FOOT:
                        ofSetColor(255, 0, 0);
                        ofDrawCircle(x/5*4+450, y/2+250, z/18, 11);
                        break;
                        
                    case JOINT_LEFT_ELBOW:
                    {
                        ofVec3f b1(x,y,z);
                        B1 = b1;
                        //cout << B1 << endl;
                        ofSetColor(255);
                        ofDrawCircle(x/5*4+450, y/2+250, z/18, 3);
                        break;
                    }
                    case JOINT_RIGHT_ELBOW:
                    {
                        ofVec3f d1(x,y,z);
                        D1 = d1;
                        ofSetColor(255);
                        ofDrawCircle(x/5*4+450, y/2+250, z/18, 3);
                        break;
                    }
                        
                    default:
                    {
                        ofSetColor(255);
                        ofDrawCircle(x/5*4+450, y/2+250, z/18, 3);
                        break;
                    }
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
                float sZ = limb.getStartJoint().getProjectivePosition().z;
                float eX = limb.getEndJoint().getProjectivePosition().x;
                float eY = limb.getEndJoint().getProjectivePosition().y;
                float eZ = limb.getEndJoint().getProjectivePosition().z;
                ofDrawLine(sX/5*4+450, sY/2+250, sZ/18, eX/5*4+450, eY/2+250, eZ/18);
            }
        }
    }
    v1 = A1-B1;
    v2 = C1-D1;
    //cout << A1 << endl;
    //cout << B1 << endl;
    //cout << C1 << endl;
    //cout << D1 << endl;
    //cout << A << endl;
    //cout << B << endl;
    //cout << C << endl;
    //cout << D << endl;
    ofVec3f vx(cos(PI/4),0,cos(PI/4));
    ofVec3f vy(0,-1,0);
    ofVec3f vz(-cos(PI/4),0,cos(PI/4));
    v3 = v3.x*vx + v3.y*vy + v3.z*vz;
    v4 = v4.x*vx + v4.y*vy + v4.z*vz;
    //cout << v3 << endl;
    //cout << v4  << endl;
    v1.normalize();
    v2.normalize();
    v3.normalize();
    v4.normalize();
    ofVec3f v0(0,0,0);
    //cout << v1 << endl;
    //cout << v2 << endl;
    //cout << v3 << endl;
    //cout << v4 << endl;
    if(v1==v0 && v2==v0){
        hue=0;
        sat=0;
        bri=0;
    }
    else if(ofGetFrameNum()%80==0){
    float dot1 = v1.dot(v3);
    float dot2 = v2.dot(v4);
    float tot = dot1 + dot2;
    //cout << dot1 << endl;
    //cout << dot2 << endl;

        if(dot1 < -0.5){
            hue = 0;
        }else{
            hue = 55 + dot1 * 200;
        }
        if(dot2 < -0.5){
            sat = 0;
        }else{
            sat = 55 + dot2 * 200;
        }
        if(tot < -0.5){
            bri = 0;
        }else{
            bri = 55 + tot * 100;
        }
        //cout << v1 << endl;
        //cout << v2 << endl;
        
        if(A==A1 && B==B1 && C==C1 && D==D1){
            hue=0;
            sat=0;
            bri=0;
        }
        A = A1;
        B = B1;
        C = C1;
        D = D1;
    }
    //cout << hue << endl;
    //cout << sat << endl;
    //cout << bri << endl;
    //cout << A << endl;
    //cout << B << endl;
    //cout << C << endl;
    //cout << D << endl;
   
    //scenery.draw(0, 0, 640, 480);
    //maskedImage.draw(0, 0, 640, 480);
    /*
    float x1=100;
    float y1=100;
    ofVec2f v1(x1,y1);
    V=v1;
    ofVec2f v2(200,100);
    ofVec2f v3=V+v2;
    cout << v3 << endl;
    
    if(ofGetFrameNum()%100==0){
        stage_mode+=1;
    }*/
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
