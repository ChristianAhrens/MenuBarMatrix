/* Copyright (c) 2024, Christian Ahrens
 *
 * This file is part of Mema <https://github.com/ChristianAhrens/Mema>
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

#include "../MemaProcessor/MemaCommanders.h"


namespace Mema
{

//==============================================================================
class CrosspointComponent : public juce::Component
{
using crosspointIdent = std::pair<int, int>;

public:
    CrosspointComponent(const crosspointIdent& ident) : juce::Component::Component() { m_ident = ident; }
    ~CrosspointComponent() {}

    const crosspointIdent& getIdent() { return m_ident; };

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
    crosspointIdent m_ident = { -1, -1 };
};

//==============================================================================
class CrosspointsControlComponent : public juce::Component, 
    public MemaCrosspointCommander
{
public:
    CrosspointsControlComponent();
    ~CrosspointsControlComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void setCrosspointEnabledValue(int input, int output, bool enabledState) override;

    //==============================================================================
    std::function<void()> onBoundsRequirementChange;
    juce::Rectangle<int> getRequiredSize();

    //==============================================================================
    void setIOCount(int inputCount, int outputCount) override;

private:
    //==============================================================================
    std::map<int, std::map<int, bool>> m_crosspointEnabledValues;
    std::map<int, std::map<int, std::unique_ptr<CrosspointComponent>>> m_crosspointComponent;

    //==============================================================================
    juce::Grid  m_matrixGrid;

    std::pair<int, int> m_ioCount{ std::make_pair(-1, -1) };

    static constexpr int s_nodeSize = 24;
    static constexpr double s_nodeGap = 1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrosspointsControlComponent)
};

}
