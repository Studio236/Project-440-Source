#include "Audio440.h"

void Audio440::init() {

	softAmplitude = 0;
	left = new float[512];
	right = new float[512];
	for(int i = 0; i < BUFFER_SIZE; i++) {
		softMagnitude[i] = 0;
	}
}

void Audio440::drawAudioDebug() {
	ofPushStyle();
	
	ofFill();
	stringstream s;
	
	s << "Hard Amp: " << getRawAmp() << "\nSoft Amp: " << getAmp();
	ofSetColor(150,100,156);
	ofRect(0,0,0,200,getAmp());
	ofSetColor(0,100,156);
	int bandNumber = 0;
	for(int i = 0; i < 10; i++) {
		bandNumber++;
	}
	for (int i = 1; i < (int)(512/2); i++){
		ofSetColor(50,0,140);
		ofRect((i*8),ofGetWindowHeight(),8,-softMagnitude[i]*10.0f);
	}
	for (int i = 0; i < 12; i++){
		ofSetColor(0,50,20);
		ofRect((i*50),0,50,getAvgBin(i)*50.0f);
	}
	
	ofSetColor(10,100,200);
	ofCircle(ofGetWidth()/2,ofGetHeight()/2,getAmp()*100.0f);
	

	ofSetColor(0,0,0);
	ofDrawBitmapString(s.str(),10,25,0);
	ofPopStyle();
	
	
}


float Audio440::getAvgBin(int index) {

	return ofMap(20* log10(logAvgBins[index]),0,50,0,10,true);

}

void Audio440::update() {

	static int index=0;
	float avg_power = 0.0f;	
	
	if(index < 80)
		index += 1;
	else
		index = 0;

	rawAmp = 0;
	
	/* do the FFT	*/
	myfft.powerSpectrum(0,(int)512/2, left,BUFFER_SIZE,&magnitude[0],&phase[0],&power[0],&avg_power);
	
	/* start from 1 because mag[0] = DC component */
	/* and discard the upper half of the buffer */
	for(int j=1; j < 512/2; j++) {
		freq[index][j] = magnitude[j];
		softMagnitude[j] += (magnitude[j] - softMagnitude[j])*0.3;
		rawAmp += magnitude[j];
	}

	rawAmp /= 255;
	softAmplitude += (rawAmp - softAmplitude)*0.1;

	//LogAvg Calculations
	for (int i = 0; i < 12; i++)
	{
		float avg = 0;
		int lowFreq;
		if ( i == 0 ){
			lowFreq = 0;
		}else{
			lowFreq = (int)((48000/2) / (float)pow((double)2, (12 - i)));
		}
		int hiFreq = (int)((48000/2) / (float)pow((double)2, (11 - i)));
		int lowBound = floor((double)(lowFreq*2100)/48000);//2100 is very abitrary, it should be 256 but that wasnt working
		int hiBound = ceil((double)(hiFreq*2100)/48000);
		for (int j = lowBound; j <= hiBound; j++)
		{
			avg += softMagnitude[j];
		}
		avg /= (hiBound - lowBound + 1);
		logAvgBins[i] = avg;
	}


	
	
	
}

float Audio440::getAmp() {
	return ofMap(20* log10(softAmplitude),0,20,0,10,true);
}

float Audio440::getRawAmp() {

	return ofMap(20* log10(rawAmp),0,20,0,10,true);
}