
#ifndef __CC_FILEUTILS_WIN32_H__
#define __CC_FILEUTILS_WIN32_H__

#include "platform/CAFileUtils.h"
#include "platform/CCPlatformMacros.h"
#include "ccTypes.h"
#include "ccTypeInfo.h"
#include <string>
#include <vector>

NS_CC_BEGIN

/**
 * @addtogroup platform
 * @{
 */

//! @brief  Helper class to handle file operations
class CC_DLL CCFileUtilsWin32 : public FileUtils
{
    friend class FileUtils;
    CCFileUtilsWin32();
public:
    /* override funtions */
    bool init();
    virtual std::string getWritablePath();
    virtual bool isFileExist(const std::string& strFilePath);
    virtual bool isAbsolutePath(const std::string& strPath);
	virtual void addSearchPath(const char* path);
	virtual void removeSearchPath(const char *path);

	/* transfer utf8 to gbk */
	std::string utf8Togbk(const char *src);

	virtual std::string fullPathForFilename(const char* pszFileName);

	protected:
    /**
     *  Gets full path for filename, resolution directory and search path.
     *
     *  @param filename The file name.
     *  @param resolutionDirectory The resolution directory.
     *  @param searchPath The search path.
     *  @return The full path of the file. It will return an empty string if the full path of the file doesn't exist.
     */
    virtual std::string getPathForFilename(const std::string& filename, const std::string& resolutionDirectory, const std::string& searchPath);
};

// end of platform group
/// @}

NS_CC_END

#endif    // __CC_FILEUTILS_WIN32_H__

