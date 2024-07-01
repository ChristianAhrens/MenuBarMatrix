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
#include "AudioSelectComponent.h"


namespace MenuBarMatrix
{

class InputControlComponent;
class CrosspointsControlComponent;
class OutputControlComponent;


//==============================================================================
/*
*/
class MenuBarMatrixEditor : public juce::AudioProcessorEditor,
                            public JUCEAppBasics::AppConfigurationBase::XmlConfigurableElement
{
public:
    MenuBarMatrixEditor(juce::AudioProcessor& processor);
    MenuBarMatrixEditor(juce::AudioProcessor* processor);
    ~MenuBarMatrixEditor();

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==========================================================================
    void lookAndFeelChanged() override;

    //==========================================================================
    std::unique_ptr<XmlElement> createStateXml() override;
    bool setStateXml(XmlElement* stateXml) override;

private:
    std::unique_ptr<InputControlComponent>      m_inputCtrl;
    std::unique_ptr<CrosspointsControlComponent> m_crosspointCtrl;
    std::unique_ptr<OutputControlComponent>     m_outputCtrl;

    juce::Grid  m_gridLayout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuBarMatrixEditor)
};

}
