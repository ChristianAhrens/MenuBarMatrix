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

#include "InputControlComponent.h"

#include "AbstractAudioVisualizer.h"
#include "MeterbridgeComponent.h"

namespace MenuBarMatrix
{

//==============================================================================
InputControlComponent::InputControlComponent()
    : AbstractAudioVisualizer(), MenuBarMatrixProcessor::InputCommander()
{
    setUsesValuesInDB(true);

    m_inputLevels = std::make_unique<MeterbridgeComponent>();
    addAndMakeVisible(m_inputLevels.get());
}

InputControlComponent::~InputControlComponent()
{

}

void InputControlComponent::resized()
{
    DBG(__FUNCTION__);

    auto bounds = getLocalBounds();

    if (m_inputLevels)
        m_inputLevels->setBounds(bounds
            .removeFromTop(bounds.getHeight() - (23+1))
            .removeFromLeft(m_inputMutes.size() * 23+1));

    if (!m_inputMutes.empty())
    {
        auto muteWidth = 23+1;
        for (auto const& inputMuteKV : m_inputMutes)
        {
            inputMuteKV.second->setBounds(bounds.removeFromLeft(static_cast<int>(muteWidth)));
        }
    }

    AbstractAudioVisualizer::resized();
}

void InputControlComponent::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // Paint whatever the base class does ontop of the solid fill
    AbstractAudioVisualizer::paint(g);
}

void InputControlComponent::setInputMute(unsigned int channel, bool muteState)
{
    if (m_inputMutes.count(channel) != 1)
        return;

    setChannelCount(channel);

    if (m_inputMutes.at(channel))
        m_inputMutes.at(channel)->setToggleState(muteState, juce::dontSendNotification);
}

void InputControlComponent::processingDataChanged(AbstractProcessorData *data)
{
    if(!data)
        return;
    
    switch(data->GetDataType())
    {
        case AbstractProcessorData::Level:
            m_levelData = *(dynamic_cast<ProcessorLevelData*>(data));
            //DBG(juce::String(__FUNCTION__) << " c:" << m_levelData.GetChannelCount());
            notifyChanges();
            break;
        case AbstractProcessorData::AudioSignal:
        case AbstractProcessorData::Spectrum:
        case AbstractProcessorData::Invalid:
        default:
            break;
    }
}

void InputControlComponent::processChanges()
{
    setChannelCount(static_cast<int>(m_levelData.GetChannelCount()));

    if (m_inputLevels)
        m_inputLevels->processingDataChanged(&m_levelData);

    AbstractAudioVisualizer::processChanges();
}

void InputControlComponent::setChannelCount(int channelCount)
{
    if (m_channelCount != channelCount)
    {
        m_channelCount = channelCount;
        DBG(__FUNCTION__ << " " << channelCount);

        auto channelsToRemove = std::vector<int>();
        for (auto const& inputMuteKV : m_inputMutes)
        {
            if (inputMuteKV.first > channelCount)
                channelsToRemove.push_back(inputMuteKV.first);
        }
        for (auto const& channel : channelsToRemove)
        {
            removeChildComponent(m_inputMutes.at(channel).get());
            auto iter = std::find_if(m_inputMutes.begin(), m_inputMutes.end(), [=](const auto& inputMuteKV) { return inputMuteKV.first == channel; });
            if (iter != m_inputMutes.end())
                m_inputMutes.erase(iter);
        }

        for (int channel = 1; channel <= channelCount; channel++)
        {
            if (m_inputMutes.count(channel) == 0)
            {
                m_inputMutes[channel] = std::make_unique<juce::TextButton>("M", "Mute");
                m_inputMutes.at(channel)->setClickingTogglesState(true);
                m_inputMutes.at(channel)->addListener(this);
                addAndMakeVisible(m_inputMutes.at(channel).get());
            }
        }

        resized();
    }
}

void InputControlComponent::buttonClicked(juce::Button* button)
{
    auto iter = std::find_if(m_inputMutes.begin(), m_inputMutes.end(), [=](const auto& inputMuteKV) { return inputMuteKV.second.get() == button; });
    if (iter != m_inputMutes.end() && nullptr != iter->second)
        inputMuteChange(iter->first, iter->second->getToggleState());
}

}
