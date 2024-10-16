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

#include "AboutComponent.h"

AboutComponent::AboutComponent()
    : juce::Component()
{
	m_appInfoLabel = std::make_unique<juce::Label>("Version", juce::JUCEApplication::getInstance()->getApplicationName() + " " + juce::JUCEApplication::getInstance()->getApplicationVersion());
	m_appInfoLabel->setJustificationType(juce::Justification::centredBottom);
	m_appInfoLabel->setFont(juce::Font(16.0, juce::Font::plain));
	addAndMakeVisible(m_appInfoLabel.get());

	m_appRepoLink = std::make_unique<juce::HyperlinkButton>(juce::JUCEApplication::getInstance()->getApplicationName() + juce::String(" on GitHub"), URL("https://www.github.com/ChristianAhrens/" + juce::JUCEApplication::getInstance()->getApplicationName()));
	m_appRepoLink->setFont(juce::Font(16.0, juce::Font::plain), false /* do not resize */);
	m_appRepoLink->setJustificationType(juce::Justification::centredTop);
	addAndMakeVisible(m_appRepoLink.get());
}

AboutComponent::~AboutComponent()
{
}

void AboutComponent::paint(juce::Graphics &g)
{
	g.fillAll(getLookAndFeel().findColour(juce::DrawableButton::backgroundColourId));

	g.setColour(juce::Colours::grey);
	g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

	juce::Component::paint(g);
}

void AboutComponent::resized()
{
	auto bounds = getLocalBounds().reduced(2);

	m_appInfoLabel->setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
	m_appRepoLink->setBounds(bounds);
}
