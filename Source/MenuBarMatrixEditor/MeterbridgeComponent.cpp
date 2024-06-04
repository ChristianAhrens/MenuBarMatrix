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

MeterbridgeComponent::~MeterbridgeComponent()
{

}

void MeterbridgeComponent::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());

	// calculate what we need for our center circle
	auto margin = 20;
    auto maxMeterWidth = 30;
	auto visuAreaWidth = static_cast<float>(getWidth());
	auto visuAreaHeight = static_cast<float>(getHeight());

    auto visuArea = getLocalBounds();
	auto visuAreaOrigY = visuAreaHeight;

	// draw meters
    auto meterSpacing = margin * 0.5f;
    auto meterWidth = (visuArea.getWidth() - (m_levelData.GetChannelCount() + 1) * meterSpacing) / m_levelData.GetChannelCount();
    meterWidth = meterWidth > maxMeterWidth ? maxMeterWidth : meterWidth;
    auto meterMaxHeight = visuArea.getHeight();
    auto meterLeft = meterSpacing;

    g.setFont(14.0f);
    for(unsigned long i=1; i<=m_levelData.GetChannelCount(); ++i)
    {
        auto level = m_levelData.GetLevel(i);
        float peakMeterHeight {0};
        float rmsMeterHeight  {0};
        float holdMeterHeight {0};
        if (getUsesValuesInDB())
        {
            peakMeterHeight = meterMaxHeight * level.GetFactorPEAKdB();
            rmsMeterHeight = meterMaxHeight * level.GetFactorRMSdB();
            holdMeterHeight = meterMaxHeight * level.GetFactorHOLDdB();
        }
        else
        {
            peakMeterHeight = meterMaxHeight * level.peak;
            rmsMeterHeight = meterMaxHeight * level.rms;
            holdMeterHeight = meterMaxHeight * level.hold;
        }

        // peak bar
        g.setColour(Colours::forestgreen.darker());
        g.fillRect(Rectangle<float>(meterLeft, visuAreaOrigY - peakMeterHeight, meterWidth, peakMeterHeight));
        // rms bar
        g.setColour(Colours::forestgreen);
        g.fillRect(Rectangle<float>(meterLeft, visuAreaOrigY - rmsMeterHeight, meterWidth, rmsMeterHeight));
        // hold strip
        g.setColour(Colours::grey);
        g.drawLine(Line<float>(meterLeft, visuAreaOrigY - holdMeterHeight, meterLeft + meterWidth, visuAreaOrigY - holdMeterHeight));
        // channel # label
		g.setColour(Colours::white);
        g.drawText(String(i), Rectangle<float>(meterLeft, visuAreaOrigY, meterWidth, float(margin)), Justification::centred, true);

        meterLeft += meterWidth + meterSpacing;
    }

    // draw a simple baseline
    g.setColour(Colours::grey);
    g.drawLine(Line<float>(0.0f, visuAreaOrigY, visuAreaWidth, visuAreaOrigY));
    // draw dBFS
    g.setFont(12.0f);
    g.setColour(Colours::grey);
    String rangeText;
    if (getUsesValuesInDB())
        rangeText = String(MenuBarMatrixProcessor::getGlobalMindB()) + " ... " + String(MenuBarMatrixProcessor::getGlobalMaxdB()) + " dBFS";
    else
        rangeText = "0 ... 1";
    g.drawText(rangeText, Rectangle<float>(visuAreaWidth - 100.0f, 0.0f, 110.0f, float(margin)), Justification::centred, true);
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


}
