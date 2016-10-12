

#ifndef __EXTENSIONMARCROS_H__
#define __EXTENSIONMARCROS_H__

#define NS_CC_EXT_BEGIN                     namespace extension {
#define NS_CC_EXT_END                       }
#define USING_NS_CC_EXT                     using namespace extension

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#define CC_EXT_DLL  //__declspec(dllexport)
#else
#define CC_EXT_DLL
#endif

#endif

