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

#include "InputMixerComponent.h"

#include "AbstractAudioVisualizer.h"
#include "MeterbridgeComponent.h"
#include "PositionEditorComponent.h"

namespace MenuBarMatrix
{

//==============================================================================
InputMixerComponent::InputMixerComponent()
    : AbstractAudioVisualizer(), MenuBarMatrixProcessor::InputCommander()
{
    setUsesValuesInDB(true);

    m_inputLevels = std::make_unique<MeterbridgeComponent>();
    addAndMakeVisible(m_inputLevels.get());
}

InputMixerComponent::~InputMixerComponent()
{

}

void InputMixerComponent::resized()
{
    if (!m_inputMutes.empty() && m_inputMutes.size() == m_inputGains.size() && m_inputMutes.size() == m_inputPositions.size())
    {
        auto usableBounds = getLocalBounds().reduced(15);

        auto maxcontrolElementWidth = 30;
        auto fixedSizeCtrlsHeight = 130;
        auto dynamicSizeCtrlsHeight = usableBounds.getHeight() - fixedSizeCtrlsHeight;

        auto meterBridgeBounds = usableBounds.removeFromTop(static_cast<int>(0.4f * dynamicSizeCtrlsHeight));
        auto fixedSizeCtrlsBounds = usableBounds.removeFromTop(fixedSizeCtrlsHeight);
        auto gainCtrlBounds = usableBounds;

        auto controlElementWidth = fixedSizeCtrlsBounds.getWidth() / static_cast<int>(m_inputMutes.size());
        controlElementWidth = controlElementWidth > maxcontrolElementWidth ? maxcontrolElementWidth : controlElementWidth;

        if (m_inputLevels)
            m_inputLevels->setBounds(meterBridgeBounds);

        fixedSizeCtrlsBounds.removeFromTop(5);

        auto reverbSlidersBounds = fixedSizeCtrlsBounds.removeFromTop(maxcontrolElementWidth);
        for (auto i = 0; i < m_inputReverbs.size(); i++)
        {
            auto const& reverbSlider = m_inputReverbs.at(i);
            if (!reverbSlider)
                continue;
            reverbSlidersBounds.removeFromLeft(10);
            auto reverbSliderBounds = reverbSlidersBounds.removeFromLeft(controlElementWidth);
            reverbSliderBounds.expand(7, 7); // weird enough, this is neccessary to make the rotary fit the given bounds nicely...
            reverbSlider->setBounds(reverbSliderBounds);
        }

        fixedSizeCtrlsBounds.removeFromTop(5);

        auto spreadSlidersBounds = fixedSizeCtrlsBounds.removeFromTop(maxcontrolElementWidth);
        for (auto i = 0; i < m_inputSpreads.size(); i++)
        {
            auto const& spreadSlider = m_inputSpreads.at(i);
            if (!spreadSlider)
                continue;
            spreadSlidersBounds.removeFromLeft(10);
            auto spreadSliderBounds = spreadSlidersBounds.removeFromLeft(controlElementWidth);
            spreadSliderBounds.expand(7, 7); // weird enough, this is neccessary to make the rotary fit the given bounds nicely...
            spreadSlider->setBounds(spreadSliderBounds);
        }

        fixedSizeCtrlsBounds.removeFromTop(5);

        auto positionComponentsBounds = fixedSizeCtrlsBounds.removeFromTop(maxcontrolElementWidth);
        for (auto i = 0; i < m_inputPositions.size(); i++)
        {
            auto const& positionComponent = m_inputPositions.at(i);
            if (!positionComponent)
                continue;
            positionComponentsBounds.removeFromLeft(10);
            positionComponent->setBounds(positionComponentsBounds.removeFromLeft(controlElementWidth));
        }

        fixedSizeCtrlsBounds.removeFromTop(5);

        auto muteButtonsBounds = fixedSizeCtrlsBounds.removeFromTop(20);
        for (auto i = 0; i < m_inputMutes.size(); i++)
        {
            auto const& muteButton = m_inputMutes.at(i);
            if (!muteButton)
                continue;
            muteButtonsBounds.removeFromLeft(10);
            muteButton->setBounds(muteButtonsBounds.removeFromLeft(controlElementWidth));
        }

        auto gainSlidersBounds = gainCtrlBounds;
        for (auto i = 0; i < m_inputGains.size(); i++)
        {
            auto const& gainSlider = m_inputGains.at(i);
            if (!gainSlider)
                continue;
            gainSlidersBounds.removeFromLeft(10);
            gainSlider->setBounds(gainSlidersBounds.removeFromLeft(controlElementWidth));
        }
    }

    AbstractAudioVisualizer::resized();
}

void InputMixerComponent::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // Paint whatever the base class does ontop of the solid fill
    AbstractAudioVisualizer::paint(g);

    // and draw text indication on spread/reverb sliders
    if (m_inputReverbs.empty())
        return;

    g.setColour(getLookAndFeel().findColour(TextButton::textColourOnId));
    g.setFont(11);

    auto usableBounds = getLocalBounds().reduced(15);

    auto maxcontrolElementWidth = 30;
    auto fixedSizeCtrlsHeight = 130;
    auto fixedSizeCtrlsBounds = usableBounds.removeFromTop(fixedSizeCtrlsHeight);

    auto controlElementWidth = fixedSizeCtrlsBounds.getWidth() / static_cast<int>(m_inputReverbs.size());
    controlElementWidth = controlElementWidth > maxcontrolElementWidth ? maxcontrolElementWidth : controlElementWidth;

    fixedSizeCtrlsBounds.removeFromTop(5);

    auto reverbSlidersBounds = fixedSizeCtrlsBounds.removeFromTop(maxcontrolElementWidth - 1);
    for (auto i = 0; i < m_inputReverbs.size(); i++)
    {
        reverbSlidersBounds.removeFromLeft(10);
        auto reverbSliderBounds = reverbSlidersBounds.removeFromLeft(controlElementWidth);
        g.drawText("rv", reverbSliderBounds, juce::Justification::centred);
    }

    fixedSizeCtrlsBounds.removeFromTop(5);

    auto spreadSlidersBounds = fixedSizeCtrlsBounds.removeFromTop(maxcontrolElementWidth);
    for (auto i = 0; i < m_inputSpreads.size(); i++)
    {
        auto const& spreadSlider = m_inputSpreads.at(i);
        if (!spreadSlider)
            continue;
        spreadSlidersBounds.removeFromLeft(10);
        auto spreadSliderBounds = spreadSlidersBounds.removeFromLeft(controlElementWidth);
        g.drawText("s", spreadSliderBounds, juce::Justification::centred);
    }
}

void InputMixerComponent::setInputMute(unsigned int channel, bool muteState)
{
    if (channel > m_inputMutes.size())
        setChannelCount(channel);

    auto muteButtonIter = m_inputMutes.begin() + channel - 1;
    if (muteButtonIter != m_inputMutes.end() && muteButtonIter->get())
        muteButtonIter->get()->setToggleState(muteState, juce::dontSendNotification);
}

void InputMixerComponent::setInputGain(unsigned int channel, float gainValue)
{
    if (channel > m_inputGains.size())
        setChannelCount(channel);

    auto gainSliderIter = m_inputGains.begin() + channel - 1;
    if (gainSliderIter != m_inputGains.end() && gainSliderIter->get())
        gainSliderIter->get()->setValue(gainValue, juce::dontSendNotification);
}

void InputMixerComponent::setInputPosition(unsigned int channel, juce::Point<float> position)
{
    if (channel > m_inputPositions.size())
        setChannelCount(channel);

    auto positionComponentIter = m_inputPositions.begin() + channel - 1;
    if (positionComponentIter != m_inputPositions.end() && positionComponentIter->get())
        positionComponentIter->get()->setCurrentPosition(position);
}

void InputMixerComponent::setInputSpread(unsigned int channel, float spreadValue)
{
    if (channel > m_inputSpreads.size())
        setChannelCount(channel);

    auto spreadSliderIter = m_inputSpreads.begin() + channel - 1;
    if (spreadSliderIter != m_inputSpreads.end() && spreadSliderIter->get())
        spreadSliderIter->get()->setValue(spreadValue, juce::dontSendNotification);
}

void InputMixerComponent::setInputReverb(unsigned int channel, float reverbValue)
{
    if (channel > m_inputReverbs.size())
        setChannelCount(channel);

    auto reverbSliderIter = m_inputReverbs.begin() + channel - 1;
    if (reverbSliderIter != m_inputReverbs.end() && reverbSliderIter->get())
        reverbSliderIter->get()->setValue(reverbValue, juce::dontSendNotification);
}

void InputMixerComponent::processingDataChanged(AbstractProcessorData *data)
{
    if(!data)
        return;

    if (m_inputLevels)
        m_inputLevels->processingDataChanged(data);
    
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

void InputMixerComponent::processChanges()
{
    setChannelCount(static_cast<int>(m_levelData.GetChannelCount()));

    AbstractAudioVisualizer::processChanges();
}

void InputMixerComponent::setChannelCount(int channelCount)
{
    auto resizeRequired = false;

    if (m_inputMutes.size() != channelCount)
    {
        if (m_inputMutes.size() < channelCount)
        {
            auto missingCnt = channelCount - m_inputMutes.size();
            for (; missingCnt > 0; missingCnt--)
            {
                m_inputMutes.push_back(std::make_unique<TextButton>("M"));
                auto muteButton = m_inputMutes.back().get();
                muteButton->setColour(TextButton::ColourIds::buttonOnColourId, juce::Colours::red);
                muteButton->setClickingTogglesState(true);
                muteButton->onClick = [muteButton, this] {
                    auto foundMuteButtonIter = std::find_if(m_inputMutes.begin(), m_inputMutes.end(), [muteButton](std::unique_ptr<TextButton>& b) { return b.get() == muteButton; });
                    if (foundMuteButtonIter == m_inputMutes.end())
                        return;
                    auto channelIdx = foundMuteButtonIter - m_inputMutes.begin();
                    auto channel = static_cast<int>(channelIdx + 1);
                    auto muteState = muteButton->getToggleState();
                    inputMuteChange(channel, muteState);
                };
                addAndMakeVisible(*m_inputMutes.back());
            }
        }
        else if (m_inputMutes.size() > channelCount)
        {
            auto overheadCnt = m_inputMutes.size() - channelCount;
            for (; overheadCnt; overheadCnt--)
            {
                removeChildComponent(m_inputMutes.back().get());
                m_inputMutes.erase(m_inputMutes.end());
            }
        }

        resizeRequired = true;
    }

    if (m_inputReverbs.size() != channelCount)
    {
        if (m_inputReverbs.size() < channelCount)
        {
            auto missingCnt = channelCount - m_inputReverbs.size();
            for (; missingCnt > 0; missingCnt--)
            {
                m_inputReverbs.push_back(std::make_unique<Slider>("Rv"));
                auto rvGainSlider = m_inputReverbs.back().get();
                rvGainSlider->setSliderStyle(juce::Slider::SliderStyle::Rotary);
                rvGainSlider->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
                rvGainSlider->setRange(0.0, 1.0);
                rvGainSlider->setValue(1.0, dontSendNotification);
                rvGainSlider->onValueChange = [rvGainSlider, this] {
                    auto foundGainSliderIter = std::find_if(m_inputReverbs.begin(), m_inputReverbs.end(), [rvGainSlider](std::unique_ptr<Slider>& b) { return b.get() == rvGainSlider; });
                    if (foundGainSliderIter == m_inputReverbs.end())
                        return;
                    auto channelIdx = foundGainSliderIter - m_inputReverbs.begin();
                    auto channel = static_cast<int>(channelIdx + 1);
                    auto gainValue = static_cast<float>(rvGainSlider->getValue());
                    inputReverbChange(channel, gainValue);
                };
                addAndMakeVisible(*m_inputReverbs.back());
            }
        }
        else if (m_inputReverbs.size() > channelCount)
        {
            auto overheadCnt = m_inputReverbs.size() - channelCount;
            for (; overheadCnt; overheadCnt--)
            {
                removeChildComponent(m_inputReverbs.back().get());
                m_inputReverbs.erase(m_inputReverbs.end());
            }
        }

        resizeRequired = true;
    }

    if (m_inputSpreads.size() != channelCount)
    {
        if (m_inputSpreads.size() < channelCount)
        {
            auto missingCnt = channelCount - m_inputSpreads.size();
            for (; missingCnt > 0; missingCnt--)
            {
                m_inputSpreads.push_back(std::make_unique<Slider>("S"));
                auto spreadSlider = m_inputSpreads.back().get();
                spreadSlider->setSliderStyle(juce::Slider::SliderStyle::Rotary);
                spreadSlider->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
                spreadSlider->setRange(0.0, 1.0);
                spreadSlider->setValue(1.0, dontSendNotification);
                spreadSlider->onValueChange = [spreadSlider, this] {
                    auto foundSpreadSliderIter = std::find_if(m_inputSpreads.begin(), m_inputSpreads.end(), [spreadSlider](std::unique_ptr<Slider>& b) { return b.get() == spreadSlider; });
                    if (foundSpreadSliderIter == m_inputSpreads.end())
                        return;
                    auto channelIdx = foundSpreadSliderIter - m_inputSpreads.begin();
                    auto channel = static_cast<int>(channelIdx + 1);
                    auto spreadValue = static_cast<float>(spreadSlider->getValue());
                    inputSpreadChange(channel, spreadValue);
                };
                addAndMakeVisible(*m_inputSpreads.back());
            }
        }
        else if (m_inputSpreads.size() > channelCount)
        {
            auto overheadCnt = m_inputSpreads.size() - channelCount;
            for (; overheadCnt; overheadCnt--)
            {
                removeChildComponent(m_inputSpreads.back().get());
                m_inputSpreads.erase(m_inputSpreads.end());
            }
        }

        resizeRequired = true;
    }

    if (m_inputPositions.size() != channelCount)
    {
        if (m_inputPositions.size() < channelCount)
        {
            auto missingCnt = channelCount - m_inputPositions.size();
            for (; missingCnt > 0; missingCnt--)
            {
                m_inputPositions.push_back(std::make_unique<PositionEditorComponent>());
                auto positionComponent = m_inputPositions.back().get();
                positionComponent->setCurrentPosition(juce::Point<float>(0.5f, 0.5f));
                positionComponent->setPositionCallback = [positionComponent, this](juce::Component*, const juce::Point<float>& position) {
                    auto foundPositionComponentIter = std::find_if(m_inputPositions.begin(), m_inputPositions.end(), [positionComponent](std::unique_ptr<PositionEditorComponent>& b) { return b.get() == positionComponent; });
                    if (foundPositionComponentIter == m_inputPositions.end())
                        return;
                    auto channelIdx = foundPositionComponentIter - m_inputPositions.begin();
                    auto channel = static_cast<int>(channelIdx + 1);
                    inputPositionChange(channel, position);
                };
                addAndMakeVisible(*m_inputPositions.back());
            }
        }
        else if (m_inputPositions.size() > channelCount)
        {
            auto overheadCnt = m_inputPositions.size() - channelCount;
            for (; overheadCnt; overheadCnt--)
            {
                removeChildComponent(m_inputPositions.back().get());
                m_inputPositions.erase(m_inputPositions.end());
            }
        }

        resizeRequired = true;
    }

    if (m_inputGains.size() != channelCount)
    {
        if (m_inputGains.size() < channelCount)
        {
            auto missingCnt = channelCount - m_inputGains.size();
            for (; missingCnt > 0; missingCnt--)
            {
                m_inputGains.push_back(std::make_unique<Slider>("M"));
                auto gainSlider = m_inputGains.back().get();
                gainSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
                gainSlider->setRange(0.0, 1.0);
                gainSlider->setValue(1.0, dontSendNotification);
                gainSlider->onValueChange = [gainSlider, this] {
                    auto foundGainSliderIter = std::find_if(m_inputGains.begin(), m_inputGains.end(), [gainSlider](std::unique_ptr<Slider>& b) { return b.get() == gainSlider; });
                    if (foundGainSliderIter == m_inputGains.end())
                        return;
                    auto channelIdx = foundGainSliderIter - m_inputGains.begin();
                    auto channel = static_cast<int>(channelIdx + 1);
                    auto gainValue = static_cast<float>(gainSlider->getValue());
                    inputGainChange(channel, gainValue);
                };
                addAndMakeVisible(*m_inputGains.back());
            }
        }
        else if (m_inputGains.size() > channelCount)
        {
            auto overheadCnt = m_inputGains.size() - channelCount;
            for (; overheadCnt; overheadCnt--)
            {
                removeChildComponent(m_inputGains.back().get());
                m_inputGains.erase(m_inputGains.end());
            }
        }

        resizeRequired = true;
    }

    if (resizeRequired)
        resized();
}

}
