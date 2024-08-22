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

#include "ProcessorAudioSignalData.h"
#include "ProcessorLevelData.h"
#include "ProcessorSpectrumData.h"


namespace MenuBarMatrix
{

//==============================================================================
/*
*/
class AudioBufferMessage : public juce::Message
{
public:
    enum FlowDirection
    {
        Invalid,
        Input, 
        Output,
    };

public:
    AudioBufferMessage(AudioBuffer<float>& buffer);
    ~AudioBufferMessage();

    juce::MemoryBlock getSerializedMessage() const;

    const AudioBuffer<float>& getAudioBuffer() const;
    const FlowDirection getFlowDirection() const { return m_direction; };

protected:
    FlowDirection m_direction{ FlowDirection::Invalid };

private:
    AudioBuffer<float> m_buffer;

};

class AudioInputBufferMessage : public AudioBufferMessage
{
public:
    AudioInputBufferMessage(AudioBuffer<float>& buffer) : AudioBufferMessage(buffer) { m_direction = FlowDirection::Input; }
};

class AudioOutputBufferMessage : public AudioBufferMessage
{
public:
    AudioOutputBufferMessage(AudioBuffer<float>& buffer) : AudioBufferMessage(buffer) { m_direction = FlowDirection::Output; }
};

//==============================================================================
/*
*/
class ProcessorDataAnalyzer :    public Timer
{
public:
    class Listener
    {
    public:
        virtual ~Listener() {};

        virtual void processingDataChanged(AbstractProcessorData* data) = 0;
    };

public:
    ProcessorDataAnalyzer();
    ~ProcessorDataAnalyzer();

    //==============================================================================
    void initializeParameters(double sampleRate, int bufferSize);
    void clearParameters();

    void setHoldTime(int holdTimeMs);

    ProcessorAudioSignalData& GetCentiSecondBuffer() { return m_centiSecondBuffer; };
    ProcessorLevelData& GetLevel() { return m_level; };
    ProcessorSpectrumData& GetSpectrum() { return m_spectrum; };
    String& GetName() { return m_Name; };

    //==============================================================================
    void addListener(Listener* listener);
    void removeListener(Listener* listener);

    //==============================================================================
    void analyzeData(const AudioBuffer<float>& buffer);

    //==============================================================================
    void timerCallback() override;

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

private:
    void BroadcastData(AbstractProcessorData* data);
    void FlushHold();

    ProcessorAudioSignalData    m_centiSecondBuffer;
    ProcessorLevelData          m_level;
    ProcessorSpectrumData       m_spectrum;

    String                      m_Name;
    Array<Listener*>            m_callbackListeners;

    //==============================================================================
    CriticalSection     m_readLock;

    float**             m_processorChannels;

    unsigned long       m_sampleRate = 0;
    int                 m_samplesPerCentiSecond = 0;
    int                 m_bufferSize = 0;
    int                 m_missingSamplesForCentiSecond;

    //==============================================================================
    enum
    {
        fftOrder = 12,
        fftSize = 1 << fftOrder
    };
    dsp::FFT                                    m_fwdFFT;
    dsp::WindowingFunction<float>               m_windowF;
    float                                       m_FFTdata[2 * fftSize];
    int                                         m_FFTdataPos;

    int                                         m_holdTimeMs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorDataAnalyzer)
};

} // namespace MenuBarMatrix
