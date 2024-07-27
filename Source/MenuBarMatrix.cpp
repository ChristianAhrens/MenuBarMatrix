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
    m_menuBarMatrixProcessor = std::make_unique<MenuBarMatrixProcessor>();

    m_audioDeviceSelectComponent = std::make_unique<AudioSelectComponent>(m_menuBarMatrixProcessor->getDeviceManager(),
                                                                          MenuBarMatrixProcessor::s_minInputsCount,
                                                                          MenuBarMatrixProcessor::s_maxChannelCount,
                                                                          MenuBarMatrixProcessor::s_minOutputsCount,
                                                                          MenuBarMatrixProcessor::s_maxChannelCount,
                                                                          false, false, false, false);

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


}
