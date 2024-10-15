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

#include "MainComponent.h"

#include "MenuBarMatrixMonitorComponent.h"
#include "MenuBarMatrixDiscoverComponent.h"

#include <CustomLookAndFeel.h>
#include "MenuBarMatrixProcessor/MenuBarMatrixMessages.h"
#include "MenuBarMatrixProcessor/MenuBarMatrixServiceData.h"

MainComponent::MainComponent()
    : juce::Component()
{
    m_networkConnection = std::make_unique<InterprocessConnectionImpl>();
    m_networkConnection->onConnectionMade = [=]() {
        DBG(__FUNCTION__);
        //if (m_monitorComponent)
        //    m_monitorComponent->setRunning(true);
        //
        //m_currentStatus = Status::Monitoring;
        //resized();
    };
    m_networkConnection->onConnectionLost = [=]() {
        DBG(__FUNCTION__);
        if (m_monitorComponent)
            m_monitorComponent->setRunning(false);

        m_currentStatus = Status::Discovering;
        resized();
    };
    m_networkConnection->onMessageReceived = [=](const juce::MemoryBlock& message) {
        auto knownMessage = MenuBarMatrix::SerializableMessage::initFromMemoryBlock(message);
        if (m_monitorComponent && nullptr != knownMessage)
            m_monitorComponent->handleMessage(*knownMessage);
        MenuBarMatrix::SerializableMessage::freeMessageData(knownMessage);
    };

    m_monitorComponent = std::make_unique<MenuBarMatrixMonitorComponent>();
    m_monitorComponent->onExitClick = [=]() {
        m_discoverComponent->setDiscoveredServices(m_availableServices->getServices());

        if (m_monitorComponent)
            m_monitorComponent->setRunning(false);

        m_currentStatus = Status::Discovering;
        resized();
    };
    m_monitorComponent->setRunning(false);
    addAndMakeVisible(m_monitorComponent.get());

    m_discoverComponent = std::make_unique<MenuBarMatrixDiscoverComponent>();
    m_discoverComponent->onServiceSelected = [=](const juce::NetworkServiceDiscovery::Service& selectedService) {
        m_currentStatus = Status::Monitoring;
        resized();

        if (m_monitorComponent)
            m_monitorComponent->setRunning(true);

        if (m_networkConnection)
            m_networkConnection->connectToSocket(selectedService.address.toString(), selectedService.port, 100);
    };
    addAndMakeVisible(m_discoverComponent.get());

    m_availableServices = std::make_unique<juce::NetworkServiceDiscovery::AvailableServiceList>(
        MenuBarMatrix::ServiceData::getServiceTypeUID(), 
        MenuBarMatrix::ServiceData::getBroadcastPort());
    m_availableServices->onChange = [=]() { 
        m_discoverComponent->setDiscoveredServices(m_availableServices->getServices());
    };

#ifdef NIX//DEBUG
    auto inputs = 11;
    auto outputs = 12;
    auto buffer = juce::AudioBuffer<float>();
    auto refSample = 11.11f;
    auto sr = 48000;
    auto mespb = 256;

    auto apm = std::make_unique<MenuBarMatrix::AnalyzerParametersMessage>(sr, mespb);
    auto apmb = apm->getSerializedMessage();
    auto apmcpy = MenuBarMatrix::AnalyzerParametersMessage(apmb);
    auto test5 = apmcpy.getSampleRate();
    auto test6 = apmcpy.getMaximumExpectedSamplesPerBlock();
    jassert(test5 == sr);
    jassert(test6 == mespb);

    auto rcm = std::make_unique<MenuBarMatrix::ReinitIOCountMessage>(inputs, outputs);
    auto rcmb = rcm->getSerializedMessage();
    auto rcmcpy = MenuBarMatrix::ReinitIOCountMessage(rcmb);
    auto test7 = rcmcpy.getInputCount();
    auto test8 = rcmcpy.getOutputCount();
    jassert(test7 == inputs);
    jassert(test8 == outputs);

    auto channelCount = 2;
    auto sampleCount = 6;
    buffer.setSize(channelCount, sampleCount, false, true, false);
    for (int i = 0; i < channelCount; i++)
    {
        for (int j = 0; j < sampleCount; j++)
        {
            buffer.setSample(i, j, ++refSample);
        }
    }
    auto rrefSample1 = refSample;
    auto aibm1 = std::make_unique<MenuBarMatrix::AudioInputBufferMessage>(buffer);
    for (int i = channelCount - 1; i >= 0; i--)
    {
        for (int j = sampleCount - 1; j >= 0; j--)
        {
            auto test1 = aibm1->getAudioBuffer().getSample(i, j);
            jassert(int(test1) == int(refSample));
            refSample--;
        }
    }
    auto aibmb1 = aibm1->getSerializedMessage();
    auto aibmcpy1 = MenuBarMatrix::AudioInputBufferMessage(aibmb1);
    for (int i = channelCount - 1; i >= 0; i--)
    {
        for (int j = sampleCount - 1; j >= 0; j--)
        {
            auto test1 = aibmcpy1.getAudioBuffer().getSample(i, j);
            jassert(int(test1) == int(rrefSample1));
            rrefSample1--;
        }
    }

    buffer.setSize(channelCount, sampleCount, false, true, false);
    for (int i = 0; i < channelCount; i++)
    {
        for (int j = 0; j < sampleCount; j++)
        {
            buffer.setSample(i, j, ++refSample);
        }
    }
    auto rrefSample2 = refSample;
    auto aibm2 = std::make_unique<MenuBarMatrix::AudioOutputBufferMessage>(buffer);
    for (int i = channelCount - 1; i >= 0; i--)
    {
        for (int j = sampleCount - 1; j >= 0; j--)
        {
            auto test2 = aibm2->getAudioBuffer().getSample(i, j);
            jassert(int(test2) == int(rrefSample2));
            rrefSample2--;
        }
    }
    auto aibmb2 = aibm2->getSerializedMessage();
    auto aibmcpy2 = MenuBarMatrix::AudioOutputBufferMessage(aibmb2);
    for (int i = channelCount - 1; i >= 0; i--)
    {
        for (int j = sampleCount - 1; j >= 0; j--)
        {
            auto test2 = aibmcpy2.getAudioBuffer().getSample(i, j);
            jassert(int(test2) == int(refSample));
            refSample--;
        }
    }
#endif

    setSize(400, 350);
}

MainComponent::~MainComponent()
{
}

void MainComponent::resized()
{
    switch (m_currentStatus)
    {
        case Status::Monitoring:
            m_discoverComponent->setVisible(false);
            m_monitorComponent->setVisible(true);
            m_monitorComponent->setBounds(getLocalBounds());
            break;
        case Status::Discovering:
        default:
            m_monitorComponent->setVisible(false);
            m_discoverComponent->setVisible(true);
            m_discoverComponent->setBounds(getLocalBounds());
            break;
    }
}

