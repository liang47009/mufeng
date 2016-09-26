/*
 * Engine.h
 *
 *  Created on: 2016-9-26
 *      Author: xll
 */

#ifndef ENGINE_H_
#define ENGINE_H_

#include <NDKHelper.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <cpu-features.h>

#include "engine/GLES2Renderer.h"

class Engine {
//	TeapotRenderer renderer_;

	ndk_helper::GLContext* gl_context_;

	bool initialized_resources_;
	bool has_focus_;

	ndk_helper::DoubletapDetector doubletap_detector_;
	ndk_helper::PinchDetector pinch_detector_;
	ndk_helper::DragDetector drag_detector_;
	ndk_helper::PerfMonitor monitor_;

	ndk_helper::TapCamera tap_camera_;

	android_app* app_;

	GLES2Renderer* render_;

	ASensorManager* sensor_manager_;
	const ASensor* accelerometer_sensor_;
	ASensorEventQueue* sensor_event_queue_;

	void UpdateFPS(float fFPS);
	void ShowUI();
	void TransformPosition(ndk_helper::Vec2& vec);

public:
	static void HandleCmd(struct android_app* app, int32_t cmd);
	static int32_t HandleInput(android_app* app, AInputEvent* event);

	Engine();
	~Engine();
	void SetState(android_app* state);
	int InitDisplay();
	void LoadResources();
	void UnloadResources();
	void DrawFrame();
	void TermDisplay();
	void TrimMemory();
	bool IsReady();

	void UpdatePosition(AInputEvent* event, int32_t iIndex, float& fX,
			float& fY);

	void InitSensors();
	void ProcessSensors(int32_t id);
	void SuspendSensors();
	void ResumeSensors();
};

#endif /* ENGINE_H_ */
