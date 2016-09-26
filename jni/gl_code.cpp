/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// OpenGL ES 2.0 code
#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include <android/asset_manager_jni.h>
#include <NDKHelper.h>
#include "utils/texture_utils.h"
#include "utils/ResourceGroupManager.h"
#include "CEGUI/DataContainer.h"

GLuint gProgram;
GLuint gPositionLoc;
GLuint gTexCoordLoc;
GLuint gSamplerLoc;
GLuint gTexture;
AAssetManager* gAssetMgr;

static ResourceGroupManager* rgm;

void created(AAssetManager* assetMgr) {
	gAssetMgr = assetMgr;

	rgm = new ResourceGroupManager(assetMgr);

	rgm->setResourceGroupDirectory("texture", "texture");
	LOGI("find resource group texture:",
			rgm->getResourceGroupDirectory("texture").c_str());
}

bool setupGraphics(int w, int h) {
	LOGI("setupGraphics(%d, %d)", w, h);

	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);

	GLuint vert_shader, frag_shader;
//	char *vert_shader_pathname, *frag_shader_pathname;
	const char* strVsh = "Shaders/mufeng.vsh";
	const char* strFsh = "Shaders/mufeng.fsh";

	// Create shader program
	gProgram = glCreateProgram();
	LOGI("Created Shader %d", gProgram);

	// Create and compile vertex shader
	if (!ndk_helper::shader::CompileShader(&vert_shader, GL_VERTEX_SHADER,
			strVsh)) {
		LOGI("Failed to compile vertex shader");
		glDeleteProgram(gProgram);
		return false;
	}

	// Create and compile fragment shader
	if (!ndk_helper::shader::CompileShader(&frag_shader, GL_FRAGMENT_SHADER,
			strFsh)) {
		LOGI("Failed to compile fragment shader");
		glDeleteProgram(gProgram);
		return false;
	}

	// Attach vertex shader to program
	glAttachShader(gProgram, vert_shader);

	// Attach fragment shader to program
	glAttachShader(gProgram, frag_shader);

	// Get the attribute locations
	gPositionLoc = glGetAttribLocation(gProgram, "a_position");
	gTexCoordLoc = glGetAttribLocation(gProgram, "a_texCoord");
	// Get the sampler location
	gSamplerLoc = glGetUniformLocation(gProgram, "s_texture");
	// Load the texture
	gTexture = CreateSimpleTexture2D();
//	gTexture = CreateSimpleTexture2D(gAssetMgr, "texture/brick.png");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// Set the viewport
	glViewport(0, 0, w, h);
	return true;
}

void renderFrame() {
	GLfloat vertices[] = { -0.5f, 0.5f, 0.0f,  // Position 0
			0.0f, 0.0f,        // TexCoord 0
			-0.5f, -0.5f, 0.0f,  // Position 1
			0.0f, 1.0f,        // TexCoord 1
			0.5f, -0.5f, 0.0f,  // Position 2
			1.0f, 1.0f,        // TexCoord 2
			0.5f, 0.5f, 0.0f,  // Position 3
			1.0f, 0.0f         // TexCoord 3
			};
	GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Use the program object
	glUseProgram(gProgram);

	// Load the vertex position
	glVertexAttribPointer(gPositionLoc, 3, GL_FLOAT, GL_FALSE,
			5 * sizeof(GLfloat), vertices);
	// Load the texture coordinate
	glVertexAttribPointer(gTexCoordLoc, 2, GL_FLOAT, GL_FALSE,
			5 * sizeof(GLfloat), vertices + 3);

	glEnableVertexAttribArray(gPositionLoc);
	glEnableVertexAttribArray(gTexCoordLoc);

	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture);

	// Set the texture sampler to texture unit to 0
	glUniform1i(gSamplerLoc, 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

extern "C" {
JNIEXPORT void JNICALL Java_com_yunfeng_mufeng_GL2JNILib_init(JNIEnv *,
		jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_yunfeng_mufeng_GL2JNILib_step(JNIEnv *,
		jobject);
JNIEXPORT void JNICALL Java_com_yunfeng_mufeng_GL2JNILib_created(JNIEnv *,
		jobject, jobject);
}
;

JNIEXPORT void JNICALL Java_com_yunfeng_mufeng_GL2JNILib_init(JNIEnv *, jobject,
		jint width, jint height) {
	setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_yunfeng_mufeng_GL2JNILib_step(JNIEnv *,
		jobject) {
	renderFrame();
}
JNIEXPORT void JNICALL Java_com_yunfeng_mufeng_GL2JNILib_created(JNIEnv *env,
		jobject, jobject assetManager) {
	AAssetManager* assetMgr = AAssetManager_fromJava(env, assetManager);
	created(assetMgr);
}
