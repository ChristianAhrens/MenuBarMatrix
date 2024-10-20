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
    class MenuBarMatrix;
}
class LoadBar;
class EmptySpace;
class AboutComponent;


class MainComponent :   public juce::Component,
                        public juce::DarkModeSettingListener,
                        public juce::FocusChangeListener
{
public:
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void darkModeSettingChanged() override;
    
    //========================================================================*
    void paint(Graphics&) override;
    void resized() override;

    //========================================================================*
    void lookAndFeelChanged() override;

    //========================================================================*
    void globalFocusChanged(Component* focusedComponent) override;

    //========================================================================*
    std::function<void()> onFocusLostWhileVisible;

private:
    std::unique_ptr<MenuBarMatrix::MenuBarMatrix>   m_mbm;
    std::unique_ptr<juce::DrawableButton>           m_setupToggleButton;
    std::unique_ptr<juce::DrawableButton>           m_aboutToggleButton;
    std::unique_ptr<juce::DrawableButton>           m_powerButton;
    std::unique_ptr<EmptySpace>                     m_emptySpace;
    std::unique_ptr<LoadBar>                        m_sysLoadBar;
    std::unique_ptr<juce::LookAndFeel>              m_lookAndFeel;
    std::unique_ptr<AboutComponent>                 m_aboutComponent;
    
    static constexpr int sc_buttonSize = 26;
    static constexpr int sc_loadWidth = 100;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

