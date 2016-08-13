
#ifndef __MISC_NODE_PROGRESS_TIMER_H__
#define __MISC_NODE_PROGRESS_TIMER_H__

#include "CGSprite.h"

NS_CC_BEGIN

class CC_DLL CGProgressTimer : public CGNode
{
public:

    enum class Type
    {
        RADIAL,/** Radial Counter-Clockwise. */
        BAR,/** Bar. */
    };

    CGProgressTimer();

    ~CGProgressTimer(void);
    
    inline Type getType(void) { return m_eType; }
    
    inline float getPercentage(void) {return m_fPercentage; }
    
    inline CGSprite* getSprite(void) { return m_pSprite; }
    
    bool initWithSprite(CGSprite* sp);
    
    void setPercentage(float fPercentage);
    void setSprite(CGSprite* pSprite);
    void setType(Type type);

    void setReverseProgress(bool reverse);
    
    virtual void draw(void);
    void setAnchorPoint(DPoint anchorPoint);
    
    virtual void setColor(const CAColor4B& color);
    virtual const CAColor4B& getColor() const;
    virtual float getAlpha() const;
    virtual void setAlpha(float alpha);
    
    inline bool isReverseDirection() { return m_bReverseDirection; };
    inline void setReverseDirection(bool value) { m_bReverseDirection = value; };
    
public:

    static CGProgressTimer* create(CGSprite* sp);
protected:
    
    ccTex2F textureCoordFromAlphaPoint(DPoint alpha);
    DPoint vertexFromAlphaPoint(DPoint alpha);
    void updateProgress(void);
    void updateBar(void);
    void updateRadial(void);
    void updateColor(void);
    DPoint boundaryTexCoord(char index);
    
protected:
    
    Type m_eType;
    float m_fPercentage;
    CGSprite* m_pSprite;
    int m_nVertexDataCount;
    ccV2F_C4B_T2F *m_pVertexData;

    CC_PROPERTY(DPoint, m_tMidpoint, Midpoint);

    CC_SYNTHESIZE(DPoint, m_tBarChangeRate, BarChangeRate);
    
    bool m_bReverseDirection;
};

// end of misc_nodes group
/// @}

NS_CC_END

#endif //__MISC_NODE_CCPROGRESS_TIMER_H__
