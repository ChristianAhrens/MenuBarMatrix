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
        static constexpr auto pi = juce::MathConstants<float>::pi;
        static constexpr auto arcStart = 0;// pi / 2.0f;

        ~StartupRunningIndicator() = default;
        void paint(juce::Graphics& g)
        {
            juce::Path p;
            if (progress != -1)
            {
                p.addCentredArc(centre.getX(),
                    centre.getY(),
                    radius,
                    radius,
                    0.0f,
                    arcStart,
                    arcStart - 2.0f * pi * (float(progress) / 100.0f),
                    true);
            }
            else
            {
                auto ep1 = centre.translated((radius), (radius));
                auto ep2 = centre.translated(-(radius), (radius));
                auto ep3 = centre.translated((radius), -(radius));
                auto ep4 = centre.translated(-(radius), -(radius));
                p.addLineSegment({ ep1, ep4 }, 1.0f);
                p.addLineSegment({ ep3, ep2 }, 1.0f);
            }
            g.strokePath(p, { thickness, juce::PathStrokeType::mitered });
        }
        void setBounds(const juce::Rectangle<int>& rect)
        {
            auto length = rect.getAspectRatio() < 1 ? rect.getWidth() : rect.getHeight();
            centre = rect.getCentre().toFloat();
            radius = length / 6.0f;
            thickness = radius / 2.0f;
        }

        int progress = 0; // -1 for failure, 0-100 for actual progress

    private:
        juce::Point<float> centre;
        float radius = 0.0f, thickness = 0.0f;
    };

public:
    MenuBarMatrixMonitorComponent();
    ~MenuBarMatrixMonitorComponent() override;

    //========================================================================*
    void paint(Graphics&) override;
    void resized() override;

    void mouseUp(const juce::MouseEvent& e) override;

    //========================================================================*
    void handleMessage(const Message& message) override;

    //========================================================================*
    void timerCallback() override;

    //========================================================================*
    void setRunning(bool running);
    
    //========================================================================*
    std::function<void()>   onExitClick;

private:
    //========================================================================*
    void updateStartupAnimation();
    void showStartupTimeout();

    //========================================================================*
    RunningStatus m_runningStatus = RunningStatus::Inactive;
    int m_startRunningAttemptTime = 0;
    static constexpr int sc_startRunningRefreshInterval = 50; // 50ms update rate
    static constexpr int sc_startRunningTimeout = 5000; // 5s running before attempt is considered failed

    StartupRunningIndicator m_startRunningIndicator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuBarMatrixMonitorComponent)
};

