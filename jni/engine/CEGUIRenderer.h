/*
 * CEGUIRenderer.h
 *
 *  Created on: 2016-9-26
 *      Author: xll
 */

#ifndef CEGUIRENDERER_H_
#define CEGUIRENDERER_H_

#include <android/asset_manager.h>

#include "engine/GLES2Renderer.h"

class CEGUIRenderer: public GLES2Renderer {
public:
	CEGUIRenderer(){};
	~CEGUIRenderer(){};
	bool initDisplay();
	void drawFrame();

	AAssetManager* assetManager;
};

#endif /* CEGUIRENDERER_H_ */
