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

#include "OutputControlComponent.h"

#include "AbstractAudioVisualizer.h"
#include "MeterbridgeComponent.h"

namespace MenuBarMatrix
{

//==============================================================================
OutputControlComponent::OutputControlComponent()
    : AbstractAudioVisualizer(), MenuBarMatrixProcessor::OutputCommander()
{
    setUsesValuesInDB(true);

    m_OutputLevels = std::make_unique<MeterbridgeComponent>();
    addAndMakeVisible(m_OutputLevels.get());
}

OutputControlComponent::~OutputControlComponent()
{

}

void OutputControlComponent::resized()
{
    if (!m_OutputMutes.empty())
    {
        auto usableBounds = getLocalBounds().reduced(15);

        auto maxcontrolElementWidth = 30;
        auto fixedSizeCtrlsHeight = 130;
        auto dynamicSizeCtrlsHeight = usableBounds.getHeight() - fixedSizeCtrlsHeight;

        auto meterBridgeBounds = usableBounds.removeFromTop(static_cast<int>(0.4f * dynamicSizeCtrlsHeight));
        auto fixedSizeCtrlsBounds = usableBounds.removeFromTop(fixedSizeCtrlsHeight);
        auto gainCtrlBounds = usableBounds;

        auto controlElementWidth = fixedSizeCtrlsBounds.getWidth() / static_cast<int>(m_OutputMutes.size());
        controlElementWidth = controlElementWidth > maxcontrolElementWidth ? maxcontrolElementWidth : controlElementWidth;

        if (m_OutputLevels)
            m_OutputLevels->setBounds(meterBridgeBounds);

        fixedSizeCtrlsBounds.removeFromTop(5);

        auto muteButtonsBounds = fixedSizeCtrlsBounds.removeFromTop(20);
        for (auto i = 0; i < m_OutputMutes.size(); i++)
        {
            auto const& muteButton = m_OutputMutes.at(i);
            if (!muteButton)
                continue;
            muteButtonsBounds.removeFromLeft(10);
            muteButton->setBounds(muteButtonsBounds.removeFromLeft(controlElementWidth));
        }
    }

    AbstractAudioVisualizer::resized();
}

void OutputControlComponent::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // Paint whatever the base class does ontop of the solid fill
    AbstractAudioVisualizer::paint(g);
}

void OutputControlComponent::setOutputMute(unsigned int channel, bool muteState)
{
    if (channel > m_OutputMutes.size())
        setChannelCount(channel);

    auto muteButtonIter = m_OutputMutes.begin() + channel - 1;
    if (muteButtonIter != m_OutputMutes.end() && muteButtonIter->get())
        muteButtonIter->get()->setToggleState(muteState, juce::dontSendNotification);
}

void OutputControlComponent::processingDataChanged(AbstractProcessorData *data)
{
    if(!data)
        return;

    if (m_OutputLevels)
        m_OutputLevels->processingDataChanged(data);
    
    switch(data->GetDataType())
    {
        case AbstractProcessorData::Level:
            m_levelData = *(dynamic_cast<ProcessorLevelData*>(data));
            notifyChanges();
            break;
        case AbstractProcessorData::AudioSignal:
        case AbstractProcessorData::Spectrum:
        case AbstractProcessorData::Invalid:
        default:
            break;
    }
}

void OutputControlComponent::processChanges()
{
    setChannelCount(static_cast<int>(m_levelData.GetChannelCount()));

    AbstractAudioVisualizer::processChanges();
}

void OutputControlComponent::setChannelCount(int channelCount)
{
    auto resizeRequired = false;

    if (m_OutputMutes.size() != channelCount)
    {
        if (m_OutputMutes.size() < channelCount)
        {
            auto missingCnt = channelCount - m_OutputMutes.size();
            for (; missingCnt > 0; missingCnt--)
            {
                m_OutputMutes.push_back(std::make_unique<TextButton>("M"));
                auto muteButton = m_OutputMutes.back().get();
                muteButton->setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
                muteButton->setClickingTogglesState(true);
                muteButton->onClick = [muteButton, this] {
                    auto foundMuteButtonIter = std::find_if(m_OutputMutes.begin(), m_OutputMutes.end(), [muteButton](std::unique_ptr<TextButton>& b) { return b.get() == muteButton; });
                    if (foundMuteButtonIter == m_OutputMutes.end())
                        return;
                    auto channelIdx = foundMuteButtonIter - m_OutputMutes.begin();
                    auto channel = static_cast<int>(channelIdx + 1);
                    auto muteState = muteButton->getToggleState();
                    outputMuteChange(channel, muteState);
                };
                addAndMakeVisible(*m_OutputMutes.back());
            }
        }
        else if (m_OutputMutes.size() > channelCount)
        {
            auto overheadCnt = m_OutputMutes.size() - channelCount;
            for (; overheadCnt; overheadCnt--)
            {
                removeChildComponent(m_OutputMutes.back().get());
                m_OutputMutes.erase(m_OutputMutes.end());
            }
        }

        resizeRequired = true;
    }

    if (resizeRequired)
        resized();
}

}
