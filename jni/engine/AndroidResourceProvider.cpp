/*
 * AndroidResourceProvider.cpp
 *
 *  Created on: 2016-9-26
 *      Author: xll
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>

#include "CEGUI/Exceptions.h"
#include "CEGUI/DefaultResourceProvider.h"

#include "utils/texture_utils.h"
#include "engine/AndroidResourceProvider.h"

AndroidResourceProvider::AndroidResourceProvider() {

}
AndroidResourceProvider::~AndroidResourceProvider(void) {

}

void AndroidResourceProvider::loadRawDataContainer(
		const CEGUI::String& filename, CEGUI::RawDataContainer& output,
		const CEGUI::String& resourceGroup) {
//	if (gAssetMgr) {
	const CEGUI::String final_filename(
			getFinalFilename(filename, resourceGroup));
	//Fallback to assetManager
	AAsset* assetFile = AAssetManager_open(gAssetMgr, final_filename.c_str(),
			AASSET_MODE_BUFFER);
	if (!assetFile) {
		LOGI("file not exist: %s", final_filename.c_str());
		//		pthread_mutex_unlock(&mutex_);
		return;
	}
//	CEGUI::uint8* data = (CEGUI::uint8*) AAsset_getBuffer(assetFile);
	int32_t size = AAsset_getLength(assetFile);

	unsigned char* const buffer =
			CEGUI_NEW_ARRAY_PT(unsigned char, size, RawDataContainer);
	int size_read = AAsset_read(assetFile, buffer, size);
	if (size_read != size) {
		CEGUI_DELETE_ARRAY_PT(buffer, unsigned char, size, BufferAllocator);
		LOGI("size_read: %d, size:%d", size_read, size);
		return;
	}
	if (buffer == NULL) {
		AAsset_close(assetFile);
		LOGI("no data found: %s", final_filename.c_str());
		//		pthread_mutex_unlock(&mutex_);
		return;
	}
	output.setSize(size);
	output.setData(buffer);
	LOGI("fileName:%s, len:%d", filename.c_str(), size);
	AAsset_close(assetFile);
	//	pthread_mutex_unlock(&mutex_);
//	} else {
//		if (filename.empty()) {
//			LOGE("fileName cant be empty!");
//			return;
//		}
//		const CEGUI::String final_filename(
//				getFinalFilename(filename, resourceGroup));
//
//		FILE* file = fopen(final_filename.c_str(), "rb");
//
//		if (file == 0) {
//			LOGE("%s does not exist", final_filename.c_str());
//			return;
//		}
//
//		fseek(file, 0, SEEK_END);
//		const size_t size = ftell(file);
//		fseek(file, 0, SEEK_SET);
//
//		unsigned char* const buffer =
//				CEGUI_NEW_ARRAY_PT(unsigned char, size, RawDataContainer);
//
//		const size_t size_read = fread(buffer, sizeof(char), size, file);
//		fclose(file);
//		if (size_read != size) {
//			CEGUI_DELETE_ARRAY_PT(buffer, unsigned char, size, BufferAllocator);
//			LOGE(
//					"A problem occurred while reading file: %s", final_filename.c_str());
//			return;
//		}
//		output.setData(buffer);
//		output.setSize(size);
//	}
}

size_t AndroidResourceProvider::getResourceGroupFileNames(
		std::vector<CEGUI::String>& out_vec, const CEGUI::String& file_pattern,
		const CEGUI::String& resource_group) {
	// look-up resource group name
	ResourceGroupMap::const_iterator iter = d_resourceGroups.find(
			resource_group.empty() ? d_defaultResourceGroup : resource_group);
	// get directory that's set for the resource group
	const CEGUI::String dir_name(
			iter != d_resourceGroups.end() ? (*iter).second : "./");

	size_t entries = 0;

	DIR* dirp;

	if ((dirp = opendir(dir_name.c_str()))) {
		struct dirent* dp;

		while ((dp = readdir(dirp))) {
			const CEGUI::String filename(dir_name + dp->d_name);
			struct stat s;

			if ((stat(filename.c_str(), &s) == 0) && S_ISREG(s.st_mode)
					&& (fnmatch(file_pattern.c_str(), dp->d_name, 0) == 0)) {
				out_vec.push_back(dp->d_name);
				++entries;
			}
		}

		closedir(dirp);
	}

	return entries;
}
