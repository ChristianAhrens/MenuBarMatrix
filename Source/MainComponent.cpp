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
#include <CustomLookAndFeel.h>

 //==============================================================================
class LoadBar : public juce::Component
{
public:
    LoadBar() : juce::Component::Component() {}
    ~LoadBar() {}

    //==============================================================================
    void paint(Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        g.setColour(getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
        
        auto barBounds = bounds.reduced(1);
        g.fillRect(barBounds.removeFromLeft(barBounds.getWidth() * (float(m_loadPercent) / 100.0f)));

        g.drawRect(bounds, 1.0f);

        g.drawText(juce::String("Load ") + juce::String(m_loadPercent) + juce::String("%"), bounds, juce::Justification::centred);
    };

    //==============================================================================
    void setLoadPercent(int loadPercent)
    {
        m_loadPercent = loadPercent;
        repaint();
    };

private:
    int m_loadPercent = 0;
};

MainComponent::MainComponent()
    : juce::Component()
{
    m_mbm = std::make_unique<MenuBarMatrix::MenuBarMatrix>();
    m_mbm->onSizeChangeRequested = [=](juce::Rectangle<int> requestedSize) { setSize(requestedSize.getWidth(), requestedSize.getHeight()); };
    addAndMakeVisible(m_mbm->getUIComponent());

    m_setupToggleButton = std::make_unique<TextButton>("Audio Device Setup");
    m_setupToggleButton->onClick = [this] {
        auto setupComponent = m_mbm->getDeviceSetupComponent();
        if (setupComponent)
        {
            if (setupComponent->isVisible())
            {
                setupComponent->setVisible(false);
                setupComponent->removeFromDesktop();
            }
            else
            {
                setupComponent->setVisible(true);
                setupComponent->addToDesktop(juce::ComponentPeer::StyleFlags::windowHasDropShadow | juce::ComponentPeer::StyleFlags::windowHasCloseButton);
                setupComponent->setBounds(getScreenBounds().translated(0, 27));
            }

            resized();
        }
    };
    addAndMakeVisible(m_setupToggleButton.get());

    m_sysLoadBar = std::make_unique<LoadBar>();
    m_mbm->onCpuUsageUpdate = [=](int loadPercent) { m_sysLoadBar->setLoadPercent(loadPercent); };
    addAndMakeVisible(m_sysLoadBar.get());

    juce::Desktop::getInstance().addDarkModeSettingListener(this);
    darkModeSettingChanged(); // initially trigger correct colourscheme
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(AlertWindow::backgroundColourId).darker());
    
    auto safeBounds = getLocalBounds();

    auto setupAreaBounds = safeBounds.removeFromTop(26);
    g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
    g.fillRect(setupAreaBounds);
}

void MainComponent::resized()
{
    auto safeBounds = getLocalBounds();

    auto margin = 3;
    auto setupAreaBounds = safeBounds.removeFromTop(26).reduced(margin);
    auto contentAreaBounds = safeBounds;
    contentAreaBounds.removeFromTop(1);

    auto setupElementArea = setupAreaBounds.removeFromTop(20);
    if (m_setupToggleButton)
        m_setupToggleButton->setBounds(setupElementArea.removeFromRight(100));
    if (m_sysLoadBar)
        m_sysLoadBar->setBounds(setupElementArea.removeFromLeft(100));

    auto MenuBarMatrixComponent = m_mbm->getUIComponent();
    if (MenuBarMatrixComponent)
        MenuBarMatrixComponent->setBounds(contentAreaBounds);
}

void MainComponent::darkModeSettingChanged()
{
    if (juce::Desktop::getInstance().isDarkModeActive())
    {
        // go dark
        m_lookAndFeel = std::make_unique<JUCEAppBasics::CustomLookAndFeel>(JUCEAppBasics::CustomLookAndFeel::PS_Dark);
        Desktop::getInstance().setDefaultLookAndFeel(m_lookAndFeel.get());
    }
    else
    {
        // go light
        m_lookAndFeel = std::make_unique<JUCEAppBasics::CustomLookAndFeel>(JUCEAppBasics::CustomLookAndFeel::PS_Light);
        Desktop::getInstance().setDefaultLookAndFeel(m_lookAndFeel.get());
    }
}

