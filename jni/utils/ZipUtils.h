/*
 * ZipUtils.h
 *
 *  Created on: 2016年9月24日
 *      Author: XIAYUNFENG
 */

#ifndef UTILS_ZIPUTILS_H_
#define UTILS_ZIPUTILS_H_

#include <fnmatch.h>
#define FNMATCH(p, s)    fnmatch(p, s, FNM_PATHNAME)

#include <vector>
#include <string>
#include <map>
#include "CEGUI/DataContainer.h"

namespace ZipUtils {

void setArchive(const std::string& archive);
void setLoadLocal(bool load = true);
std::string getFinalFilename(const std::string& filename,
		const std::string& resourceGroup);
void loadRawDataContainer(const std::string& filename, CEGUI::RawDataContainer& output,
		const std::string& resourceGroup);
size_t getResourceGroupFileNames(std::vector<std::string>& out_vec,
		const std::string& file_pattern, const std::string& resource_group);
bool doesFileExist(const std::string& filename);
void openArchive();
void closeArchive();
void unloadRawDataContainer(CEGUI::RawDataContainer& data);
void setResourceGroupDirectory(const std::string& resourceGroup,
		const std::string& directory);
const std::string& getResourceGroupDirectory(const std::string& resourceGroup);
void clearResourceGroupDirectory(const std::string& resourceGroup);

typedef std::map<std::string, std::string> ResourceGroupMap;
ResourceGroupMap d_resourceGroups;
struct Impl;
Impl* d_pimpl;
}

#endif /* UTILS_ZIPUTILS_H_ */
