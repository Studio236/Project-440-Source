#pragma once
#include "Kinect440.h"
#include "ColorTheme.h"
#include "Audio440.h"
#include "Scanlines.h"
#include "IVisualiserBase.h"


class HeiroglyphsParticle{
public:
	HeiroglyphsParticle(float x,float y, float sx, float sy) : age(1) {
		position = ofPoint(x,y);
		speed = ofVec2f(sx,sy);
	
	};
	~HeiroglyphsParticle(){};

	void update() {

		position += speed;

		age++;
	};

	void draw(float amp) {
		ofPushMatrix();
		ofPushStyle();
		ofSetColor(255,255,255);
		ofTranslate(position);
		ofSphere(amp);
		ofPopStyle();
		ofPopMatrix();

	}
	float age;
	ofPoint position;
	ofVec2f speed;
};

class Heiroglyphs : public VisualiserBase{

public:

	Heiroglyphs() : screenToggle(false){};
	~Heiroglyphs(){};

	void init(Audio440&,Kinect440&, ColorTheme&); //Setup vars and refs and such
	void queueIntro(); //Switch on the boolean to start and set up initial vals
	void draw(); //Standard draw loop (also draws intro and outro, use conditionals)
	void update(); //Update data, happens before draw
	void queueOutro(); //Switch outro bool
	bool getCleanupState(); //return a value that specifies the cleanup is over


private:

	Kinect440 * kinect;
	Audio440 * audio;
	ColorTheme * theme;


	Scanlines scan;
	float age;
	float lastAmp;
	
	vector <HeiroglyphsParticle> particles;
	float intro;
	ofFbo fbo;
	ofFbo abrFbo;
	ofShader bulge;
	ofShader abr;
	ofImage testScreenOne;
	ofImage testScreenTwo;
	ofImage heiro;
	ofPoint sph;
	bool isOutro;
	bool isCleanup;
	ofPoint head, hip, pointLeftHand, pointRightHand;
	bool screenToggle;
	int heiroNumber;
};