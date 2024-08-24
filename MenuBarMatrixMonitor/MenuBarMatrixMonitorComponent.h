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

#pragma once

#include <JuceHeader.h>


class MenuBarMatrixMonitorComponent :   public juce::Component, juce::MessageListener, juce::Timer
{
public:
    enum RunningStatus
    {
        Inactive,
        Standby,
        Active
    };

    struct StartupRunningIndicator
    {
        static constexpr auto pi = MathConstants<float>::pi;
        static constexpr auto arcStart = pi / 2.0f;

        ~StartupRunningIndicator() = default;
        void paint(Graphics& g)
        {
            Path p;
            p.addCentredArc(centre.getX(),
                centre.getY(),
                radius,
                radius,
                0.0f,
                arcStart,
                arcStart + 2.0f * pi * (float(progress) / 100.0f),
                true);

            g.setColour(colour);
            g.strokePath(p, { thickness, PathStrokeType::mitered });
        }

        int progress = 0; // -1 for failure, 0-100 for actual progress
        juce::Point<float> centre;
        juce::Colour colour = juce::Colours::white;

    private:
        float radius = 0.0f, thickness = 0.0f;
    };

public:
    MenuBarMatrixMonitorComponent();
    ~MenuBarMatrixMonitorComponent() override;

    //========================================================================*
    void paint(Graphics&) override;
    void resized() override;

    //========================================================================*
    void handleMessage(const Message& message) override;

    //========================================================================*
    void timerCallback() override;

    //========================================================================*
    void setRunning(bool running);

private:
    //========================================================================*
    void updateStartupAnimation();
    void showStartupTimeout();

    //========================================================================*
    RunningStatus m_runningStatus = RunningStatus::Inactive;
    int m_startRunningAttemptTime = 0;
    static constexpr int sc_startRunningRefreshInterval = 50; // 50ms update rate
    static constexpr int sc_startRunningTimeout = 4000; // 4s running before attempt is considered failed

    StartupRunningIndicator m_startRunningIndicator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuBarMatrixMonitorComponent)
};

