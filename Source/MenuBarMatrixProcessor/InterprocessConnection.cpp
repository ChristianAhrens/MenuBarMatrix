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
    
}

InterprocessConnectionServerImpl::~InterprocessConnectionServerImpl()
{
    for (auto const& connection : m_connections)
    {
        endMessageThread(connection.first);
    }
}

void InterprocessConnectionServerImpl::createMessageThread(int id)
{
    DBG(juce::String(__FUNCTION__) << id);

    m_sendMessageResults[id].store(true);

    m_sendMessageThreadsActive[id].store(true);
    m_sendMessageThreads[id] = std::make_unique<std::thread>([this, id]() {
        auto thisId = id;
        std::unique_lock<std::mutex> sendMessageSignal(m_sendMessageCVMutexs[thisId]);
        while (m_sendMessageThreadsActive[thisId].load())
        {
            m_sendMessageCVs[id].wait(sendMessageSignal);

            std::unique_lock<std::mutex> l(m_sendMessageMutexs[thisId]);
            while (!m_sendMessageLists[thisId].empty())
            {
                auto messageData = m_sendMessageLists[thisId].front();
                m_sendMessageLists[thisId].pop();
                l.unlock();
                if (m_connections[thisId] && m_connections[thisId]->isConnected() && !m_connections[thisId]->sendMessage(messageData))
                    m_sendMessageResults[thisId].store(false);
                l.lock();
            }
        }

        DBG(juce::String(__FUNCTION__) << juce::String("end ") << id);

    });
}

void InterprocessConnectionServerImpl::endMessageThread(int id)
{
    DBG(juce::String(__FUNCTION__) << id);

    {
        std::lock_guard<std::mutex> sendMessageSignal(m_sendMessageCVMutexs[id]);
        m_sendMessageThreadsActive[id].store(false);
    }
    m_sendMessageCVs[id].notify_all();
    if (m_sendMessageThreads[id])
    {
        m_sendMessageThreads[id]->join();
    }

    m_sendMessageMutexs.erase(id);
    m_sendMessageLists.erase(id);
    m_sendMessageResults.erase(id);

    m_sendMessageThreadsActive.erase(id);
    m_sendMessageThreads.erase(id);
    m_sendMessageCVs.erase(id);
    m_sendMessageCVMutexs.erase(id);
}

std::map<int, double> InterprocessConnectionServerImpl::getListHealth()
{
    std::map<int, double> rList;
    for (auto const& list : m_sendMessageLists)
    {
        auto listSize = size_t(0);
        {
            std::lock_guard<std::mutex> l(m_sendMessageMutexs[list.first]);
            listSize = m_sendMessageLists[list.first].size();
        }

        if (listSize >= 35)
            rList[list.first] = 1.0;
        else
            rList[list.first] = double(listSize) / 35.0;

        DBG(juce::String(__FUNCTION__) << " " << list.first << " " << listSize);
    }
    return rList;
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
    {
        m_connections.erase(id);
        endMessageThread(id);
    }
}

bool InterprocessConnectionServerImpl::enqueueMessage(const MemoryBlock& message)
{
    auto rVal = true;
    for (auto const& th : m_sendMessageThreads)
    {
        {
            std::lock_guard<std::mutex> l(m_sendMessageMutexs[th.first]);
            m_sendMessageLists[th.first].push(message);
            if (!m_sendMessageResults[th.first].load())
            {
                rVal = false;
                m_sendMessageResults[th.first].store(true);
            }
        }
        m_sendMessageCVs[th.first].notify_all();
    }

    return rVal;
}

InterprocessConnection* InterprocessConnectionServerImpl::createConnectionObject()
{
    m_connections[++m_connectionIdIter] = std::make_unique<InterprocessConnectionImpl>(m_connectionIdIter);
    
    createMessageThread(m_connectionIdIter);

    m_connections[m_connectionIdIter]->onConnectionLost = [=](int /*connectionId*/) {
        cleanupDeadConnections();
        };

    if (onConnectionCreated)
        onConnectionCreated(m_connectionIdIter);

    return m_connections[m_connectionIdIter].get();
}


} // namespace MenuBarMatrix
