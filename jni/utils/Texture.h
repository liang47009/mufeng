///***********************************************************************
// created:  21/2/2004
// author:   Paul D Turner
//
// purpose: Defines abstract interface for texture objects.  Texture
// objects are created & destroyed by the Renderer.
// *************************************************************************/
///***************************************************************************
// *   Copyright (C) 2004 - 2009 Paul D Turner & The CEGUI Development Team
// *
// *   Permission is hereby granted, free of charge, to any person obtaining
// *   a copy of this software and associated documentation files (the
// *   "Software"), to deal in the Software without restriction, including
// *   without limitation the rights to use, copy, modify, merge, publish,
// *   distribute, sublicense, and/or sell copies of the Software, and to
// *   permit persons to whom the Software is furnished to do so, subject to
// *   the following conditions:
// *
// *   The above copyright notice and this permission notice shall be
// *   included in all copies or substantial portions of the Software.
// *
// *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// *   OTHER DEALINGS IN THE SOFTWARE.
// ***************************************************************************/
//#ifndef _CEGUITexture_h_
//#define _CEGUITexture_h_
//
//class Texture {
//public:
//	enum PixelFormat {
//		//! Each pixel is 3 bytes. RGB in that order.
//		PF_RGB,
//		//! Each pixel is 4 bytes. RGBA in that order.
//		PF_RGBA,
//		//! Each pixel is 2 bytes. RGBA in that order.
//		PF_RGBA_4444,
//		//! Each pixel is 2 bytes. RGB in that order.
//		PF_RGB_565,
//		//! PVR texture compression. Each pixel is 2 bits.
//		PF_PVRTC2,
//		//! PVR texture compression. Each pixel is 4 bits.
//		PF_PVRTC4,
//		//! S3 DXT1 texture compression (RGB).
//		PF_RGB_DXT1,
//		//! S3 DXT1 texture compression (RGBA).
//		PF_RGBA_DXT1,
//		//! S3 DXT1 texture compression (RGBA).
//		PF_RGBA_DXT3,
//		//! S3 DXT1 texture compression (RGBA).
//		PF_RGBA_DXT5
//	};
//	virtual ~Texture() {
//	}
//	virtual const String& getName() const = 0;
//	virtual const Sizef& getSize() const = 0;
//	virtual const Sizef& getOriginalDataSize() const = 0;
//	virtual const Vector2f& getTexelScaling() const = 0;
//	virtual void loadFromFile(const String& filename,
//			const String& resourceGroup) = 0;
//	virtual void loadFromMemory(const void* buffer, const Sizef& buffer_size,
//			PixelFormat pixel_format) = 0;
//	virtual void blitFromMemory(const void* sourceData, const Rectf& area) = 0;
//	virtual void blitToMemory(void* targetData) = 0;
//	virtual bool isPixelFormatSupported(const PixelFormat fmt) const = 0;
//};
//
//#endif // end of guard _CEGUITexture_h_
