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

#include "MenuBarMatrix.h"

#include "MenuBarMatrixEditor/MenuBarMatrixEditor.h"
#include "MenuBarMatrixProcessor/MenuBarMatrixProcessor.h"

namespace MenuBarMatrix
{

//==============================================================================
MenuBarMatrix::MenuBarMatrix() :
    juce::Component(), juce::Timer()
{
    // create the configuration object (is being initialized from disk automatically)
    m_config = std::make_unique<AppConfiguration>(JUCEAppBasics::AppConfigurationBase::getDefaultConfigFilePath());
    m_config->addDumper(this);

    // check if config creation was able to read a valid config from disk...
    if (!m_config->isValid())
    {
        m_config->ResetToDefault();
    }

    // add this main component to watchers
    m_config->addWatcher(this, true); // this initial update cannot yet reach all parts of the app, esp. settings page that relies on fully initialized pagecomponentmanager, therefor a manual watcher update is triggered below

    m_menuBarMatrixProcessor = std::make_unique<MenuBarMatrixProcessor>(m_config->getConfigState(AppConfiguration::getTagName(AppConfiguration::TagID::PROCESSORCONFIG)).get());

    m_audioDeviceSelectComponent = std::make_unique<AudioSelectComponent>(m_menuBarMatrixProcessor->getDeviceManager(),
                                                                          MenuBarMatrixProcessor::s_minInputsCount,
                                                                          MenuBarMatrixProcessor::s_maxChannelCount,
                                                                          MenuBarMatrixProcessor::s_minOutputsCount,
                                                                          MenuBarMatrixProcessor::s_maxChannelCount,
                                                                          false, false, false, false);

    // do the initial update for the whole application with config contents
    m_config->triggerWatcherUpdate();

    startTimer(500);
}

MenuBarMatrix::~MenuBarMatrix()
{
    if (m_menuBarMatrixProcessor)
        m_menuBarMatrixProcessor->editorBeingDeleted(m_menuBarMatrixProcessor->getActiveEditor());
}

void MenuBarMatrix::timerCallback()
{
    if (m_menuBarMatrixProcessor && m_menuBarMatrixProcessor->getDeviceManager() && onCpuUsageUpdate)
    {
        onCpuUsageUpdate(int(m_menuBarMatrixProcessor->getDeviceManager()->getCpuUsage() * 100.0));
    }
}

juce::Component* MenuBarMatrix::getUIComponent()
{
    if (m_menuBarMatrixProcessor)
    {
        if (nullptr == m_menuBarMatrixProcessor->getActiveEditor())
            m_menuBarMatrixProcessor->createEditorIfNeeded();

        if (auto editor = dynamic_cast<MenuBarMatrixEditor*>(m_menuBarMatrixProcessor->getActiveEditor()))
        {
            jassert(onSizeChangeRequested); // should be set before handling the ui component!
            editor->onSizeChangeRequested = onSizeChangeRequested;
        }

        return m_menuBarMatrixProcessor->getActiveEditor();
    }
    else
        return nullptr;
}

juce::Component* MenuBarMatrix::getDeviceSetupComponent()
{
    if (m_audioDeviceSelectComponent)
        return m_audioDeviceSelectComponent.get();
    else
        return nullptr;
}

void MenuBarMatrix::performConfigurationDump()
{
    if (m_config)
    {
        auto stateXml = m_config->getConfigState();

        if (stateXml && m_menuBarMatrixProcessor)
        {
            m_config->setConfigState(m_menuBarMatrixProcessor->createStateXml(), AppConfiguration::getTagName(AppConfiguration::TagID::PROCESSORCONFIG));

            if (auto editor = dynamic_cast<MenuBarMatrixEditor*>(m_menuBarMatrixProcessor->getActiveEditor()))
            {
                m_config->setConfigState(editor->createStateXml(), AppConfiguration::getTagName(AppConfiguration::TagID::EDITORCONFIG));
            }
        }
    }
}

void MenuBarMatrix::onConfigUpdated()
{
    auto processorConfigState = m_config->getConfigState(AppConfiguration::getTagName(AppConfiguration::TagID::PROCESSORCONFIG));
    if (processorConfigState && m_menuBarMatrixProcessor)
    {
        m_menuBarMatrixProcessor->setStateXml(processorConfigState.get());
    }
        
    auto editorConfigState = m_config->getConfigState(AppConfiguration::getTagName(AppConfiguration::TagID::EDITORCONFIG));
    if (editorConfigState && m_menuBarMatrixProcessor && m_menuBarMatrixProcessor->getActiveEditor())
    {
        if (auto editor = dynamic_cast<MenuBarMatrixEditor*>(m_menuBarMatrixProcessor->getActiveEditor()))
        {
            editor->setStateXml(editorConfigState.get());
        }
    }
}

void MenuBarMatrix::lookAndFeelChanged()
{
    if (m_menuBarMatrixProcessor)
        m_menuBarMatrixProcessor->environmentChanged();
}


}
