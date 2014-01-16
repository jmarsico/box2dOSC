#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxOsc.h"
#include "newCircle.h"
#include "ofxSuperCollider.h"


#define HOST "localhost"            //setup the host IP here
#define PORT 12345                  //set up the port here

class testApp : public ofBaseApp{

public:
    ~testApp();
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    

    // these are the functions for collision detection
    void contactStart(ofxBox2dContactArgs &e);
    
    static const int numStaticCircles = 6;
    newCircle staticCircles[numStaticCircles];
    
    bool showStats;
        
    ofxBox2d box2d;                         // the box2d world
    vector <newCircle> circles;       // default box2d circles
    vector <ofxSCSynth> synths;         //vector to hold synths
    
    
    
    
    ofxOscSender sender;                    //the OSC sender object
    
    ofxSCSynth	*synth;
	
    ofxSCSynth	*playbuf;
     
    
    
		
};
