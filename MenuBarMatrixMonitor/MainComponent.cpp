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

