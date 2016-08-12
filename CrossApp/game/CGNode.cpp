//
//  CGNode.cpp
//  CrossApp
//
//  Created by Li Yuanfeng on 16-7-12.
//  Copyright (c) 2014Âπ?http://9miao.com All rights reserved.
//

#include "CGNode.h"
#include "support/CAPointExtension.h"
#include "math/TransformUtils.h"
#include "basics/CACamera.h"
#include "basics/CAApplication.h"
#include "basics/CAScheduler.h"
#include "dispatcher/CATouch.h"
#include "kazmath/GL/matrix.h"
#include "CCEGLView.h"
#include "platform/CADensityDpi.h"
#include "game/actions/CGActionManager.h"
#include "ccMacros.h"
#include <stdarg.h>

NS_CC_BEGIN;

static int s_globalOrderOfArrival = 1;

static std::map<int, CGNode*> s_pMap;

std::map<int, CGNode*>& CGNode::getInstanceMap()
{
    return s_pMap;
}

CGNode::CGNode(void)
: m_fRotationX(0.0f)
, m_fRotationY(0.0f)
, m_fRotationZ_X(0.0f)
, m_fRotationZ_Y(0.0f)
, m_fScaleX(1.0f)
, m_fScaleY(1.0f)
, m_fScaleZ(1.0f)
, m_fSkewX(0.0f)
, m_fSkewY(0.0f)
, m_obAnchorPointInPoints(DPointZero)
, m_obAnchorPoint(DPointZero)
, m_obPosition(DPointZero)
, m_obNormalizedPosition(DPointZero)
, m_fPositionZ(0.0f)
, m_bUsingNormalizedPosition(false)
, m_obContentSize(DSizeZero)
, m_tAdditionalTransform(Mat4::IDENTITY)
, m_bTransformDirty(true)
, m_bInverseDirty(true)
, m_bUseAdditionalTransform(false)
, m_bTransformUpdated(true)
, m_pCamera(NULL)
, m_nZOrder(0)
, m_pParent(NULL)
, m_uOrderOfArrival(0)
, m_eGLServerState(ccGLServerState(0))
, m_pShaderProgram(NULL)
, m_bRunning(false)
, m_bVisible(true)
, m_bReorderChildDirty(false)
, _displayedAlpha(1.0f)
, _realAlpha(1.0f)
, _displayedColor(CAColor_white)
, _realColor(CAColor_white)
, m_bDisplayRange(true)
, m_bHasChildren(false)
, m_pSuperviewCAView(NULL)
, m_pCAView(NULL)
{
    memset((void*)&m_sQuad, 0, sizeof(m_sQuad));
    
    m_tTransform = m_tInverse = m_tAdditionalTransform = Mat4::IDENTITY;
    
    this->updateRotationQuat();
    
    s_pMap[m_u__ID] = this;
//    CCLog("CGNode = %ld\n", s_pMap.size());
}

CGNode::~CGNode(void)
{
    CC_SAFE_RELEASE(m_pCamera);
    CC_SAFE_RELEASE(m_pShaderProgram);
    
    this->stopAllActions();
    
    if(!m_obChildren.empty())
    {
        CAVector<CGNode*>::iterator itr;
        for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
        {
            (*itr)->setParent(NULL);
        }
    }
    m_obChildren.clear();
    if (m_pCAView)
    {
        m_pCAView->m_pParentCGNode = NULL;
        m_pCAView->release();
    }
    s_pMap.erase(m_u__ID);
//    CCLog("~CGNode = %ld\n", s_pMap.size());
}

CGNode * CGNode::create(void)
{
    CGNode * pRet = new CGNode();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

bool CGNode::init()
{
    return true;
}

float CGNode::getSkewX()
{
    return m_fSkewX;
}

void CGNode::setSkewX(float newSkewX)
{
    if (m_fSkewX != newSkewX)
    {
        m_fSkewX = newSkewX;
        this->updateDraw();
    }
}

float CGNode::getSkewY()
{
    return m_fSkewY;
}

void CGNode::setSkewY(float newSkewY)
{
    if (m_fSkewY != newSkewY)
    {
        m_fSkewY = newSkewY;
        this->updateDraw();
    }
}

int CGNode::getZOrder()
{
    return m_nZOrder;
}

void CGNode::_setZOrder(int z)
{
    m_nZOrder = z;
}

void CGNode::setZOrder(int z)
{
    if (m_pParent)
    {
        m_pParent->reorderChild(this, z);
    }
    else
    {
        this->_setZOrder(z);
    }
}

int CGNode::getRotation()
{
    CCAssert(m_fRotationZ_X == m_fRotationZ_Y, "CGNode#rotation. m_fRotationZ_X != m_fRotationZ_Y. Don't know which one to return");
    return m_fRotationZ_X;
}

void CGNode::setRotation(int newRotation)
{
    if (m_fRotationZ_X != newRotation || m_fRotationZ_Y != newRotation)
    {
        m_fRotationZ_X = m_fRotationZ_Y = newRotation;
        this->updateRotationQuat();
        this->updateDraw();
    }
}

int CGNode::getRotationX()
{
    return m_fRotationZ_X;
}

void CGNode::setRotationX(int fRotationX)
{
    if (m_fRotationZ_X != fRotationX)
    {
        m_fRotationZ_X = fRotationX;
        this->updateRotationQuat();
        this->updateDraw();
    }
}

int CGNode::getRotationY()
{
    return m_fRotationZ_Y;
}

void CGNode::setRotationY(int fRotationY)
{
    if (m_fRotationZ_Y != fRotationY)
    {
        m_fRotationZ_Y = fRotationY;
        this->updateRotationQuat();
        this->updateDraw();
    }
}

DPoint3D CGNode::getRotation3D() const
{
    CCAssert(m_fRotationZ_X == m_fRotationZ_Y, "CGNode#rotation. m_fRotationZ_X != m_fRotationZ_Y. Don't know which one to return");
    return DPoint3D(m_fRotationX, m_fRotationY, m_fRotationZ_X);
}

void CGNode::setRotation3D(const DPoint3D& rotation)
{
    if (rotation.x != m_fRotationX || rotation.y != m_fRotationY || rotation.z != m_fRotationZ_X)
    {
        m_fRotationX = rotation.x;
        m_fRotationY = rotation.y;
        m_fRotationZ_Y = m_fRotationZ_X = rotation.z;
        this->updateRotationQuat();
        this->updateDraw();
    }
}

void CGNode::setRotationQuat(const Quaternion& quat)
{
    m_obRotationQuat = quat;
    updateRotation3D();
    this->updateDraw();
}

Quaternion CGNode::getRotationQuat() const
{
    return m_obRotationQuat;
}

void CGNode::updateRotationQuat()
{
    float halfRadx = CC_DEGREES_TO_RADIANS(m_fRotationX / 2.f);
    float halfRady = CC_DEGREES_TO_RADIANS(m_fRotationY / 2.f);
    float halfRadz = m_fRotationZ_X == m_fRotationZ_Y ? -CC_DEGREES_TO_RADIANS(m_fRotationZ_X / 2.f) : 0;
    
    float coshalfRadx = cosf(halfRadx), sinhalfRadx = sinf(halfRadx), coshalfRady = cosf(halfRady), sinhalfRady = sinf(halfRady), coshalfRadz = cosf(halfRadz), sinhalfRadz = sinf(halfRadz);
    
    m_obRotationQuat.x = sinhalfRadx * coshalfRady * coshalfRadz - coshalfRadx * sinhalfRady * sinhalfRadz;
    m_obRotationQuat.y = coshalfRadx * sinhalfRady * coshalfRadz + sinhalfRadx * coshalfRady * sinhalfRadz;
    m_obRotationQuat.z = coshalfRadx * coshalfRady * sinhalfRadz - sinhalfRadx * sinhalfRady * coshalfRadz;
    m_obRotationQuat.w = coshalfRadx * coshalfRady * coshalfRadz + sinhalfRadx * sinhalfRady * sinhalfRadz;
}

void CGNode::updateRotation3D()
{
    //convert quaternion to Euler angle
    float x = m_obRotationQuat.x, y = m_obRotationQuat.y, z = m_obRotationQuat.z, w = m_obRotationQuat.w;
    m_fRotationX = atan2f(2.f * (w * x + y * z), 1.f - 2.f * (x * x + y * y));
    float sy = 2.f * (w * y - z * x);
    sy = clampf(sy, -1, 1);
    m_fRotationY = asinf(sy);
    m_fRotationZ_X = atan2f(2.f * (w * z + x * y), 1.f - 2.f * (y * y + z * z));
    
    m_fRotationX = CC_RADIANS_TO_DEGREES(m_fRotationX);
    m_fRotationY = CC_RADIANS_TO_DEGREES(m_fRotationY);
    m_fRotationZ_X = m_fRotationZ_Y = -CC_RADIANS_TO_DEGREES(m_fRotationZ_X);
}

float CGNode::getScale(void)
{
    CCAssert( m_fScaleX == m_fScaleY, "CGNode#scale. ScaleX != ScaleY. Don't know which one to return");
    return m_fScaleX;
}

void CGNode::setScale(float scale)
{
    this->setScale(scale, scale, scale);
}

void CGNode::setScale(float fScaleX, float fScaleY, float fScaleZ)
{
    this->setScaleX(fScaleX);
    this->setScaleY(fScaleY);
    this->setScaleZ(fScaleY);
}

float CGNode::getScaleX()
{
    return m_fScaleX;
}

void CGNode::setScaleX(float newScaleX)
{
    if (m_fScaleX != newScaleX)
    {
        m_fScaleX = newScaleX;
        this->updateDraw();
    }
}

float CGNode::getScaleY()
{
    return m_fScaleY;
}

void CGNode::setScaleY(float newScaleY)
{
    if (m_fScaleY != newScaleY)
    {
        m_fScaleY = newScaleY;
        this->updateDraw();
    }
}

float CGNode::getScaleZ() const
{
    return m_fScaleZ;
}

void CGNode::setScaleZ(float scaleZ)
{
    if (m_fScaleZ != scaleZ)
    {
        m_fScaleZ = scaleZ;
        this->updateDraw();
    }
}

/// scaleY getter


const CAVector<CGNode*>& CGNode::getChildren()
{
    return m_obChildren;
}

unsigned int CGNode::getChildrenCount() const
{
    return (unsigned int)m_obChildren.size();
}

CACamera* CGNode::getCamera()
{
    if (!m_pCamera)
    {
        m_pCamera = new CACamera();
    }
    
    return m_pCamera;
}

bool CGNode::isVisible()
{
    return m_bVisible;
}

void CGNode::setVisible(bool var)
{
    m_bVisible = var;
    this->updateDraw();
}

const DPoint& CGNode::getAnchorPointInPoints()
{
    return m_obAnchorPointInPoints;
}

const DPoint& CGNode::getAnchorPoint()
{
    return m_obAnchorPoint;
}

void CGNode::setAnchorPoint(const DPoint& anchorPoint)
{
    if( ! anchorPoint.equals(m_obAnchorPoint))
    {
        m_obAnchorPoint = anchorPoint;
        m_obAnchorPointInPoints = ccpCompMult(m_obContentSize, anchorPoint);
        this->updateDraw();
    }
}

const DPoint& CGNode::getNormalizedPosition() const
{
    return m_obNormalizedPosition;
}

void CGNode::setNormalizedPosition(const DPoint& position)
{
    if ( ! position.equals(m_obNormalizedPosition))
    {
        m_obNormalizedPosition = position;
        m_bUsingNormalizedPosition = true;
        this->updateDraw();
    }
}

const DPoint& CGNode::getPosition()
{
    return m_obPosition;
}

void CGNode::setPosition(const DPoint& position)
{
    if( ! position.equals(m_obPosition))
    {
        m_obPosition = position;
        m_bUsingNormalizedPosition = false;
        this->updateDraw();
    }
}

float CGNode::getPositionX() const
{
    return m_obPosition.x;
}

void CGNode::setPositionX(float x)
{
    setPosition(DPoint(x, m_obPosition.y));
}

float CGNode::getPositionY() const
{
    return  m_obPosition.y;
}

void CGNode::setPositionY(float y)
{
    setPosition(DPoint(m_obPosition.x, y));
}

float CGNode::getPositionZ() const
{
    return m_fPositionZ;
}

void CGNode::setPositionZ(float positionZ)
{
    if (positionZ != m_fPositionZ)
    {
        m_fPositionZ = positionZ;
        this->updateDraw();
    }
}


void CGNode::setPosition3D(const DPoint3D& position)
{
    setPositionZ(position.z);
    setPosition(DPoint(position.x, position.y));
}

DPoint3D CGNode::getPosition3D() const
{
    return DPoint3D(m_obPosition.x, m_obPosition.y, m_fPositionZ);
}

const DSize& CGNode::getContentSize()
{
    return m_obContentSize;
}

void CGNode::setContentSize(const DSize & contentSize)
{
    if (!contentSize.equals(m_obContentSize))
    {
        m_obContentSize = contentSize;
        m_obAnchorPointInPoints.x = m_obContentSize.width * m_obAnchorPoint.x;
        m_obAnchorPointInPoints.y = m_obContentSize.height * m_obAnchorPoint.y;
        
        if (m_pCAView)
        {
            m_pCAView->reViewlayout(m_obContentSize);
        }
        
        this->updateDraw();
    }
}

bool CGNode::isRunning()
{
    return m_bRunning;
}

CGNode * CGNode::getParent()
{
    return m_pParent;
}

void CGNode::setParent(CrossApp::CGNode * Parent)
{
    m_pParent = Parent;
}

unsigned int CGNode::getOrderOfArrival()
{
    return m_uOrderOfArrival;
}

void CGNode::setOrderOfArrival(unsigned int uOrderOfArrival)
{
    m_uOrderOfArrival = uOrderOfArrival;
}

ccGLServerState CGNode::getGLServerState()
{
    return m_eGLServerState;
}

void CGNode::setGLServerState(ccGLServerState glServerState)
{
    m_eGLServerState = glServerState;
}

CAGLProgram* CGNode::getShaderProgram()
{
    return m_pShaderProgram;
}

void CGNode::setShaderProgram(CAGLProgram *pShaderProgram)
{
    CC_SAFE_RETAIN(pShaderProgram);
    CC_SAFE_RELEASE(m_pShaderProgram);
    m_pShaderProgram = pShaderProgram;
}

DRect CGNode::boundingBox()
{
    DRect rect = DRect(0, 0, m_obContentSize.width, m_obContentSize.height);
    rect = RectApplyAffineTransform(rect, getNodeToParentAffineTransform());
    rect.origin.y += rect.size.height;
    if (this->m_pParent)
    {
        rect.origin.y = this->m_pParent->m_obContentSize.height - rect.origin.y;
    }
    else
    {
        rect.origin.y = CAApplication::getApplication()->getWinSize().height - rect.origin.y;
    }
    return rect;
}

const char* CGNode::description()
{
    return crossapp_format_string("<CGNode | TextTag = %s | Tag = %d >", m_sTextTag.c_str(), m_nTag).c_str();
}

void CGNode::updateDraw()
{
    CGNode* v = this->getParent();
    while (v && v == v->getParent())
    {
        CC_RETURN_IF(v && v->isVisible());
    }
    m_bTransformDirty = m_bInverseDirty = m_bInverseDirty = true;
    CAApplication::getApplication()->updateDraw();
}

CGNode* CGNode::getChildByTag(int aTag)
{
    CCAssert( aTag != kCAObjectTagInvalid, "Invalid tag");
    
    if(!m_obChildren.empty())
    {
        CAVector<CGNode*>::iterator itr;
        for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
        {
            if ((*itr)->m_nTag == aTag)
            {
                return *itr;
            }
        }
    }
    return NULL;
}

CGNode* CGNode::getChildByTextTag(const std::string& textTag)
{
    CCAssert( !textTag.empty(), "Invalid tag");
    
    if(!m_obChildren.empty())
    {
        CAVector<CGNode*>::iterator itr;
        for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
        {
            if ((*itr)->m_sTextTag.compare(textTag) == 0)
            {
                return *itr;
            }
        }
    }
    return NULL;
}

void CGNode::addChild(CGNode* child)
{
    this->insertChild(child, child->getZOrder());
}

void CGNode::insertChild(CGNode* child, int z)
{
    CCAssert(child != NULL, "Argument must be non-nil");
    CCAssert(child->m_pParent == NULL, "child already added. It can't be added again");
    
    m_bReorderChildDirty = true;
    m_obChildren.pushBack(child);
    child->_setZOrder(z);
    
    child->setParent(this);
    child->setOrderOfArrival(s_globalOrderOfArrival++);
    
    if(m_bRunning)
    {
       child->onEnter();
       child->onEnterTransitionDidFinish();
    }
}

void CGNode::removeFromParent()
{
    if (m_pParent != NULL)
    {
        m_pParent->removeChild(this);
    }
}

void CGNode::removeChild(CGNode* child)
{
    if (m_obChildren.contains(child))
    {
        this->detachChild(child);
    }
}

void CGNode::removeChildByTag(int tag)
{
    CCAssert( tag != kCAObjectTagInvalid, "Invalid tag");
    
    CGNode* child = this->getChildByTag(tag);
    
    if (child)
    {
        this->removeChild(child);
    }
}

void CGNode::removeChildByTextTag(const std::string& textTag)
{
    CCAssert( !textTag.empty(), "Invalid tag");
    
    CGNode* child = this->getChildByTextTag(textTag);
    
    if (child)
    {
        this->removeChild(child);
    }
}

void CGNode::removeAllChildren()
{
    if (!m_obChildren.empty())
    {
        CAVector<CGNode*>::iterator itr;
        for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
        {
            if(m_bRunning)
            {
                (*itr)->onExitTransitionDidStart();
                (*itr)->onExit();
            }
            
            (*itr)->setParent(NULL);
        }
        
        m_obChildren.clear();
    }
    
    m_bHasChildren = false;
}


void CGNode::detachChild(CGNode* child)
{
    if (m_bRunning)
    {
       child->onExitTransitionDidStart();
       child->onExit();
    }
    
    child->setParent(NULL);
    
    m_obChildren.eraseObject(child);
    
    this->updateDraw();
}

void CGNode::reorderChild(CGNode* child, int zOrder)
{
    if (zOrder == child->getZOrder())
    {
        return;
    }
    
    CCAssert(child != NULL, "Child must be non-nil");
    m_bReorderChildDirty = true;
    child->setOrderOfArrival(s_globalOrderOfArrival++);
    child->_setZOrder(zOrder);
    this->updateDraw();
}

void CGNode::sortAllChildren()
{
    if (m_bReorderChildDirty && !m_obChildren.empty())
    {
        std::sort(m_obChildren.begin(), m_obChildren.end(), compareChildrenZOrder);
        m_bReorderChildDirty = false;
    }
}

void CGNode::draw()
{

}

void CGNode::visit()
{
    CC_RETURN_IF(!m_bVisible);
    
    kmGLPushMatrix();
    
    this->transform();
    
    bool isScissor = (bool)glIsEnabled(GL_SCISSOR_TEST);
    DRect restoreScissorRect = DRectZero;
    if (isScissor)
    {
        GLfloat params[4];
        glGetFloatv(GL_SCISSOR_BOX, params);
        restoreScissorRect = DRect(params[0], params[1], params[2], params[3]);
    }
    
    if (!m_bDisplayRange)
    {
        DRect frame = DRectZero;
        frame.size = m_obContentSize;
        frame = this->convertRectToWorldSpace(frame);
        frame.origin.y = CAApplication::getApplication()->getWinSize().height - frame.size.height - frame.origin.y;
        if (isScissor)
        {
            float x1 = MAX(s_dip_to_px(frame.getMinX()), restoreScissorRect.getMinX());
            float y1 = MAX(s_dip_to_px(frame.getMinY()), restoreScissorRect.getMinY());
            float x2 = MIN(s_dip_to_px(frame.getMaxX()) + 0.5f, restoreScissorRect.getMaxX());
            float y2 = MIN(s_dip_to_px(frame.getMaxY()) + 0.5f, restoreScissorRect.getMaxY());
            float width = MAX(x2-x1, 0);
            float height = MAX(y2-y1, 0);
            glScissor(x1, y1, width, height);
        }
        else
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(s_dip_to_px(frame.origin.x),
                      s_dip_to_px(frame.origin.y),
                      s_dip_to_px(frame.size.width) + 0.5f,
                      s_dip_to_px(frame.size.height) + 0.5f);
        }
    }
    
    this->sortAllChildren();
    
    CAVector<CGNode*>::iterator itr=m_obChildren.begin();
    while (itr!=m_obChildren.end())
    {
        CC_BREAK_IF((*itr)->m_nZOrder >= 0);
        (*itr)->visit();
        itr++;
    }
    
    this->draw();
    
    while (itr!=m_obChildren.end())
    {
        (*itr)->visit();
        itr++;
    }
    
    if (m_pCAView)
    {
        m_pCAView->visit();
    }
    
    //m_uOrderOfArrival = 0;
    
    if (!m_bDisplayRange)
    {
        if (isScissor)
        {
            glScissor(restoreScissorRect.origin.x,
                      restoreScissorRect.origin.y ,
                      restoreScissorRect.size.width,
                      restoreScissorRect.size.height);
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
        }
    }
    
    kmGLPopMatrix();
}

void CGNode::visitEve(void)
{
    CAVector<CGNode*>::iterator itr=m_obChildren.begin();
    while (itr!=m_obChildren.end())
    {
        (*itr)->visitEve();
        itr++;
    }
    
    if (m_pCAView)
    {
        m_pCAView->visitEve();
    }
}

CGNode* CGNode::copy()
{
    CGNode* view = NULL;

    return view;
}

void CGNode::onEnter()
{
    m_bRunning = true;
    CAScheduler::getScheduler()->resumeTarget(this);
    ActionManager::getInstance()->resumeTarget(this);
    
    if (!m_obChildren.empty())
    {
        CAVector<CGNode*>::iterator itr;
        for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
        {
            (*itr)->onEnter();
        }
    }
    
    if (m_pCAView)
    {
        m_pCAView->reViewlayout(m_obContentSize);
        m_pCAView->onEnter();
    }
    
    this->updateDraw();
}

void CGNode::onEnterTransitionDidFinish()
{
    if (!m_obChildren.empty())
    {
        CAVector<CGNode*>::iterator itr;
        for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
            (*itr)->onEnterTransitionDidFinish();
    }
    
    if (m_pCAView)
    {
        m_pCAView->onEnterTransitionDidFinish();
    }
}

void CGNode::onExitTransitionDidStart()
{
    if (!m_obChildren.empty())
    {
        CAVector<CGNode*>::iterator itr;
        for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
            (*itr)->onExitTransitionDidStart();
    }
    
    if (m_pCAView)
    {
        m_pCAView->onExitTransitionDidStart();
    }
}

void CGNode::onExit()
{
    m_bRunning = false;
    CAScheduler::getScheduler()->pauseTarget(this);
    ActionManager::getInstance()->pauseTarget(this);
    
    if (!m_obChildren.empty())
    {
        CAVector<CGNode*>::iterator itr;
        for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
            (*itr)->onExit();
    }
    
    if (m_pCAView)
    {
        m_pCAView->onExit();
    }
}

void CGNode::update(float fDelta)
{
    
}

AffineTransform CGNode::getNodeToParentAffineTransform() const
{
    AffineTransform ret;
    GLToCGAffine(getNodeToParentTransform().m.mat, &ret);
    
    return ret;
}


Mat4 CGNode::getNodeToParentTransform(CGNode* ancestor) const
{
    Mat4 t(this->getNodeToParentTransform());
    
    CGNode *p = m_pParent;
    if (p)
    {
        while (p)
        {
            t = p->getNodeToParentTransform() * t;
            CC_BREAK_IF(!p->getParent());
            p = p->getParent();
            CC_BREAK_IF(p == ancestor);
        }
        
        if (p && p != ancestor)
        {
            if (CAView *s = p->m_pSuperviewCAView)
            {
                t = s->getViewToSuperviewTransform(nullptr) * t;
            }
        }
    }
    else
    {
        if (CAView *s = m_pSuperviewCAView)
        {
            t = s->getViewToSuperviewTransform(nullptr) * t;
        }
    }

    return t;
}

AffineTransform CGNode::getNodeToParentAffineTransform(CGNode* ancestor) const
{
    AffineTransform t(this->getNodeToParentAffineTransform());
    
    CGNode *p = m_pParent;
    if (p)
    {
        while (p)
        {
            t = AffineTransformConcat(t, p->getNodeToParentAffineTransform());
            CC_BREAK_IF(!p->getParent());
            p = p->getParent();
            CC_BREAK_IF(p == ancestor);
        }
        
        if (p && p != ancestor)
        {
            if (CAView *s = p->m_pSuperviewCAView)
            {
                t = AffineTransformConcat(t, s->getViewToSuperviewAffineTransform(nullptr));
            }
        }
    }
    else
    {
        if (CAView *s = m_pSuperviewCAView)
        {
            t = AffineTransformConcat(t, s->getViewToSuperviewAffineTransform(nullptr));
        }
    }
    
    return t;
}

const Mat4& CGNode::getNodeToParentTransform() const
{
    if (m_bTransformDirty)
    {
        DSize size;
        if (this->m_pParent)
        {
            size = this->m_pParent->m_obContentSize;
        }
        else if (this->m_pSuperviewCAView)
        {
            size = this->m_pSuperviewCAView->m_obContentSize;
        }
        else
        {
            size = CAApplication::getApplication()->getWinSize();
        }
        
        float x, y;
        if (m_bUsingNormalizedPosition)
        {
            
            x = m_obNormalizedPosition.x * size.width;
            y = (1 - m_obNormalizedPosition.y) * size.height;
        }
        else
        {
            x = m_obPosition.x;
            y = size.height - m_obPosition.y;
        }
        float z = m_fPositionZ;
        
        bool needsSkewMatrix = ( m_fSkewX || m_fSkewY );
        
        
        DPoint anchorPoint(m_obAnchorPointInPoints.x * m_fScaleX, (m_obContentSize.height - m_obAnchorPointInPoints.y) * m_fScaleY);
        
        if (! needsSkewMatrix && !anchorPoint.equals(DSizeZero))
        {
            x += -anchorPoint.x;
            y += -anchorPoint.y;
        }
        
        Mat4 translation;
        Mat4::createTranslation(x + anchorPoint.x, y + anchorPoint.y, z, &translation);
        Mat4::createRotation(m_obRotationQuat, &m_tTransform);
        
        if (m_fRotationZ_X != m_fRotationZ_Y)
        {
            // Rotation values
            // Change rotation code to handle X and Y
            // If we skew with the exact same value for both x and y then we're simply just rotating
            float radiansX = -CC_DEGREES_TO_RADIANS(m_fRotationZ_X);
            float radiansY = -CC_DEGREES_TO_RADIANS(m_fRotationZ_Y);
            float cx = cosf(radiansX);
            float sx = sinf(radiansX);
            float cy = cosf(radiansY);
            float sy = sinf(radiansY);
            
            float m0 = m_tTransform.m.mat[0], m1 = m_tTransform.m.mat[1], m4 = m_tTransform.m.mat[4], m5 = m_tTransform.m.mat[5], m8 = m_tTransform.m.mat[8], m9 = m_tTransform.m.mat[9];
            
            m_tTransform.m.mat[0] = cy * m0 - sx * m1, m_tTransform.m.mat[4] = cy * m4 - sx * m5, m_tTransform.m.mat[8] = cy * m8 - sx * m9;
            m_tTransform.m.mat[1] = sy * m0 + cx * m1, m_tTransform.m.mat[5] = sy * m4 + cx * m5, m_tTransform.m.mat[9] = sy * m8 + cx * m9;
        }
        
        m_tTransform = translation * m_tTransform;
        m_tTransform.translate(-anchorPoint.x, -anchorPoint.y, 0);
        
        
        if (m_fScaleX != 1.f)
        {
            m_tTransform.m.mat[0] *= m_fScaleX, m_tTransform.m.mat[1] *= m_fScaleX, m_tTransform.m.mat[2] *= m_fScaleX;
        }
        if (m_fScaleY != 1.f)
        {
            m_tTransform.m.mat[4] *= m_fScaleY, m_tTransform.m.mat[5] *= m_fScaleY, m_tTransform.m.mat[6] *= m_fScaleY;
        }
        if (m_fScaleZ != 1.f)
        {
            m_tTransform.m.mat[8] *= m_fScaleZ, m_tTransform.m.mat[9] *= m_fScaleZ, m_tTransform.m.mat[10] *= m_fScaleZ;
        }
        
        if (needsSkewMatrix)
        {
            float skewMatArray[16] =
            {
                1, (float)tanf(CC_DEGREES_TO_RADIANS(m_fSkewY)), 0, 0,
                (float)tanf(CC_DEGREES_TO_RADIANS(m_fSkewX)), 1, 0, 0,
                0,  0,  1, 0,
                0,  0,  0, 1
            };
            Mat4 skewMatrix(skewMatArray);
            
            m_tTransform = m_tTransform * skewMatrix;
            
            if (!m_obAnchorPointInPoints.equals(DSizeZero))
            {
                DPoint anchorPoint(m_obAnchorPointInPoints.x * m_fScaleX, (m_obContentSize.height - m_obAnchorPointInPoints.y) * m_fScaleY);
                m_tTransform.m.mat[12] += m_tTransform.m.mat[0] * -anchorPoint.x + m_tTransform.m.mat[4] * -anchorPoint.y;
                m_tTransform.m.mat[13] += m_tTransform.m.mat[1] * -anchorPoint.x + m_tTransform.m.mat[5] * -anchorPoint.y;
            }
        }
        
        if (m_bUseAdditionalTransform)
        {
            m_tTransform = m_tTransform * m_tAdditionalTransform;
        }
        
        m_bTransformDirty = false;
    }
    
    return m_tTransform;
}

void CGNode::setViewToParentTransform(const Mat4& transform)
{
    m_tTransform = transform;
    m_bTransformDirty = false;
    m_bTransformUpdated = true;
}

void CGNode::setAdditionalTransform(const AffineTransform& additionalTransform)
{
    Mat4 tmp;
    CGAffineToGL(additionalTransform, tmp.m.mat);
    setAdditionalTransform(&tmp);
}

void CGNode::setAdditionalTransform(Mat4* additionalTransform)
{
    if (additionalTransform == nullptr)
    {
        m_bUseAdditionalTransform = false;
    }
    else
    {
        m_tAdditionalTransform = *additionalTransform;
        m_bUseAdditionalTransform = true;
    }
    m_bTransformUpdated = m_bTransformDirty = m_bInverseDirty = true;
}


AffineTransform CGNode::getParentToNodeAffineTransform() const
{
    AffineTransform ret;
    
    GLToCGAffine(getParentToNodeTransform().m.mat,&ret);
    return ret;
}

const Mat4& CGNode::getParentToNodeTransform() const
{
    if ( m_bInverseDirty )
    {
        m_tInverse = getNodeToParentTransform().getInversed();
        m_bInverseDirty = false;
    }
    
    return m_tInverse;
}


AffineTransform CGNode::getNodeToWorldAffineTransform() const
{
    return this->getNodeToParentAffineTransform(nullptr);
}

Mat4 CGNode::getNodeToWorldTransform() const
{
    return this->getNodeToParentTransform(nullptr);
}

AffineTransform CGNode::getWorldToViewAffineTransform() const
{
    return AffineTransformInvert(this->getNodeToWorldAffineTransform());
}

Mat4 CGNode::getWorldToViewTransform() const
{
    return getNodeToWorldTransform().getInversed();
}

void CGNode::transformAncestors()
{
    if( m_pParent != NULL  )
    {
        m_pParent->transformAncestors();
        m_pParent->transform();
    }
}

void CGNode::transform()
{
    kmMat4 transfrom4x4 = this->getNodeToParentTransform().m;
    
    kmGLMultMatrix( &transfrom4x4 );
    
    if ( m_pCamera != NULL)
    {
        DPoint anchorPointInPoints = DPoint(m_obAnchorPointInPoints.x,
                                            m_obContentSize.height - m_obAnchorPointInPoints.y);
        
        bool translate = (anchorPointInPoints.x != 0.0f || anchorPointInPoints.y != 0.0f);
        
        if( translate )
            kmGLTranslatef(RENDER_IN_SUBPIXEL(anchorPointInPoints.x),
                           RENDER_IN_SUBPIXEL(anchorPointInPoints.y),
                           0 );
        
        m_pCamera->locate();
        
        if( translate )
            kmGLTranslatef(RENDER_IN_SUBPIXEL(-anchorPointInPoints.x),
                           RENDER_IN_SUBPIXEL(-anchorPointInPoints.y),
                           0 );
    }
    
}

void CGNode::updateTransform()
{
    for (const auto& var : m_obChildren)
        var->updateTransform();
}

DRect CGNode::convertRectToNodeSpace(const CrossApp::DRect &worldRect)
{
    DRect ret = worldRect;
    ret.origin = this->convertToNodeSpace(ret.origin);
    ret.size = this->convertToNodeSize(ret.size);
    return ret;
}

DRect CGNode::convertRectToWorldSpace(const CrossApp::DRect &nodeRect)
{
    DRect ret = nodeRect;
    ret.origin = this->convertToWorldSpace(ret.origin);
    ret.size = this->convertToWorldSize(ret.size);
    return ret;
}

DPoint CGNode::convertToNodeSpace(const DPoint& worldPoint)
{
    Mat4 tmp = getWorldToViewTransform();
    DPoint3D vec3(worldPoint.x, CAApplication::getApplication()->getWinSize().height - worldPoint.y, 0);
    DPoint3D ret;
    tmp.transformPoint(vec3,&ret);
    return DPoint(ret.x, m_obContentSize.height - ret.y);
}

DPoint CGNode::convertToWorldSpace(const DPoint& nodePoint)
{
    Mat4 tmp = getNodeToWorldTransform();
    DPoint3D vec3(nodePoint.x, m_obContentSize.height - nodePoint.y, 0);
    DPoint3D ret;
    tmp.transformPoint(vec3,&ret);
    return DPoint(ret.x, CAApplication::getApplication()->getWinSize().height - ret.y);
}

DPoint CGNode::convertToNodeSize(const DSize& worldSize)
{
    DSize ret = worldSize;
    for (CGNode* v = this; v; v = v->m_pParent)
    {
        ret.width /= v->m_fScaleX;
        ret.height /= v->m_fScaleY;
    }
    return ret;
}

DPoint CGNode::convertToWorldSize(const DSize& nodeSize)
{
    DSize ret = nodeSize;
    for (CGNode* v = this; v; v = v->m_pParent)
    {
        ret.width *= v->m_fScaleX;
        ret.height *= v->m_fScaleY;
    }
    return ret;
}

DPoint CGNode::convertTouchToNodeSpace(CATouch *touch)
{
    DPoint point = touch->getLocation();
    return this->convertToNodeSpace(point);
}

bool CGNode::isDisplayRange()
{
    return m_bDisplayRange;
}

void CGNode::setDisplayRange(bool value)
{
    m_bDisplayRange = value;
}

float CGNode::getAlpha(void)
{
    return _realAlpha;
}

float CGNode::getDisplayedAlpha(void)
{
    return _displayedAlpha;
}

void CGNode::setAlpha(float alpha)
{
    alpha = MIN(alpha, 1.0f);
    alpha = MAX(alpha, 0.0f);
    
    _realAlpha = alpha;
    
    float parentAlpha = m_pParent ? m_pParent->getDisplayedAlpha() : 1.0f;
    
    this->updateDisplayedAlpha(parentAlpha);
}

void CGNode::updateDisplayedAlpha(float parentAlpha)
{
    _displayedAlpha = _realAlpha * parentAlpha;
    
    if (!m_obChildren.empty())
    {
        CAVector<CGNode*>::iterator itr;
        for (itr=m_obChildren.begin(); itr!=m_obChildren.end(); itr++)
            (*itr)->updateDisplayedAlpha(_displayedAlpha);
    }
    
    this->updateColor();
}

const CAColor4B& CGNode::getColor(void)
{
    return _realColor;
}

const CAColor4B& CGNode::getDisplayedColor()
{
    return _displayedColor;
}

void CGNode::setColor(const CAColor4B& color)
{
    _realColor = color;
    _displayedColor = color;
    this->updateColor();
}

void CGNode::updateDisplayedColor(const CAColor4B& superviewColor)
{
    _displayedColor.r = _realColor.r * superviewColor.r/255.0;
    _displayedColor.g = _realColor.g * superviewColor.g/255.0;
    _displayedColor.b = _realColor.b * superviewColor.b/255.0;
    _displayedColor.a = _realColor.a * superviewColor.a/255.0;
    
    updateColor();
}

void CGNode::updateColor(void)
{
    CAColor4B color4 = _displayedColor;
    color4.a = color4.a * _displayedAlpha;

    m_sQuad.bl.colors = color4;
    m_sQuad.br.colors = color4;
    m_sQuad.tl.colors = color4;
    m_sQuad.tr.colors = color4;

    this->updateDraw();
}

void CGNode::updateBlendFunc(void)
{
    m_sBlendFunc.src = CC_BLEND_SRC;
    m_sBlendFunc.dst = CC_BLEND_DST;
}


Action * CGNode::runAction(Action* action)
{
    CCAssert( action != nullptr, "Argument must be non-nil");
    ActionManager::getInstance()->addAction(action, this, !m_bRunning);
    return action;
}

void CGNode::stopAllActions()
{
    ActionManager::getInstance()->removeAllActionsFromTarget(this);
}

void CGNode::stopAction(Action* action)
{
    ActionManager::getInstance()->removeAction(action);
}

void CGNode::stopActionByTag(int tag)
{
    CCAssert( tag != Action::INVALID_TAG, "Invalid tag");
    ActionManager::getInstance()->removeActionByTag(tag, this);
}

void CGNode::stopAllActionsByTag(int tag)
{
    CCAssert( tag != Action::INVALID_TAG, "Invalid tag");
    ActionManager::getInstance()->removeAllActionsByTag(tag, this);
}

void CGNode::stopActionsByFlags(unsigned int flags)
{
    if (flags > 0)
    {
        ActionManager::getInstance()->removeActionsByFlags(flags, this);
    }
}

Action * CGNode::getActionByTag(int tag)
{
    CCAssert( tag != Action::INVALID_TAG, "Invalid tag");
    return ActionManager::getInstance()->getActionByTag(tag, this);
}

size_t CGNode::getNumberOfRunningActions() const
{
    return ActionManager::getInstance()->getNumberOfRunningActionsInTarget(this);
}


bool CGNode::ccTouchBegan(CATouch *pTouch, CAEvent *pEvent)
{
    return false;
}

void CGNode::ccTouchMoved(CATouch *pTouch, CAEvent *pEvent)
{
}

void CGNode::ccTouchEnded(CATouch *pTouch, CAEvent *pEvent)
{
}

void CGNode::ccTouchCancelled(CATouch *pTouch, CAEvent *pEvent)
{
}

CAView* CGNode::getCAView()
{
    return m_pCAView;
}

void CGNode::setCAView(CrossApp::CAView *var)
{
    if (m_bRunning && m_pCAView && m_pCAView->isRunning())
    {
        m_pCAView->m_pParentCGNode = NULL;
        m_pCAView->onExitTransitionDidStart();
        m_pCAView->onExit();
    }
    
    CC_SAFE_RETAIN(var);
    CC_SAFE_RELEASE(m_pCAView);
    m_pCAView = var;
    m_pCAView->m_pParentCGNode = this;
    
    if (m_bRunning && m_pCAView && !m_pCAView->isRunning())
    {
        m_pCAView->onEnter();
        m_pCAView->onEnterTransitionDidFinish();
    }
    
    CAApplication::getApplication()->updateDraw();
}

NS_CC_END;