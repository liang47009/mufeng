/*
 * ResourceGroupManager.h
 *
 *  Created on: 2016年9月24日
 *      Author: XIAYUNFENG
 */

#ifndef UTILS_RESOURCEGROUPMANAGER_H_
#define UTILS_RESOURCEGROUPMANAGER_H_

#include <map>
#include <android/asset_manager.h>

typedef std::map<std::string, std::string> ResourceGroupMap;

class ResourceGroupManager {
public:
	ResourceGroupManager(AAssetManager* assetMgr) {
		this->assetMgr = assetMgr;
	}
	~ResourceGroupManager(void) {
	}

	void setResourceGroupDirectory(const std::string& resourceGroup,
			const std::string& directory);

	const std::string& getResourceGroupDirectory(
			const std::string& resourceGroup);

	void clearResourceGroupDirectory(const std::string& resourceGroup);

private:
	ResourceGroupMap mResourceGroup;
	AAssetManager* assetMgr;
};

#endif /* UTILS_RESOURCEGROUPMANAGER_H_ */
