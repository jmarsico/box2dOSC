#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0, 0, 0);
    sender.setup(HOST, PORT);           //OSC setup (change these params in header)
    showStats = true;
    
    //-----------------------------------------------------------
    //-------------------- Setup box2d --------------------------
    //-----------------------------------------------------------

    box2d.init();
    box2d.setGravity(0, 10);
    box2d.createGround();
    box2d.setFPS(30.0);
    
    ofAddListener(box2d.contactStartEvents, this, &testApp::contactStart);
    
    //build staticCircles
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
            
            //make sure staticCircles are not touching
            while(xPos+radius > prevXPos - prevRad && xPos-radius < prevXPos + prevRad)
            {
                xPos = ofRandom(0+radius, ofGetWidth() - radius);
            }
            while((yPos+radius > prevYPos - prevRad) && (yPos-radius < prevYPos + prevRad))
            {
                yPos = ofRandom(40+radius, ofGetHeight() - radius);
            }
        }
        //setup static circle location, fixture and color
        staticCircles[i].setPhysics(0.0, 0.70, 0.5);
        staticCircles[i].setup(box2d.getWorld(), xPos, yPos, radius);
        staticCircles[i].setData(&staticCircles[i].radius);
        staticCircles[i].color.r = (int)ofRandom(180, 255);
        staticCircles[i].color.g = (int)ofRandom(75, 90);
        staticCircles[i].color.b = (int)ofRandom(100, 150);
        
    }

    //--------------------------------------------------------------
    //--------------------Setup SuperCollider-----------------------
    //--------------------------------------------------------------
	// Create a SuperCollider synth with default parameters
	// and spawn it on the server.
	//--------------------------------------------------------------
	synth = new ofxSCSynth("sine_harmonic");
	synth->create();
    
	//--------------------------------------------------------------
	// Load up a buffer with a sound file, and route its playback
	// to an fx unit via a 2-channel audio bus.
	//--------------------------------------------------------------
	buffer = new ofxSCBuffer();
	buffer->read(ofToDataPath("bell.aif", true));
	
	bus = new ofxSCBus(2);
    
	playbuf = new ofxSCSynth("playbuf_1");
	playbuf->set("bufnum", buffer->index);
	playbuf->set("outbus", bus->index);
	
	delay = new ofxSCSynth("fx_delay");
	delay->set("wet", 0.4);
	delay->set("delaytime", 0.4);
	delay->set("decaytime", 4);
	delay->set("inbus", bus->index);
	delay->set("outbus", 0);
	delay->addToTail();
	

    
    
}

//--------------------------------------------------------------
void testApp::contactStart(ofxBox2dContactArgs &e)
{
    if(e.a != NULL & e.b != NULL)
    {
                
        if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle)
        {
            //ofLog() << "userdataA: " << *((float*)(e.a->GetBody()->GetUserData())); //print userdata of body
            //ofLog() << "userdataB: " << *((float*)(e.b->GetBody()->GetUserData())); //print userdata of body
          
            float velocityB = e.b->GetBody()->GetLinearVelocity().Length();
            float velocityA = e.a->GetBody()->GetLinearVelocity().Length();
            
            velocityA = ofMap(velocityA, 0.0, 60.0, 0.0, 1.0);

            
//------------------------build OSC messages---------------------------------
   /*
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
    */
            
            synth->set("freq", *((float*)e.b->GetBody()->GetUserData())*2);
            synth->set("amp", 1.0f);
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

testApp::~testApp()
{
	//--------------------------------------------------------------
	// Free synths and buffers when we're done - otherwise
	// they'll hang about on SC server, occupying CPU and memory.
	//--------------------------------------------------------------
	synth->free();
	delay->free();
	buffer->free();
	bus->free();
}

//--------------------------------------------------------------
void testApp::draw(){
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
    //--------------------------------------------------------------
	// Modulate synth params based on mouse position.
	//--------------------------------------------------------------
	
	if (button == 0)
	{
		synth->set("freq", x + 40);
		synth->set("amp", 1.0f - (float) y / ofGetHeight());
		synth->set("pan", (float) x / ofGetHeight() - 0.5f);
        
		ofSetColor(255, 255, 255, 100);
		ofCircle(x, y, 10 * (1.0 - (float) y / ofGetHeight()));
	}


}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    //--------------------------------------------------------------
	// Trigger stuff. Press right mouse button for bells.
	//--------------------------------------------------------------
	
	if (button == 0)
	{
		this->mouseDragged(x, y, button);
		synth->set("amp", 0.8f);
	}
	else if (button == 2)
	{
		playbuf->set("rate", 2.0f * x / ofGetWidth());
		playbuf->grain();
		
		ofSetColor(255, 255, 0, 200);
		ofTriangle(x, y - 10, x + 10, y + 10, x - 10, y + 10);
	}

    
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
            
            float hitVel = 10.0;
            ofxOscMessage tap;                                    //set up a new message
            tap.setAddress("/mouseHit");
            tap.addFloatArg(staticCircles[i].radius);          //add (radius) from circ
            tap.addFloatArg(hitVel);                           //add velocity of hit (will come from piezo)
            sender.sendMessage(tap);
        }

    }
    
    
        
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    synth->set("amp", 0.1f);

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
