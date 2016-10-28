//
//  CAGif.h
//  CrossApp
//
//  Created by qiaoxin on 15-06-01.
//  Copyright (c) 2014 http://www.9miao.com All rights reserved.
//

#ifndef __CAGIF_H__
#define __CAGIF_H__

#include "CAImage.h"
#include "basics/CASTLContainer.h"
#include "gif_lib/gif_lib.h"

NS_CC_BEGIN

class CC_DLL CAGif : public CAObject
{
public:
    
    CAGif();
    
    virtual ~CAGif();
    
    static CAGif* create(const std::string& filePath);
    
    CC_DEPRECATED_ATTRIBUTE static CAGif* createWithFilePath(const std::string& filePath) { return CAGif::create(filePath); }
    
    static CAGif* createWithData(unsigned char* data, unsigned long lenght);

    bool initWithFilePath(const std::string& filePath);
    
    bool initWithData(unsigned char* data, unsigned long lenght);
    
    void next();
    
    CC_SYNTHESIZE_READONLY(CAImage*, m_pImage, Image);
    
    CC_SYNTHESIZE_READONLY(float, m_fDelay, Delay);
    
    CC_SYNTHESIZE_READONLY(unsigned int, m_uPixelsWide, PixelsWide);
    
    CC_SYNTHESIZE_READONLY(unsigned int, m_uPixelsHigh, PixelsHigh);

    CC_SYNTHESIZE(int, m_iImageIndex, ImageIndex);
    
    CC_SYNTHESIZE(int, m_iImageCount, ImageCount);
    
protected:
    
    void getTransparencyAndDisposalMethod(const SavedImage* frame, bool* trans, int* disposal);
    
    bool checkIfCover(const SavedImage* target, const SavedImage* covered);
    
    bool checkIfWillBeCleared(const SavedImage* frame);
    
    void copyLine(unsigned char* dst, const unsigned char* src, const ColorMapObject* cmap, int transparent, int width);
    
    float getImageDelay(const SavedImage* image);

    CAImage* getImageWithIndex(int index);
    
protected:

    unsigned char*  m_pData;
    
    unsigned long   m_uDataLenght;
    
    GifFileType*    m_pGIF;
};

NS_CC_END

#endif //__CAGIF_H__

