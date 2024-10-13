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

#include "MenuBarMatrixMonitorComponent.h"

#include "MenuBarMatrixEditor/MeterbridgeComponent.h"
#include "MenuBarMatrixProcessor/MenuBarMatrixMessages.h"
#include "MenuBarMatrixProcessor/MenuBarMatrixProcessor.h"
#include "MenuBarMatrixProcessor/ProcessorDataAnalyzer.h"

MenuBarMatrixMonitorComponent::MenuBarMatrixMonitorComponent()
    : juce::Component()
{
    m_inputMeteringComponent = std::make_unique<MenuBarMatrix::MeterbridgeComponent>(MenuBarMatrix::MeterbridgeComponent::Direction::Horizontal);
    addAndMakeVisible(m_inputMeteringComponent.get());
    m_outputMeteringComponent = std::make_unique<MenuBarMatrix::MeterbridgeComponent>(MenuBarMatrix::MeterbridgeComponent::Direction::Horizontal);
    addAndMakeVisible(m_outputMeteringComponent.get());

    m_inputDataAnalyzer = std::make_unique<MenuBarMatrix::ProcessorDataAnalyzer>();
    m_inputDataAnalyzer->addListener(m_inputMeteringComponent.get());

    m_outputDataAnalyzer = std::make_unique<MenuBarMatrix::ProcessorDataAnalyzer>();
    m_outputDataAnalyzer->addListener(m_outputMeteringComponent.get());
}

MenuBarMatrixMonitorComponent::~MenuBarMatrixMonitorComponent()
{
}

void MenuBarMatrixMonitorComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::ColourIds::backgroundColourId));
    if (RunningStatus::Active != m_runningStatus)
    {
        g.setColour(getLookAndFeel().findColour(juce::TextButton::textColourOnId));
        m_startRunningIndicator.paint(g);
    }
    else
    {

    }
}

void MenuBarMatrixMonitorComponent::resized()
{
    if (RunningStatus::Active != m_runningStatus)
        m_startRunningIndicator.setBounds(getLocalBounds());
    else if (m_inputMeteringComponent && m_outputMeteringComponent)
    {
        auto bounds = getLocalBounds();
        auto margin = 3;
        m_inputMeteringComponent->setBounds(bounds.removeFromLeft(bounds.getWidth() / 2).removeFromRight(margin / 2));
        m_outputMeteringComponent->setBounds(bounds.removeFromLeft(margin / 2));
    }
}

void MenuBarMatrixMonitorComponent::mouseUp(const juce::MouseEvent& e)
{
    if (getLocalBounds().contains(e.getPosition()) && e.mouseWasClicked() && m_startRunningIndicator.progress == -1 && onExitClick)
        onExitClick();

    juce::Component::mouseUp(e);
}

void MenuBarMatrixMonitorComponent::handleMessage(const Message& message)
{
    if (RunningStatus::Inactive != m_runningStatus)
    {
        m_runningStatus = RunningStatus::Active;
        resized();
    }
    
    if (auto const apm = dynamic_cast<const MenuBarMatrix::AnalyzerParametersMessage*>(&message))
    {
        auto sampleRate = apm->getSampleRate();
        jassert(sampleRate > 0);
        auto maximumExpectedSamplesPerBlock = apm->getMaximumExpectedSamplesPerBlock();
        jassert(maximumExpectedSamplesPerBlock > 0);

        if (m_inputDataAnalyzer)
            m_inputDataAnalyzer->initializeParameters(sampleRate, maximumExpectedSamplesPerBlock);
        if (m_outputDataAnalyzer)
            m_outputDataAnalyzer->initializeParameters(sampleRate, maximumExpectedSamplesPerBlock);
    }
    else if (auto const iom = dynamic_cast<const MenuBarMatrix::ReinitIOCountMessage*>(&message))
    {
        auto inputCount = iom->getInputCount();
        jassert(inputCount > 0);
        m_inputMeteringComponent->setChannelCount(inputCount);
        auto outputCount = iom->getOutputCount();
        jassert(outputCount > 0);
        m_outputMeteringComponent->setChannelCount(outputCount);
    }
    else if (auto m = dynamic_cast<const MenuBarMatrix::AudioBufferMessage*>(&message))
    {
        if (m->getFlowDirection() == MenuBarMatrix::AudioBufferMessage::FlowDirection::Input && m_inputDataAnalyzer)
        {
            m_inputDataAnalyzer->analyzeData(m->getAudioBuffer());
        }
        else if (m->getFlowDirection() == MenuBarMatrix::AudioBufferMessage::FlowDirection::Output && m_outputDataAnalyzer)
        {
            m_outputDataAnalyzer->analyzeData(m->getAudioBuffer());
        }
    }
}

void MenuBarMatrixMonitorComponent::timerCallback()
{
    if (RunningStatus::Standby == m_runningStatus)
    {
        m_startRunningAttemptTime -= sc_startRunningRefreshInterval;
        if (m_startRunningAttemptTime > 0)
        {
            updateStartupAnimation();
            startTimer(sc_startRunningRefreshInterval);
        }
        else
            showStartupTimeout();
    }
}

void MenuBarMatrixMonitorComponent::setRunning(bool running)
{
    m_runningStatus = running ? RunningStatus::Standby : RunningStatus::Inactive;

    if (running)
    {
        m_startRunningAttemptTime = sc_startRunningTimeout;
        startTimer(sc_startRunningRefreshInterval);
    }
}

void MenuBarMatrixMonitorComponent::updateStartupAnimation()
{
    m_startRunningIndicator.progress = int((float(m_startRunningAttemptTime) / float(sc_startRunningTimeout)) * 100.0f);
    repaint();
}

void MenuBarMatrixMonitorComponent::showStartupTimeout()
{
    m_startRunningIndicator.progress = -1;
    repaint();
}
