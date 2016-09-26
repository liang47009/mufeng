/*
 * Engine.cpp
 *
 *  Created on: 2016-9-26
 *      Author: xll
 */

#include "Engine.h"
#include "utils/texture_utils.h"

//-------------------------------------------------------------------------
//Preprocessor
//-------------------------------------------------------------------------
#define HELPER_CLASS_NAME "com/yunfeng/mufeng/NDKHelper" //Class name of helper function
//-------------------------------------------------------------------------
//Shared state for our app.
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//Ctor
//-------------------------------------------------------------------------
Engine::Engine() :
		initialized_resources_(false), has_focus_(false), app_(NULL), sensor_manager_(
				NULL), accelerometer_sensor_(NULL), sensor_event_queue_(NULL) {
	gl_context_ = ndk_helper::GLContext::GetInstance();
	gSamplerLoc = NULL;
	gTexCoordLoc = NULL;
	gProgram = NULL;
	gPositionLoc = NULL;
	gTexture = NULL;
}

//-------------------------------------------------------------------------
//Dtor
//-------------------------------------------------------------------------
Engine::~Engine() {
}

/**
 * Load resources
 */
void Engine::LoadResources() {
//	renderer_.Init();
//	renderer_.Bind(&tap_camera_);
}

/**
 * Unload resources
 */
void Engine::UnloadResources() {
//	renderer_.Unload();
}

/**
 * Initialize an EGL context for the current display.
 */
int Engine::InitDisplay() {
	if (!initialized_resources_) {
		gl_context_->Init(app_->window);
		LoadResources();
		initialized_resources_ = true;
	} else {
		// initialize OpenGL ES and EGL
		if (EGL_SUCCESS != gl_context_->Resume(app_->window)) {
			UnloadResources();
			LoadResources();
		}
	}

//	ShowUI();

// Initialize GL state.
	glEnable (GL_CULL_FACE);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

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

	// Link program
	if (!ndk_helper::shader::LinkProgram(gProgram)) {
		LOGI("Failed to link program: %d", gProgram);

		if (vert_shader) {
			glDeleteShader(vert_shader);
			vert_shader = 0;
		}
		if (frag_shader) {
			glDeleteShader(frag_shader);
			frag_shader = 0;
		}
		if (gProgram) {
			glDeleteProgram(gProgram);
		}
		return false;
	}
	if (!ndk_helper::shader::ValidateProgram(gProgram)) {
		LOGI("Failed to ValidateProgram: %d", gProgram);
		if (gProgram) {
			glDeleteProgram(gProgram);
		}
		return false;
	}
	// Get the attribute locations
	gPositionLoc = glGetAttribLocation(gProgram, "a_position");
	gTexCoordLoc = glGetAttribLocation(gProgram, "a_texCoord");
	// Get the sampler location
	gSamplerLoc = glGetUniformLocation(gProgram, "s_texture");

	// Load the texture
//	gTexture = CreateSimpleTexture2D();
//	gTexture = CreateSimpleTexture2D(gAssetMgr, "texture/brick.png");
	gTexture = ndk_helper::JNIHelper::GetInstance()->LoadTexture(
			"texture/brick.png");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Note that screen size might have been changed
	glViewport(0, 0, gl_context_->GetScreenWidth(),
			gl_context_->GetScreenHeight());
//	renderer_.UpdateViewport();

	tap_camera_.SetFlip(1.f, -1.f, -1.f);
	tap_camera_.SetPinchTransformFactor(2.f, 2.f, 8.f);

	return 0;
}

GLfloat vertices[] = { -0.5f, 0.5f, 0.0f, // Position 0
		0.0f, 0.0f, // TexCoord 0
		-0.5f, -0.5f, 0.0f, // Position 1
		0.0f, 1.0f, // TexCoord 1
		0.5f, -0.5f, 0.0f, // Position 2
		1.0f, 1.0f, // TexCoord 2
		0.5f, 0.5f, 0.0f, // Position 3
		1.0f, 0.0f // TexCoord 3
		};

GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
/**
 * Just the current frame in the display.
 */
void Engine::DrawFrame() {
	float fFPS;
	if (monitor_.Update(fFPS)) {
//		UpdateFPS(fFPS);
	}
//	renderer_.Update(monitor_.GetCurrentTime());

// Just fill the screen with a color.
	glClearColor(0.5f, 0.5f, 0.5f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	renderer_.Render();
	// Clear the color buffer
	glClear (GL_COLOR_BUFFER_BIT);

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
	glActiveTexture (GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture);

	// Set the texture sampler to texture unit to 0
	glUniform1i(gSamplerLoc, 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
// Swap
	if (EGL_SUCCESS != gl_context_->Swap()) {
		UnloadResources();
		LoadResources();
	}
}

/**
 * Tear down the EGL context currently associated with the display.
 */
void Engine::TermDisplay() {
	gl_context_->Suspend();

}

void Engine::TrimMemory() {
	LOGI("Trimming memory");
	gl_context_->Invalidate();
}
/**
 * Process the next input event.
 */
int32_t Engine::HandleInput(android_app* app, AInputEvent* event) {
	Engine* eng = (Engine*) app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		ndk_helper::GESTURE_STATE doubleTapState =
				eng->doubletap_detector_.Detect(event);
		ndk_helper::GESTURE_STATE dragState = eng->drag_detector_.Detect(event);
		ndk_helper::GESTURE_STATE pinchState = eng->pinch_detector_.Detect(
				event);

		//Double tap detector has a priority over other detectors
		if (doubleTapState == ndk_helper::GESTURE_STATE_ACTION) {
			//Detect double tap
			eng->tap_camera_.Reset(true);
		} else {
			//Handle drag state
			if (dragState & ndk_helper::GESTURE_STATE_START) {
				//Otherwise, start dragging
				ndk_helper::Vec2 v;
				eng->drag_detector_.GetPointer(v);
				eng->TransformPosition(v);
				eng->tap_camera_.BeginDrag(v);
			} else if (dragState & ndk_helper::GESTURE_STATE_MOVE) {
				ndk_helper::Vec2 v;
				eng->drag_detector_.GetPointer(v);
				eng->TransformPosition(v);
				eng->tap_camera_.Drag(v);
			} else if (dragState & ndk_helper::GESTURE_STATE_END) {
				eng->tap_camera_.EndDrag();
			}

			//Handle pinch state
			if (pinchState & ndk_helper::GESTURE_STATE_START) {
				//Start new pinch
				ndk_helper::Vec2 v1;
				ndk_helper::Vec2 v2;
				eng->pinch_detector_.GetPointers(v1, v2);
				eng->TransformPosition(v1);
				eng->TransformPosition(v2);
				eng->tap_camera_.BeginPinch(v1, v2);
			} else if (pinchState & ndk_helper::GESTURE_STATE_MOVE) {
				//Multi touch
				//Start new pinch
				ndk_helper::Vec2 v1;
				ndk_helper::Vec2 v2;
				eng->pinch_detector_.GetPointers(v1, v2);
				eng->TransformPosition(v1);
				eng->TransformPosition(v2);
				eng->tap_camera_.Pinch(v1, v2);
			}
		}
		return 1;
	}
	return 0;
}

/**
 * Process the next main command.
 */
void Engine::HandleCmd(struct android_app* app, int32_t cmd) {
	Engine* eng = (Engine*) app->userData;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		break;
	case APP_CMD_INIT_WINDOW:
		// The window is being shown, get it ready.
		if (app->window != NULL) {
			eng->InitDisplay();
			eng->DrawFrame();
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// The window is being hidden or closed, clean it up.
		eng->TermDisplay();
		eng->has_focus_ = false;
		break;
	case APP_CMD_STOP:
		break;
	case APP_CMD_GAINED_FOCUS:
		eng->ResumeSensors();
		//Start animation
		eng->has_focus_ = true;
		break;
	case APP_CMD_LOST_FOCUS:
		eng->SuspendSensors();
		// Also stop animating.
		eng->has_focus_ = false;
		eng->DrawFrame();
		break;
	case APP_CMD_LOW_MEMORY:
		//Free up GL resources
		eng->TrimMemory();
		break;
	case APP_CMD_DESTROY:
#ifdef USE_NDK_PROFILER
#if !defined(__i386__)
		setenv("CPUPROFILE", "/sdcard/gmon.out", 1);
		moncleanup();
#endif// __i386__
#endif//USE_NDK_PROFILER
		break;
	}
}

//-------------------------------------------------------------------------
//Sensor handlers
//-------------------------------------------------------------------------
void Engine::InitSensors() {
	sensor_manager_ = ASensorManager_getInstance();
	accelerometer_sensor_ = ASensorManager_getDefaultSensor(sensor_manager_,
			ASENSOR_TYPE_ACCELEROMETER);
	sensor_event_queue_ = ASensorManager_createEventQueue(sensor_manager_,
			app_->looper, LOOPER_ID_USER, NULL, NULL);
}

void Engine::ProcessSensors(int32_t id) {
	// If a sensor has data, process it now.
	if (id == LOOPER_ID_USER) {
		if (accelerometer_sensor_ != NULL) {
			ASensorEvent event;
			while (ASensorEventQueue_getEvents(sensor_event_queue_, &event, 1)
					> 0) {
			}
		}
	}
}

void Engine::ResumeSensors() {
	// When our app gains focus, we start monitoring the accelerometer.
	if (accelerometer_sensor_ != NULL) {
		ASensorEventQueue_enableSensor(sensor_event_queue_,
				accelerometer_sensor_);
		// We'd like to get 60 events per second (in us).
		ASensorEventQueue_setEventRate(sensor_event_queue_,
				accelerometer_sensor_, (1000L / 60) * 1000);
	}
}

void Engine::SuspendSensors() {
	// When our app loses focus, we stop monitoring the accelerometer.
	// This is to avoid consuming battery while not being used.
	if (accelerometer_sensor_ != NULL) {
		ASensorEventQueue_disableSensor(sensor_event_queue_,
				accelerometer_sensor_);
	}
}

//-------------------------------------------------------------------------
//Misc
//-------------------------------------------------------------------------
void Engine::SetState(android_app* state) {
	app_ = state;
	doubletap_detector_.SetConfiguration(app_->config);
	drag_detector_.SetConfiguration(app_->config);
	pinch_detector_.SetConfiguration(app_->config);

	//Init helper functions
	ndk_helper::JNIHelper::Init(state->activity, HELPER_CLASS_NAME);

}

bool Engine::IsReady() {
	if (has_focus_)
		return true;

	return false;
}

void Engine::TransformPosition(ndk_helper::Vec2& vec) {
	vec = ndk_helper::Vec2(2.0f, 2.0f) * vec
			/ ndk_helper::Vec2(gl_context_->GetScreenWidth(),
					gl_context_->GetScreenHeight())
			- ndk_helper::Vec2(1.f, 1.f);
}

void Engine::ShowUI() {
	JNIEnv *jni;
	app_->activity->vm->AttachCurrentThread(&jni, NULL);

	//Default class retrieval
	jclass clazz = jni->GetObjectClass(app_->activity->clazz);
	jmethodID methodID = jni->GetMethodID(clazz, "showUI", "()V");
	jni->CallVoidMethod(app_->activity->clazz, methodID);

	app_->activity->vm->DetachCurrentThread();
	return;
}

void Engine::UpdateFPS(float fFPS) {
	JNIEnv *jni;
	app_->activity->vm->AttachCurrentThread(&jni, NULL);

	//Default class retrieval
	jclass clazz = jni->GetObjectClass(app_->activity->clazz);
	jmethodID methodID = jni->GetMethodID(clazz, "updateFPS", "(F)V");
	jni->CallVoidMethod(app_->activity->clazz, methodID, fFPS);

	app_->activity->vm->DetachCurrentThread();
	return;
}

