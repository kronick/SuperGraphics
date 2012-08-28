import processing.opengl.*;

PShape s;
ArrayList<ShapeElement> shapes;
int animationSpeed = 1;
float zoom = 0.5;

PImage bg;

void setup() {
  size(1280,800, JAVA2D);
  smooth();
  frameRate(60);  
  
  loadFile("village-one.svg");
  bg = loadImage("bg-1.jpg");
}

void loadFile(String path) {
  s = loadShape(path);
  shapes = shapeChildren(s);
  for(int i=0; i< shapes.size(); i++) {
    shapes.get(i).reset();
  }
}

ArrayList<ShapeElement> shapeChildren(PShape s) {
  ArrayList<ShapeElement> newShapes = new ArrayList<ShapeElement>();
  
  PShape[] children = s.getChildren();
  for(int i=0; i<children.length; i++) {
    PShape c = children[i];
    if(c.getChildren().length > 0) {
      newShapes.addAll(shapeChildren(c));
      continue;
    }
    else newShapes.add(new ShapeElement(c));
  }    
  
  return newShapes;
}

void draw() {
  background(255);
  //background(bg);
  image(bg, 0,0, width, height);
  
  translate(width/2, height/2);
  scale(zoom);
  translate(-s.width/2, -s.height/2);
  
  for(int i=0; i<shapes.size(); i++) {
    for(int j=0; j<animationSpeed; j++)
      shapes.get(i).update();
    shapes.get(i).draw();
  }
}


class ShapeElement {
  PVector center;
  PVector position;
  PVector positionTarget;
  PVector positionVelocity;
  float rotation;
  float rotationTarget;
  float rotationVelocity;
  float size;
  float sizeTarget;
  PShape shape;
  
  float noiseScale;
  float springTension;
  
  ArrayList<PVector> positionHistory;
  ArrayList<Float> rotationHistory;
  ArrayList<Float> sizeHistory;
  boolean recording = false;
  boolean playback = false;
  int playbackStep = 0;
  int playbackDirection = -1;
  
  public ShapeElement(PShape s) {
    this.shape = s;
    this.size = 1;
    this.sizeTarget = this.size;
    this.rotation = random(-TWO_PI,TWO_PI);
    this.rotationTarget = 0;
    this.rotationVelocity = 0;
    
    this.center = findCenter(s);
    this.position = new PVector(0,0);
    this.positionTarget = this.position.get();
    this.position.add(random(-100,100), random(-100,100), 0);
    this.positionVelocity = new PVector(0,0);
    
    this.springTension = 0.1;
    
    this.noiseScale = 1;
    
    this.positionHistory = new ArrayList<PVector>();
    this.rotationHistory = new ArrayList<Float>();
    this.sizeHistory = new ArrayList<Float>();
  }
  
  public void update() {
    if(!playback) {
      // Get spring-like attraction force
      PVector a = PVector.sub(positionTarget, position);
      a.mult(springTension);
      
      // Get perlin noise force
      float noiseValue = (noise((center.x + position.x) / 1000.,
                               (center.y + position.y) / 1000.,
                               frameCount / 1000.) * 2 - 1) * 30;
      PVector fNoise = new PVector(cos(noiseValue) * noiseScale, sin(noiseValue) * noiseScale);
      
      a.add(fNoise);
      
      positionVelocity.add(a);
      positionVelocity.mult(0.9);  // Drag
      //if(positionVelocity.mag() > 100) {
      //  positionVelocity.normalize();
      //  positionVelocity.mult(100);
      //}
      position.add(PVector.mult(positionVelocity, 0.1));
      
      //size = 1 + cos(frameCount/10.);
      //rotation = (frameCount / 10.);
  
      rotationTarget += noiseValue * 0.001 * noiseScale;    
      //println(noiseValue);
      while(rotationTarget > TWO_PI)  rotationTarget -= TWO_PI;
      while(rotationTarget < -TWO_PI) rotationTarget += TWO_PI;    
      
      
      float rotationDiff = rotationTarget - rotation;
      while(rotationDiff > PI)  rotationDiff -= TWO_PI;
      while(rotationDiff < -PI) rotationDiff += TWO_PI;    
      
      float rA = rotationDiff * 0.1; // Rotational acceleration
  
      rotationVelocity += rA;
      rotationVelocity *= 0.8;   // Damping 
      if(rotationVelocity > 0.1)  rotationVelocity =  0.1;
      if(rotationVelocity < -0.1) rotationVelocity = -0.1;
      
      rotation += rotationVelocity;
      
      if(recording) {
        positionHistory.add(position.get());
        rotationHistory.add(rotation);
        sizeHistory.add(size);  
      }
    }
    else {
      // Play back recording
      position = positionHistory.get(playbackStep);  
      rotation = rotationHistory.get(playbackStep);
      size = sizeHistory.get(playbackStep);
      
      playbackStep += playbackDirection;
      //if(playbackStep < 0) playbackStep = positionHistory.size() - 1;
      if(playbackDirection < 0 && playbackStep <= 0) {
        playbackStep = 0;
        playbackDirection *= -1;
      }
      else if(playbackDirection > 0 && playbackStep >= positionHistory.size() - 1) {
        playbackStep = positionHistory.size() - 1;
        playbackDirection *= -1;
      }
    }
  }
  
  public void draw() {
    pushMatrix();
    translate(position.x, position.y);
    translate(center.x, center.y);
    rotate(rotation);
    scale(size);
    translate(-center.x, -center.y);
    //translate(position.x, position.y);
    shape(shape, 0,0);
    popMatrix();      
    
    //ellipse(center.x + position.x, center.y + position.y, 10,10);
  }
  
  void toggleRecording() {
    if(!recording) {
      playback = false;
      recording = true;
      positionHistory.clear();
      sizeHistory.clear();
      rotationHistory.clear();
    }
    else {
      recording = true;
    }
  }
  
  void togglePlayback() {
    if(!playback) {
      recording = false;
      playback = true;
      playbackStep = positionHistory.size() - 1;
      playbackDirection = -3;
    }
    else {
      playback = false;
    }
  }
  
  void randomize() {
    this.positionTarget = new PVector(random(-500,500), random(-500,500));
    //this.rotationTarget = random(-TWO_PI, TWO_PI);
    this.noiseScale = 0.1;
    this.springTension = 0.00;
  }
  
  void freePosition() {
    this.springTension = 0.1;
    this.noiseScale = 0;
  }
  
  void reset() {
    this.rotationTarget = 0;
    this.positionTarget = new PVector(0,0);  
    this.noiseScale = 0;
    this.springTension = 0.1;
  }

  PVector findCenter(PShape s) {
    //println(s.getFamily());
    if(s.getFamily() == PShape.PRIMITIVE)
      return new PVector(0,0);
      
    int vertCount = s.getVertexCount();
    PVector center = new PVector(0,0);
    for(int i=0; i<vertCount; i++) {
      center.add(new PVector(s.getVertexX(i), s.getVertexY(i)));
    }
    
    center.div(vertCount);
    return center;
  }
  
}

void keyPressed() {
  switch(key) {
    case 'a':
      loadFile("village-one.svg");
      break;
    case 'b':
      loadFile("Apartments.svg");
      break;
      
    case 'r':
      for(int i=0; i< shapes.size(); i++) {
        shapes.get(i).toggleRecording();
      }      
      break;
    case 'p':
      for(int i=0; i< shapes.size(); i++) {
        shapes.get(i).togglePlayback();
      }      
      break;      
    case 'f':
      animationSpeed = animationSpeed > 1 ? 1 : 10;
      break;
    case 'l':
      for(int i=0; i<shapes.size(); i++) {
        shapes.get(i).freePosition();
        shapes.get(i).positionTarget = new PVector(((i/(float)shapes.size() * width) / zoom - width/2) * 0.75, height/2 / zoom);
        shapes.get(i).positionTarget.sub(shapes.get(i).center);
      }
      break;
  }  
}

void mousePressed() {
  if(mouseButton == LEFT) {
    for(int i=0; i< shapes.size(); i++) {
      shapes.get(i).randomize();
    }
  }
  else {
    for(int i=0; i< shapes.size(); i++) {
      shapes.get(i).reset();
    }
  }  
}
 
