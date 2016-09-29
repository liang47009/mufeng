/*
 * GLES2Renderer.h
 *
 *  Created on: 2016-9-26
 *      Author: xll
 */

#ifndef GLES2RENDERER_H_
#define GLES2RENDERER_H_

class GLES2Renderer {
public:
	virtual ~GLES2Renderer()=0;
	virtual bool initDisplay()=0;
	virtual void drawFrame()=0;
};

#endif /* GLES2RENDERER_H_ */
