#ifndef __CGSpriteBatchNode_H__
#define __CGSpriteBatchNode_H__

#include "CGNode.h"
#include "dispatcher/CAProtocols.h"
#include "images/CAImageCache.h"
#include "ccMacros.h"

NS_CC_BEGIN

#define kDefaultSpriteBatchCapacity   29

class CGSprite;

class CC_DLL CGSpriteBatchNode : public CGNode, public CAImageProtocol
{
public:

    CGSpriteBatchNode();

    virtual ~CGSpriteBatchNode();

    inline CAImageAtlas* getImageAtlas(void) { return m_pobImageAtlas; }
    
    inline void setImageAtlas(CAImageAtlas* ImageAtlas) 
    { 
        if (ImageAtlas != m_pobImageAtlas)
        {
            CC_SAFE_RETAIN(ImageAtlas);
            CC_SAFE_RELEASE(m_pobImageAtlas);
            m_pobImageAtlas = ImageAtlas;
        }
    }

    inline const CAVector<CGSprite*>& getDescendants(void) { return m_obDescendants; }

    static CGSpriteBatchNode* createWithImage(CAImage* image, unsigned int capacity);
    
    static CGSpriteBatchNode* createWithImage(CAImage* image)
    {
        return CGSpriteBatchNode::createWithImage(image, kDefaultSpriteBatchCapacity);
    }

    static CGSpriteBatchNode* create(const std::string& fileImage, unsigned int capacity);
    
    static CGSpriteBatchNode* create(const std::string& fileImage)
    {
        return CGSpriteBatchNode::create(fileImage, kDefaultSpriteBatchCapacity);
    }

    bool initWithImage(CAImage *image, unsigned int capacity);

    bool initWithFile(const std::string& fileImage, unsigned int capacity);
    
    bool init();

    void increaseAtlasCapacity();

    void removeChildAtIndex(unsigned int index, bool doCleanup);

    void appendChild(CGSprite* sprite);
    void removeSpriteFromAtlas(CGSprite *sprite);

    ssize_t rebuildIndexInOrder(CGSprite *parent, ssize_t index);
    ssize_t highestAtlasIndexInChild(CGSprite *sprite);
    ssize_t lowestAtlasIndexInChild(CGSprite *sprite);
    ssize_t atlasIndexForChild(CGSprite *sprite, int z);

    void reorderBatch(bool reorder);
    
    virtual CAImage* getImage(void);
    
    virtual void setImage(CAImage *image);
    
    virtual void setBlendFunc(BlendFunc blendFunc);

    virtual BlendFunc getBlendFunc(void);

    virtual void visit(void);
    
    virtual void insertChild(CGNode* child, int z);

    virtual void addChild(CGNode* child);
    
    virtual void removeChild(CGNode* child);

    virtual void removeAllChildren();
    
    virtual void sortAllChildren();
    
    virtual void draw(void);

protected:

    void insertQuadFromSprite(CGSprite *sprite, ssize_t index);

    void updateQuadFromSprite(CGSprite *sprite, ssize_t index);

    CGSpriteBatchNode * addSpriteWithoutQuad(CGSprite*child, ssize_t z, int aTag);

private:
    void updateAtlasIndex(CGSprite* sprite, ssize_t* curIndex);
    
    void swap(ssize_t oldIndex, ssize_t newIndex);
    
    void updateBlendFunc();

protected:
    
    CAImageAtlas *m_pobImageAtlas;
    
    BlendFunc m_blendFunc;
    
    CAVector<CGSprite*> m_obDescendants;
};

// end of sprite_nodes group
/// @}

NS_CC_END

#endif // __CGSpriteBatchNode_H__
