
#include "CGSpriteBatchNode.h"
#include "ccConfig.h"
#include "CGSprite.h"
#include "view/CADrawingPrimitives.h"
#include "support/CAPointExtension.h"
#include "shaders/CAShaderCache.h"
#include "shaders/CAGLProgram.h"
#include "shaders/ccGLStateCache.h"
#include "basics/CAApplication.h"
#include "math/TransformUtils.h"
#include "kazmath/GL/matrix.h"

NS_CC_BEGIN


CGSpriteBatchNode* CGSpriteBatchNode::createWithImage(CAImage* image, unsigned int capacity/* = kDefaultSpriteBatchCapacity*/)
{
    CGSpriteBatchNode *batchNode = new CGSpriteBatchNode();
    batchNode->initWithImage(image, capacity);
    batchNode->autorelease();

    return batchNode;
}

/*
* creation with File Image
*/

CGSpriteBatchNode* CGSpriteBatchNode::create(const std::string& fileImage, unsigned int capacity/* = kDefaultSpriteBatchCapacity*/)
{
    CGSpriteBatchNode *batchNode = new CGSpriteBatchNode();
    batchNode->initWithFile(fileImage, capacity);
    batchNode->autorelease();

    return batchNode;
}

/*
* init with CAImage
*/
bool CGSpriteBatchNode::initWithImage(CAImage *image, unsigned int capacity)
{
    m_blendFunc.src = CC_BLEND_SRC;
    m_blendFunc.dst = CC_BLEND_DST;
    m_pobImageAtlas = new CAImageAtlas();

    if (0 == capacity)
    {
        capacity = kDefaultSpriteBatchCapacity;
    }
    
    m_pobImageAtlas->initWithImage(image, capacity);

    updateBlendFunc();

    setShaderProgram(CAShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));
    return true;
}

bool CGSpriteBatchNode::init()
{
    CAImage * image = new CAImage();
    image->autorelease();
    return this->initWithImage(image, 0);
}

/*
* init with FileImage
*/
bool CGSpriteBatchNode::initWithFile(const std::string& fileImage, unsigned int capacity)
{
    return initWithImage(CAImage::create(fileImage), capacity);
}

CGSpriteBatchNode::CGSpriteBatchNode()
: m_pobImageAtlas(NULL)
{
}

CGSpriteBatchNode::~CGSpriteBatchNode()
{
    CC_SAFE_RELEASE(m_pobImageAtlas);
}

// override visit
// don't call visit on it's children
void CGSpriteBatchNode::visit(void)
{
    CC_PROFILER_START_CATEGORY(kCCProfilerCategoryBatchSprite, "CGSpriteBatchNode - visit");

    CC_RETURN_IF(!m_bVisible);
    
    kmGLPushMatrix();
    
    this->transform();
    this->sortAllChildren();
    this->draw();

    if (m_pCAView)
    {
        m_pCAView->visit();
    }
    
    kmGLPopMatrix();
    this->setOrderOfArrival(0);

    CC_PROFILER_STOP_CATEGORY(kCCProfilerCategoryBatchSprite, "CGSpriteBatchNode - visit");
}

void CGSpriteBatchNode::insertChild(CGNode* child, int z)
{
    if (CGSprite* sprite = dynamic_cast<CGSprite*>(child))
    {
        CGNode::insertChild(sprite, z);
        
        this->appendChild(sprite);
    }
}

void CGSpriteBatchNode::addChild(CGNode* child)
{
    this->insertChild(child, child->getZOrder());
}

void CGSpriteBatchNode::removeChild(CGNode* child)
{
    if (CGSprite* sprite = dynamic_cast<CGSprite*>(child))
    {
        this->removeSpriteFromAtlas(sprite);
        
        CGNode::removeChild(sprite);
    }
}

void CGSpriteBatchNode::removeAllChildren()
{
    CGNode::removeAllChildren();
    m_pobImageAtlas->removeAllQuads();
    m_obDescendants.clear();
}

void CGSpriteBatchNode::sortAllChildren()
{
    if (m_bReorderChildDirty)
    {
        std::sort(m_obChildren.begin(), m_obChildren.end(), compareChildrenZOrder);
        
        if (!m_obChildren.empty())
        {
            CAVector<CGNode*>::iterator itr;
            
            for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
                if(m_bRunning) (*itr)->sortAllChildren();
            
            int index=0;
            for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
                updateAtlasIndex(*itr, &index);
        }
        m_bReorderChildDirty=false;
    }
}

void CGSpriteBatchNode::updateAtlasIndex(CGNode* node, int* curIndex)
{
    CGSprite* sprite = (CGSprite*)node;
    const CAVector<CGNode*> pVector = node->getChildren();
    unsigned int count = (unsigned int)pVector.size();
    
    int oldIndex = 0;
    
    if( count == 0 )
    {
        oldIndex = sprite->getAtlasIndex();
        sprite->setAtlasIndex(*curIndex);
        sprite->setOrderOfArrival(0);
        if (oldIndex != *curIndex)
        {
            swap(oldIndex, *curIndex);
        }
        (*curIndex)++;
    }
    else
    {
        bool needNewIndex = true;
        
        if (pVector.front()->getZOrder() >= 0)
        {
            oldIndex = sprite->getAtlasIndex();
            sprite->setAtlasIndex(*curIndex);
            sprite->setOrderOfArrival(0);
            if (oldIndex != *curIndex)
            {
                swap(oldIndex, *curIndex);
            }
            (*curIndex)++;
            
            needNewIndex = false;
        }
        
        CAVector<CGNode*>::const_iterator itr;
        for (itr=pVector.begin(); itr!=pVector.end(); itr++)
        {
            CGNode* child = *itr;
            if (needNewIndex && child->getZOrder() >= 0)
            {
                oldIndex = sprite->getAtlasIndex();
                sprite->setAtlasIndex(*curIndex);
                sprite->setOrderOfArrival(0);
                if (oldIndex != *curIndex)
                {
                    this->swap(oldIndex, *curIndex);
                }
                (*curIndex)++;
                needNewIndex = false;
                
            }
            
            updateAtlasIndex(child, curIndex);
        }
        
        if (needNewIndex)
        {
            oldIndex = sprite->getAtlasIndex();
            sprite->setAtlasIndex(*curIndex);
            sprite->setOrderOfArrival(0);
            if (oldIndex!=*curIndex)
            {
                swap(oldIndex, *curIndex);
            }
            (*curIndex)++;
        }
    }
}

void CGSpriteBatchNode::swap(int oldIndex, int newIndex)
{
    ccV3F_C4B_T2F_Quad* quads = m_pobImageAtlas->getQuads();
    
    ccV3F_C4B_T2F_Quad tempItemQuad = quads[oldIndex];

    m_obDescendants.at(newIndex)->setAtlasIndex(oldIndex);
    m_obDescendants.swap(oldIndex, newIndex);
    quads[oldIndex]=quads[newIndex];
    quads[newIndex]=tempItemQuad;
}

void CGSpriteBatchNode::reorderBatch(bool reorder)
{
    m_bReorderChildDirty=reorder;
}

void CGSpriteBatchNode::draw(void)
{
    CC_PROFILER_START("CGSpriteBatchNode - draw");

    if( m_pobImageAtlas->getTotalQuads() == 0 )
    {
        return;
    }

    CAIMAGE_DRAW_SETUP();

    for (auto& var : m_obChildren)
    {
        CGSprite* sprite = (CGSprite*)var;
        sprite->updateTransform();
    }
    
    ccGLBlendFunc( m_blendFunc.src, m_blendFunc.dst );

    m_pobImageAtlas->drawQuads();

    CC_PROFILER_STOP("CGSpriteBatchNode - draw");
}

void CGSpriteBatchNode::increaseAtlasCapacity(void)
{
    unsigned int quantity = (m_pobImageAtlas->getCapacity() + 1) * 4 / 3;

    CCLOG("CrossApp:CGSpriteBatchNode: resizing TextureAtlas capacity from [%lu] to [%lu].",
        (long)m_pobImageAtlas->getCapacity(),
        (long)quantity);

    if (! m_pobImageAtlas->resizeCapacity(quantity))
    {
        // serious problems
        CCLOG("CrossApp: WARNING: Not enough memory to resize the atlas");
        CCAssert(false, "Not enough memory to resize the atlas");
    }
}

unsigned int CGSpriteBatchNode::rebuildIndexInOrder(CGSprite *pobParent, unsigned int uIndex)
{
    const CAVector<CGNode*> pVector = pobParent->getChildren();
    
    CAVector<CGNode*>::const_iterator itr=pVector.begin();
    while (itr!=pVector.end())
    {
        CC_BREAK_IF((*itr)->getZOrder() >= 0);
        CGSprite* sprite = (CGSprite*)(*itr);
        uIndex = rebuildIndexInOrder(sprite, uIndex);
        itr++;
    }
    
    if (!pobParent->isEqual(this))
    {
        pobParent->setAtlasIndex(uIndex);
        uIndex++;
    }
    
    while (itr!=pVector.end())
    {
        CGSprite* sprite = (CGSprite*)(*itr);
        uIndex = rebuildIndexInOrder(sprite, uIndex);
    }
    
    return uIndex;
}

unsigned int CGSpriteBatchNode::highestAtlasIndexInChild(CGSprite *pSprite)
{
    const CAVector<CGNode*>& children = pSprite->getChildren();

    if (children.empty())
    {
        return pSprite->getAtlasIndex();
    }
    else
    {
        CGSprite* sprite = (CGSprite*)(children.back());
        return highestAtlasIndexInChild(sprite);
    }
}

unsigned int CGSpriteBatchNode::lowestAtlasIndexInChild(CGSprite *pSprite)
{
    const CAVector<CGNode*>& children = pSprite->getChildren();

    if (children.empty())
    {
        return pSprite->getAtlasIndex();
    }
    else
    {
        CGSprite* sprite = (CGSprite*)(children.back());
        return lowestAtlasIndexInChild(sprite);
    }
}

unsigned int CGSpriteBatchNode::atlasIndexForChild(CGSprite *pobSprite, int nZ)
{
    const CAVector<CGNode*>& pBrothers = pobSprite->getParent()->getChildren();
    unsigned int uChildIndex = pBrothers.getIndex(pobSprite);

    bool bIgnoreParent = (CGSpriteBatchNode*)(pobSprite->getParent()) == this;
    CGSprite *pPrevious = NULL;
    if (uChildIndex > 0 && uChildIndex < UINT_MAX)
    {
        pPrevious = (CGSprite*)( pBrothers.at(uChildIndex - 1) );
    }

    if (bIgnoreParent)
    {
        if (uChildIndex == 0)
        {
            return 0;
        }

        return highestAtlasIndexInChild(pPrevious) + 1;
    }

    if (uChildIndex == 0)
    {
        CGSprite *p = (CGSprite*)(pobSprite->getParent());

        if (nZ < 0)
        {
            return p->getAtlasIndex();
        }
        else
        {
            return p->getAtlasIndex() + 1;
        }
    }
    else
    {
        if ((pPrevious->getZOrder() < 0 && nZ < 0) || (pPrevious->getZOrder() >= 0 && nZ >= 0))
        {
            return highestAtlasIndexInChild(pPrevious) + 1;
        }

        CGSprite *p = (CGSprite*)(pobSprite->getParent());
        return p->getAtlasIndex() + 1;
    }

    CCAssert(0, "should not run here");
    return 0;
}

void CGSpriteBatchNode::appendChild(CGSprite *pSprite, unsigned int uIndex)
{
    pSprite->setBatchNode(this);
    pSprite->setAtlasIndex(uIndex);
    pSprite->setDirty(true);

    if(m_pobImageAtlas->getTotalQuads() == m_pobImageAtlas->getCapacity())
    {
        increaseAtlasCapacity();
    }

    ccV3F_C4B_T2F_Quad quad = pSprite->getQuad();
    m_pobImageAtlas->insertQuad(&quad, uIndex);
    m_obDescendants.insert(uIndex, pSprite);

    if (m_obDescendants.size() > uIndex + 1)
    {
        for (CAVector<CGSprite*>::const_iterator itr=m_obDescendants.begin() + uIndex + 1;
             itr!=m_obDescendants.end();
             itr++)
        {
            (*itr)->setAtlasIndex(pSprite->getAtlasIndex() + 1);
        }
    }

    for (CAVector<CGNode*>::const_iterator itr=pSprite->getChildren().begin();
         itr!=pSprite->getChildren().end();
         itr++)
    {
        CGSprite *sprite = (CGSprite*)(*itr);
        unsigned int idx = atlasIndexForChild(sprite, sprite->getZOrder());
        this->appendChild(sprite, idx);
    }
}

void CGSpriteBatchNode::appendChild(CGSprite* sprite)
{
    m_bReorderChildDirty=true;
    sprite->setBatchNode(this);
    sprite->setDirty(true);

    if(m_pobImageAtlas->getTotalQuads() == m_pobImageAtlas->getCapacity())
    {
        increaseAtlasCapacity();
    }

    m_obDescendants.pushBack(sprite);

    unsigned int index = (unsigned int)m_obDescendants.size() - 1;
    sprite->setAtlasIndex(index);

    ccV3F_C4B_T2F_Quad quad = sprite->getQuad();
    m_pobImageAtlas->insertQuad(&quad, index);
    
    CAVector<CGNode*>::const_iterator itr;
    for (itr=sprite->getChildren().begin(); itr!=sprite->getChildren().end(); itr++)
    {
        CGSprite* s = (CGSprite*)(*itr);
        this->appendChild(s);
    }
}

void CGSpriteBatchNode::removeSpriteFromAtlas(CGSprite *pobSprite)
{
    m_pobImageAtlas->removeQuadAtIndex(pobSprite->getAtlasIndex());

    pobSprite->setBatchNode(NULL);

    unsigned int uIndex = m_obDescendants.getIndex(pobSprite);
    if (uIndex != UINT_MAX)
    {
        m_obDescendants.erase(uIndex);
        unsigned int count = (unsigned int)m_obDescendants.size();
        
        for(; uIndex < count; ++uIndex)
        {
            CGSprite* s = (CGSprite*)m_obDescendants.at(uIndex);
            s->setAtlasIndex( s->getAtlasIndex() - 1 );
        }
    }

    CAVector<CGNode*>::const_iterator itr;
    for (itr=pobSprite->getChildren().begin(); itr!=pobSprite->getChildren().end(); itr++)
    {
        CGSprite* s = (CGSprite*)(*itr);
        this->removeSpriteFromAtlas(s);
    }
}

void CGSpriteBatchNode::updateBlendFunc(void)
{
    if (! m_pobImageAtlas->getImage()->hasPremultipliedAlpha())
    {
        m_blendFunc.src = GL_SRC_ALPHA;
        m_blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
    }
    else
    {
        m_blendFunc.src = CC_BLEND_SRC;
        m_blendFunc.dst = CC_BLEND_DST;
    }
}

void CGSpriteBatchNode::setBlendFunc(BlendFunc blendFunc)
{
    m_blendFunc = blendFunc;
}

BlendFunc CGSpriteBatchNode::getBlendFunc(void)
{
    return m_blendFunc;
}

CAImage* CGSpriteBatchNode::getImage(void)
{
    return m_pobImageAtlas->getImage();
}

void CGSpriteBatchNode::setImage(CAImage *image)
{
    m_pobImageAtlas->setImage(image);
    updateBlendFunc();
}

void CGSpriteBatchNode::insertQuadFromSprite(CGSprite *sprite, unsigned int index)
{
    CCAssert( sprite != NULL, "Argument must be non-NULL");
    CCAssert( dynamic_cast<CGSprite*>(sprite), "CGSpriteBatchNode only supports CCSprites as children");

    while(index >= m_pobImageAtlas->getCapacity() || m_pobImageAtlas->getCapacity() == m_pobImageAtlas->getTotalQuads())
    {
        this->increaseAtlasCapacity();
    }

    sprite->setBatchNode(this);
    sprite->setAtlasIndex(index);

    ccV3F_C4B_T2F_Quad quad = sprite->getQuad();
    m_pobImageAtlas->insertQuad(&quad, index);

    sprite->setDirty(true);
    sprite->updateTransform();
}

void CGSpriteBatchNode::updateQuadFromSprite(CGSprite *sprite, unsigned int index)
{
    CCAssert(sprite != NULL, "Argument must be non-nil");
    CCAssert(dynamic_cast<CGSprite*>(sprite) != NULL, "CGSpriteBatchNode only supports CCSprites as children");
    
	while (index >= m_pobImageAtlas->getCapacity() || m_pobImageAtlas->getCapacity() == m_pobImageAtlas->getTotalQuads())
    {
		this->increaseAtlasCapacity();
    }

	sprite->setBatchNode(this);
    sprite->setAtlasIndex(index);
	sprite->setDirty(true);
	sprite->updateTransform();
}

CGSpriteBatchNode * CGSpriteBatchNode::addSpriteWithoutQuad(CGSprite*child, unsigned int z, int aTag)
{
    CCAssert( child != NULL, "Argument must be non-NULL");
    CCAssert( dynamic_cast<CGSprite*>(child), "CGSpriteBatchNode only supports CCSprites as children");

    child->setAtlasIndex(z);

    int i=0;
 
    CAVector<CGSprite*>::iterator itr;
    for (itr=m_obDescendants.begin(); itr!=m_obDescendants.end(); itr++)
    {
        CC_CONTINUE_IF((*itr)->getAtlasIndex() < z);
        ++i;
    }
    m_obDescendants.insert(i, child);

    CGNode::insertChild(child, z);
    child->setTag(aTag);
    reorderBatch(false);

    return this;
}

NS_CC_END
