#include "Fireflies.h"


Fireflies::Fireflies(void)
{
}


Fireflies::~Fireflies(void)
{
}

void Fireflies::init(Audio440& aud,Kinect440& kin, ColorTheme& the){
	kinect = &kin;
	audio = &aud;
	cTheme = &the;
	age = 0;

	fbo.allocate(ofGetWidth(), ofGetHeight());
	bulge.load("shaders/bulge");
	scan = Scanlines();

	abr.load("shaders/chrabr");
	abrFbo.allocate(ofGetWidth(), ofGetHeight());

	flash.loadImage("images/flashorb.png");
	flash.resize(ofGetHeight(),ofGetHeight());

	sph = ofPoint(41,20);
	intro = 0.0f;
	lastAmp = 0;
	isOutro = false;

	int nBandsToGet = 256;

	prevBandAverage = 0;

	for (int i = 0;i < nBandsToGet; i++){
		pEllipse pe = pEllipse(audio->getBin(i), *cTheme);
		myParts.push_back(pe);
	}
}
	
void Fireflies::queueIntro(){
	intro = 0.0f;
	isOutro = false;
	isCleanup = false;
}
void Fireflies::draw(){
	

	if(!isCleanup) {
		fbo.begin();

			ofPushStyle();

			ofBackground(0);

			ofPushMatrix();
				//ofTranslate(0,sin(age/2)*100);
				ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
				ofRotateX(sin(age/2)*5);
				ofRotateY(cos(age/2)*5);
				ofRotateZ(-sin(age/2)*5);
				ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
					

			ofColor rectColor = ofColor(cTheme->color1.getHue(), cTheme->color1.getSaturation(), cTheme->color1.getBrightness(), audio->getAmp()*10);
			ofSetColor(rectColor);
			ofFill();
			ofRect(-ofGetWidth()/2, -ofGetHeight()/2, 0, ofGetWidth()*2, ofGetHeight()*2);

			ofPopStyle();

			ofPushStyle();

			for (int i = 0; i < myParts.size(); i++)
			{   
				float avDiff = fabs(audio->getAmp() - prevBandAverage);

				if(avDiff > 0.5){
					for(int j = 0; j < myParts.size(); j++){
						myParts[j].explode(10);
					}
				}

				
				if(avDiff > 1){
					for(int j = 0; j < myParts.size(); j++){
						myParts[j].implode(0.5);
					}
				}

				myParts[i].update();
				myParts[i].draw(audio->getAvgBin(i%10)*intro);
			}

				ofPushMatrix();
					ofPushStyle();
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE);
						ofSetColor(audio->getAmp()*35,audio->getAmp()*35,audio->getAmp()*35,(audio->getAmp()*35)*intro);
						//flash.draw(sin(age/100)*ofGetWidth()/4  + ofGetWidth()/2 - flash.width/2,cos(age/100)*ofGetHeight()/4 + ofGetHeight()/2 - flash.height/2,flash.height,flash.width);
						//flash.draw(-sin(age/100)*ofGetWidth()/4  + ofGetWidth()/2 - flash.width/2,cos(age/100)*ofGetHeight()/4 + ofGetHeight()/2 - flash.height/2,flash.height,flash.width);
						//flash.draw(sin(age/100)*ofGetWidth()/4  + ofGetWidth()/2 - flash.width/2,-cos(age/100)*ofGetHeight()/4 + ofGetHeight()/2 - flash.height/2,flash.height,flash.width);
						//flash.draw(-sin(age/100)*ofGetWidth()/4  + ofGetWidth()/2 - flash.width/2,-cos(age/100)*ofGetHeight()/4 + ofGetHeight()/2 - flash.height/2,flash.height,flash.width);
						flash.draw(ofGetWidth()/2 - flash.width/2, ofGetHeight()/2 - flash.height/2, flash.height, flash.width);
						glDisable(GL_BLEND);
			
					ofPopStyle();
				ofPopMatrix();

			ofPopStyle();

			scan.draw();

			ofPopMatrix();

		fbo.end();


		abrFbo.begin();
			abr.begin();
				//abr.setUniform1f("amount", audio->getAvgBin(5)*50);
				//abr.setUniform1f("intensity", ofMap(audio->getAmp(),0,8,0.01,0.12));
				//fbo.draw(0,0);
			abr.end();
		abrFbo.end();

		bulge.begin();
			bulge.setUniform1f("width", ofGetWidth()/2 + sin(age/10)*200);
			bulge.setUniform1f("height", ofGetHeight()/2 + cos(age/10)*200);
			bulge.setUniform1f("radius", (ofMap(audio->getAvgBin(5),0,10,0.8,-0.3))*intro);
			

			fbo.draw(0,0,ofGetWidth(),ofGetHeight());
			abrFbo.draw(0,0);
		bulge.end();

		prevBandAverage = audio->getAmp();
	}


	if(isOutro) {
		intro-= 0.1f;
		if(intro <= 0.1) {
			isCleanup = true;
		}
	}else{
		if(intro < 1.0) {
			intro += 0.1;
		}else{
			intro = 1.0;
		}
	}

	age += 0.3;
}
void Fireflies::update(){
	scan.update((audio->getAvgBin(0))*intro);
}
void Fireflies::queueOutro(){
	isOutro = true;
}

bool Fireflies::getCleanupState(){
	return isCleanup;
}