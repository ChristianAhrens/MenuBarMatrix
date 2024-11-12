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

class InterprocessConnectionImpl;
class InterprocessConnectionServerImpl;
class MenuBarMatrixChannelCommander;
class MenuBarMatrixInputCommander;
class MenuBarMatrixOutputCommander;
class MenuBarMatrixCrosspointCommander;
#if JUCE_WINDOWS
struct ServiceAdvertiser;
#endif


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
    MenuBarMatrixProcessor(XmlElement* stateXml);
    ~MenuBarMatrixProcessor();

    //==============================================================================
    void addInputListener(ProcessorDataAnalyzer::Listener* listener);
    void removeInputListener(ProcessorDataAnalyzer::Listener* listener);
    void addOutputListener(ProcessorDataAnalyzer::Listener* listener);
    void removeOutputListener(ProcessorDataAnalyzer::Listener* listener);

    //==============================================================================
    void addInputCommander(MenuBarMatrixInputCommander* commander);
    void initializeInputCommander(MenuBarMatrixInputCommander* commander);
    void removeInputCommander(MenuBarMatrixInputCommander* commander);

    void addOutputCommander(MenuBarMatrixOutputCommander* commander);
    void initializeOutputCommander(MenuBarMatrixOutputCommander* commander);
    void removeOutputCommander(MenuBarMatrixOutputCommander* comander);

    void addCrosspointCommander(MenuBarMatrixCrosspointCommander* commander);
    void initializeCrosspointCommander(MenuBarMatrixCrosspointCommander* commander);
    void removeCrosspointCommander(MenuBarMatrixCrosspointCommander* comander);

    //==============================================================================
    bool getInputMuteState(int channelNumber);
    void setInputMuteState(int channelNumber, bool muted, MenuBarMatrixChannelCommander* sender = nullptr);
    
    bool getMatrixCrosspointEnabledValue(int inputNumber, int outputNumber);
    void setMatrixCrosspointEnabledValue(int inputNumber, int outputNumber, bool enabled, MenuBarMatrixChannelCommander* sender = nullptr);

    bool getOutputMuteState(int channelNumber);
    void setOutputMuteState(int channelNumber, bool muted, MenuBarMatrixChannelCommander* sender = nullptr);

    void setChannelCounts(int inputChannelCount, int outputChannelCount);

    //==============================================================================
    AudioDeviceManager* getDeviceManager();

    //==============================================================================
    std::map<int, std::pair<double, bool>> getNetworkHealth();

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
    void environmentChanged();

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
    double m_sampleRate = 0.0;
    int m_bufferSize = 0;

    //==============================================================================
    std::unique_ptr<AudioDeviceManager> m_deviceManager;

    //==============================================================================
    std::unique_ptr<ProcessorDataAnalyzer>  m_inputDataAnalyzer;
    std::unique_ptr<ProcessorDataAnalyzer>  m_outputDataAnalyzer;

    //==============================================================================
    std::vector<MenuBarMatrixInputCommander*>    m_inputCommanders;
    std::vector<MenuBarMatrixOutputCommander*>   m_outputCommanders;
    std::vector<MenuBarMatrixCrosspointCommander*>   m_crosspointCommanders;

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
#if JUCE_WINDOWS
    std::unique_ptr<ServiceAdvertiser>  m_serviceAdvertiser;
#else
    std::unique_ptr<juce::NetworkServiceDiscovery::Advertiser>  m_serviceAdvertiser;
#endif
    std::unique_ptr<InterprocessConnectionServerImpl> m_networkServer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarMatrixProcessor)
};

} // namespace MenuBarMatrix
