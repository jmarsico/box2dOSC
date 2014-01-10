//
//  newCircle.h
//  box2d_osc
//
//  Created by Jakob Marsico on 1/9/14.
//
//

#ifndef __box2d_osc__newCircle__
#define __box2d_osc__newCircle__
#pragma once

#include "ofxBox2d.h"

class newCircle : public ofxBox2dCircle
{
public:
    
    float radius;
    ofColor color;
    newCircle(){
        
    }
    
    void draw()
    {
        radius = getRadius();
		
		glPushMatrix();
		glTranslatef(getPosition().x, getPosition().y, 0);
		ofSetColor(color);
		ofFill();
		ofCircle(0, 0, radius);
		
		glPopMatrix();
    }
    
    
};



#endif /* defined(__box2d_osc__newCircle__) */
