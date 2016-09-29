/*
 * AndroidResourceProvider.h
 *
 *  Created on: 2016-9-26
 *      Author: xll
 */

#ifndef ANDROIDRESOURCEPROVIDER_H_
#define ANDROIDRESOURCEPROVIDER_H_

#include <android/asset_manager.h>

#include "CEGUI/Base.h"
#include "CEGUI/DefaultResourceProvider.h"

class AndroidResourceProvider: public CEGUI::DefaultResourceProvider {
public:
	AndroidResourceProvider();
	~AndroidResourceProvider(void);

	void loadRawDataContainer(const CEGUI::String& filename, CEGUI::RawDataContainer& output,
			const CEGUI::String& resourceGroup);
	size_t getResourceGroupFileNames(std::vector<CEGUI::String>& out_vec,
			const CEGUI::String& file_pattern, const CEGUI::String& resource_group);
	AAssetManager* gAssetMgr;
};

#endif /* ANDROIDRESOURCEPROVIDER_H_ */
