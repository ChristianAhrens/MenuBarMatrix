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

#include <JuceHeader.h>
#include "MainComponent.h"

//==============================================================================
class MenuBarMatrixApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    MenuBarMatrixApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& /*commandLine*/) override
    {
        m_mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        m_mainWindow.reset(); // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String& /*commandLine*/) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow
    {
    public:
        MainWindow (juce::String name)
        {
            m_taskbarComponent.reset (new TaskbarComponent(*this));
            m_mainComponent.reset (new MainComponent ());
            m_mainComponent->setSize(m_mainComponentW, m_mainComponentH);
            m_mainComponent->setVisible(m_isMainComponentVisible);
            
            int taskbarIconX = m_taskbarComponent->getX();
            
            m_mainComponent->addToDesktop(juce::ComponentPeer::windowHasDropShadow);
            m_mainComponent->setTopLeftPosition(taskbarIconX - m_mainComponentW / 2, 50);
           
        }
        
        /* Note: Be careful if you override any DocumentWindow methods - the base
         class uses a lot of them, so by overriding you might break its functionality.
         It's best to do all your work in your content component instead, but if
         you really have to override any DocumentWindow methods, make sure your
         subclass also calls the superclass's method.
         */
        ~MainWindow()
        {
            m_mainComponent.reset();
        }
        
        void toggleVisibilty()
        {
            if (m_mainComponent != nullptr)
             {
                 m_mainComponent->setVisible(!m_mainComponent->isVisible());
                 m_isMainComponentVisible = m_mainComponent->isVisible();
            }
        }
        
        void updatePositionFromTrayIcon(int iconCenterX, int iconH)
        {
            if (m_mainComponent != nullptr)
            {
                m_mainComponent->setTopLeftPosition(iconCenterX - m_mainComponentW / 2, iconH);
            }
        }
        
        // Just add a simple icon to the Window system tray area or Mac menu bar..
        struct TaskbarComponent  : public juce::SystemTrayIconComponent, private juce::Timer
        {
            
            TaskbarComponent( MainWindow& window) : m_mainWindow(window)
            {
                setIconImage (juce::ImageFileFormat::loadFrom(BinaryData::grid_4x4_24dp_png, BinaryData::grid_4x4_24dp_pngSize),
                              juce::ImageFileFormat::loadFrom(BinaryData::grid_4x4_24dp_png, BinaryData::grid_4x4_24dp_pngSize));
                setIconTooltip (JUCEApplication::getInstance()->getApplicationName());
            }
            
    
            void mouseDown (const juce::MouseEvent& e) override
            {
                int iconCenterX = juce::Desktop::getMousePosition().x;
                int iconH = getParentMonitorArea().getY() + 5;

                m_mainWindow.updatePositionFromTrayIcon(iconCenterX, iconH);
                
                // On OSX, there can be problems launching a menu when we're not the foreground
                // process, so just in case, we'll first make our process active, and then use a
                // timer to wait a moment before opening our menu, which gives the OS some time to
                // get its act together and bring our windows to the front.
                juce::Process::makeForegroundProcess();
                m_mainWindow.toggleVisibilty();
                
                //startTimer (50);
            }
            
            
            // This is invoked when the menu is clicked or dismissed
            static void menuInvocationCallback (int chosenItemID, TaskbarComponent*)
            {
                if (chosenItemID == 1)
                    JUCEApplication::getInstance()->systemRequestedQuit();
            }
            
            void timerCallback() override
            {
                stopTimer();
                
                juce::PopupMenu m;
                m.addItem (1, "Quit");
                
                // It's always better to open menus asynchronously when possible.
                m.showMenuAsync (juce::PopupMenu::Options(),
                                 juce::ModalCallbackFunction::forComponent (menuInvocationCallback, this));
            }
            
             MainWindow& m_mainWindow;
            
        };
        
        
    private:
        int m_mainComponentW = 400;
        int m_mainComponentH = 600;
        bool m_isMainComponentVisible = false;
        std::unique_ptr<MainComponent> m_mainComponent;
        std::unique_ptr<juce::Component> m_taskbarComponent;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> m_mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (MenuBarMatrixApplication)
