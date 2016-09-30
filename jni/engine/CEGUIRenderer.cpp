/*
 * CEGUIRenderer.cpp
 *
 *  Created on: 2016-9-26
 *      Author: xll
 */
#include "CEGUI/Base.h"
#include "CEGUI/Font.h"
#include "CEGUI/System.h"
#include "CEGUI/Window.h"
#include "CEGUI/XMLParser.h"
#include "CEGUI/GUIContext.h"
#include "CEGUI/FontManager.h"
#include "CEGUI/ImageManager.h"
#include "CEGUI/SchemeManager.h"
#include "CEGUI/WindowManager.h"
#include "CEGUI/AnimationManager.h"
#include "CEGUI/widgets/FrameWindow.h"
#include "CEGUI/DefaultResourceProvider.h"
#include "CEGUI/falagard/WidgetLookManager.h"
#include "CEGUI/RendererModules/OpenGLES/Renderer.h"
#include "CEGUI/ImageCodecModules/FreeImage/ImageCodecModule.h"

#include "engine/CEGUIRenderer.h"
#include "engine/AndroidResourceProvider.h"
#include "utils/texture_utils.h"

bool CEGUIRenderer::initDisplay() {
	using namespace CEGUI;
	CEGUI::ImageCodec* d_imageCodec = createImageCodec();
	CEGUI::OpenGLESRenderer* d_renderer = &CEGUI::OpenGLESRenderer::create();
	AndroidResourceProvider* d_resourceProvider =
			CEGUI_NEW_AO AndroidResourceProvider();
	d_resourceProvider->gAssetMgr = this->assetManager;
	// start up CEGUI system using objects created in subclass constructor.
	CEGUI::System::create(*d_renderer, d_resourceProvider, 0, d_imageCodec, 0,
			"", "/sdcard/CEGUI.log");
	// initialise resource system
	// initialise the required dirs for the DefaultResourceProvider
//	CEGUI::DefaultResourceProvider* rp =
//			static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

	/* for each resource type, set a resource group directory. We cast strings
	 to "const CEGUI::utf8*" in order to support general Unicode strings,
	 rather than only ASCII strings (even though currently they're all ASCII).
	 */
	d_resourceProvider->setResourceGroupDirectory("schemes", "schemes/");
	d_resourceProvider->setResourceGroupDirectory("imagesets", "imagesets/");
	d_resourceProvider->setResourceGroupDirectory("fonts", "fonts/");
	d_resourceProvider->setResourceGroupDirectory("layouts", "layouts/");
	d_resourceProvider->setResourceGroupDirectory("looknfeels", "looknfeel/");
//	rp->setResourceGroupDirectory("lua_scripts", "/lua_scripts/");
	d_resourceProvider->setResourceGroupDirectory("schemas", "xml_schemas/");
	d_resourceProvider->setResourceGroupDirectory("animations", "animations/");

	// set the default resource groups to be used
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
//	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	CEGUI::AnimationManager::setDefaultResourceGroup("animations");

	// setup default group for validation schemas
	CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
	if (parser->isPropertyPresent("SchemaDefaultResourceGroup")) {
		parser->setProperty("SchemaDefaultResourceGroup", "schemas");
	}

	CEGUI::String d_usedFiles;
	d_usedFiles = CEGUI::String(__FILE__);
	CEGUI::GUIContext* guiContext =
			&System::getSingleton().getDefaultGUIContext();
    Renderer* const renderer = System::getSingleton().getRenderer();
    if (!renderer) {
    	LOGI("error renderer!");
    }
	// CEGUI relies on various systems being set-up, so this is what we do
	// here first.
	//
	// The first thing to do is load a CEGUI 'scheme' this is basically a file
	// that groups all the required resources and definitions for a particular
	// skin so they can be loaded / initialised easily
	//
	// So, we use the SchemeManager singleton to load in a scheme that loads the
	// imagery and registers widgets for the TaharezLook skin.  This scheme also
	// loads in a font that gets used as the system default.
	SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");

	// The next thing we do is to set a default mouse cursor image.  This is
	// not strictly essential, although it is nice to always have a visible
	// cursor if a window or widget does not explicitly set one of its own.
	//
	// The TaharezLook Imageset contains an Image named "MouseArrow" which is
	// the ideal thing to have as a defult mouse cursor image.
	guiContext->getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

	// Now the system is initialised, we can actually create some UI elements, for
	// this first example, a full-screen 'root' window is set as the active GUI
	// sheet, and then a simple frame window will be created and attached to it.

	// All windows and widgets are created via the WindowManager singleton.
	WindowManager& winMgr = WindowManager::getSingleton();

	// Here we create a "DefaultWindow".  This is a native type, that is, it does
	// not have to be loaded via a scheme, it is always available.  One common use
	// for the DefaultWindow is as a generic container for other windows.  Its
	// size defaults to 1.0f x 1.0f using the Relative metrics mode, which means
	// when it is set as the root GUI sheet window, it will cover the entire display.
	// The DefaultWindow does not perform any rendering of its own, so is invisible.
	//
	// Create a DefaultWindow called 'Root'.
	Window* d_root = winMgr.createWindow("DefaultWindow", "Root");

	// load font and setup default if not loaded via scheme
	Font& defaultFont = FontManager::getSingleton().createFromFile(
			"DejaVuSans-12.font");
	// Set default font for the gui context
	guiContext->setDefaultFont(&defaultFont);

	// Set the root window as root of our GUI Context
	guiContext->setRootWindow(d_root);

	// A FrameWindow is a window with a frame and a titlebar which may be moved around
	// and resized.
	//
	// Create a FrameWindow in the TaharezLook style, and name it 'Demo Window'
	FrameWindow* wnd = (FrameWindow*) winMgr.createWindow(
			"TaharezLook/FrameWindow", "Demo Window");

	// Here we attach the newly created FrameWindow to the previously created
	// DefaultWindow which we will be using as the root of the displayed gui.
	d_root->addChild(wnd);

	// Windows are in Relative metrics mode by default.  This means that we can
	// specify sizes and positions without having to know the exact pixel size
	// of the elements in advance.  The relative metrics mode co-ordinates are
	// relative to the parent of the window where the co-ordinates are being set.
	// This means that if 0.5f is specified as the width for a window, that window
	// will be half as its parent window.
	//
	// Here we set the FrameWindow so that it is half the size of the display,
	// and centered within the display.
	wnd->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.25f)));
	wnd->setSize(USize(cegui_reldim(0.5f), cegui_reldim( 0.5f)));

	// now we set the maximum and minum sizes for the new window.  These are
	// specified using relative co-ordinates, but the important thing to note
	// is that these settings are aways relative to the display rather than the
	// parent window.
	//
	// here we set a maximum size for the FrameWindow which is equal to the size
	// of the display, and a minimum size of one tenth of the display.
	wnd->setMaxSize(USize(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
	wnd->setMinSize(USize(cegui_reldim(0.1f), cegui_reldim( 0.1f)));

	// As a final step in the initialisation of our sample window, we set the window's
	// text to "Hello World!", so that this text will appear as the caption in the
	// FrameWindow's titlebar.
	wnd->setText("Hello World!");

	// return true so that the samples framework knows that initialisation was a
	// success, and that it should now run the sample.
	return true;
}

void CEGUIRenderer::drawFrame() {
    CEGUI::System& gui_system(CEGUI::System::getSingleton());
    gui_system.getDefaultGUIContext().draw();
}
