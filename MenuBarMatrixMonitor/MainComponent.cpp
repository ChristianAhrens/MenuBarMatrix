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

#include <CustomLookAndFeel.h>
#include "MenuBarMatrixProcessor/MenuBarMatrixServiceData.h"

MainComponent::MainComponent()
    : juce::Component()
{
    m_availableServices = std::make_unique<juce::NetworkServiceDiscovery::AvailableServiceList>(
        MenuBarMatrix::ServiceData::getServiceTypeUID(), 
        MenuBarMatrix::ServiceData::getBroadcastPort());
    std::function<void()> updateServicesString = [=]() {
        auto services = m_availableServices->getServices();
        m_servicesString.clear();
        for (auto const& service : services)
        {
            m_servicesString += juce::String(service.description + " (" + service.instanceID + ")\n");
        }
        repaint();
    };
    m_availableServices->onChange = [=]() { 
        updateServicesString();
        repaint();
    };
    updateServicesString();

    setSize(400, 350);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::ColourIds::backgroundColourId));

    g.setColour(getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
    g.drawText(juce::JUCEApplication::getInstance()->getApplicationName() + " found:\n" + m_servicesString, getLocalBounds().toFloat(), juce::Justification::centred);
}

void MainComponent::resized()
{

}

