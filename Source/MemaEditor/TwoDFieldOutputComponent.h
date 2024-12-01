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

#include "AbstractAudioVisualizer.h"

namespace Mema
{

//==============================================================================
/*
*/
class TwoDFieldOutputComponent  :   public AbstractAudioVisualizer
{
public:
    TwoDFieldOutputComponent();
    ~TwoDFieldOutputComponent();

    bool setChannelConfiguration(const juce::AudioChannelSet& channelLayout);
    const juce::Array<juce::AudioChannelSet>& getSupportedChannelConfigurations();

    float getAngleForChannelTypeInCurrentConfiguration(const juce::AudioChannelSet::ChannelType& channelType);
    int getChannelNumberForChannelTypeInCurrentConfiguration(const juce::AudioChannelSet::ChannelType& channelType);
    void setClockwiseOrderedChannelTypesForCurrentConfiguration();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void processingDataChanged(AbstractProcessorData *data) override;

private:
    float   m_outerMargin{ 20.0f };
    float   m_visuAreaWidth{ 0.0f };
    float   m_visuAreaHeight{ 0.0f };
    float   m_visuAreaOrigX{ 0.0f };
    float   m_visuAreaOrigY{ 0.0f };
    juce::Rectangle<float>              m_visuArea;
    juce::Point<float>                  m_levelOrig;
    std::map<int, juce::Point<float>>   m_channelLevelMaxPoints;

    juce::AudioChannelSet                           m_channelConfiguration;
    juce::Array<juce::AudioChannelSet::ChannelType> m_clockwiseOrderedChannelTypes;
    juce::Array<juce::AudioChannelSet>              m_supportedChannelConfigurations = { 
        juce::AudioChannelSet::mono(),
        juce::AudioChannelSet::stereo(),
        juce::AudioChannelSet::createLCR(),
        juce::AudioChannelSet::createLCRS(),
        juce::AudioChannelSet::createLRS(),
        juce::AudioChannelSet::create5point1(),
        juce::AudioChannelSet::create7point1() };

    ProcessorLevelData  m_levelData;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TwoDFieldOutputComponent)
};

}