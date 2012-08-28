#pragma once

#include "ofMain.h"
#include "ofMath.h"
#include "ofxSVG.h"
#include "ofxSVGParticle.h"
#include "ofxQTKitVideoPlayer.h"

class SuperGraphics: public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

        void loadGraphic(int index);
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        ofxSVG graphic;
        std::vector<ofxSVG> graphics;
        
        ofxQTKitVideoPlayer videoPlayer;

    private:
        std::vector<ofxSVGParticle> particles;

        ofImage background;
    
        float scale, scaleTarget;
        float rotation, rotationTarget;
        float pmouseX, pmouseY;
    
        int currentGraphic;
    
        ofCamera camera;
};
