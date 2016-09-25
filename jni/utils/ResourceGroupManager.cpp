/*
 * ResourceGroupManager.cpp
 *
 *  Created on: 2016年9月24日
 *      Author: XIAYUNFENG
 */

#include "utils/ResourceGroupManager.h"

//----------------------------------------------------------------------------//
void ResourceGroupManager::setResourceGroupDirectory(
		const std::string& resourceGroup, const std::string& directory) {
	if (directory.length() == 0)
		return;

#if defined(_WIN32) || defined(__WIN32__)
	// while we rarely use the unportable '\', the user may have
	const String separators("\\/");
#else
	const std::string separators("/");
#endif

	if (std::string::npos == separators.find(directory[directory.length() - 1])) {
		mResourceGroup[resourceGroup] = directory + '/';
	} else {
		mResourceGroup[resourceGroup] = directory;
	}
}

//----------------------------------------------------------------------------//
const std::string& ResourceGroupManager::getResourceGroupDirectory(
		const std::string& resourceGroup) {
	return mResourceGroup[resourceGroup];
}

//----------------------------------------------------------------------------//
void ResourceGroupManager::clearResourceGroupDirectory(
		const std::string& resourceGroup) {
	ResourceGroupMap::iterator iter = mResourceGroup.find(resourceGroup);

	if (iter != mResourceGroup.end()) {
		mResourceGroup.erase(iter);
	}
}
