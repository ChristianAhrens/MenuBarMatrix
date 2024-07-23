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
class MenuBarMatrix   :    public juce::Component, public juce::Timer
{
public:
    MenuBarMatrix();
    ~MenuBarMatrix() override;

    //==========================================================================
    void timerCallback() override;

    //==========================================================================
    juce::Component* getUIComponent();
    juce::Component* getDeviceSetupComponent();

    //==========================================================================
    std::function<void(int)> onCpuUsageUpdate;
    std::function<void(juce::Rectangle<int>)> onSizeChangeRequested;

private:
    std::unique_ptr<MenuBarMatrixProcessor>        m_menuBarMatrixProcessor;

    std::unique_ptr<MenuBarMatrixEditor>           m_audioVisuComponent;
    std::unique_ptr<AudioSelectComponent>          m_audioDeviceSelectComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuBarMatrix)
};

};
