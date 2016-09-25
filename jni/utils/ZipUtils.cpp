/*
 * ZipUtils.cpp
 *
 *  Created on: 2016年9月24日
 *      Author: XIAYUNFENG
 */

#include <fstream>
#include "utils/minizip/unzip.h"
#include "utils/ZipUtils.h"
#include "utils/texture_utils.h"

struct ZipUtils::Impl {
	Impl(const bool loadLocal) :
			d_zfile(0), d_loadLocal(loadLocal) {
	}

	unzFile d_zfile;
	std::string d_archive;
	bool d_loadLocal;
};

//----------------------------------------------------------------------------//
// Helper function that matches names against the pattern.
bool nameMatchesPattern(const std::string& name, const std::string& pattern) {
	return !FNMATCH(pattern.c_str(), name.c_str());
}

//----------------------------------------------------------------------------//
bool ZipUtils::doesFileExist(const std::string& filename) {
	std::ifstream dataFile(filename.c_str(), std::ios::binary | std::ios::ate);
	if (dataFile) {
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------//
void ZipUtils::openArchive() {
	d_pimpl->d_zfile = unzOpen(d_pimpl->d_archive.c_str());

	if (d_pimpl->d_zfile == 0) {
		LOGE("'%s' does not exist", d_pimpl->d_archive.c_str());
	}
}

//----------------------------------------------------------------------------//
void ZipUtils::closeArchive() {
	if (unzClose(d_pimpl->d_zfile) != Z_OK) {
		// do not throw an exception as this method is called from the destructor!
		LOGE("MinizipResourceProvider::closeArchive: '%s' error upon closing",
				d_pimpl->d_archive.c_str());
	}
	d_pimpl->d_zfile = 0;
}

std::string ZipUtils::getFinalFilename(const std::string& filename,
		const std::string& resourceGroup) {
	std::string final_filename;

	// look up resource group directory
	ResourceGroupMap::const_iterator iter = d_resourceGroups.find(
			resourceGroup);

	// if there was an entry for this group, use it's directory as the
	// first part of the filename
	if (iter != d_resourceGroups.end()) {
		final_filename = (*iter).second;
	}

	// append the filename part that we were passed
	final_filename += filename;

	// return result
	return final_filename;
}

//----------------------------------------------------------------------------//
void ZipUtils::loadRawDataContainer(const std::string& filename,
		RawDataContainer& output, const std::string& resourceGroup) {
	const std::string final_filename = getFinalFilename(filename,
			resourceGroup);

//	if (d_pimpl->d_loadLocal && doesFileExist(final_filename)) {
//		DefaultResourceProvider::loadRawDataContainer(filename, output,
//				resourceGroup);
//		return;
//	}

	if (d_pimpl->d_zfile == 0) {
		LOGE("'%s' cannot be "
				"loaded because the archive has not been set",
				final_filename.c_str());
		return;
	}

	if (unzLocateFile(d_pimpl->d_zfile, final_filename.c_str(), 0) != UNZ_OK) {
		LOGE("'%s' does not exist", final_filename.c_str());
		return;
	}

	unz_file_info file_info;

	if (unzGetCurrentFileInfo(d_pimpl->d_zfile, &file_info, 0, 0, 0, 0, 0,
			0) != UNZ_OK) {
		LOGE("'%s' error reading file header", final_filename.c_str());
		return;
	}

	if (unzOpenCurrentFile(d_pimpl->d_zfile) != Z_OK) {
		LOGE("'%s' error opening file", final_filename.c_str());
		return;
	}

	ulong size = file_info.uncompressed_size;
	uint8_t* buffer = NEW_ARRAY_PT(uint8_t, size, RawDataContainer);

	if (unzReadCurrentFile(d_pimpl->d_zfile, buffer, size) < 0) {
		LOGE("'%s' error reading file", final_filename.c_str());
		return;
	}

	if (unzCloseCurrentFile(d_pimpl->d_zfile) != UNZ_OK) {
		LOGE("'%s' error validating file", final_filename.c_str());
		return;
	}

	output.setData(buffer);
	output.setSize(size);
}

//----------------------------------------------------------------------------//
size_t ZipUtils::getResourceGroupFileNames(std::vector<std::string>& out_vec,
		const std::string& file_pattern, const std::string& resource_group) {
	// look-up resource group name
	ResourceGroupMap::const_iterator iter = d_resourceGroups.find(
			resource_group);
	// get directory that's set for the resource group
	const std::string dir_name(
			iter != d_resourceGroups.end() ? (*iter).second : "");

	size_t entries = 0;

	// get local (non zip) matches if local mode is set.
//	if (d_pimpl->d_loadLocal)
//		entries += DefaultResourceProvider::getResourceGroupFileNames(out_vec,
//				file_pattern, resource_group);

	// exit now if no zip file is loaded
	if (!d_pimpl->d_zfile)
		return entries;

	char current_name[1024];
	unz_file_info file_info;

	if (unzGoToFirstFile(d_pimpl->d_zfile) != UNZ_OK) {
		LOGE("unzGoToFirstFile failed, skipping zip file scan.");
		return entries;
	}

	do {
		if (unzGetCurrentFileInfo(d_pimpl->d_zfile, &file_info, current_name,
				1024, 0, 0, 0, 0) != UNZ_OK) {
			LOGE("unzGetCurrentFileInfo failed, terminating scan.");
			return entries;
		}

		// skip this file if it does not match the pattern.
		if (!nameMatchesPattern(current_name, dir_name + file_pattern))
			continue;

		// strip the resource directory name and append the matched file
		out_vec.push_back(std::string(current_name).substr(dir_name.length()));
		++entries;
	} while (unzGoToNextFile(d_pimpl->d_zfile) == UNZ_OK);

	return entries;
}

//----------------------------------------------------------------------------//
void ZipUtils::unloadRawDataContainer(RawDataContainer& data) {
	data.release();
}

//----------------------------------------------------------------------------//
void ZipUtils::setResourceGroupDirectory(const std::string& resourceGroup,
		const std::string& directory) {
	if (directory.length() == 0)
		return;

#if defined(_WIN32) || defined(__WIN32__)
	// while we rarely use the unportable '\', the user may have
	const String separators("\\/");
#else
	const std::string separators("/");
#endif

	if (std::string::npos == separators.find(directory[directory.length() - 1])) {
		d_resourceGroups[resourceGroup] = directory + '/';
	} else {
		d_resourceGroups[resourceGroup] = directory;
	}
}

//----------------------------------------------------------------------------//
const std::string& ZipUtils::getResourceGroupDirectory(
		const std::string& resourceGroup) {
	return d_resourceGroups[resourceGroup];
}

//----------------------------------------------------------------------------//
void ZipUtils::clearResourceGroupDirectory(const std::string& resourceGroup) {
	ResourceGroupMap::iterator iter = d_resourceGroups.find(resourceGroup);

	if (iter != d_resourceGroups.end())
		d_resourceGroups.erase(iter);
}
//----------------------------------------------------------------------------//
void ZipUtils::setLoadLocal(bool load) {
	d_pimpl->d_loadLocal = load;
}
