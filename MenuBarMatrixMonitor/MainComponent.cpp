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
#include "MenuBarMatrixProcessor/MenuBarMatrixServiceData.h"

MainComponent::MainComponent()
    : juce::Component()
{
    m_monitorComponent = std::make_unique<MenuBarMatrixMonitorComponent>();
    addAndMakeVisible(m_monitorComponent.get());

    m_discoverComponent = std::make_unique<MenuBarMatrixDiscoverComponent>();
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
    m_discoverComponent->setBounds(getLocalBounds());
}

