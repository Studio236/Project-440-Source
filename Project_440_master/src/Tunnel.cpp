#include "Tunnel.h"
#include "fullscreenQuad.h"

void Tunnel::init(Audio440& aud,Kinect440& kin,ColorTheme& the){

	audio = &aud;
	kinect = &kin;
	theme = &the;
	mesh = ofMesh();
	age = 0;
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	scan = Scanlines();
	direction = ofVec3f(0,0,200);
	tunnell.allocate(ofGetWidth(),ofGetHeight());
	tunnelr.allocate(ofGetWidth(),ofGetHeight());
	anaglyphFbo.allocate(ofGetWidth(),ofGetHeight());
	offset = 0;
	prevamp = 0;
	basic.load("shaders/default");
	bulge.load("shaders/bulge");
	anaglyph.load("shaders/fullscreenQuad");
	circle.loadImage("images/softy.png");
	generate();
	light.setup();
	light.setPointLight();
	light.setPosition(ofGetWidth()/2,ofGetHeight()/2,-100);
	
	flash.loadImage("images/flashorb.png");
	flash.resize(ofGetHeight(),ofGetHeight());

	rangeX = 500;
	rangeY = 500;

	FullscreenQuad::setup(anaglyph.getAttributeLocation("a_vertex"), anaglyph.getAttributeLocation("a_texCoord"));
 
	cout << anaglyph.getAttributeLocation("a_vertex") << ", " <<  anaglyph.getAttributeLocation("a_texCoord") << endl;

	kinectHandSize = 100;
} 
void Tunnel::queueIntro(){
	
	isIntro = true;
	isOutro = false;
	isCleanup = false;
	
} 
void Tunnel::draw(){

	if(!isCleanup) {

		
		tunnelr.begin();
		camera.begin();
		camera.setPosition(5 + ofGetWidth()/2,ofGetHeight()/2,1000);
		generate();
		camera.end();
		tunnelr.end();
		
		
		
		tunnell.begin();
		camera.begin();
		camera.setPosition(-5 + ofGetWidth()/2,ofGetHeight()/2,1000);
		generate();
		camera.end();
		tunnell.end();
		
		
		anaglyph.begin();
		anaglyphFbo.begin();
		
		anaglyph.setUniformTexture("left",tunnell.getTextureReference(),0);
		anaglyph.setUniformTexture("right",tunnelr.getTextureReference(),1);


		/*
		glEnable(GL_TEXTURE_RECTANGLE_ARB);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tunnelr.getTextureReference().texData.textureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, tunnell.getTextureReference().texData.textureID);
		
		anaglyph.setUniform1i("left", 1);
		anaglyph.setUniform1i("right",0);
		*/

		anaglyph.setUniform2i("u_display_resolution",ofGetWidth(),ofGetHeight());
		
		FullscreenQuad::draw();
		anaglyphFbo.end();
		anaglyph.end();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB,0);

		bulge.begin();
			bulge.setUniform1f("width", ofGetWidth()/2);
			bulge.setUniform1f("height", ofGetHeight()/2);
			bulge.setUniform1f("radius", (ofMap(audio->getAvgBin(0),0,10,1.0,-0.3))*((float)introCounter/200));

			anaglyphFbo.draw(0,0);
		bulge.end();
		
		offset += 30;

		glShadeModel(GL_SMOOTH);

		if(offset >= 2000){
			offset = 0;
		}

		if(isIntro && introCounter < 200) {
			introCounter+= 10;
		}else if(isIntro){
			isIntro = false;
		}

		if(isOutro && introCounter > 0) {
			introCounter-= 10;
		}else if(isOutro) {
			isOutro = false;
			isCleanup = true;
		}
		
	}

} 

void Tunnel::generate() {
	ofEnableLighting();
		light.enable();
		glEnable(GL_DEPTH_TEST);
		glShadeModel(GL_FLAT);
		ofPushMatrix();
		ofTranslate(0,sin(age)*100);
		ofRotateX(sin(age)*2);

		ofPushStyle();
		mesh.clear();
		for(int i = 1; i < introCounter; i++) {
			for(int j = 1; j < 11; j++) {
				mesh.addVertex(ofVec3f((sin((double)((2*PI)/10)*j)*(ofGetHeight()/4)),(cos((double)((2*PI)/10)*j)*(ofGetHeight()/4)),-i*100  + offset + 1000 ));
				mesh.addVertex(ofVec3f((sin((double)((2*PI)/10)*j)*(ofGetHeight()/4)),(cos((double)((2*PI)/10)*j)*(ofGetHeight()/4)),-i*100  + offset + 100 + 1000 ));
				mesh.addColor(ofColor::fromHsb(theme->color1.getHue(),audio->getAvgBin((i*10 + j) % 14)*25,audio->getAvgBin((i*10 + j) % 14)*25));
				mesh.addColor(ofColor::fromHsb(theme->color1.getHue(),audio->getAvgBin((i*10 + j) % 14)*25,audio->getAvgBin((i*10 + j) % 14)*25));
			}
				
		}

		//KINECT STUFF
		
		if(kinect->updateActivePlayer() != 0){
			pointHead = kinect->getSkeletonJoint(Kinect440::FIRST_ACTIVE, NUI_SKELETON_POSITION_HEAD);
			pointLeftHand = kinect->getSkeletonJoint(Kinect440::FIRST_ACTIVE, NUI_SKELETON_POSITION_HAND_LEFT);
			pointRightHand = kinect->getSkeletonJoint(Kinect440::FIRST_ACTIVE, NUI_SKELETON_POSITION_HAND_RIGHT);
		}

		rangeX = ofMap(pointHead.x, 0, ofGetWidth(), 15, -15);
		rangeY = ofMap(pointHead.y, 0, ofGetHeight(), -10, 15);

		kinectHandSize = abs(ofDist(pointLeftHand.x, pointLeftHand.y, pointRightHand.x, pointRightHand.y));
		
		kinectHandSize = ofMap(kinectHandSize, 0, ofGetWidth(), 0.5, 2);

		ofClear(0);
		//ofBackground(255,255,255);
		ofPushMatrix();
		ofTranslate(ofGetWidth()/2 + (sin(age)*50),ofGetHeight()/2 + (cos(age/2.25)*50));
		ofRotateX(rangeY);
		ofRotateY(rangeX);
		//ofRotateZ(age*10);
		mesh.drawFaces();
		ofPushStyle();
		ofSetColor(ofColor::fromHsb(theme->color3.getHue(),audio->getAvgBin(0)*25,audio->getAvgBin(1)*25));
		ofPushMatrix();
		ofRotateX(age*20);
		ofRotateY(age*10);
		
		/*extra two cubes, can be taken out if necessary or you don't like it
		if(audio->getAmp() > 0.5){
			ofPushMatrix();
			ofTranslate(-200, -200, -200);
			ofBox(audio->getAvgBin(0)*(ofGetHeight()/30)*((float)introCounter/200)   *kinectHandSize    /3);
			ofPopMatrix();

			ofPushMatrix();
			ofTranslate(200, 200, 200);
			ofBox(audio->getAvgBin(0)*(ofGetHeight()/30)*((float)introCounter/200)   *kinectHandSize    /3);
			ofPopMatrix();
		}*/

		ofBox(audio->getAvgBin(0)*(ofGetHeight()/30)*((float)introCounter/200)   *kinectHandSize);
		ofPopMatrix();
		ofPopStyle();
		age += 0.01;

		if(audio->getAmp() - prevamp >= 3 && prevamp < 5) {
			generate();
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		for(int i = 0; i < parts.size(); i++) {
			if(parts[i].position.z > 4000) {
				parts.erase(parts.begin() + i);
			}
			parts[i].update();
			parts[i].draw(audio->getAvgBin(i%14));
		}
		glDisable(GL_BLEND);

		while(parts.size() < 300) {
			parts.push_back(TunnelParticle(ofRandom(-ofGetWidth()/2,ofGetWidth()/2),ofRandom(-ofGetHeight()/2,ofGetHeight()/2),ofRandom(-1000,-800),ofRandom(-1,1),ofRandom(-1,1),ofRandom(30,50),circle));

		}

		light.setPosition(ofGetWidth()/2,ofGetHeight()/2, audio->getAmp()*400);
		prevamp = audio->getAmp();
		//ofCircle(0,0,500);
		ofPopStyle();
		ofPopMatrix();
		ofPopMatrix();
		glDisable(GL_DEPTH_TEST);
		//glDisable(GL_FOG);
		//light.draw();
		//light.disable();
		scan.draw();

		/* To get rid of the weird lines, draw the scanlines after
		ofPushStyle();

			ofSetColor(theme->color3);
			ofFill();

			ofPushMatrix();
				ofTranslate(pointLeftHand.x, pointLeftHand.y, -1000);
				ofSphere(50);
			ofPopMatrix();

			ofPushMatrix();
				ofTranslate(pointRightHand.x, pointRightHand.y, -1000);
				ofSphere(50);
			ofPopMatrix();

		ofPopStyle();
		I dont like the way it looks with the flat shading*/

		light.disable();
		ofDisableLighting();
		
		

}
	
void Tunnel::update(){
	scan.update(audio->getAmp()*25);
} 
void Tunnel::queueOutro(){
	isOutro = true;
	isIntro = false;
} 
bool Tunnel::getCleanupState(){
	return isCleanup;
} //return a value that specifies the cleanup is over