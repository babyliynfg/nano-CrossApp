//
//  CAUIEditorParser.hpp
//  CrossApp
//
//  Created by 栗元峰 on 15/11/26.
//  Copyright © 2015年 http://www.9miao.com. All rights reserved.
//

#ifndef CAUIEditorParser_h
#define CAUIEditorParser_h

#include "basics/CAObject.h"
#include "tinyxml2/tinyxml2.h"
#include <map>

NS_CC_BEGIN

CC_DLL class CAThemeManager : public CAObject
{
public:
    
    static CAThemeManager *getInstance();
    
    static void destroyInstance();
    
protected:
    
    CAThemeManager();
    
    virtual ~CAThemeManager();
    
protected:
    
    std::map<std::string, std::map<std::string, std::string> > m_mPathss;

	tinyxml2::XMLDocument* m_pMyDocument;
};

NS_CC_END
#endif /* CAUIEditorParser_h */
