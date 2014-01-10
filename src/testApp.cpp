#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetBackgroundColor(0, 0, 0);
    
    box2d.init();
    box2d.setGravity(0, 10);
    box2d.createGround();
    box2d.setFPS(30.0);
    //box2d.registerGrabbing();
    
    
    ofAddListener(box2d.contactStartEvents, this, &testApp::contactStart);
    //ofAddListener(box2d.contactEndEvents, this, &testApp::contactEnd);
    
    sender.setup(HOST, PORT);           //OSC setup (change these params in header)
    
    showStats = true;
    
 
    
    for(int i = 0; i < numStaticCircles; i++)
    {
        float radius = ofRandom(20.0, 100.0);
        float xPos = ofRandom(0+radius, ofGetWidth() - radius);
        float yPos = ofRandom(40+radius, ofGetHeight() - radius);
        for(int j = 0; j < i; j++)
        {
            float prevRad = staticCircles[j].radius;
            float prevXPos = staticCircles[j].getPosition().x;
            float prevYPos = staticCircles[j].getPosition().y;
            
            while(xPos+radius > prevXPos - prevRad && xPos-radius < prevXPos + prevRad)
            {
                xPos = ofRandom(0+radius, ofGetWidth() - radius);
            }
            
            while((yPos+radius > prevYPos - prevRad) && (yPos-radius < prevYPos + prevRad))
            {
                yPos = ofRandom(40+radius, ofGetHeight() - radius);
            }
        }

        staticCircles[i].setPhysics(0.0, 0.70, 0.5);
        staticCircles[i].setup(box2d.getWorld(), xPos, yPos, radius);
        staticCircles[i].setData(&staticCircles[i].radius);
        staticCircles[i].color.r = (int)ofRandom(180, 255);
        staticCircles[i].color.g = (int)ofRandom(75, 90);
        staticCircles[i].color.b = (int)ofRandom(100, 150);
        
    }
    
    
}

//--------------------------------------------------------------
void testApp::contactStart(ofxBox2dContactArgs &e)
{
    if(e.a != NULL & e.b != NULL)
    {
                
        if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle)
        {
            ofLog() << "userdataA: " << *((float*)(e.a->GetBody()->GetUserData())); //print userdata of body      
            ofLog() << "userdataB: " << *((float*)(e.b->GetBody()->GetUserData())); //print userdata of body
          
            float velocityB = e.b->GetBody()->GetLinearVelocity().Length();
            float velocityA = e.a->GetBody()->GetLinearVelocity().Length();

            
//------------------------build OSC messages---------------------------------
            
            ofxOscMessage a;                                    //set up a new message
            a.setAddress("/bodyA");                              //initial param of message
            a.addFloatArg(*((float*)e.a->GetBody()->GetUserData()));    //add radius from body.a
            a.addFloatArg(velocityA);                           //add velocity of body.a
            sender.sendMessage(a);                              //send the message
            
            ofxOscMessage b;                                    //set up a new message
            b.setAddress("/bodyB");                              
            b.addFloatArg(*((float*)e.b->GetBody()->GetUserData()));    //add (radius) from body.b
            b.addFloatArg(velocityB);                           //add velocity of body.b
            sender.sendMessage(b);
        }
    }
}


//--------------------------------------------------------------
void testApp::update(){
    
    
    for(int i = 0; i<circles.size(); i++)
    {
        if(circles.at(i).getPosition().x > ofGetWidth()
           || circles.at(i).getPosition().x < 0
           || circles.at(i).getPosition().y > ofGetHeight())
        {
            circles.at(i).destroy();
            circles.erase(circles.begin()+i);
        }
           
    }
    box2d.update();
    
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetBackgroundColor(0, 0, 0);
    ofSetCircleResolution(22);
    for(int i = 0; i < circles.size(); i++)
    {
        ofFill();
        circles[i].draw();
    }
    
    
    ofSetCircleResolution(100);
    for(int i = 0; i < numStaticCircles; i ++)
    {
        staticCircles[i].draw();
    }
    
    if(showStats)
    {
        string info = "";
        info += "Total Bodies: "+ofToString(box2d.getBodyCount())+"\n";
        info += "Vector size: " + ofToString(circles.size())+"\n";
        info += "FPS: "+ofToString(ofGetFrameRate())+"\n";
        ofSetHexColor(0x444342);
        ofDrawBitmapString(info, 30, 30);
    }

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (key == ' ') {
        showStats = !showStats;
    }
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
    for(int i = 0; i < numStaticCircles; i++)
    {
        if(mouseX > staticCircles[i].getPosition().x - staticCircles[i].radius
           && mouseX < staticCircles[i].getPosition().x + staticCircles[i].radius
           && mouseY > staticCircles[i].getPosition().y - staticCircles[i].radius
           && mouseY < staticCircles[i].getPosition().y + staticCircles[i].radius)
        {
            for(int j = 0; j < 20; j++)
            {
                float r = ofRandom(4, 20);                                  // a random radius 4px - 20px
                float xVel = ofRandom(-10, 10);
                float yVel = ofRandom(-10,3);
                newCircle circle;                                           //construct new newCircle object
                circle.setPhysics(3.0, 0.53, 0.1);                          //set the physics
                circle.setup(box2d.getWorld(), mouseX, mouseY, r);          //set up the new circle
                circle.setVelocity(xVel,yVel);
                circles.push_back(circle);                                  //put this new circle at the end
                circle.setData(&circles.at(circles.size()-1).radius);       //setData to reference of radius of last added circle
 
            }
        }
    }
    
    
        
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
