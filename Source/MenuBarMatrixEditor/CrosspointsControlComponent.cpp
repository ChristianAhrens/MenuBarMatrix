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

#include "CrosspointsControlComponent.h"


namespace MenuBarMatrix
{

//==============================================================================
CrosspointsControlComponent::CrosspointsControlComponent()
    : MenuBarMatrixProcessor::CrosspointCommander()
{
    m_matrixGrid.rowGap.pixels = 1.0;
    m_matrixGrid.columnGap.pixels = 1.0;
}

CrosspointsControlComponent::~CrosspointsControlComponent()
{
}

void CrosspointsControlComponent::resized()
{
    m_matrixGrid.performLayout(getLocalBounds());
}

void CrosspointsControlComponent::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void CrosspointsControlComponent::setCrosspointEnabledValue(int input, int output, bool enabledState)
{
    if (input > m_crosspointEnabledValues.size() || output > m_crosspointEnabledValues[input].size())
        setIOCount(input, output);

    DBG(__FUNCTION__ << " " << input << " " << output << " " << int(enabledState));

    m_crosspointEnabledValues[input][output] = enabledState;
    if (m_crosspointComponent[input][output])
        m_crosspointComponent[input][output]->setChecked(enabledState);

    repaint();
}

void CrosspointsControlComponent::setIOCount(int inputCount, int outputCount)
{
    DBG(__FUNCTION__ << " " << inputCount << " " << outputCount);

    std::function<void(int, int)> initCrosspoint = [=](int input, int output) {
        m_crosspointEnabledValues[input][output] = false;
        m_crosspointComponent[input][output] = std::make_unique<CrosspointComponent>();
        m_crosspointComponent[input][output]->onCheckedChanged = [=](bool checkedState, CrosspointComponent* sender) {
            // emit smth here
        };
        addAndMakeVisible(m_crosspointComponent[input][output].get());

        for (int j = m_matrixGrid.templateRows.size(); j < input; j++)
            m_matrixGrid.templateRows.add(juce::Grid::TrackInfo(juce::Grid::Px(20)));

        for (int j = m_matrixGrid.templateColumns.size(); j < output; j++)
            m_matrixGrid.templateColumns.add(juce::Grid::TrackInfo(juce::Grid::Px(20))); 
    };

    if (1 != m_crosspointEnabledValues.count(inputCount))
    {
        for (int i = 1; i <= outputCount; i++)
            initCrosspoint(inputCount, i);
    }
    else
    {
        if (1 != m_crosspointEnabledValues[inputCount].count(outputCount))
        {
            for (int i = 1; i <= outputCount; i++)
            {
                if (1 != m_crosspointEnabledValues[inputCount].count(i))
                    initCrosspoint(inputCount, i);
            }
        }
    }

    m_matrixGrid.items.clear();
    for (int i = 1; i <= inputCount; i++)
    {
        for (int o = 1; o <= outputCount; o++)
        {
            if (m_crosspointComponent.at(i).at(o))
                m_matrixGrid.items.add(juce::GridItem(m_crosspointComponent.at(i).at(o).get()));
            else
                jassertfalse;
        }
    }
}

}
