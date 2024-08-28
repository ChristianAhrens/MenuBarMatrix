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

MenuBarMatrixMonitorComponent::MenuBarMatrixMonitorComponent()
    : juce::Component()
{
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
    m_startRunningIndicator.setBounds(getLocalBounds());
}

void MenuBarMatrixMonitorComponent::mouseUp(const juce::MouseEvent& e)
{
    if (getLocalBounds().contains(e.getPosition()) && e.mouseWasClicked() && m_startRunningIndicator.progress == -1 && onExitClick)
        onExitClick();

    juce::Component::mouseUp(e);
}

void MenuBarMatrixMonitorComponent::handleMessage(const Message& message)
{
    DBG(__FUNCTION__);
    if (RunningStatus::Inactive != m_runningStatus)
        m_runningStatus = RunningStatus::Active;
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
