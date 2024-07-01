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
    DBG(__FUNCTION__);

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

    DBG(__FUNCTION__ << " " << int(channel) << " " << int(muteState));
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
    if (m_channelCount != channelCount)
    {
        m_channelCount = channelCount;
        DBG(__FUNCTION__ << " " << channelCount);
    }
}

}
