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

#include "ProcessorDataAnalyzer.h"
#include "../MenuBarMatrixEditor/MenuBarMatrixEditor.h"
#include "../AppConfiguration.h"


namespace MenuBarMatrix
{


//==============================================================================
/*
*/
class MenuBarMatrixProcessor :  public juce::AudioProcessor,
					            public juce::AudioIODeviceCallback,
                                public juce::MessageListener,
                                public juce::ChangeListener,
                                public AppConfiguration::XmlConfigurableElement
{
public:
    class ChannelCommander
    {
    public:
        ChannelCommander();
        virtual ~ChannelCommander();

        virtual void setChannelCount(int channelCount) = 0;

    protected:

    private:
    };

    class InputCommander : public ChannelCommander
    {
    public:
        InputCommander();
        virtual ~InputCommander() override;

        void setInputMuteChangeCallback(const std::function<void(InputCommander* sender, int, bool)>& callback);
        void setInputLevelChangeCallback(const std::function<void(InputCommander* sender, int, float)>& callback);
        void setInputMutePollCallback(const std::function<void(InputCommander* sender, int)>& callback);
        void setInputLevelPollCallback(const std::function<void(InputCommander* sender, int)>& callback);

        virtual void setInputMute(unsigned int channel, bool muteState) = 0;
        virtual void setInputLevel(unsigned int channel, float levelValue) { ignoreUnused(channel); ignoreUnused(levelValue); };

    protected:
        void inputMuteChange(int channel, bool muteState);
        void inputLevelChange(int channel, float levelValue);
        
        void inputMutePoll(int channel);
        void inputLevelPoll(int channel);

    private:
        std::function<void(InputCommander* sender, int, float)> m_inputLevelChangeCallback{ nullptr };
        std::function<void(InputCommander* sender, int)>        m_inputLevelPollCallback{ nullptr };
        std::function<void(InputCommander* sender, int, bool)>  m_inputMuteChangeCallback{ nullptr };
        std::function<void(InputCommander* sender, int)>        m_inputMutePollCallback{ nullptr };
    };

    class OutputCommander : public ChannelCommander
    {
    public:
        OutputCommander();
        virtual ~OutputCommander() override;

        void setOutputMuteChangeCallback(const std::function<void(OutputCommander* sender, int, bool)>& callback);
        void setOutputLevelChangeCallback(const std::function<void(OutputCommander* sender, int, float)>& callback);
        void setOutputMutePollCallback(const std::function<void(OutputCommander* sender, int)>& callback);
        void setOutputLevelPollCallback(const std::function<void(OutputCommander* sender, int)>& callback);

        virtual void setOutputMute(unsigned int channel, bool muteState) = 0;
        virtual void setOutputLevel(unsigned int channel, float levelValue) { ignoreUnused(channel); ignoreUnused(levelValue); };

    protected:
        void outputMuteChange(int channel, bool muteState);
        void outputLevelChange(int channel, float levelValue);
        
        void outputMutePoll(int channel);
        void outputLevelPoll(int channel);

    private:
        std::function<void(OutputCommander* sender, int, float)>    m_outputLevelChangeCallback{ nullptr };
        std::function<void(OutputCommander* sender, int)>           m_outputLevelPollCallback{ nullptr };
        std::function<void(OutputCommander* sender, int, bool)>     m_outputMuteChangeCallback{ nullptr };
        std::function<void(OutputCommander* sender, int)>           m_outputMutePollCallback{ nullptr };
    };
    
    class CrosspointCommander : public ChannelCommander
    {
    public:
        CrosspointCommander();
        virtual ~CrosspointCommander() override;

        void setCrosspointEnabledChangeCallback(const std::function<void(CrosspointCommander* sender, int, int, bool)>& callback);
        void setCrosspointEnabledPollCallback(const std::function<void(CrosspointCommander* sender, int, int)>& callback);

        virtual void setCrosspointEnabledValue(int input, int output, bool enabledState) = 0;

        virtual void setIOCount(int inputCount, int outputCount) = 0;

    protected:
        void crosspointEnabledChange(int input, int output, bool enabledState);
        void crosspointEnabledPoll(int input, int output);

    private:
        void setChannelCount(int channelCount) override { ignoreUnused(channelCount); };

        std::function<void(CrosspointCommander* sender, int, int, bool)> m_crosspointEnabledChangeCallback{ nullptr };
        std::function<void(CrosspointCommander* sender, int, int)>       m_crosspointEnabledPollCallback{ nullptr };
    };

private:
    class ReinitIOCountMessage : public juce::Message
    {
    public:
        ReinitIOCountMessage(int inputs, int outputs) { m_inputCount = inputs; m_outputCount = outputs; }

        juce::MemoryBlock getSerializedMessage() const
        {
            juce::MemoryBlock data;
            data.append(&m_inputCount, sizeof(int));
            data.append(&m_outputCount, sizeof(int));
            return data;
        };

        int getInputCount() const { return m_inputCount; };
        int getOutputCount() const { return m_outputCount; };

    private:
        int m_inputCount = 0;
        int m_outputCount = 0;
    };

    class InterprocessConnectionImpl : public juce::InterprocessConnection
    {
    public:
        InterprocessConnectionImpl() : juce::InterprocessConnection() {};
        virtual ~InterprocessConnectionImpl() { disconnect(); };

        void connectionMade() override { if (onConnectionMade) onConnectionMade(); };

        void connectionLost() override { if (onConnectionLost) onConnectionLost(); };

        void messageReceived(const MemoryBlock& message) override { if (onMessageReceived) onMessageReceived(message); };

        std::function<void()>                   onConnectionMade;
        std::function<void()>                   onConnectionLost;
        std::function<void(const MemoryBlock&)> onMessageReceived;

    private:

    };

    class InterprocessConnectionServerImpl : public juce::InterprocessConnectionServer
    {
    public:
        InterprocessConnectionServerImpl() : juce::InterprocessConnectionServer() {};
        virtual ~InterprocessConnectionServerImpl() {};

        bool hasActiveConnection() { return m_connection && m_connection->isConnected(); };

        const std::unique_ptr<InterprocessConnectionImpl>& getActiveConnection() { return m_connection; };

        std::function<void()>   onConnectionCreated;

    private:
        InterprocessConnection* createConnectionObject() {
            m_connection = std::make_unique<InterprocessConnectionImpl>();

            if (onConnectionCreated)
                onConnectionCreated();

            return m_connection.get();
        };

        std::unique_ptr<InterprocessConnectionImpl> m_connection;
    };

public:
    MenuBarMatrixProcessor(XmlElement* stateXml);
    ~MenuBarMatrixProcessor();

    //==============================================================================
    void addInputListener(ProcessorDataAnalyzer::Listener* listener);
    void removeInputListener(ProcessorDataAnalyzer::Listener* listener);
    void addOutputListener(ProcessorDataAnalyzer::Listener* listener);
    void removeOutputListener(ProcessorDataAnalyzer::Listener* listener);

    //==============================================================================
    void addInputCommander(InputCommander* commander);
    void initializeInputCommander(InputCommander* commander);
    void removeInputCommander(InputCommander* commander);

    void addOutputCommander(OutputCommander* commander);
    void initializeOutputCommander(OutputCommander* commander);
    void removeOutputCommander(OutputCommander* comander);

    void addCrosspointCommander(CrosspointCommander* commander);
    void initializeCrosspointCommander(CrosspointCommander* commander);
    void removeCrosspointCommander(CrosspointCommander* comander);

    //==============================================================================
    bool getInputMuteState(int channelNumber);
    void setInputMuteState(int channelNumber, bool muted, ChannelCommander* sender = nullptr);
    
    bool getMatrixCrosspointEnabledValue(int inputNumber, int outputNumber);
    void setMatrixCrosspointEnabledValue(int inputNumber, int outputNumber, bool enabled, ChannelCommander* sender = nullptr);

    bool getOutputMuteState(int channelNumber);
    void setOutputMuteState(int channelNumber, bool muted, ChannelCommander* sender = nullptr);


    //==============================================================================
    AudioDeviceManager* getDeviceManager();

    //==============================================================================
    const String getName() const override;
    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

    double getTailLengthSeconds() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const AudioIODeviceCallbackContext& context) override;
    
    void audioDeviceAboutToStart(AudioIODevice* device) override;
    void audioDeviceStopped() override;

    //==============================================================================
    void changeListenerCallback(ChangeBroadcaster* source) override;

    //==============================================================================
    void handleMessage(const Message& message) override;

    //==============================================================================
    std::unique_ptr<XmlElement> createStateXml() override;
    bool setStateXml(XmlElement* stateXml) override;

    //==============================================================================
    enum dBRange
    {
        min = -90,
        max = 0,
    };

    static int getGlobalMindB()
    {
        return dBRange::min;
    }

    static int getGlobalMaxdB()
    {
        return dBRange::max;
    }

    static constexpr int s_maxChannelCount = 64;
    static constexpr int s_maxNumSamples = 1024;

    static constexpr int s_minInputsCount = 1;
    static constexpr int s_minOutputsCount = 1;

protected:
    //==============================================================================
    void initializeCtrlValues(int inputCount, int outputCount);

private:
    //==============================================================================
    juce::String    m_Name;

    //==============================================================================
    juce::CriticalSection   m_readLock;

    float** m_processorChannels;

    //==============================================================================
    std::unique_ptr<AudioDeviceManager> m_deviceManager;

    //==============================================================================
    std::unique_ptr<ProcessorDataAnalyzer>  m_inputDataAnalyzer;
    std::unique_ptr<ProcessorDataAnalyzer>  m_outputDataAnalyzer;

    //==============================================================================
    std::vector<InputCommander*>    m_inputCommanders;
    std::vector<OutputCommander*>   m_outputCommanders;
    std::vector<CrosspointCommander*>   m_crosspointCommanders;

    //==============================================================================
    std::map<int, bool> m_inputMuteStates;
    std::map<int, bool> m_outputMuteStates;

    //==============================================================================
    int m_inputChannelCount{ 1 };
    int m_outputChannelCount{ 1 };

    //==============================================================================
    std::map<int, std::map<int, bool>>  m_matrixCrosspointEnabledValues;

    //==============================================================================
    std::unique_ptr<MenuBarMatrixEditor>  m_processorEditor;

    //==============================================================================
    std::unique_ptr<juce::NetworkServiceDiscovery::Advertiser>  m_serviceAdvertiser;
    std::unique_ptr<InterprocessConnectionServerImpl> m_networkServer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarMatrixProcessor)
};

} // namespace MenuBarMatrix
