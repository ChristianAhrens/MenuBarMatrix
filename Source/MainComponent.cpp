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

#include "MenuBarMatrix.h"

#include <iOS_utils.h>

MainComponent::MainComponent()
    : juce::Component()
{
    m_mbm = std::make_unique<MenuBarMatrix::MenuBarMatrix>();
    addAndMakeVisible(m_mbm->getUIComponent());

    m_setupToggleButton = std::make_unique<TextButton>("Audio Device Setup");
    m_setupToggleButton->onClick = [this] {
        auto setupComponent = m_mbm->getDeviceSetupComponent();
        if (setupComponent)
        {
            if (setupComponent->isVisible())
            {
                removeChildComponent(setupComponent);
                setupComponent->setVisible(false);
            }
            else
            {
                setupComponent->setVisible(true);
                addAndMakeVisible(setupComponent);
            }

            resized();
        }
    };
    addAndMakeVisible(m_setupToggleButton.get());

    setSize(900, 600);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(AlertWindow::backgroundColourId).darker());
    
    //auto safety = JUCEAppBasics::iOS_utils::getDeviceSafetyMargins();
    auto safeBounds = getLocalBounds();
    //safeBounds.removeFromTop(safety._top);
    //safeBounds.removeFromBottom(safety._bottom);
    //safeBounds.removeFromLeft(safety._left);
    //safeBounds.removeFromRight(safety._right);
    
    auto setupAreaBounds = safeBounds.removeFromTop(26);
    g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    g.fillRect(setupAreaBounds.reduced(6, 0));
}

void MainComponent::resized()
{
    //auto safety = JUCEAppBasics::iOS_utils::getDeviceSafetyMargins();
    auto safeBounds = getLocalBounds();
    //safeBounds.removeFromTop(safety._top);
    //safeBounds.removeFromBottom(safety._bottom);
    //safeBounds.removeFromLeft(safety._left);
    //safeBounds.removeFromRight(safety._right);

    auto margin = 3;
    auto setupAreaBounds = safeBounds.removeFromTop(26).reduced(9, margin);
    auto contentAreaBounds = safeBounds;

    if (m_setupToggleButton)
        m_setupToggleButton->setBounds(setupAreaBounds.removeFromRight(100).removeFromTop(20));
    setupAreaBounds.removeFromRight(margin);

    auto MenuBarMatrixComponent = m_mbm->getUIComponent();
    if (MenuBarMatrixComponent)
        MenuBarMatrixComponent->setBounds(contentAreaBounds);

    auto setupComponent = m_mbm->getDeviceSetupComponent();
    if (setupComponent && setupComponent->isVisible())
        setupComponent->setBounds(contentAreaBounds.reduced(15));
}

