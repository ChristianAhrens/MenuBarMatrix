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

#include "AboutComponent.h"
#include "CustomPopupMenuComponent.h"
#include "MenuBarMatrix.h"

#include <CustomLookAndFeel.h>
#include <AppConfigurationBase.h>

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

        g.setColour(getLookAndFeel().findColour(juce::LookAndFeel_V4::ColourScheme::windowBackground));
        g.fillRect(bounds);

        g.setColour(getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
        
        auto barBounds = bounds.reduced(1);
        g.fillRect(barBounds.removeFromLeft(barBounds.getWidth() * (float(m_loadPercent) / 100.0f)));

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

//==============================================================================
class EmptySpace :public juce::Component
{
public:
    EmptySpace() : juce::Component::Component() {}
    ~EmptySpace() {}

    //==============================================================================
    void paint(Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    };
};

MainComponent::MainComponent()
    : juce::Component()
{
    m_mbm = std::make_unique<MenuBarMatrix::MenuBarMatrix>();
    m_mbm->onSizeChangeRequested = [=](juce::Rectangle<int> requestedSize) {
        auto width = requestedSize.getWidth();
        auto height = requestedSize.getHeight() + sc_buttonSize;
        
        if (width < (sc_loadWidth + 5 * sc_buttonSize))
            width = sc_loadWidth + 5 * sc_buttonSize;
        
        setSize(width, height);
    };
    addAndMakeVisible(m_mbm->getUIComponent());

    m_setupButton = std::make_unique<juce::DrawableButton>("Audio Device Setup", juce::DrawableButton::ButtonStyle::ImageFitted);
    m_setupButton->setTooltip("Audio Device Setup");
    m_setupButton->onClick = [this] {
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
                setupComponent->setBounds(getScreenBounds().translated(0, sc_buttonSize + 1));
            }

            resized();
        }
    };
    addAndMakeVisible(m_setupButton.get());

    m_aboutComponent = std::make_unique<AboutComponent>(BinaryData::MenuBarMatrixRect_png, BinaryData::MenuBarMatrixRect_pngSize);
    m_aboutButton = std::make_unique<juce::DrawableButton>("About", juce::DrawableButton::ButtonStyle::ImageFitted);
    m_aboutButton->setTooltip(juce::String("About") + juce::JUCEApplication::getInstance()->getApplicationName());
    m_aboutButton->onClick = [this] {
        juce::PopupMenu aboutMenu;
        aboutMenu.addCustomItem(1, std::make_unique<CustomAboutItem>(m_aboutComponent.get(), juce::Rectangle<int>(250, 250)), nullptr, juce::String("Info about") + juce::JUCEApplication::getInstance()->getApplicationName());
        aboutMenu.showMenuAsync(juce::PopupMenu::Options());
    };
    addAndMakeVisible(m_aboutButton.get());

    m_powerButton = std::make_unique<juce::DrawableButton>("Quit application", juce::DrawableButton::ButtonStyle::ImageFitted);
    m_powerButton->setTooltip("Quit application");
    m_powerButton->onClick = [this] {
        juce::JUCEApplication::getInstance()->quit();
    };
    addAndMakeVisible(m_powerButton.get());

    m_emptySpace = std::make_unique<EmptySpace>();
    addAndMakeVisible(m_emptySpace.get());

    m_sysLoadBar = std::make_unique<LoadBar>();
    m_mbm->onCpuUsageUpdate = [=](int loadPercent) { m_sysLoadBar->setLoadPercent(loadPercent); };
    addAndMakeVisible(m_sysLoadBar.get());

    juce::Desktop::getInstance().addDarkModeSettingListener(this);
    darkModeSettingChanged(); // initially trigger correct colourscheme

    juce::Desktop::getInstance().addFocusChangeListener(this);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::LookAndFeel_V4::ColourScheme::defaultFill));
}

void MainComponent::resized()
{
    auto safeBounds = getLocalBounds();

    auto margin = 1;
    auto setupElementArea = safeBounds.removeFromTop(sc_buttonSize);
    auto contentAreaBounds = safeBounds;
    contentAreaBounds.removeFromTop(1);

    if (m_powerButton)
        m_powerButton->setBounds(setupElementArea.removeFromRight(setupElementArea.getHeight()));
    setupElementArea.removeFromRight(margin);
    if (m_aboutButton)
        m_aboutButton->setBounds(setupElementArea.removeFromRight(setupElementArea.getHeight()));
    setupElementArea.removeFromRight(margin);
    if (m_setupButton)
        m_setupButton->setBounds(setupElementArea.removeFromRight(setupElementArea.getHeight()));
    setupElementArea.removeFromRight(margin);
    if (m_sysLoadBar)
        m_sysLoadBar->setBounds(setupElementArea.removeFromLeft(sc_loadWidth));
    setupElementArea.removeFromLeft(margin);
    if (m_emptySpace)
        m_emptySpace->setBounds(setupElementArea);

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
        juce::Desktop::getInstance().setDefaultLookAndFeel(m_lookAndFeel.get());
    }
    else
    {
        // go light
        m_lookAndFeel = std::make_unique<JUCEAppBasics::CustomLookAndFeel>(JUCEAppBasics::CustomLookAndFeel::PS_Light);
        juce::Desktop::getInstance().setDefaultLookAndFeel(m_lookAndFeel.get());
    }

    lookAndFeelChanged();
}

void MainComponent::lookAndFeelChanged()
{
    auto powerDrawable = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::power_settings_24dp_svg).get());
    powerDrawable->replaceColour(juce::Colours::black, getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
    m_powerButton->setImages(powerDrawable.get());

    auto setupButtonDrawable = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::tune_24dp_svg).get());
    setupButtonDrawable->replaceColour(juce::Colours::black, getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
    m_setupButton->setImages(setupButtonDrawable.get());

    auto aboutButtonDrawable = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::question_mark_24dp_svg).get());
    aboutButtonDrawable->replaceColour(juce::Colours::black, getLookAndFeel().findColour(juce::TextButton::ColourIds::textColourOnId));
    m_aboutButton->setImages(aboutButtonDrawable.get());
    
    m_mbm->lookAndFeelChanged();
}

void MainComponent::globalFocusChanged(Component* focusedComponent)
{
    if(nullptr == focusedComponent)
    {
        if (onFocusLostWhileVisible && isVisible() && (m_mbm && !m_mbm->getDeviceSetupComponent()->isVisible()))
            onFocusLostWhileVisible();
    }
    else
    {
    }
}

