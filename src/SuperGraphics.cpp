#include "SuperGraphics.h"

//--------------------------------------------------------------
void SuperGraphics::setup(){
    ofBackground(255,255,255);
    ofSetFrameRate(24);
    ofSetVerticalSync(true);
    
    ofDirectory data(".");
    data.allowExt("svg");
    data.listDir();
    
    for(int i=0; i<data.numFiles(); i++) {
        ofLogNotice(data.getPath(i));
    }
    
    ofEnableSmoothing();
    ofEnableAlphaBlending();
        
    currentGraphic = 0;
    loadGraphic(currentGraphic);

    background.loadImage("bg-1.jpg");
    
    this->scale = 0.1;
    scaleTarget = 0.6;
    rotation = rotationTarget = 0;
    pmouseX = 0;
    pmouseY = 0;
    
    videoPlayer.loadMovie("video-c.MOV");
    videoPlayer.play();
    //videoPlayer.setSpeed(0.5);
    
    //ofSetupScreenPerspective(ofGetWidth(), ofGetHeight(), OF_ORIENTATION_DEFAULT, false, 180, 0, 20000);
    
    camera.resetTransform();
    camera.setupPerspective(true, 10, 0, 20000);
    camera.setPosition(ofGetWidth()/2, ofGetHeight()/2, 2000);
  //  camera.setFov(35);
}

void SuperGraphics::loadGraphic(int index) {
    ofDirectory data(".");
    data.allowExt("svg");
    data.listDir();
    
    index = index % data.numFiles();
    if(index < 0) return;
    
    if(graphics.size() <= index) {
        ofxSVG newGraphic;
        newGraphic.load(data.getPath(index));
        graphics.push_back(newGraphic);
        graphic = newGraphic;
    }
    else {
        graphic = graphics[index];
    }
    //graphic.load(data.getPath(index));
    
    particles.clear();
    vector<ofxSVGObject*> objects = graphic.getObjects();
    for(int i=0; i<objects.size(); i++) {
        particles.push_back(objects[i]);
        particles[i].drift();
    }
    cout << particles.size() << " particles" << endl;
}

//--------------------------------------------------------------
void SuperGraphics::update(){
    videoPlayer.update();
    
    for(int i=0; i<particles.size(); i++) {
        particles[i].update();
    }

    scale += (scaleTarget-scale) * 0.2;
    rotation += (rotationTarget-rotation) * 0.2;
    
    //cout << ofGetFrameRate() << endl;
}

//--------------------------------------------------------------
void SuperGraphics::draw(){
    ofSetColor(255,255,255);
    ofBackground(255,255,255);
    //background.draw(0,0, ofGetWindowWidth(), ofGetWindowHeight());
    ofSetColor(255, 255, 255);
    videoPlayer.draw(0,0, ofGetWindowWidth(), ofGetWindowHeight());
    
    camera.begin();
    ofTranslate(ofGetWindowSize()/2);
    ofScale(scale, scale);
    ofRotateY(rotation);
    ofTranslate(-graphic.getSize()/2);

    for(int i=0; i<particles.size(); i++) {
        ofPushMatrix();
        //ofTranslate(0,0,cos((ofGetFrameNum()+i*5)/50.) * 100);
        particles[i].draw();
        ofPopMatrix();
    }
    camera.end();
    
    ofSetColor(255, 255, 255, scale * 128);
    videoPlayer.draw(0,0, ofGetWindowWidth(), ofGetWindowHeight());

}

//--------------------------------------------------------------
void SuperGraphics::keyPressed(int key){
    switch(key) {
        case 'b':
            for(int i=0; i<particles.size(); i++) {
                particles[i].blur = !particles[i].blur;
            }
            break;
        case 'd':
            for(int i=0; i<particles.size(); i++) {
                particles[i].drift();
            }
            break;            
        case OF_KEY_RIGHT:
            currentGraphic++;
            loadGraphic(currentGraphic);
            break;
        case OF_KEY_LEFT:
            currentGraphic--;
            loadGraphic(currentGraphic);
            break;            
    }
}

//--------------------------------------------------------------
void SuperGraphics::keyReleased(int key){

}

//--------------------------------------------------------------
void SuperGraphics::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void SuperGraphics::mouseDragged(int x, int y, int button){
    if(button == 2) {
        float dX = x - pmouseX;
        float dY = y - pmouseY;
        scaleTarget *= 1 + dY / 100;
        rotationTarget += dX;
    }
    if(scaleTarget < 0.01) scaleTarget = 0.01;
    pmouseX = x;
    pmouseY = y;
}

//--------------------------------------------------------------
void SuperGraphics::mousePressed(int x, int y, int button){
    if(button == 0) {
        for(int i=0; i<particles.size(); i++) {
            particles[i].randomize();
        }
    }
    
    pmouseX = x;
    pmouseY = y;
}

//--------------------------------------------------------------
void SuperGraphics::mouseReleased(int x, int y, int button){
    if(button == 0) {
        for(int i=0; i<particles.size(); i++) {
            particles[i].reset();
        }
    }
}

//--------------------------------------------------------------
void SuperGraphics::windowResized(int w, int h){

}

//--------------------------------------------------------------
void SuperGraphics::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void SuperGraphics::dragEvent(ofDragInfo dragInfo){

}
