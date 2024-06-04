/* Copyright (c) 2024, Christian Ahrens
 *
 * This file is part of MenuBarMatrix <https://github.com/ChristianAhrens/MenuBarMatrix>
 *
 * This tool is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This tool is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this tool; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

#include <JuceHeader.h>

#include "AppConfigurationBase.h"


namespace MenuBarMatrix
{

/**
 * Fwd. decls
 */
class AudioSelectComponent;
class MenuBarMatrixEditor;
class MenuBarMatrixProcessor;
class MenuBarMatrixRemoteWrapper;

//==============================================================================
/*
 *
 */
class MenuBarMatrix   :    public Component,
                                public JUCEAppBasics::AppConfigurationBase::XmlConfigurableElement
{
public:
    MenuBarMatrix();
    ~MenuBarMatrix() override;
    
    //==========================================================================
    juce::Component* getUIComponent();
    juce::Component* getDeviceSetupComponent();

    //==========================================================================
    std::unique_ptr<XmlElement> createStateXml() override;
    bool setStateXml(XmlElement* stateXml) override;

    //==========================================================================
    void lockCurrentLayout(bool doLock);

private:
    std::unique_ptr<MenuBarMatrixProcessor>        m_MenuBarMatrixProcessor;

    std::unique_ptr<MenuBarMatrixEditor>           m_audioVisuComponent;
    std::unique_ptr<AudioSelectComponent>          m_audioDeviceSelectComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuBarMatrix)
};

};
