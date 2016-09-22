//
//  CAGif.h
//  CrossApp
//
//  Created by qiaoxin on 15-06-01.
//  Copyright (c) 2014 http://www.9miao.com All rights reserved.
//

#include "CAGif.h"
#include "platform/CAFileUtils.h"

NS_CC_BEGIN

static int s_nDataMark = 0;
static unsigned char* s_pData = nullptr;

static int DecodeCallBackProc(GifFileType* gif, GifByteType* bytes, int lenght)
{
    for(int i=0; i<lenght; i++, s_nDataMark++)
    {
        bytes[i] = s_pData[s_nDataMark];
    }
    return lenght;
}

CAGif::CAGif()
:m_fDelay(0.0f)
,m_pData(nullptr)
,m_uDataLenght(0)
{

}

CAGif::~CAGif()
{
    CC_SAFE_DELETE(m_pData);
}

CAGif* CAGif::create(const std::string& filePath)
{
    CAGif* gif = new CAGif();
    if(gif && gif->initWithFilePath(filePath))
    {
        gif->autorelease();
        return gif;
    }
    CC_SAFE_DELETE(gif);
    return NULL;
}

CAGif* CAGif::createWithData(unsigned char* data, unsigned long lenght)
{
    CAGif* gif = new CAGif();
    if(gif && gif->initWithData(data, lenght))
    {
        gif->autorelease();
        return gif;
    }
    CC_SAFE_DELETE(gif);
    return NULL;
}

bool CAGif::initWithFilePath(const std::string& filePath)
{
    unsigned long lenght = 0;
    unsigned char* data = FileUtils::getInstance()->getFileData(filePath, "rb", &lenght);
    return this->initWithData(data, lenght);
}

bool CAGif::initWithData(unsigned char* data, unsigned long lenght)
{
    if(data == nullptr || lenght == 0 || !CAGif::init())
    {
        return false;
    }
    
    s_pData = data;
    
    int error = 0;
    m_pGIF = DGifOpen(nullptr, &DecodeCallBackProc, &error);
    
    if (nullptr == m_pGIF || DGifSlurp(m_pGIF) != GIF_OK)
    {
        DGifCloseFile(m_pGIF);
        m_pGIF = nullptr;
        s_pData = nullptr;
        s_nDataMark = 0;
        return false;
    }
    
    m_uPixelsWide = m_pGIF->SWidth;
    m_uPixelsHigh = m_pGIF->SHeight;
    m_pData = (unsigned char*)malloc(sizeof(unsigned char) * m_uPixelsWide * m_uPixelsHigh * 4);
    
    for (unsigned int i = 0; i < m_uPixelsWide * m_uPixelsHigh; i++)
    {
        *(m_pData + i * 4)     = '\0';
        *(m_pData + i * 4 + 1) = '\0';
        *(m_pData + i * 4 + 2) = '\0';
        *(m_pData + i * 4 + 3) = '\0';
    }

    m_fDelay = getImageDelay(&m_pGIF->SavedImages[0]);
    
    for (int i=0; i<m_pGIF->ImageCount; ++i)
    {
        m_vImages.pushBack(this->getImageWithIndex(i));
    }
    
    s_pData = nullptr;
    s_nDataMark = 0;
    return true;
}

void CAGif::copyLine(unsigned char* dst, const unsigned char* src, const ColorMapObject* cmap, int transparent, int width)
{
    for (; width > 0; width--, src++, dst+=4) {
        if (*src != transparent) {
            const GifColorType& col = cmap->Colors[*src];
            *dst     = col.Red;
            *(dst+1) = col.Green;
            *(dst+2) = col.Blue;
            *(dst+3) = 0xFF;
        }
    }
}

CAImage* CAGif::getImageWithIndex(int index)
{
    index = MIN(index, m_pGIF->ImageCount - 1);
 
    CAColor4B bgColor;
    if (m_pGIF->SColorMap != NULL)
    {
        const GifColorType& col = m_pGIF->SColorMap->Colors[m_pGIF->SBackGroundColor];
        bgColor = ccc4(col.Red, col.Green, col.Blue, 0xFF);
    }
    
    static CAColor4B paintingColor;
    
    const SavedImage* prev = &m_pGIF->SavedImages[index - 1];
    const SavedImage* curr = &m_pGIF->SavedImages[index];

    if (index == 0)
    {
        bool trans;
        int disposal;
        this->getTransparencyAndDisposalMethod(curr, &trans, &disposal);
        if (!trans && m_pGIF->SColorMap != NULL)
        {
            paintingColor = ccc4(0, 0, 0, 0);
        }
    }
    else
    {
        bool curTrans;
        int curDisposal;
        this->getTransparencyAndDisposalMethod(prev, &curTrans, &curDisposal);
        
        bool currTrans;
        int currDisposal;
        this->getTransparencyAndDisposalMethod(curr, &currTrans, &currDisposal);
        
        if (currTrans || !checkIfCover(curr, prev))
        {
            if (curDisposal == 2)
            {
                unsigned char* dst = &m_pData[(prev->ImageDesc.Top * m_uPixelsWide + prev->ImageDesc.Left) * 4];
                GifWord copyWidth = prev->ImageDesc.Width;
                
                if (prev->ImageDesc.Left + copyWidth > m_uPixelsWide)
                {
                    copyWidth = m_uPixelsWide - prev->ImageDesc.Left;
                }
                
                GifWord copyHeight = prev->ImageDesc.Height;
                if (prev->ImageDesc.Top + copyHeight > m_uPixelsHigh)
                {
                    copyHeight = m_uPixelsHigh - prev->ImageDesc.Top;
                }
                
                for (; copyHeight > 0; copyHeight--)
                {
                    for(int wIndex = 0; wIndex < m_uPixelsWide; wIndex++, dst+=4)
                    {
                        *dst     = paintingColor.r;
                        *(dst+1) = paintingColor.g;
                        *(dst+2) = paintingColor.b;
                        *(dst+3) = paintingColor.a;
                    }
                }
            }
            else if (curDisposal == 3)
            {
                //swap;
            }
        }
    }
    
    {
        int transparent = -1;
        for (int i = 0; i < curr->ExtensionBlockCount; ++i)
        {
            ExtensionBlock* eb = curr->ExtensionBlocks + i;
            if (eb->Function == GRAPHICS_EXT_FUNC_CODE &&
                eb->ByteCount == 4)
            {
                bool has_transparency = ((eb->Bytes[0] & 1) == 1);
                if (has_transparency)
                {
                    transparent = (unsigned char)eb->Bytes[3];
                }
            }
        }
        
        if (curr->ImageDesc.ColorMap != NULL)
        {
            m_pGIF->SColorMap = curr->ImageDesc.ColorMap;
        }
        
        if (m_pGIF->SColorMap && m_pGIF->SColorMap->ColorCount == (1 << m_pGIF->SColorMap->BitsPerPixel))
        {
            unsigned char* src = (unsigned char*)curr->RasterBits;
            unsigned char* dst = &m_pData[(curr->ImageDesc.Top * m_uPixelsWide + curr->ImageDesc.Left) * 4];
            
            GifWord copyWidth = curr->ImageDesc.Width;
            if (curr->ImageDesc.Left + copyWidth > m_uPixelsWide)
            {
                copyWidth = m_uPixelsWide - curr->ImageDesc.Left;
            }
            
            GifWord copyHeight = curr->ImageDesc.Height;
            if (curr->ImageDesc.Top + copyHeight > m_uPixelsHigh)
            {
                copyHeight = m_uPixelsHigh - curr->ImageDesc.Top;
            }
            
            for (; copyHeight > 0; copyHeight--)
            {
                copyLine(dst, src, m_pGIF->SColorMap, transparent, copyWidth);
                src += curr->ImageDesc.Width;
                dst += m_uPixelsWide*4;
            }
        }
    }
    CAImage* image = new CAImage();
    image->initWithRawData(m_pData, CAImage::PixelFormat_RGBA8888, m_uPixelsWide, m_uPixelsHigh);
    image->autorelease();
    return image;
}

void CAGif::getTransparencyAndDisposalMethod(const SavedImage* frame, bool* trans, int* disposal)
{
    *trans = false;
    *disposal = 0;
    for (int i = 0; i < frame->ExtensionBlockCount; ++i)
    {
        ExtensionBlock* eb = frame->ExtensionBlocks + i;
        if (eb->Function == GRAPHICS_EXT_FUNC_CODE &&
            eb->ByteCount == 4)
        {
            *trans = ((eb->Bytes[0] & 1) == 1);
            *disposal = ((eb->Bytes[0] >> 2) & 7);
        }
    }
}

bool CAGif::checkIfCover(const SavedImage* target, const SavedImage* covered)
{
    if (target->ImageDesc.Left <= covered->ImageDesc.Left
        && covered->ImageDesc.Left + covered->ImageDesc.Width <=
        target->ImageDesc.Left + target->ImageDesc.Width
        && target->ImageDesc.Top <= covered->ImageDesc.Top
        && covered->ImageDesc.Top + covered->ImageDesc.Height <=
        target->ImageDesc.Top + target->ImageDesc.Height) {
        return true;
    }
    return false;
}

bool CAGif::checkIfWillBeCleared(const SavedImage* frame)
{
    for (int i = 0; i < frame->ExtensionBlockCount; ++i)
    {
        ExtensionBlock* eb = frame->ExtensionBlocks + i;
        if (eb->Function == GRAPHICS_EXT_FUNC_CODE &&
            eb->ByteCount == 4)
        {
            // check disposal method
            int disposal = ((eb->Bytes[0] >> 2) & 7);
            if (disposal == 2 || disposal == 3)
            {
                return true;
            }
        }
    }
    return false;
}

float CAGif::getImageDelay(const SavedImage* image)
{
    float duration = 0;
    for (int j = 0; j < image->ExtensionBlockCount; j++)
    {
        if (image->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE)
        {
            int size = image->ExtensionBlocks[j].ByteCount;
            //assert(size >= 4);
            if(size < 4) break;
            const uint8_t* b = (const uint8_t*)image->ExtensionBlocks[j].Bytes;
            duration =  ((b[2] << 8) | b[1]) * 10;
            break;
        }
    }
    duration = duration <= 100 ? 100 : duration;
    return duration;
}


NS_CC_END
