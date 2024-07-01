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

#include "../MenuBarMatrixProcessor/MenuBarMatrixProcessor.h"


namespace MenuBarMatrix
{

//==============================================================================
class CrosspointComponent : public juce::Component
{
public:
    CrosspointComponent() : juce::Component::Component() {}
    ~CrosspointComponent() {}

    //==============================================================================
    void paint(Graphics& g) override
    {
        auto bounds = getLocalBounds().reduced(2).toFloat();
        g.setColour(getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
        if (m_checked)
            g.fillEllipse(bounds.reduced(2));
        
        g.drawEllipse(bounds, 1.0f);
    };

    //==============================================================================
    void setChecked(bool checked)
    {
        m_checked = checked;
        repaint();
    };
    void toggleChecked()
    {
        setChecked(!m_checked);
        
        if (onCheckedChanged)
            onCheckedChanged(m_checked, this);
    }

    //==============================================================================
    void mouseUp(const MouseEvent& e) override
    {
        if (getLocalBounds().contains(e.getPosition()))
            toggleChecked();
    };

    std::function<void(bool, CrosspointComponent*)> onCheckedChanged;

private:
    bool m_checked = false;
};

//==============================================================================
class CrosspointsControlComponent : public juce::Component, 
    public MenuBarMatrixProcessor::CrosspointCommander
{
public:
    CrosspointsControlComponent();
    ~CrosspointsControlComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void setCrosspointEnabledValue(int input, int output, bool enabledState) override;

private:
    //==============================================================================
    void setIOCount(int inputCount, int outputCount);

    //==============================================================================
    std::map<int, std::map<int, bool>> m_crosspointEnabledValues;
    std::map<int, std::map<int, std::unique_ptr<CrosspointComponent>>> m_crosspointComponent;

    //==============================================================================
    juce::Grid  m_matrixGrid;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrosspointsControlComponent)
};

}
