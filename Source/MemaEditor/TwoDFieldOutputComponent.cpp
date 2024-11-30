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

#include "TwoDFieldOutputComponent.h"


namespace Mema
{

//#define PAINTINGHELPER

//==============================================================================
TwoDFieldOutputComponent::TwoDFieldOutputComponent()
    :   AbstractAudioVisualizer()
{
    setUsesValuesInDB(true);
}

TwoDFieldOutputComponent::~TwoDFieldOutputComponent()
{
}

void TwoDFieldOutputComponent::paint (Graphics& g)
{
    AbstractAudioVisualizer::paint(g);

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // fill circle background
    g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId).darker());
    g.fillEllipse(m_visuArea);

#if defined DEBUG && defined PAINTINGHELPER
    g.setColour(juce::Colours::red);
    g.drawRect(m_visuArea);
    g.setColour(juce::Colours::blue);
    g.drawRect(getLocalBounds());
#endif

    // draw level indication lines
    std::map<int, juce::Point<float>> maxPoints;
    for (auto const& channelType : m_channelConfiguration.getChannelTypes())
        maxPoints[channelType] = m_levelOrig - m_channelLevelMaxPoints[channelType];

    // hold values
    std::map<int, float> holdLevels;
    if (getUsesValuesInDB())
    {
        for (auto const& channelType : m_channelConfiguration.getChannelTypes())
            holdLevels[channelType] = m_levelData.GetLevel(channelType).GetFactorHOLDdB();
    }
    else
    {
        for (auto const& channelType : m_channelConfiguration.getChannelTypes())
            holdLevels[channelType] = m_levelData.GetLevel(channelType).hold;
    }

    g.setColour(juce::Colours::grey);
    juce::Path holdPath;
    auto holdPathStarted = false;
    for (auto const& channelType : m_channelConfiguration.getChannelTypes())
    {
        if (!holdPathStarted)
        {
            holdPath.startNewSubPath(m_levelOrig - maxPoints[channelType] * holdLevels[channelType]);
            holdPathStarted = true;
        }
        else
            holdPath.lineTo(m_levelOrig - maxPoints[channelType] * holdLevels[channelType]);
    }
    g.strokePath(holdPath, PathStrokeType(1));
#if defined DEBUG && defined PAINTINGHELPER
    g.setColour(juce::Colours::yellow);
    g.drawRect(holdPath.getBounds());
#endif

    // peak values
    std::map<int, float> peakLevels;
    if (getUsesValuesInDB())
    {
        for (auto const& channelType : m_channelConfiguration.getChannelTypes())
            peakLevels[channelType] = m_levelData.GetLevel(channelType).GetFactorPEAKdB();
    }
    else
    {
        for (auto const& channelType : m_channelConfiguration.getChannelTypes())
            peakLevels[channelType] = m_levelData.GetLevel(channelType).peak;
    }

    g.setColour(juce::Colours::forestgreen.darker());
    juce::Path peakPath;
    auto peakPathStarted = false;
    for (auto const& channelType : m_channelConfiguration.getChannelTypes())
    {
        if (!peakPathStarted)
        {
            peakPath.startNewSubPath(m_levelOrig - maxPoints[channelType] * peakLevels[channelType]);
            peakPathStarted = true;
        }
        else
            peakPath.lineTo(m_levelOrig - maxPoints[channelType] * peakLevels[channelType]);
    }
    g.fillPath(peakPath);
#if defined DEBUG && defined PAINTINGHELPER
    g.setColour(juce::Colours::orange);
    g.drawRect(peakPath.getBounds());
#endif

    // rms values    
    std::map<int, float> rmsLevels;
    if (getUsesValuesInDB())
    {
        for (auto const& channelType : m_channelConfiguration.getChannelTypes())
            rmsLevels[channelType] = m_levelData.GetLevel(channelType).GetFactorRMSdB();
    }
    else
    {
        for (auto const& channelType : m_channelConfiguration.getChannelTypes())
            rmsLevels[channelType] = m_levelData.GetLevel(channelType).rms;
    }

    g.setColour(juce::Colours::forestgreen);
    juce::Path rmsPath;
    auto rmsPathStarted = false;
    for (auto const& channelType : m_channelConfiguration.getChannelTypes())
    {
        if (!rmsPathStarted)
        {
            rmsPath.startNewSubPath(m_levelOrig - maxPoints[channelType] * rmsLevels[channelType]);
            rmsPathStarted = true;
        }
        else
            rmsPath.lineTo(m_levelOrig - maxPoints[channelType] * rmsLevels[channelType]);
    }
    g.fillPath(rmsPath);
#if defined DEBUG && defined PAINTINGHELPER
    g.setColour(juce::Colours::turquoise);
    g.drawRect(rmsPath.getBounds());
#endif

    // draw a simple circle surrounding
    g.setColour(getLookAndFeel().findColour(juce::TextButton::textColourOffId));
    g.drawEllipse(m_visuArea, 1);

    // draw dashed field dimension indication lines
    float dparam[]{ 4.0f, 5.0f };
    for (auto const& channelType : m_channelConfiguration.getChannelTypes())
        g.drawDashedLine(juce::Line<float>(m_channelLevelMaxPoints[channelType], m_levelOrig), dparam, 2);

    //// draw L C R LS RS legend
    //auto textRectSize = juce::Point<float>(20.0f, 20.0f);
    //auto gainTextRectWidth = juce::Point<float>(40.0f, 0.0f);
    //auto textRectSizeInv = juce::Point<float>(20.0f, -20.0f);
    //
    //auto textLRect = juce::Rectangle<float>(m_leftMaxPoint, m_leftMaxPoint - textRectSize - gainTextRectWidth);
    //g.setColour(Colours::white);
    //g.drawText("L", textLRect, Justification::centred, true);
    //
    //auto textCRect = juce::Rectangle<float>(m_centerMaxPoint + 0.5f * gainTextRectWidth, m_centerMaxPoint - textRectSize - 0.5f * gainTextRectWidth) + juce::Point<float>(0.5f * textRectSize.getX(), 0.0f);
    //g.setColour(Colours::white);
    //g.drawText("C", textCRect, Justification::centred, true);
    //
    //auto textRRect = juce::Rectangle<float>(m_rightMaxPoint, m_rightMaxPoint + textRectSizeInv + gainTextRectWidth);
    //g.setColour(Colours::white);
    //g.drawText("R", textRRect, Justification::centred, true);
    //
    //auto textLSRect = juce::Rectangle<float>(m_leftSurroundMaxPoint, m_leftSurroundMaxPoint - textRectSizeInv - gainTextRectWidth);
    //g.setColour(Colours::white);
    //g.drawText("LS", textLSRect, Justification::centred, true);
    //
    //auto textRSRect = juce::Rectangle<float>(m_rightSurroundMaxPoint, m_rightSurroundMaxPoint + textRectSize + gainTextRectWidth);
    //g.setColour(Colours::white);
    //g.drawText("RS", textRSRect, Justification::centred, true);

#if defined DEBUG && defined PAINTINGHELPER
    //g.setColour(juce::Colours::lightblue);
    //g.drawRect(textLRect);
    //g.drawRect(textCRect);
    //g.drawRect(textRRect);
    //g.drawRect(textLSRect);
    //g.drawRect(textRSRect);
#endif

    // draw dBFS
    g.setFont(12.0f);
    g.setColour(getLookAndFeel().findColour(juce::TextButton::textColourOffId));
    String rangeText;
    if (getUsesValuesInDB())
        rangeText = juce::String(MemaProcessor::getGlobalMindB()) + " ... " + juce::String(MemaProcessor::getGlobalMaxdB()) + " dBFS";
    else
        rangeText = "0 ... 1";
    g.drawText(rangeText, getLocalBounds(), juce::Justification::topRight, true);


}

void TwoDFieldOutputComponent::resized()
{
    // calculate what we need for our center circle
    auto width = getWidth();
    auto height = getHeight();

    m_visuAreaWidth = static_cast<float>(width < height ? width : height) - 2 * m_outerMargin;
    m_visuAreaHeight = static_cast<float>(width < height ? width : height) - 2 * m_outerMargin;

    m_visuAreaOrigX = float(0.5f * (width - m_visuAreaWidth));
    m_visuAreaOrigY = height - float(0.5f * (height - m_visuAreaHeight));

    m_visuArea = juce::Rectangle<float>(m_visuAreaOrigX, m_visuAreaOrigY - m_visuAreaHeight, m_visuAreaWidth, m_visuAreaHeight);

    auto visuAreaHalfHeight = m_visuAreaHeight * 0.5f;
    auto visuAreaHalfWidth = m_visuAreaWidth * 0.5f;

    m_levelOrig = juce::Point<float>(m_visuAreaOrigX + 0.5f * m_visuAreaWidth, m_visuAreaOrigY - 0.5f * m_visuAreaHeight);

    for (auto const& channelType : m_channelConfiguration.getChannelTypes())
    {
        auto xLength = cosf(juce::MathConstants<float>::pi / 180.0f * (getAngleForChannelTypeInCurrentConfiguration(channelType) + 90.0f)) * visuAreaHalfWidth;
        auto yLength = sinf(juce::MathConstants<float>::pi / 180.0f * (getAngleForChannelTypeInCurrentConfiguration(channelType) + 90.0f)) * visuAreaHalfHeight;
        m_channelLevelMaxPoints[channelType] = m_levelOrig + juce::Point<float>(xLength, -yLength);
    }

    AbstractAudioVisualizer::resized();
}

void TwoDFieldOutputComponent::processingDataChanged(AbstractProcessorData *data)
{
    if(!data)
        return;
    
    switch(data->GetDataType())
    {
        case AbstractProcessorData::Level:
            m_levelData = *(static_cast<ProcessorLevelData*>(data));
            notifyChanges();
            break;
        case AbstractProcessorData::AudioSignal:
        case AbstractProcessorData::Spectrum:
        case AbstractProcessorData::Invalid:
        default:
            break;
    }
}

bool TwoDFieldOutputComponent::setChannelConfiguration(const juce::AudioChannelSet& channelLayout)
{
    if (getSupportedChannelConfigurations().contains(channelLayout))
    {
        m_channelConfiguration = channelLayout;
        return true;
    }
    else
    {
        m_channelConfiguration = juce::AudioChannelSet::mono();
        return false;
    }
}

const juce::Array<juce::AudioChannelSet>& TwoDFieldOutputComponent::getSupportedChannelConfigurations()
{
    return m_supportedChannelConfigurations;
}

float TwoDFieldOutputComponent::getAngleForChannelTypeInCurrentConfiguration(const juce::AudioChannelSet::ChannelType& channelType)
{
    if (juce::AudioChannelSet::mono() == m_channelConfiguration)
    {
        switch (channelType)
        {
        case juce::AudioChannelSet::ChannelType::centre:
            return 0.0f;
        default:
            jassertfalse;
        }
    }
    else if (juce::AudioChannelSet::stereo() == m_channelConfiguration)
    {
        switch (channelType)
        {
        case juce::AudioChannelSet::ChannelType::left:
            return -30.0f;
        case juce::AudioChannelSet::ChannelType::right:
            return 30.0f;
        default:
            jassertfalse;
        }
    }
    else if (juce::AudioChannelSet::createLCR() == m_channelConfiguration)
    {
        switch (channelType)
        {
        case juce::AudioChannelSet::ChannelType::left:
            return -30.0f;
        case juce::AudioChannelSet::ChannelType::right:
            return 30.0f;
        case juce::AudioChannelSet::ChannelType::centre:
            return 0.0f;
        default:
            jassertfalse;
        }
    }
    else if (juce::AudioChannelSet::createLCRS() == m_channelConfiguration)
    {
        switch (channelType)
        {
        case juce::AudioChannelSet::ChannelType::left:
            return -30.0f;
        case juce::AudioChannelSet::ChannelType::right:
            return 30.0f;
        case juce::AudioChannelSet::ChannelType::centre:
            return 0.0f;
        case juce::AudioChannelSet::ChannelType::surround:
            return 180.0f;
        default:
            jassertfalse;
        }
    }
    else if (juce::AudioChannelSet::createLRS() == m_channelConfiguration)
    {
        switch (channelType)
        {
        case juce::AudioChannelSet::ChannelType::left:
            return -30.0f;
        case juce::AudioChannelSet::ChannelType::right:
            return 30.0f;
        case juce::AudioChannelSet::ChannelType::surround:
            return 180.0f;
        default:
            jassertfalse;
        }
    }
    else if (juce::AudioChannelSet::create5point1() == m_channelConfiguration)
    {
        switch (channelType)
        {
        case juce::AudioChannelSet::ChannelType::left:
            return -30.0f;
        case juce::AudioChannelSet::ChannelType::right:
            return 30.0f;
        case juce::AudioChannelSet::ChannelType::centre:
            return 0.0f;
        case juce::AudioChannelSet::ChannelType::LFE:
            return 0.0f;
        case juce::AudioChannelSet::ChannelType::leftSurround:
            return -110.0f;
        case juce::AudioChannelSet::ChannelType::rightSurround:
            return 110.0f;
        default:
            jassertfalse;
        }
    }
    else if (juce::AudioChannelSet::create7point1() == m_channelConfiguration)
    {
        switch (channelType)
        {
        case juce::AudioChannelSet::ChannelType::left:
            return -30.0f;
        case juce::AudioChannelSet::ChannelType::right:
            return 30.0f;
        case juce::AudioChannelSet::ChannelType::centre:
            return 0.0f;
        case juce::AudioChannelSet::ChannelType::LFE:
            return 0.0f;
        case juce::AudioChannelSet::ChannelType::leftSurroundSide:
            return -100.0f;
        case juce::AudioChannelSet::ChannelType::rightSurroundSide:
            return 100.0f;
        case juce::AudioChannelSet::ChannelType::leftSurroundRear:
            return -145.0f;
        case juce::AudioChannelSet::ChannelType::rightSurroundRear:
            return 145.0f;
        default:
            jassertfalse;
        }
    }
    else
        jassertfalse;

    return 0.0f;
}


}
