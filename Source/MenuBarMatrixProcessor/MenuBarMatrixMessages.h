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
/*
 *
 */
class ReinitIOCountMessage;
class AudioInputBufferMessage;
class AudioOutputBufferMessage;
class SerializableMessage : public juce::Message
{
public:
    enum SerializableMessageType
    {
        None = 0,
        ReinitIOCount,
        AudioInputBuffer,
        AudioOutputBuffer
    };

public:
    SerializableMessage() = default;
    virtual ~SerializableMessage() = default;

    juce::MemoryBlock getSerializedMessage() const
    {
        size_t contentSize = 0;
        juce::MemoryBlock blob;
        blob.append(&m_type, sizeof(SerializableMessageType));
        blob.append(createSerializedContent(contentSize).getData(), contentSize);
        return blob;
    };
    static SerializableMessage* initFromMemoryBlock(const juce::MemoryBlock& blob)
    {
        jassert(blob.getSize() >= sizeof(SerializableMessage));
        if (blob.getSize() < sizeof(SerializableMessage))
            return nullptr;

        auto type = static_cast<SerializableMessageType>(blob[0]);
        switch (type)
        {
        case ReinitIOCount:
            return reinterpret_cast<SerializableMessage*>(std::make_unique<ReinitIOCountMessage>(blob).release());
        case AudioInputBuffer:
            return reinterpret_cast<SerializableMessage*>(std::make_unique<AudioInputBufferMessage>(blob).release());
        case AudioOutputBuffer:
            return reinterpret_cast<SerializableMessage*>(std::make_unique<AudioOutputBufferMessage>(blob).release());
        case None:
        default:
            return nullptr;
        }
    };

protected:
    //==============================================================================
    virtual juce::MemoryBlock createSerializedContent(size_t& contentSize) const = 0;

    //==============================================================================
    std::uint32_t ReadUint32(const char* buffer)
    {
        return (((static_cast<std::uint8_t>(buffer[0]) << 24) & 0xff000000) +
            ((static_cast<std::uint8_t>(buffer[1]) << 16) & 0x00ff0000) +
            ((static_cast<std::uint8_t>(buffer[2]) << 8) & 0x0000ff00) +
            static_cast<std::uint8_t>(buffer[3]));
    };
    std::uint16_t ReadUint16(const char* buffer)
    {
        return (((static_cast<std::uint8_t>(buffer[0]) << 8) & 0xff00) +
            static_cast<std::uint8_t>(buffer[1]));
    };

    //==============================================================================
    SerializableMessageType m_type = SerializableMessageType::None;
};

//==============================================================================
/*
 *
 */
class ReinitIOCountMessage : public SerializableMessage
{
public:
    ReinitIOCountMessage() = default;
    ReinitIOCountMessage(int inputs, int outputs) { m_type = SerializableMessageType::ReinitIOCount; m_inputCount = std::uint16_t(inputs); m_outputCount = std::uint16_t(outputs); };
    ReinitIOCountMessage(const juce::MemoryBlock& blob)
    {
        jassert(SerializableMessageType::ReinitIOCount == static_cast<SerializableMessageType>(blob[0]));

        m_type = SerializableMessageType::ReinitIOCount;
        m_inputCount = ReadUint16(blob.begin() + sizeof(SerializableMessageType));
        m_outputCount = ReadUint16(blob.begin() + sizeof(SerializableMessageType) + sizeof(int));

    };
    ~ReinitIOCountMessage() = default;

    int getInputCount() const { return m_inputCount; };
    int getOutputCount() const { return m_outputCount; };

protected:
    juce::MemoryBlock createSerializedContent(size_t& contentSize) const override
    {
        juce::MemoryBlock blob;
        blob.append(&m_inputCount, sizeof(std::uint16_t));
        blob.append(&m_outputCount, sizeof(std::uint16_t));
        contentSize = blob.getSize();
        return blob;
    };

private:
    std::uint16_t m_inputCount = 0;
    std::uint16_t m_outputCount = 0;
};

//==============================================================================
/*
 * 
 */
class AudioBufferMessage : public SerializableMessage
{
public:
    enum FlowDirection
    {
        Invalid,
        Input,
        Output,
    };

public:
    AudioBufferMessage() = default;
    AudioBufferMessage(juce::AudioBuffer<float>& buffer) { m_buffer = buffer; };
    ~AudioBufferMessage() = default;

    const juce::AudioBuffer<float>& getAudioBuffer() const { return m_buffer; };
    const FlowDirection getFlowDirection() const { return m_direction; };

protected:
    juce::MemoryBlock createSerializedContent(size_t& contentSize) const {
        auto numChannels = std::uint16_t(m_buffer.getNumChannels());
        auto numSamples = std::uint16_t(m_buffer.getNumSamples());
        juce::MemoryBlock blob;
        blob.append(&m_direction, sizeof(FlowDirection));
        blob.append(&numChannels, sizeof(std::uint16_t));
        blob.append(&numSamples, sizeof(std::uint16_t));
        blob.append(m_buffer.getReadPointer(1), sizeof(float) * m_buffer.getNumChannels() * m_buffer.getNumSamples());
        contentSize = blob.getSize();
        return blob;
    };

    FlowDirection               m_direction{ FlowDirection::Invalid };
    juce::AudioBuffer<float>    m_buffer;

};

//==============================================================================
/*
 *
 */
class AudioInputBufferMessage : public AudioBufferMessage
{
public:
    AudioInputBufferMessage() = default;
    AudioInputBufferMessage(juce::AudioBuffer<float>& buffer) : AudioBufferMessage(buffer) { m_type = SerializableMessageType::AudioInputBuffer; m_direction = FlowDirection::Input; };
    AudioInputBufferMessage(const juce::MemoryBlock& blob)
    {
        jassert(SerializableMessageType::AudioInputBuffer == static_cast<SerializableMessageType>(blob[0]));

        m_type = SerializableMessageType::AudioInputBuffer;

        auto readPos = blob.begin() + sizeof(SerializableMessageType);
        m_direction = FlowDirection(ReadUint32(readPos));
        jassert(FlowDirection::Input == m_direction);

        readPos += sizeof(FlowDirection);
        auto numChannels = ReadUint16(readPos);
        readPos += sizeof(std::uint16_t);
        auto numSamples = ReadUint16(readPos);
        readPos += sizeof(std::uint16_t);
        auto data = reinterpret_cast<const float*>(readPos);

        m_buffer = juce::AudioBuffer<float>(numChannels, numSamples);
        m_buffer.copyFrom(1, 1, data, numChannels * numSamples);
    };
    ~AudioInputBufferMessage() = default;
};

//==============================================================================
/*
 *
 */
class AudioOutputBufferMessage : public AudioBufferMessage
{
public:
    AudioOutputBufferMessage() = default;
    AudioOutputBufferMessage(juce::AudioBuffer<float>& buffer) : AudioBufferMessage(buffer) { m_type = SerializableMessageType::AudioOutputBuffer; m_direction = FlowDirection::Output; };
    AudioOutputBufferMessage(const juce::MemoryBlock& blob)
    {
        jassert(SerializableMessageType::AudioOutputBuffer == static_cast<SerializableMessageType>(blob[0]));

        m_type = SerializableMessageType::AudioOutputBuffer;

        auto readPos = blob.begin() + sizeof(SerializableMessageType);
        m_direction = FlowDirection(ReadUint32(readPos));
        jassert(FlowDirection::Output == m_direction);

        readPos += sizeof(FlowDirection);
        auto numChannels = ReadUint16(readPos);
        readPos += sizeof(std::uint16_t);
        auto numSamples = ReadUint16(readPos);
        readPos += sizeof(std::uint16_t);
        auto data = reinterpret_cast<const float*>(readPos);

        m_buffer = juce::AudioBuffer<float>(numChannels, numSamples);
        m_buffer.copyFrom(1, 1, data, numChannels * numSamples);
    };
    ~AudioOutputBufferMessage() = default;
};


};
