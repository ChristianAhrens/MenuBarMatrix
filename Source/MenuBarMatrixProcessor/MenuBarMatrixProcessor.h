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


namespace MenuBarMatrix
{


//==============================================================================
/*
*/
class MenuBarMatrixProcessor : public AudioProcessor,
					                public AudioIODeviceCallback,
                                    public MessageListener
{
public:
    class ChannelCommander
    {
    public:
        ChannelCommander();
        virtual ~ChannelCommander();

    protected:

    private:
    };

    class InputCommander : public ChannelCommander
    {
    public:
        InputCommander();
        virtual ~InputCommander() override;

        void setInputMuteChangeCallback(const std::function<void(InputCommander* sender, int, bool)>& callback);
        void setInputGainChangeCallback(const std::function<void(InputCommander* sender, int, float)>& callback);
        void setInputLevelChangeCallback(const std::function<void(InputCommander* sender, int, float)>& callback);
        void setInputPositionChangeCallback(const std::function<void(InputCommander* sender, int, juce::Point<float>)>& callback);
        void setInputSpreadChangeCallback(const std::function<void(InputCommander* sender, int, float)>& callback);
        void setInputReverbChangeCallback(const std::function<void(InputCommander* sender, int, float)>& callback);

        void setInputMutePollCallback(const std::function<void(InputCommander* sender, int)>& callback);
        void setInputGainPollCallback(const std::function<void(InputCommander* sender, int)>& callback);
        void setInputLevelPollCallback(const std::function<void(InputCommander* sender, int)>& callback);
        void setInputPositionPollCallback(const std::function<void(InputCommander* sender, int)>& callback);
        void setInputSpreadPollCallback(const std::function<void(InputCommander* sender, int)>& callback);
        void setInputReverbPollCallback(const std::function<void(InputCommander* sender, int)>& callback);

        virtual void setInputMute(unsigned int channel, bool muteState) = 0;
        virtual void setInputGain(unsigned int channel, float gainValue) = 0;
        virtual void setInputPosition(unsigned int channel, juce::Point<float> position) = 0;
        virtual void setInputSpread(unsigned int channel, float spreadValue) = 0;
        virtual void setInputReverb(unsigned int channel, float reverbValue) = 0;

        virtual void setInputLevel(unsigned int channel, float levelValue) { ignoreUnused(channel); ignoreUnused(levelValue); };

    protected:
        void inputMuteChange(int channel, bool muteState);
        void inputGainChange(int channel, float gainValue);
        void inputLevelChange(int channel, float levelValue);
        void inputPositionChange(int channel, const juce::Point<float>& positionValue);
        void inputSpreadChange(int channel, const float spreadValue);
        void inputReverbChange(int channel, const float reverbValue);

        void inputMutePoll(int channel);
        void inputGainPoll(int channel);
        void inputLevelPoll(int channel);
        void inputPositionPoll(int channel);
        void inputSpreadPoll(int channel);
        void inputReverbPoll(int channel);

    private:
        std::function<void(InputCommander* sender, int, bool)>                  m_inputMuteChangeCallback{ nullptr };
        std::function<void(InputCommander* sender, int, float)>                 m_inputGainChangeCallback{ nullptr };
        std::function<void(InputCommander* sender, int, float)>                 m_inputLevelChangeCallback{ nullptr };
        std::function<void(InputCommander* sender, int, juce::Point<float>)>    m_inputPositionChangeCallback{ nullptr };
        std::function<void(InputCommander* sender, int, float)>                 m_inputSpreadChangeCallback{ nullptr };
        std::function<void(InputCommander* sender, int, float)>                 m_inputReverbChangeCallback{ nullptr };

        std::function<void(InputCommander* sender, int)>    m_inputMutePollCallback{ nullptr };
        std::function<void(InputCommander* sender, int)>    m_inputGainPollCallback{ nullptr };
        std::function<void(InputCommander* sender, int)>    m_inputLevelPollCallback{ nullptr };
        std::function<void(InputCommander* sender, int)>    m_inputPositionPollCallback{ nullptr };
        std::function<void(InputCommander* sender, int)>    m_inputSpreadPollCallback{ nullptr };
        std::function<void(InputCommander* sender, int)>    m_inputReverbPollCallback{ nullptr };
    };

    class OutputCommander : public ChannelCommander
    {
    public:
        OutputCommander();
        virtual ~OutputCommander() override;

        void setOutputMuteChangeCallback(const std::function<void(OutputCommander* sender, int, bool)>& callback);
        void setOutputGainChangeCallback(const std::function<void(OutputCommander* sender, int, float)>& callback);
        void setOutputLevelChangeCallback(const std::function<void(OutputCommander* sender, int, float)>& callback);
        void setOutputSchemeChangeCallback(const std::function<void(OutputCommander* sender, unsigned int)>& callback);

        void setOutputMutePollCallback(const std::function<void(OutputCommander* sender, int)>& callback);
        void setOutputGainPollCallback(const std::function<void(OutputCommander* sender, int)>& callback);
        void setOutputLevelPollCallback(const std::function<void(OutputCommander* sender, int)>& callback);
        void setOutputSchemePollCallback(const std::function<void(OutputCommander* sender)>& callback);

        virtual void setOutputMute(unsigned int channel, bool muteState) = 0;
        virtual void setOutputGain(unsigned int channel, float gainValue) = 0;
        virtual void setOutputScheme(unsigned int outputScheme) = 0;

        virtual void setOutputLevel(unsigned int channel, float levelValue) { ignoreUnused(channel); ignoreUnused(levelValue); };

    protected:
        void outputMuteChange(int channel, bool muteState);
        void outputGainChange(int channel, float gainValue);
        void outputLevelChange(int channel, float levelValue);
        void outputSchemeChange(unsigned int outputScheme);

        void outputMutePoll(int channel);
        void outputGainPoll(int channel);
        void outputLevelPoll(int channel);
        void outputSchemePoll();

    private:
        std::function<void(OutputCommander* sender, int, bool)>     m_outputMuteChangeCallback{ nullptr };
        std::function<void(OutputCommander* sender, int, float)>    m_outputGainChangeCallback{ nullptr };
        std::function<void(OutputCommander* sender, int, float)>    m_outputLevelChangeCallback{ nullptr };
        std::function<void(OutputCommander* sender, unsigned int)>  m_outputSchemeChangeCallback{ nullptr };

        std::function<void(OutputCommander* sender, int)>   m_outputMutePollCallback{ nullptr };
        std::function<void(OutputCommander* sender, int)>   m_outputGainPollCallback{ nullptr };
        std::function<void(OutputCommander* sender, int)>   m_outputLevelPollCallback{ nullptr };
        std::function<void(OutputCommander* sender)>        m_outputSchemePollCallback{ nullptr };
    };

public:
    MenuBarMatrixProcessor();
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

    //==============================================================================
    bool getInputMuteState(int channelNumber);
    void setInputMuteState(int channelNumber, bool muted, ChannelCommander* sender = nullptr);
    float getInputGainValue(int channelNumber);
    void setInputGainValue(int channelNumber, float value, ChannelCommander* sender = nullptr);
    float getInputReverbValue(int channelNumber);
    void setInputReverbValue(int channelNumber, float value, ChannelCommander* sender = nullptr);
    float getInputSpreadValue(int channelNumber);
    void setInputSpreadValue(int channelNumber, float value, ChannelCommander* sender = nullptr);
    const juce::Point<float>& getInputPositionValue(int channelNumber);
    void setInputPositionValue(int channelNumber, const juce::Point<float>& position, ChannelCommander* sender = nullptr);

    bool getOutputMuteState(int channelNumber);
    void setOutputMuteState(int channelNumber, bool muted, ChannelCommander* sender = nullptr);
    float getOutputGainValue(int channelNumber);
    void setOutputGainValue(int channelNumber, float value, ChannelCommander* sender = nullptr);


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
    float getInputToOutputGain(int input, int output);

    //==============================================================================
    void handleMessage(const Message& message) override;

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
    static constexpr int s_minOutputsCount = 5;

    static constexpr juce::Point<float> s_defaultPos(){return juce::Point<float>(0.5f, 0.5f);};
    juce::Point<float> m_leftPos;
    juce::Point<float> m_rightPos;
    juce::Point<float> m_centerPos;
    juce::Point<float> m_leftSurroundPos;
    juce::Point<float> m_rightSurroundPos;

protected:
    //==============================================================================
    void initializeInputCtrlValues(int inputCount);
    void initializeOutputCtrlValues(int outputCount);

private:
    const juce::Point<float> getInputPosition(int channelNumber);
    const juce::Point<float> getOutputPosition(int channelNumber);

    //==============================================================================
    const juce::Point<float> getNormalizedDefaultPosition(juce::AudioChannelSet::ChannelType channelIdent);

    //==============================================================================
    String                      m_Name;

    //==============================================================================
    CriticalSection     m_readLock;

    float**             m_processorChannels;

    //==============================================================================
    std::unique_ptr<AudioDeviceManager> m_deviceManager;

    //==============================================================================
    std::unique_ptr<ProcessorDataAnalyzer>  m_inputDataAnalyzer;
    std::unique_ptr<ProcessorDataAnalyzer>  m_outputDataAnalyzer;

    //==============================================================================
    std::vector<InputCommander*>    m_inputCommanders;
    std::vector<OutputCommander*>   m_outputCommanders;

    //==============================================================================
    std::map<int, bool> m_inputMuteStates;
    std::map<int, bool> m_outputMuteStates;

    //==============================================================================
    std::map<int, float> m_inputGainValues;
    std::map<int, float> m_outputGainValues;

    //==============================================================================
    std::map<int, float> m_inputReverbValues;
    
    //==============================================================================
    std::map<int, float> m_inputSpreadValues;

    //==============================================================================
    std::map<int, juce::Point<float>>  m_inputPositionValues;

    //==============================================================================
    std::unique_ptr<MenuBarMatrixEditor>  m_processorEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarMatrixProcessor)
};

} // namespace MenuBarMatrix
