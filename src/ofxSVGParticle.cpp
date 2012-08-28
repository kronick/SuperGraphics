#include "ofxSVGParticle.h"

ofxSVGParticle::ofxSVGParticle()
{
    //ctor
}

ofxSVGParticle::ofxSVGParticle(ofxSVGObject* obj) {
    this->obj = obj;

    center = findCenter();
    cout << "Center: " << center << endl;
    position = ofVec3f(0,0,0);
    positionTarget = ofVec3f(0,0,0);
    positionVelocity = ofVec3f(0,0,0);

    rotation = 0;
    rotationTarget = 0;
    rotationVelocity = 0;

    scale = 1;
    scaleTarget = 1;

    noiseScale = 1;
    springTension = 2;
    
    blur = true;
    blurSteps = 40;
    
    mode = 0;
}

ofxSVGParticle::~ofxSVGParticle()
{
    //dtor
}

void ofxSVGParticle::update() {
    static int frame = 0;
    
    positionSteps.clear();
    rotationSteps.clear();
    scaleSteps.clear();
    
     // Get spring-like attraction force
    ofVec3f a = (positionTarget - position) * springTension;
    if(a.lengthSquared() < 0.1 && mode == 1) {
        randomize();
        update();
        return;
    }

    
    // Get perlin noise force
    float noiseValue = (ofNoise((center.x + position.x) / 1000.,
                       (center.y + position.y) / 1000.,
                       frame / 1000.) * 2 - 1) * 15;
    ofVec3f fNoise(cos(noiseValue) * noiseScale, sin(noiseValue) * noiseScale);

    a += fNoise;
    

    positionVelocity += a;    // Accelerate
    positionVelocity *= 0.9;  // Drag

    ofVec3f oldPosition(position);
    position += positionVelocity * 0.1;

    float rotationDiff = rotationTarget - rotation;
    while(rotationDiff > M_PI) rotationDiff -= M_PI*2;
    while(rotationDiff < -M_PI) rotationDiff += M_PI*2;

    float rA = rotationDiff * 0.1;
    rA += noiseValue * noiseScale / 10000.0f;
    
    rotationVelocity += rA;
    rotationVelocity *= 0.8;
    
    float oldRotation = rotation;
    rotation += rotationVelocity;
    
    if(blur) {
        // Linear interpolation for motion blur steps
        
        // First, figure out how many steps are needed based on translation and rotation distance
        ofVec3f dP = position - oldPosition;
        float dR = rotation - oldRotation;
        
        blurSteps = (int)max((int)dP.length() / 2, (int)abs((ofRadToDeg(dR) / 4)));
        
        for(int i=blurSteps-1; i>=0; i--) {
            positionSteps.push_back(oldPosition + (dP * (1 + i/(float)blurSteps)) / 2); // 180 degree shutter angle
            //positionSteps.push_back(oldPosition + (dP * i/(float)blurSteps));         // 360 degree shutter angle
            rotationSteps.push_back(oldRotation + (dR * (1 + i/(float)blurSteps)) / 2); // 180 degree shutter angle
            //rotationSteps.push_back(oldRotation + (dR * i/(float)blurSteps));         // 360 degree shutter angle
        }
    }
    
    frame++;
}

void ofxSVGParticle::draw() {
    if(blur) {
        float originalOpacity = obj->opacity;
        //obj->setOpacity(255 / ((float)positionSteps.size()+1));
        for(int i=0; i<positionSteps.size(); i+=8) {
            obj->setOpacity((positionSteps.size() - i) / (float)positionSteps.size() * 32);
            ofPushMatrix();
            ofTranslate(positionSteps[i]);
            ofTranslate(center);
            ofRotateZ(ofRadToDeg(rotationSteps[i]));
            ofScale(scale,scale);
            ofTranslate(-center);
            obj->draw();
            ofPopMatrix();            
        }
        obj->setOpacity(originalOpacity);
    }
    ofPushMatrix();
    ofTranslate(position);
    ofTranslate(center);
    ofRotateZ(ofRadToDeg(rotation));
    ofScale(scale,scale);
    ofTranslate(-center);
    obj->draw();
    ofPopMatrix();
}

void ofxSVGParticle::randomize() {
    //positionTarget = ofVec3f(ofRandom(-1000,1000), ofRandom(-1000,1000), ofRandom(-1000,1000));
    positionTarget = ofVec3f(positionTarget.x, positionTarget.y, ofRandom(-1000,1000));
    rotationTarget = ofRandom(-M_PI, M_PI);
    noiseScale = 0.1;
    springTension = 0.2;
}
void ofxSVGParticle::drift() {
    springTension = 0;
    noiseScale = 10;
    mode = mode == 1 ? 0 : 1;
}
void ofxSVGParticle::reset() {
    rotationTarget = 0;
    positionTarget = ofVec3f(0,0,0);
    noiseScale = 0;
    springTension = 0.2;
    mode = 0;
}


ofVec3f ofxSVGParticle::findCenter() {
    ofVec3f center;
    if (ofxSVGCircle *o = dynamic_cast<ofxSVGCircle *>(obj)) {
        center = ofVec3f(o->x, o->y, 0);
    }
    if (ofxSVGRectangle *o = dynamic_cast<ofxSVGRectangle *>(obj)) {
        center = ofVec3f(o->x + o->width/2, o->y + o->height/2, 0);
    }    
    else if(ofxSVGPath *o = dynamic_cast<ofxSVGPath *>(obj)) {
        vector<ofPolyline> polylines = o->getOutline();
        ofVec3f center(0,0);
        int points = 0;
        for(int i=0; i<polylines.size(); i++) {
            for(int j=0; j<polylines[i].size(); j++) {
                center += polylines[i][j];
                points++;
            }
        }

        center /= points;
        cout << "Path center: " << center << " (" << points << " points)" << endl;
        return center;
    }
    else if(ofxSVGPolygon *o = dynamic_cast<ofxSVGPolygon *>(obj)) {
        ofPolyline polyline = o->path;
        ofVec3f center(0,0,0);
        int points;

        for(int j=0; j<polyline.size(); j++) {
            center += polyline[j];
            points++;
        }

        center /= points;
        cout << "Polygon center: " << center << endl;
    }

    return center;
}
