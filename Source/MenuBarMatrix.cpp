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
    Component(),
    JUCEAppBasics::AppConfigurationBase::XmlConfigurableElement()
{
    m_MenuBarMatrixProcessor = std::make_unique<MenuBarMatrixProcessor>();

    m_audioDeviceSelectComponent = std::make_unique<AudioSelectComponent>(m_MenuBarMatrixProcessor->getDeviceManager(), 1, MenuBarMatrixProcessor::s_maxChannelCount, 1, 8, false, false, false, false);

}

MenuBarMatrix::~MenuBarMatrix()
{
    if (m_MenuBarMatrixProcessor)
        m_MenuBarMatrixProcessor->editorBeingDeleted(m_MenuBarMatrixProcessor->getActiveEditor());
}

juce::Component* MenuBarMatrix::getUIComponent()
{
    if (m_MenuBarMatrixProcessor)
    {
        if (nullptr == m_MenuBarMatrixProcessor->getActiveEditor())
            m_MenuBarMatrixProcessor->createEditorIfNeeded();
        return m_MenuBarMatrixProcessor->getActiveEditor();
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

std::unique_ptr<XmlElement> MenuBarMatrix::createStateXml()
{
    jassertfalse;
    return nullptr;
}

bool MenuBarMatrix::setStateXml(XmlElement* stateXml)
{
    ignoreUnused(stateXml);
    jassertfalse;
    return false;
}

void MenuBarMatrix::lockCurrentLayout(bool doLock)
{
    auto MenuBarMatrixProcessorEditor = dynamic_cast<MenuBarMatrixEditor*>(getUIComponent());
    if (MenuBarMatrixProcessorEditor)
        MenuBarMatrixProcessorEditor->lockCurrentLayout(doLock);
}


}