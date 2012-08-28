#pragma mark once

#ifndef OFXSVGPARTICLE_H
#define OFXSVGPARTICLE_H

#include "ofxSVG.h"

class ofxSVGParticle
{
    public:
        ofxSVGParticle();
        ofxSVGParticle(ofxSVGObject* obj);
        virtual ~ofxSVGParticle();

        void update();
        void draw();

        void randomize();
        void reset();
        void drift();

        ofVec3f center;
        ofVec3f position;
        ofVec3f positionTarget;
        ofVec3f positionVelocity;
        std::vector<ofVec3f> positionSteps; // Used in motion blur
    
        float rotation;
        float rotationTarget;
        float rotationVelocity;
        std::vector<float> rotationSteps; // Used in motion blur

        
        float scale;
        float scaleTarget;
        float scaleVelocity;
        std::vector<float> scaleSteps; // Used in motion blur

        int mode;
    
        bool blur;
        int blurSteps;
    
        ofxSVGObject* obj;

        float noiseScale;
        float springTension;

    protected:
    private:
        ofVec3f findCenter();
};

#endif // OFXSVGPARTICLE_H
