/*
 * SampleRender.h
 *
 *  Created on: 2016-9-26
 *      Author: xll
 */

#ifndef SAMPLERENDER_H_
#define SAMPLERENDER_H_

#include <GLES2/gl2.h>

#include "engine/GLES2Renderer.h"

class SampleRender : public GLES2Renderer {
public:
	SampleRender() {
		gSamplerLoc = NULL;
		gTexCoordLoc = NULL;
		gProgram = NULL;
		gPositionLoc = NULL;
		gTexture = NULL;
	}
	~SampleRender() {
	}
protected:
	bool initDisplay();
	void drawFrame();
private:
	GLuint gProgram;
	GLuint gPositionLoc;
	GLuint gTexCoordLoc;
	GLuint gSamplerLoc;
	GLuint gTexture;

};

#endif /* SAMPLERENDER_H_ */
