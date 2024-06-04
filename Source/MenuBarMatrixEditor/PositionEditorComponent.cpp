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

#include "PositionEditorComponent.h"


namespace MenuBarMatrix
{


//==============================================================================
PositionEditorPopupComponent::PositionEditorPopupComponent()
{
    lookAndFeelChanged();

    setSize(100, 100);
}

PositionEditorPopupComponent::~PositionEditorPopupComponent()
{

}

void PositionEditorPopupComponent::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());

    auto bounds = getLocalBounds();

    // draw 1px border
    g.setColour(getLookAndFeel().findColour(TextButton::buttonColourId));
    g.drawRect(bounds);

    auto positioningNormalArea = bounds.reduced(10);

    g.setColour(getLookAndFeel().findColour(TextButton::buttonColourId).darker());
    g.drawRect(positioningNormalArea);
    g.drawEllipse(positioningNormalArea.toFloat(), 1.0f);

    if (getCurrentPositionCallback)
    {
        auto pnaf = positioningNormalArea.toFloat();
        auto relPos = pnaf.getTopLeft() + (juce::Point<float>(pnaf.getWidth(), pnaf.getHeight()) * getCurrentPositionCallback());
        auto verticalPosLine = juce::Line<float>(juce::Point<float>(relPos.getX(), pnaf.getY()), juce::Point<float>(relPos.getX(), pnaf.getBottom()));
        g.drawLine(verticalPosLine, 1.0f);
        auto horizontalPosLine = juce::Line<float>(juce::Point<float>(pnaf.getX(), relPos.getY()), juce::Point<float>(pnaf.getRight(), relPos.getY()));
        g.drawLine(horizontalPosLine, 1.0f);

        g.setColour(Colours::forestgreen);
        g.fillEllipse(juce::Rectangle<float>(relPos - juce::Point<float>(5.0f, 5.0f), relPos + juce::Point<float>(5.0f, 5.0f)));
    }

    Component::paint(g);
}

void PositionEditorPopupComponent::resized()
{
    Component::resized();
}

void PositionEditorPopupComponent::mouseUp(const MouseEvent& event)
{
    updatePosition(event.getPosition());
}

void PositionEditorPopupComponent::mouseDrag(const MouseEvent& event)
{
    updatePosition(event.getPosition());
}

void PositionEditorPopupComponent::updatePosition(const juce::Point<int>& mousePosition)
{
    if (setCurrentPositionCallback)
    {
        auto positioningNormalArea = getLocalBounds().reduced(10);

        auto mousePos = (mousePosition - positioningNormalArea.getTopLeft()).toFloat();

        auto relPosX = mousePos.getX() / static_cast<float>(positioningNormalArea.getWidth());
        auto relPosY = mousePos.getY() / static_cast<float>(positioningNormalArea.getHeight());

        auto currentPosition = juce::Point<float>(relPosX, -1.0f * (relPosY - 1.0f));

        setCurrentPositionCallback(currentPosition);
    }
}

//==============================================================================
PositionEditorComponent::PositionEditorComponent()
{
    lookAndFeelChanged();

    Desktop::getInstance().addGlobalMouseListener(this);
}

PositionEditorComponent::~PositionEditorComponent()
{

}

void PositionEditorComponent::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());

    auto bounds = getLocalBounds();

    // draw 1px border
    g.setColour(getLookAndFeel().findColour(TextButton::buttonColourId));
    g.drawRect(bounds);

    auto positioningNormalArea = bounds.reduced(3);

    g.setColour(getLookAndFeel().findColour(TextButton::buttonColourId).darker());
    g.drawRect(positioningNormalArea);

    auto pnaf = positioningNormalArea.toFloat();
    auto relPos = pnaf.getTopLeft() + (juce::Point<float>(pnaf.getWidth(), pnaf.getHeight()) * getCurrentPosition());
    auto verticalPosLine = juce::Line<float>(juce::Point<float>(relPos.getX(), pnaf.getY()), juce::Point<float>(relPos.getX(), pnaf.getBottom()));
    g.drawLine(verticalPosLine, 1.0f);
    auto horizontalPosLine = juce::Line<float>(juce::Point<float>(pnaf.getX(), relPos.getY()), juce::Point<float>(pnaf.getRight(), relPos.getY()));
    g.drawLine(horizontalPosLine, 1.0f);

    g.setColour(Colours::forestgreen);
    g.fillEllipse(juce::Rectangle<float>(relPos - juce::Point<float>(2.0f, 2.0f), relPos + juce::Point<float>(2.0f, 2.0f)));

    Component::paint(g);
}

void PositionEditorComponent::resized()
{
    Component::resized();
}

void PositionEditorComponent::mouseDown(const MouseEvent& event)
{
    if (m_positioningPopup)
    {
        auto mouseDownOutsideOfPopupComponent = !m_positioningPopup->getLocalBounds().contains(event.getMouseDownPosition());
        if (mouseDownOutsideOfPopupComponent)
            closePositioningPopup();
    }

    Component::mouseDown(event);
}

void PositionEditorComponent::mouseUp(const MouseEvent& event)
{
    auto relEventInParent = event.getEventRelativeTo(getParentComponent());
    auto relMouseUpPosition = relEventInParent.getMouseDownPosition() + relEventInParent.getOffsetFromDragStart();
    auto mouseUpWithinComponent = getBounds().contains(relMouseUpPosition);
    if (mouseUpWithinComponent)
    {
        auto localMouseUpPosition = event.getMouseDownPosition() + event.getOffsetFromDragStart();
        triggerPositioningPopup(localMouseUpPosition);
    }

    Component::mouseUp(event);
}

void PositionEditorComponent::lookAndFeelChanged()
{
    Component::lookAndFeelChanged();
}

void PositionEditorComponent::triggerPositioningPopup(const juce::Point<int>& popupStartPosition)
{
    if (isCurrentlyBlockedByAnotherModalComponent())
        return;

    if (!m_positioningPopup)
    {
        m_positioningPopup = std::make_unique<PositionEditorPopupComponent>();
        getParentComponent()->getParentComponent()->addChildComponent(m_positioningPopup.get());
    }

    if (m_positioningPopup)
    {
        m_positioningPopup->getCurrentPositionCallback = [this]() { return getCurrentPosition(); };
        m_positioningPopup->setCurrentPositionCallback = [this](const juce::Point<float>& position) { setCurrentPosition(position); };

        m_positioningPopup->setVisible(true);
        m_positioningPopup->enterModalState(true, nullptr, true);

        auto popupPositionInParent = getParentComponent()->getPosition() + getPosition() + popupStartPosition + juce::Point<int>(0, -m_positioningPopup->getHeight());
        
        m_positioningPopup->setTopLeftPosition(popupPositionInParent);

        m_positioningPopup->toFront(false);  // need to do this after making it modal, or it could
                                  // be stuck behind other comps that are already modal..

    }
}

void PositionEditorComponent::closePositioningPopup()
{
    if (m_positioningPopup)
    {
        m_positioningPopup->exitModalState(0);
        m_positioningPopup->setVisible(false);
        getParentComponent()->getParentComponent()->removeChildComponent(m_positioningPopup.get());
        m_positioningPopup.reset();
    }
}

void PositionEditorComponent::setCurrentPosition(const juce::Point<float>& currentPosition)
{
    m_currentPosition = currentPosition;

    if (setPositionCallback)
        setPositionCallback(this, m_currentPosition);
}

const juce::Point<float> PositionEditorComponent::getCurrentPosition()
{
    return juce::Point<float>(
        m_currentPosition.getX(),
        -1.0f * (m_currentPosition.getY() - 1.0f));
}

}
