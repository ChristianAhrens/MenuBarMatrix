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

#include "MenuBarMatrixDiscoverComponent.h"

MenuBarMatrixDiscoverComponent::MenuBarMatrixDiscoverComponent()
    : juce::Component()
{
    m_discoveredServicesLabel = std::make_unique<juce::Label>("ServicesLabel", "Available instances:");
    addAndMakeVisible(m_discoveredServicesLabel.get());

    m_discoveredServicesSelection = std::make_unique<juce::ComboBox>("ServicesComboBox");
    m_discoveredServicesSelection->onChange = [=]() {
        m_discoveredServicesSelection->getSelectedId();
        jassertfalse; // do something here
    };
    addAndMakeVisible(m_discoveredServicesSelection.get());
}

MenuBarMatrixDiscoverComponent::~MenuBarMatrixDiscoverComponent()
{
}

void MenuBarMatrixDiscoverComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::ColourIds::backgroundColourId));
}

void MenuBarMatrixDiscoverComponent::resized()
{
    auto contentWidth = 250;
    auto contentHeight = 80;
    auto elementHeight = contentHeight / 2;
    auto elementsBounds = juce::Rectangle<int>((getWidth() - contentWidth) / 2, (getHeight() - contentHeight) / 2, contentWidth, contentHeight);

    m_discoveredServicesLabel->setBounds(elementsBounds.removeFromTop(elementHeight));
    m_discoveredServicesSelection->setBounds(elementsBounds);
}

void MenuBarMatrixDiscoverComponent::setDiscoveredServices(const std::vector<juce::NetworkServiceDiscovery::Service>& services)
{
    m_discoveredServicesSelection->clear();
    int i = 1;
    for (auto const& service : services)
    {
        m_discoveredServicesSelection->addItem(service.description, i);
        i++;
    }
}
