//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef SAMPLE_UTIL_TEXTURE_UTILS_H
#define SAMPLE_UTIL_TEXTURE_UTILS_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <NDKHelper.h>

#include "DataContainer.h"

GLuint CreateSimpleTexture2D();

GLuint CreateSimpleTexture2D(AAssetManager* gAssetMgr,
		const std::string filename);

GLuint CreateSimpleTextureCubemap();

bool readFile(AAssetManager* gAssetMgr, const std::string fileName,
		RawDataContainer* buffer_ref);

void printGLString(const char *name, GLenum s);

void checkGlError(const char* op);
//GLuint CreateMipMappedTexture2D();

#endif // SAMPLE_UTIL_TEXTURE_UTILS_H
