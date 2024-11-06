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


namespace MenuBarMatrix
{


//==============================================================================
class InterprocessConnectionImpl : public juce::InterprocessConnection
{
public:
    InterprocessConnectionImpl(int id);
    virtual ~InterprocessConnectionImpl();

    void connectionMade() override;

    void connectionLost() override;

    void messageReceived(const MemoryBlock& message) override;

    int getId();

    std::function<void(int)>                onConnectionMade;
    std::function<void(int)>                onConnectionLost;
    std::function<void(const MemoryBlock&)> onMessageReceived;

private:
    int m_id;

};

//==============================================================================
class InterprocessConnectionServerImpl : public juce::InterprocessConnectionServer
{
public:
    InterprocessConnectionServerImpl();
    virtual ~InterprocessConnectionServerImpl();

    double getListHealth();

    bool hasActiveConnection(int id);
    bool hasActiveConnections();
    const std::unique_ptr<InterprocessConnectionImpl>& getActiveConnection(int id);
    void cleanupDeadConnections();

    bool enqueueMessage(const MemoryBlock& message);

    std::function<void(int)>   onConnectionCreated;

protected:
    bool sendMessage(const MemoryBlock& message);

private:
    InterprocessConnection* createConnectionObject();

    std::mutex                      m_sendMessageMutex;
    std::queue<juce::MemoryBlock>   m_sendMessageList;
    std::atomic<bool>               m_sendMessageResult;

    std::atomic<bool>               m_sendMessageThreadActive;
    std::unique_ptr<std::thread>    m_sendMessageThread;
    std::condition_variable			m_sendMessageCV;
    std::mutex                      m_sendMessageCVMutex;

    std::map<int, std::unique_ptr<InterprocessConnectionImpl>> m_connections;
    int m_connectionIdIter = 0;
};


}; // namespace MenuBarMatrix
