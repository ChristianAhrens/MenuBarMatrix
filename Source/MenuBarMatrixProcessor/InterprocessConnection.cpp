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

#include "InterprocessConnection.h"


namespace MenuBarMatrix
{


//==============================================================================
InterprocessConnectionImpl::InterprocessConnectionImpl(int id) : juce::InterprocessConnection()
{
    m_id = id;
}

InterprocessConnectionImpl::~InterprocessConnectionImpl()
{ 
    disconnect();
}

void InterprocessConnectionImpl::connectionMade()
{
    if (onConnectionMade)
        onConnectionMade(m_id); 
}

void InterprocessConnectionImpl::connectionLost()
{
    if (onConnectionLost)
        onConnectionLost(m_id);
}

void InterprocessConnectionImpl::messageReceived(const MemoryBlock& message)
{
    if (onMessageReceived)
        onMessageReceived(message);
}

int InterprocessConnectionImpl::getId()
{
    return m_id;
}


//==============================================================================
InterprocessConnectionServerImpl::InterprocessConnectionServerImpl() : juce::InterprocessConnectionServer()
{
    m_sendMessageResult.store(true);

    m_sendMessageThreadActive.store(true);
    m_sendMessageThread = std::make_unique<std::thread>([this]() {
        std::unique_lock<std::mutex> sendMessageSignal(m_sendMessageCVMutex);
        while (m_sendMessageThreadActive.load())
        {
            m_sendMessageCV.wait(sendMessageSignal);

            std::unique_lock<std::mutex> l(m_sendMessageMutex);
            while (!m_sendMessageList.empty())
            {
                auto messageData = m_sendMessageList.front();
                m_sendMessageList.pop();
                l.unlock();
                if (!sendMessage(messageData))
                    m_sendMessageResult.store(false);
                l.lock();
            }
        }
        });
}

InterprocessConnectionServerImpl::~InterprocessConnectionServerImpl()
{
    {
        std::lock_guard<std::mutex> sendMessageSignal(m_sendMessageCVMutex);
        m_sendMessageThreadActive.store(false);
    }
    m_sendMessageCV.notify_all();
    if (m_sendMessageThread)
        m_sendMessageThread->join();
}

double InterprocessConnectionServerImpl::getListHealth()
{
    auto listSize = size_t(0);
    {
        std::lock_guard<std::mutex> l(m_sendMessageMutex);
        listSize = m_sendMessageList.size();
    }

    return double(listSize) / 35.0;
}

bool InterprocessConnectionServerImpl::hasActiveConnection(int id)
{
    if (m_connections.count(id) != 1)
    {
        return false;
    }
    else
    {
        if (m_connections.at(id) && m_connections.at(id)->isConnected())
            return true;
    }

    return m_connections.at(id) && m_connections.at(id)->isConnected();
}

bool InterprocessConnectionServerImpl::hasActiveConnections()
{
    for (auto const& connection : m_connections)
    {
        if (hasActiveConnection(connection.first))
            return true;
    }
    return false;
}

const std::unique_ptr<InterprocessConnectionImpl>& InterprocessConnectionServerImpl::getActiveConnection(int id)
{
    return m_connections[id];
}

void InterprocessConnectionServerImpl::cleanupDeadConnections()
{
    auto idsToErase = std::vector<int>();
    for (auto const& connection : m_connections)
        if (connection.second && !connection.second->isConnected())
            idsToErase.push_back(connection.first);

    for (auto const& id : idsToErase)
        m_connections.erase(id);
}

bool InterprocessConnectionServerImpl::enqueueMessage(const MemoryBlock& message)
{
    auto rVal = true;
    {
        std::lock_guard<std::mutex> l(m_sendMessageMutex);
        m_sendMessageList.push(message);
        if (!m_sendMessageResult.load())
        {
            rVal = false;
            m_sendMessageResult.store(true);
        }
    }
    m_sendMessageCV.notify_all();

    return rVal;
}

bool InterprocessConnectionServerImpl::sendMessage(const MemoryBlock& message)
{
    auto success = true;
    for (auto const& connection : m_connections)
        success = success && connection.second->sendMessage(message);
    return success;
}

InterprocessConnection* InterprocessConnectionServerImpl::createConnectionObject()
{
    m_connections[++m_connectionIdIter] = std::make_unique<InterprocessConnectionImpl>(m_connectionIdIter);

    m_connections[m_connectionIdIter]->onConnectionLost = [=](int /*connectionId*/) {
        cleanupDeadConnections();
        };

    if (onConnectionCreated)
        onConnectionCreated(m_connectionIdIter);

    return m_connections[m_connectionIdIter].get();
}


} // namespace MenuBarMatrix
