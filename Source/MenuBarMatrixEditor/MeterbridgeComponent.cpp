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

#include "MeterbridgeComponent.h"

namespace MenuBarMatrix
{

//==============================================================================
MeterbridgeComponent::MeterbridgeComponent()
    : AbstractAudioVisualizer()
{
    setUsesValuesInDB(true);
}

MeterbridgeComponent::MeterbridgeComponent(Direction direction)
    : MeterbridgeComponent()
{
    setDirection(direction);
}

MeterbridgeComponent::~MeterbridgeComponent()
{

}

void MeterbridgeComponent::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());

	// calculate what we need for our center circle
	auto margin = 20;
	auto visuAreaWidth = static_cast<float>(getWidth());
	auto visuAreaHeight = static_cast<float>(getHeight());

    if (m_direction == Direction::Horizontal)
    {
        auto visuArea = getLocalBounds();
        auto visuAreaOrigY = visuAreaHeight;

        // draw meters
        auto meterSpacing = margin * 0.5f;
        auto meterThickness = (visuArea.getWidth() - (m_levelData.GetChannelCount() + 1) * meterSpacing) / m_levelData.GetChannelCount();
        auto meterMaxLength = visuArea.getHeight();
        auto meterLeft = meterSpacing;

        g.setFont(14.0f);
        for (unsigned long i = 1; i <= m_levelData.GetChannelCount(); ++i)
        {
            auto level = m_levelData.GetLevel(i);
            float peakMeterLength{ 0 };
            float rmsMeterLength{ 0 };
            float holdMeterLength{ 0 };
            if (getUsesValuesInDB())
            {
                peakMeterLength = meterMaxLength * level.GetFactorPEAKdB();
                rmsMeterLength = meterMaxLength * level.GetFactorRMSdB();
                holdMeterLength = meterMaxLength * level.GetFactorHOLDdB();
            }
            else
            {
                peakMeterLength = meterMaxLength * level.peak;
                rmsMeterLength = meterMaxLength * level.rms;
                holdMeterLength = meterMaxLength * level.hold;
            }

            // peak bar
            g.setColour(juce::Colours::forestgreen.darker());
            g.fillRect(juce::Rectangle<float>(meterLeft, visuAreaOrigY - peakMeterLength, meterThickness, peakMeterLength));
            // rms bar
            g.setColour(Colours::forestgreen);
            g.fillRect(juce::Rectangle<float>(meterLeft, visuAreaOrigY - rmsMeterLength, meterThickness, rmsMeterLength));
            // hold strip
            g.setColour(getLookAndFeel().findColour(juce::AlertWindow::backgroundColourId));
            g.drawLine(juce::Line<float>(meterLeft, visuAreaOrigY - holdMeterLength, meterLeft + meterThickness, visuAreaOrigY - holdMeterLength));
            // channel # label
            g.setColour(getLookAndFeel().findColour(juce::AlertWindow::backgroundColourId));
            g.drawText(juce::String(i), juce::Rectangle<float>(meterLeft, visuAreaOrigY, meterThickness, float(margin)), juce::Justification::centred, true);

            meterLeft += meterThickness + meterSpacing;
        }

        // draw a simple baseline
        g.setColour(getLookAndFeel().findColour(juce::AlertWindow::backgroundColourId));
        g.drawLine(juce::Line<float>(0.0f, visuAreaOrigY, visuAreaWidth, visuAreaOrigY));
        // draw dBFS
        g.setFont(12.0f);
        g.setColour(getLookAndFeel().findColour(juce::AlertWindow::backgroundColourId));
        juce::String rangeText;
        if (getUsesValuesInDB())
            rangeText = juce::String(MenuBarMatrixProcessor::getGlobalMindB()) + " ... " + juce::String(MenuBarMatrixProcessor::getGlobalMaxdB()) + " dBFS";
        else
            rangeText = "0 ... 1";
        g.drawText(rangeText, visuArea, juce::Justification::topRight, true);
    }
    else
    {
        auto visuArea = getLocalBounds();
        auto visuAreaOrigX = 0.0f;

        // draw meters
        auto meterSpacing = margin * 0.5f;
        auto meterThickness = (visuArea.getHeight() - (m_levelData.GetChannelCount() + 1) * meterSpacing) / m_levelData.GetChannelCount();
        auto meterMaxLength = visuArea.getWidth();
        auto meterTop = meterSpacing;

        g.setFont(14.0f);
        for (unsigned long i = 1; i <= m_levelData.GetChannelCount(); ++i)
        {
            auto level = m_levelData.GetLevel(i);
            float peakMeterLength{ 0 };
            float rmsMeterLength{ 0 };
            float holdMeterLength{ 0 };
            if (getUsesValuesInDB())
            {
                peakMeterLength = meterMaxLength * level.GetFactorPEAKdB();
                rmsMeterLength = meterMaxLength * level.GetFactorRMSdB();
                holdMeterLength = meterMaxLength * level.GetFactorHOLDdB();
            }
            else
            {
                peakMeterLength = meterMaxLength * level.peak;
                rmsMeterLength = meterMaxLength * level.rms;
                holdMeterLength = meterMaxLength * level.hold;
            }

            // peak bar
            g.setColour(juce::Colours::forestgreen.darker());
            g.fillRect(juce::Rectangle<float>(visuAreaOrigX - peakMeterLength, meterTop, peakMeterLength, meterThickness));
            // rms bar
            g.setColour(Colours::forestgreen);
            g.fillRect(juce::Rectangle<float>(visuAreaOrigX - rmsMeterLength, meterTop, meterThickness, rmsMeterLength));
            // hold strip
            g.setColour(getLookAndFeel().findColour(juce::AlertWindow::backgroundColourId));
            g.drawLine(juce::Line<float>(visuAreaOrigX - holdMeterLength, meterTop, visuAreaOrigX - holdMeterLength, meterTop + meterThickness));
            // channel # label
            g.setColour(getLookAndFeel().findColour(juce::AlertWindow::backgroundColourId));
            g.drawText(juce::String(i), juce::Rectangle<float>(visuAreaOrigX, meterTop, float(margin), meterThickness), juce::Justification::centred, true);

            meterTop += meterThickness + meterSpacing;
        }

        // draw a simple baseline
        g.setColour(getLookAndFeel().findColour(juce::AlertWindow::backgroundColourId));
        g.drawLine(juce::Line<float>(0.0f, 0.0f, 0.0f, visuAreaHeight));
        // draw dBFS
        g.setFont(12.0f);
        g.setColour(getLookAndFeel().findColour(juce::AlertWindow::backgroundColourId));
        juce::String rangeText;
        if (getUsesValuesInDB())
            rangeText = juce::String(MenuBarMatrixProcessor::getGlobalMindB()) + " ... " + juce::String(MenuBarMatrixProcessor::getGlobalMaxdB()) + " dBFS";
        else
            rangeText = "0 ... 1";

        g.setOrigin(visuArea.getBottomLeft());
        g.addTransform(juce::AffineTransform().rotated(90));
        g.drawText(rangeText, visuArea, juce::Justification::topRight, true);
    }
}

void MeterbridgeComponent::processingDataChanged(AbstractProcessorData* data)
{
    if (!data)
        return;

    switch (data->GetDataType())
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

void MeterbridgeComponent::setDirection(Direction direction)
{
    m_direction = direction;
    repaint();
}


}
