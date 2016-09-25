//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include <FreeImage.h>
#include <array>
#include <stdio.h>

#include "Size.h"
#include "texture_utils.h"

bool readFile(AAssetManager* gAssetMgr, const std::string fileName,
		RawDataContainer* buffer_ref) {
	//Fallback to assetManager
	AAsset* assetFile = AAssetManager_open(gAssetMgr, fileName.c_str(),
			AASSET_MODE_BUFFER);
	if (!assetFile) {
		return false;
	}
	uint8_t* data = (uint8_t*) AAsset_getBuffer(assetFile);
	int32_t size = AAsset_getLength(assetFile);
	if (data == NULL) {
		AAsset_close(assetFile);
		LOGI("Failed to load:%s", fileName.c_str());
		return false;
	}
	buffer_ref->setData(data);
	buffer_ref->setSize(size);
//	buffer_ref->reserve(size);
//	buffer_ref->assign(data, data + size);

	AAsset_close(assetFile);
	return true;
}

void printGLString(const char *name, GLenum s) {
	const char *v = (const char *) glGetString(s);
	LOGI("GL %s = %s\n", name, v);
}

void checkGlError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		LOGI("after %s() glError (0x%x)\n", op, error);
	}
}

GLuint CreateSimpleTexture2D() {
	// Use tightly packed data
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Generate a texture object
	GLuint texture;
	glGenTextures(1, &texture);

	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, texture);

	// Load the texture: 2x2 Image, 3 bytes per pixel (R, G, B)
	const size_t width = 2;
	const size_t height = 2;
	GLubyte pixels[width * height * 3] = { 255, 0, 0, // Red
			0, 255, 0, // Green
			0, 0, 255, // Blue
			255, 255, 0, // Yellow
			};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
	GL_UNSIGNED_BYTE, pixels);
	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return texture;
}

GLuint CreateSimpleTexture2D(AAssetManager* gAssetMgr,
		const std::string filename) {
	// Use tightly packed data
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Generate a texture object
	GLuint texture;
	glGenTextures(1, &texture);

	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, texture);

	// Load the texture: 2x2 Image, 3 bytes per pixel (R, G, B)
	const size_t width = 2;
	const size_t height = 2;
	GLubyte pixels[width * height * 3] = { 255, 0, 0, // Red
			0, 255, 0, // Green
			0, 0, 255, // Blue
			255, 255, 0, // Yellow
			};
	RawDataContainer buffer;
	readFile(gAssetMgr, filename, &buffer);
	FIMEMORY *mem = 0;
	FIBITMAP *img = 0;

	try {
		mem = FreeImage_OpenMemory(buffer.getDataPtr(), buffer.getSize());
		if (mem == 0) {
			LOGE("Unable to open memory stream, FreeImage_OpenMemory failed");
		}

		FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(mem,
				buffer.getSize());

		if (fif == FIF_UNKNOWN) { // it may be that it's TARGA or MNG
			fif = FIF_TARGA;
			img = FreeImage_LoadFromMemory(fif, mem, 0);
			if (img == 0) {
				fif = FIF_MNG;
				img = FreeImage_LoadFromMemory(fif, mem, 0);
			}
		} else {
			img = FreeImage_LoadFromMemory(fif, mem, 0);
		}
		if (img == 0) {
			LOGE("Unable to load image, FreeImage_LoadFromMemory failed");
		}

		FIBITMAP *newImg = FreeImage_ConvertTo32Bits(img);
		if (newImg == 0) {
			LOGE("Unable to convert image, FreeImage_ConvertTo32Bits failed");
		}
		FreeImage_Unload(img);
		img = newImg;
		newImg = 0;

		// FreeImage pixel format for little-endian architecture (which CEGUI
		// supports) is like BGRA. We need to convert that to RGBA.
		//
		// It is now:
		// RED_MASK		0x00FF0000
		// GREEN_MASK	0x0000FF00
		// BLUE_MASK	0x000000FF
		// ALPHA_MASK	0xFF000000
		//
		// It should be:
		// RED_MASK		0x000000FF
		// GREEN_MASK	0x0000FF00
		// BLUE_MASK	0x00FF0000
		// ALPHA_MASK	0xFF000000

		uint pitch = FreeImage_GetPitch(img);
		uint height = FreeImage_GetHeight(img);
		uint width = FreeImage_GetWidth(img);
		uint8_t *rawBuf = new uint8_t[width * height << 2];

		// convert the bitmap to raw bits (top-left pixel first)
		FreeImage_ConvertToRawBits(rawBuf, img, pitch, 32,
		FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);

		// We need to convert pixel format a little
		// NB: little endian only - I think(!)
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
		for (uint i = 0; i < height; ++i) {
			for (uint j = 0; j < width; ++j) {
				uint p = *(((uint*) (rawBuf + i * pitch)) + j);
				uint r = (p >> 16) & 0x000000FF;
				uint b = (p << 16) & 0x00FF0000;
				p &= 0xFF00FF00;
				p |= r | b;
				// write the adjusted pixel back
				*(((uint*) (rawBuf + i * pitch)) + j) = p;
			}
		}
#endif
		FreeImage_Unload(img);
		img = 0;

//		result->loadFromMemory(rawBuf, Sizef(width, height), Texture::PF_RGBA);
		delete[] rawBuf;
	} catch (std::exception& e) {
		LOGE("FreeImage load error: ", e.what());
	}

	if (img != 0)
		FreeImage_Unload(img);
	if (mem != 0)
		FreeImage_CloseMemory(mem);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, pixels);
	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return texture;
}

//unsigned loadTexture(const char * fileName) {
//	unsigned tId = 0;
////	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(fileName); //1:获取图片格式
////	FIBITMAP *dib = FreeImage_Load(fifmt, fileName, 0); //2:加载图片
//	FreeImageIO tio;
//	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileTypeFromHandle(&tio,
//			(fi_handle) handle, 0);
//	FreeImageIO lio;
//	FIBITMAP *dib = FreeImage_LoadFromHandle(fifmt, &lio, (fi_handle) handle,
//			0);
//
//	int format = GL_RGB;
//	//3:转化为rgb
//	if (fifmt == FIF_PNG) {
//		dib = FreeImage_ConvertTo32Bits(dib); //3:转化为rgb 32色
//		format = GL_RGBA;
//	} else {
//		dib = FreeImage_ConvertTo24Bits(dib); //3:转化为rgb 24色
//		format = GL_RGB;
//	}
//	//4:获取数据指针
//	BYTE *pixels = (BYTE*) FreeImage_GetBits(dib);
//
//	int width = FreeImage_GetWidth(dib);
//	int height = FreeImage_GetHeight(dib);
//
//	//得到的数据是BGR格式保存的(bgr -> rgb)
//	if (fifmt == FIF_PNG) {
//		for (int i = 0; i < width * height * 4; i += 4) { //rgba
//			BYTE temp = pixels[i];
//			pixels[i] = pixels[i + 2];
//			pixels[i + 2] = temp;
//		}
//	} else {
//		for (int i = 0; i < width * height * 3; i += 3) { //rgb颜色就
//			BYTE temp = pixels[i];
//			pixels[i] = pixels[i + 2];
//			pixels[i + 2] = temp;
//		}
//	}
//
//	{ //OpenGL加载Byte数据
//	  //生成纹理的索引
//		glGenTextures(1, &tId);
//		//绑定纹理（把纹理句柄绑定成2d纹理）
//		glBindTexture(GL_TEXTURE_2D, tId);
//
//		//纹理的放大缩小滤波（场景远近图片大小）
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//		/****** 将图片的rgb数据上传给opengl.
//		 * @param GLenum target  纹理是几维的
//		 * @param GLint level 制定纹理级别（0最大，9最小）
//		 * @param GLint internalformat 纹理存储格式（显冲用的纹理格式）
//		 * @param GLsizei width 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
//		 * @param GLsizei height 高度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
//		 * @param GLint border 是否有边框
//		 * @param GLenum format 数据的格式（你的图片数据格式）
//		 * @param GLenum type 纹理数据类型
//		 * @param const GLvoid* pixels 纹理数据
//		 */
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
//		GL_UNSIGNED_BYTE, pixels);
//
//	}
//	//5释放
//	FreeImage_Unload(dib);
//	return tId;
//}

GLuint CreateSimpleTextureCubemap() {
	// Generate a texture object
	GLuint texture;
	glGenTextures(1, &texture);

	// Bind the texture object
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	// Load the texture faces
	GLubyte pixels[6][3] = {
	// Face 0 - Red
			{ 255, 0, 0 },
			// Face 1 - Green,
			{ 0, 255, 0 },
			// Face 3 - Blue
			{ 0, 0, 255 },
			// Face 4 - Yellow
			{ 255, 255, 0 },
			// Face 5 - Purple
			{ 255, 0, 255 },
			// Face 6 - White
			{ 255, 255, 255 } };

	for (size_t i = 0; i < 6; i++) {
		glTexImage2D(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0,
		GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &pixels[i]);
	}

	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return texture;
}

//GLuint CreateMipMappedTexture2D()
//{
//    // Texture object handle
//    const size_t width = 256;
//    const size_t height = 256;
//    std::array<GLubyte, width * height * 3> pixels;
//
//    const size_t checkerSize = 8;
//    for (size_t y = 0; y < height; y++)
//    {
//        for (size_t x = 0; x < width; x++)
//        {
//            GLubyte rColor = 0;
//            GLubyte bColor = 0;
//
//            if ((x / checkerSize) % 2 == 0)
//            {
//                rColor = 255 * ((y / checkerSize) % 2);
//                bColor = 255 * (1 - ((y / checkerSize) % 2));
//            }
//            else
//            {
//                bColor = 255 * ((y / checkerSize) % 2);
//                rColor = 255 * (1 - ((y / checkerSize) % 2));
//            }
//
//            pixels[(y * height + x) * 3] = rColor;
//            pixels[(y * height + x) * 3 + 1] = 0;
//            pixels[(y * height + x) * 3 + 2] = bColor;
//        }
//    }
//
//    // Generate a texture object
//    GLuint texture;
//    glGenTextures(1, &texture);
//
//    // Bind the texture object
//    glBindTexture(GL_TEXTURE_2D, texture);
//
//    // Load mipmap level 0
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
//
//    // Generate mipmaps
//    glGenerateMipmap(GL_TEXTURE_2D);
//
//    // Set the filtering mode
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    return texture;
//}
