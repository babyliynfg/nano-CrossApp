//
//  CAUIEditorParser.cpp
//  CrossApp
//
//  Created by 栗元峰 on 15/11/26.
//  Copyright © 2015年 http://www.9miao.com. All rights reserved.
//

#include "CAThemeManager.h"
#include "platform/CAFileUtils.h"


NS_CC_BEGIN

static CAThemeManager* s_pThemeManager = nullptr;

CAThemeManager* CAThemeManager::getInstance()
{
    if (s_pThemeManager == nullptr)
    {
        s_pThemeManager = new CAThemeManager();
    }
    
    return  s_pThemeManager;
}

void CAThemeManager::destroyInstance()
{
    
}

CAThemeManager::CAThemeManager()
:m_pMyDocument(NULL)
{
    unsigned long size = 0;
    const char* data = (const char*)FileUtils::getInstance()->getFileData("source_material/theme.style", "rb", &size);
    if (size > 0)
    {
        std::string str;
        str.resize(size);
        for (size_t i = 0; i < size; i++)
        {
            str[i] = data[i];
        }
        
        m_pMyDocument = new tinyxml2::XMLDocument();
        m_pMyDocument->Parse(data, size);
        tinyxml2::XMLElement* rootElement = m_pMyDocument->RootElement();
        
        tinyxml2::XMLElement* controlXml = ((tinyxml2::XMLElement*)rootElement->FirstChildElement());

        while (controlXml)
        {
            std::map<std::string, std::string> map;
            const tinyxml2::XMLElement* pathXml = ((tinyxml2::XMLElement*)controlXml->FirstChildElement());;
            while (pathXml)
            {
                const tinyxml2::XMLAttribute* key_value = pathXml->FirstAttribute();
                map.insert(std::make_pair(key_value->Name(), key_value->Value()));
                
                pathXml = (tinyxml2::XMLElement*)pathXml->NextSiblingElement();
            }
            
            m_mPathss.insert(std::make_pair(controlXml->Attribute("name"), map));
            controlXml = controlXml->NextSiblingElement();
        }
    }
    
    for (auto& var : m_mPathss)
    {
        std::map<std::string, std::string>& map = var.second;
        for (auto& it : map)
        {
            CCLog("name: %s, value: %s", it.first.c_str(), it.second.c_str());
        }
        CCLog("control   name: %s", var.first.c_str());
    }
}

CAThemeManager::~CAThemeManager()
{
	CC_SAFE_DELETE(m_pMyDocument);
}



NS_CC_END