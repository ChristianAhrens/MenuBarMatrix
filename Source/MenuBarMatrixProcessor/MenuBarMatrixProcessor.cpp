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

#include "MenuBarMatrixProcessor.h"

#include "MenuBarMatrixServiceData.h"
#include "../AppConfiguration.h"

namespace MenuBarMatrix
{


MenuBarMatrixProcessor::ChannelCommander::ChannelCommander()
{
}

MenuBarMatrixProcessor::ChannelCommander::~ChannelCommander()
{
}

MenuBarMatrixProcessor::InputCommander::InputCommander()
{
}

MenuBarMatrixProcessor::InputCommander::~InputCommander()
{
}

void MenuBarMatrixProcessor::InputCommander::setInputMuteChangeCallback(const std::function<void(InputCommander* sender, int, bool)>& callback)
{
	m_inputMuteChangeCallback = callback;
}


void MenuBarMatrixProcessor::InputCommander::setInputLevelChangeCallback(const std::function<void(InputCommander* sender, int, float)>& callback)
{
	m_inputLevelChangeCallback = callback;
}
void MenuBarMatrixProcessor::InputCommander::setInputMutePollCallback(const std::function<void(InputCommander* sender, int)>& callback)
{
	m_inputMutePollCallback = callback;
}


void MenuBarMatrixProcessor::InputCommander::setInputLevelPollCallback(const std::function<void(InputCommander* sender, int)>& callback)
{
	m_inputLevelPollCallback = callback;
}
void MenuBarMatrixProcessor::InputCommander::inputMuteChange(int channel, bool muteState)
{
	if (m_inputMuteChangeCallback)
		m_inputMuteChangeCallback(this, channel, muteState);
}


void MenuBarMatrixProcessor::InputCommander::inputLevelChange(int channel, float levelValue)
{
	if (m_inputLevelChangeCallback)
		m_inputLevelChangeCallback(this, channel, levelValue);
}
void MenuBarMatrixProcessor::InputCommander::inputMutePoll(int channel)
{
	if (m_inputMutePollCallback)
		m_inputMutePollCallback(this, channel);
}


void MenuBarMatrixProcessor::InputCommander::inputLevelPoll(int channel)
{
	if (m_inputLevelPollCallback)
		m_inputLevelPollCallback(this, channel);
}


MenuBarMatrixProcessor::OutputCommander::OutputCommander()
{
}

MenuBarMatrixProcessor::OutputCommander::~OutputCommander()
{
}

void MenuBarMatrixProcessor::OutputCommander::setOutputMuteChangeCallback(const std::function<void(OutputCommander* sender, int, bool)>& callback)
{
	m_outputMuteChangeCallback = callback;
}

void MenuBarMatrixProcessor::OutputCommander::setOutputLevelChangeCallback(const std::function<void(OutputCommander* sender, int, float)>& callback)
{
	m_outputLevelChangeCallback = callback;
}
void MenuBarMatrixProcessor::OutputCommander::setOutputMutePollCallback(const std::function<void(OutputCommander* sender, int)>& callback)
{
	m_outputMutePollCallback = callback;
}

void MenuBarMatrixProcessor::OutputCommander::setOutputLevelPollCallback(const std::function<void(OutputCommander* sender, int)>& callback)
{
	m_outputLevelPollCallback = callback;
}

void MenuBarMatrixProcessor::OutputCommander::outputMuteChange(int channel, bool muteState)
{
	if (m_outputMuteChangeCallback)
		m_outputMuteChangeCallback(nullptr, channel, muteState);
}

void MenuBarMatrixProcessor::OutputCommander::outputLevelChange(int channel, float levelValue)
{
	if (m_outputLevelChangeCallback)
		m_outputLevelChangeCallback(nullptr, channel, levelValue);
}

void MenuBarMatrixProcessor::OutputCommander::outputMutePoll(int channel)
{
	if (m_outputMutePollCallback)
		m_outputMutePollCallback(nullptr, channel);
}

void MenuBarMatrixProcessor::OutputCommander::outputLevelPoll(int channel)
{
	if (m_outputLevelPollCallback)
		m_outputLevelPollCallback(nullptr, channel);
}


MenuBarMatrixProcessor::CrosspointCommander::CrosspointCommander()
{
}

MenuBarMatrixProcessor::CrosspointCommander::~CrosspointCommander()
{
}

void MenuBarMatrixProcessor::CrosspointCommander::setCrosspointEnabledChangeCallback(const std::function<void(CrosspointCommander* sender, int, int, bool)>& callback)
{
	m_crosspointEnabledChangeCallback = callback;
}

void MenuBarMatrixProcessor::CrosspointCommander::setCrosspointEnabledPollCallback(const std::function<void(CrosspointCommander* sender, int, int)>& callback)
{
	m_crosspointEnabledPollCallback = callback;
}

void MenuBarMatrixProcessor::CrosspointCommander::crosspointEnabledChange(int input, int output, bool enabledState)
{
	if (m_crosspointEnabledChangeCallback)
		m_crosspointEnabledChangeCallback(nullptr, input, output, enabledState);
}

void MenuBarMatrixProcessor::CrosspointCommander::crosspointEnabledPoll(int input, int output)
{
	if (m_crosspointEnabledPollCallback)
		m_crosspointEnabledPollCallback(nullptr, input, output);
}


//==============================================================================
MenuBarMatrixProcessor::MenuBarMatrixProcessor(XmlElement* stateXml) :
	juce::AudioProcessor()
{
	// prepare max sized processing data buffer
	m_processorChannels = new float* [s_maxChannelCount];
	for (auto i = 0; i < s_maxChannelCount; i++)
	{
		m_processorChannels[i] = new float[s_maxNumSamples];
		for (auto j = 0; j < s_maxNumSamples; j++)
		{
			m_processorChannels[i][j] = 0.0f;
		}
	}

	m_inputDataAnalyzer = std::make_unique<ProcessorDataAnalyzer>();
	m_outputDataAnalyzer = std::make_unique<ProcessorDataAnalyzer>();

	m_deviceManager = std::make_unique<AudioDeviceManager>();
	m_deviceManager->addAudioCallback(this);
    m_deviceManager->addChangeListener(this);
	
	if (!setStateXml(stateXml))
	{
        setStateXml(nullptr); // call without actual xml config causes default init
		triggerConfigurationUpdate(false);
	}

	// init the announcement of this app instance as discoverable service
	m_serviceAdvertiser = std::make_unique<juce::NetworkServiceDiscovery::Advertiser>(
		MenuBarMatrix::ServiceData::getServiceTypeUID(), 
		MenuBarMatrix::ServiceData::getServiceDescription(),
		MenuBarMatrix::ServiceData::getBroadcastPort(),
		MenuBarMatrix::ServiceData::getConnectionPort());

	m_networkServer = std::make_unique<InterprocessConnectionServerImpl>();
	m_networkServer->beginWaitingForSocket(MenuBarMatrix::ServiceData::getConnectionPort());
    m_networkServer->onConnectionCreated = [=]() {
        auto connection = dynamic_cast<InterprocessConnectionImpl*>(m_networkServer->getActiveConnection().get());
        if (connection)
        {
            connection->onConnectionLost = [=]() {};
            connection->onConnectionMade = [=]() {};
            connection->onMessageReceived = [=](const juce::MemoryBlock& data) {};
        }
    };
}

MenuBarMatrixProcessor::~MenuBarMatrixProcessor()
{
	m_networkServer->stop();

	m_deviceManager->removeAudioCallback(this);

	// cleanup processing data buffer (do this elsewhere in productive code to avoid excessive mem alloc/free)
	for (auto i = 0; i < s_maxChannelCount; i++)
		delete[] m_processorChannels[i];
	delete[] m_processorChannels;
}

std::unique_ptr<XmlElement> MenuBarMatrixProcessor::createStateXml()
{
	auto stateXml = std::make_unique<XmlElement>(AppConfiguration::getTagName(AppConfiguration::TagID::PROCESSORCONFIG));
	if (m_deviceManager)
	{
		auto devConfElm = std::make_unique<XmlElement>(AppConfiguration::getTagName(AppConfiguration::TagID::DEVCONFIG));
		devConfElm->addChildElement(m_deviceManager->createStateXml().release());
		stateXml->addChildElement(devConfElm.release());
	}
	else
		return nullptr;

	return std::move(stateXml);
}

bool MenuBarMatrixProcessor::setStateXml(XmlElement* stateXml)
{
    juce::XmlElement* deviceSetupXml = nullptr;
    
	if (nullptr != stateXml && stateXml->getTagName() == AppConfiguration::getTagName(AppConfiguration::TagID::PROCESSORCONFIG))
    {
        auto devConfElm = stateXml->getChildByName(AppConfiguration::getTagName(AppConfiguration::TagID::DEVCONFIG));
        if (nullptr != devConfElm)
            deviceSetupXml = devConfElm->getChildByName("DEVICESETUP");
    }
    
	if (m_deviceManager)
	{
        // Hacky bit of device manager initialization:
        // We first intialize it to be able to get a valid device setup,
        // then initialize with a dummy xml config to trigger the internal xml structure being reset
        // and finally apply the original initialized device setup again to have the audio running correctly.
        // If we did not do so, either the internal xml would not be present as long as the first configuration change was made
        // and therefor no valid config file could be written by the application or the audio would not be running
        // on first start and manual config would be required.
        m_deviceManager->initialiseWithDefaultDevices(s_maxChannelCount, s_maxChannelCount);
        auto audioDeviceSetup = m_deviceManager->getAudioDeviceSetup();
		auto result = m_deviceManager->initialise(s_maxChannelCount, s_maxChannelCount, deviceSetupXml, true, {}, &audioDeviceSetup);
        if (result.isNotEmpty())
        {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, juce::JUCEApplication::getInstance()->getApplicationName() + " device init failed", result);
            return false;
        }
        else
        {
#if JUCE_IOS
            if (audioDeviceSetup.bufferSize < 512)
                audioDeviceSetup.bufferSize = 512; // temp. workaround for iOS where buffersizes <512 lead to no sample data being delivered?
#endif
            m_deviceManager->setAudioDeviceSetup(audioDeviceSetup, true);
            return true;
        }
	}
	else
		return false;
}

void MenuBarMatrixProcessor::addInputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->addListener(listener);
}

void MenuBarMatrixProcessor::removeInputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->removeListener(listener);
}

void MenuBarMatrixProcessor::addOutputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->addListener(listener);
}

void MenuBarMatrixProcessor::removeOutputListener(ProcessorDataAnalyzer::Listener* listener)
{
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->removeListener(listener);
}

void MenuBarMatrixProcessor::addInputCommander(InputCommander* commander)
{
	if (commander == nullptr)
		return;

	if (std::find(m_inputCommanders.begin(), m_inputCommanders.end(), commander) == m_inputCommanders.end())
	{
		initializeInputCommander(commander);

		m_inputCommanders.push_back(commander);
		commander->setInputMuteChangeCallback([=](ChannelCommander* sender, int channel, bool state) { return setInputMuteState(channel, state, sender); } );
	}
}

void MenuBarMatrixProcessor::initializeInputCommander(InputCommander* commander)
{
	if (nullptr != commander)
	{
        const ScopedLock sl(m_readLock);
        for (auto const& inputMuteStatesKV : m_inputMuteStates)
            commander->setInputMute(inputMuteStatesKV.first, inputMuteStatesKV.second);
	}
}

void MenuBarMatrixProcessor::removeInputCommander(InputCommander* commander)
{
	if (commander == nullptr)
		return;

	auto existingInputCommander = std::find(m_inputCommanders.begin(), m_inputCommanders.end(), commander);
	if (existingInputCommander != m_inputCommanders.end())
		m_inputCommanders.erase(existingInputCommander);
}

void MenuBarMatrixProcessor::addOutputCommander(OutputCommander* commander)
{
	if (commander == nullptr)
		return;

	if (std::find(m_outputCommanders.begin(), m_outputCommanders.end(), commander) == m_outputCommanders.end())
	{
		initializeOutputCommander(commander);

		m_outputCommanders.push_back(commander);
		commander->setOutputMuteChangeCallback([=](ChannelCommander* sender, int channel, bool state) { return setOutputMuteState(channel, state, sender); });
	}
}

void MenuBarMatrixProcessor::initializeOutputCommander(OutputCommander* commander)
{
	if (nullptr != commander)
	{
        const ScopedLock sl(m_readLock);
        for (auto const& outputMuteStatesKV : m_outputMuteStates)
            commander->setOutputMute(outputMuteStatesKV.first, outputMuteStatesKV.second);
	}
}

void MenuBarMatrixProcessor::removeOutputCommander(OutputCommander* commander)
{
	if (commander == nullptr)
		return;

	auto existingOutputCommander = std::find(m_outputCommanders.begin(), m_outputCommanders.end(), commander);
	if (existingOutputCommander != m_outputCommanders.end())
		m_outputCommanders.erase(existingOutputCommander);
}

void MenuBarMatrixProcessor::addCrosspointCommander(CrosspointCommander* commander)
{
	if (commander == nullptr)
		return;

	if (std::find(m_crosspointCommanders.begin(), m_crosspointCommanders.end(), commander) == m_crosspointCommanders.end())
	{
		initializeCrosspointCommander(commander);

		m_crosspointCommanders.push_back(commander);
		commander->setCrosspointEnabledChangeCallback([=](ChannelCommander* sender, int input, int output, bool state) { return setMatrixCrosspointEnabledValue(input, output, state, sender); });
	}
}

void MenuBarMatrixProcessor::initializeCrosspointCommander(CrosspointCommander* commander)
{
	if (nullptr != commander)
	{
		const ScopedLock sl(m_readLock);
		for (auto const& matrixCrosspointEnabledKV : m_matrixCrosspointEnabledValues)
		{
			for (auto const& matrixCrosspointEnabledNodeKV : matrixCrosspointEnabledKV.second)
			{
				auto& input = matrixCrosspointEnabledKV.first;
				auto& output = matrixCrosspointEnabledNodeKV.first;
				auto& state = matrixCrosspointEnabledNodeKV.second;
				commander->setCrosspointEnabledValue(input, output, state);
			}
		}
	}
}

void MenuBarMatrixProcessor::removeCrosspointCommander(CrosspointCommander* commander)
{
	if (commander == nullptr)
		return;

	auto existingCrosspointCommander = std::find(m_crosspointCommanders.begin(), m_crosspointCommanders.end(), commander);
	if (existingCrosspointCommander != m_crosspointCommanders.end())
		m_crosspointCommanders.erase(existingCrosspointCommander);
}

bool MenuBarMatrixProcessor::getInputMuteState(int inputChannelNumber)
{
	jassert(inputChannelNumber > 0);
	const ScopedLock sl(m_readLock);
	return m_inputMuteStates[inputChannelNumber];
}

void MenuBarMatrixProcessor::setInputMuteState(int inputChannelNumber, bool muted, ChannelCommander* sender)
{
	jassert(inputChannelNumber > 0);

	for (auto const& inputCommander : m_inputCommanders)
	{
		if (inputCommander != reinterpret_cast<InputCommander*>(sender))
			inputCommander->setInputMute(inputChannelNumber, muted);
	}

	const ScopedLock sl(m_readLock);
	m_inputMuteStates[inputChannelNumber] = muted;
}

bool MenuBarMatrixProcessor::getMatrixCrosspointEnabledValue(int inputNumber, int outputNumber)
{
    jassert(inputNumber > 0 && outputNumber > 0);
    const ScopedLock sl(m_readLock);
    return m_matrixCrosspointEnabledValues[inputNumber][outputNumber];
}

void MenuBarMatrixProcessor::setMatrixCrosspointEnabledValue(int inputNumber, int outputNumber, bool enabled, ChannelCommander* sender)
{
    jassert(inputNumber > 0 && outputNumber > 0);

    for (auto const& crosspointCommander : m_crosspointCommanders)
    {
        if (crosspointCommander != reinterpret_cast<CrosspointCommander*>(sender))
            crosspointCommander->setCrosspointEnabledValue(inputNumber, outputNumber, enabled);
    }

    const ScopedLock sl(m_readLock);
    m_matrixCrosspointEnabledValues[inputNumber][outputNumber] = enabled;
}

bool MenuBarMatrixProcessor::getOutputMuteState(int outputChannelNumber)
{
	jassert(outputChannelNumber > 0);
	const ScopedLock sl(m_readLock);
	return m_outputMuteStates[outputChannelNumber];
}

void MenuBarMatrixProcessor::setOutputMuteState(int outputChannelNumber, bool muted, ChannelCommander* sender)
{
	jassert(outputChannelNumber > 0);

	for (auto const& outputCommander : m_outputCommanders)
	{
		if (outputCommander != reinterpret_cast<OutputCommander*>(sender))
			outputCommander->setOutputMute(outputChannelNumber, muted);
	}

	const ScopedLock sl(m_readLock);
	m_outputMuteStates[outputChannelNumber] = muted;
}

void MenuBarMatrixProcessor::setChannelCounts(int inputChannelCount, int outputChannelCount)
{
    auto reinitRequired = false;
    if (m_inputChannelCount != inputChannelCount)
    {
        m_inputChannelCount = inputChannelCount;
        reinitRequired = true;
    }
    if (m_outputChannelCount != outputChannelCount)
    {
        m_outputChannelCount = outputChannelCount;
        reinitRequired = true;
    }
    if (reinitRequired)
        postMessage(new ReinitIOCountMessage(m_inputChannelCount, m_outputChannelCount));
}

AudioDeviceManager* MenuBarMatrixProcessor::getDeviceManager()
{
	if (m_deviceManager)
		return m_deviceManager.get();
	else
		return nullptr;
}

//==============================================================================
const String MenuBarMatrixProcessor::getName() const
{
	return m_Name;
}

void MenuBarMatrixProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->initializeParameters(sampleRate, maximumExpectedSamplesPerBlock);
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->initializeParameters(sampleRate, maximumExpectedSamplesPerBlock);
}

void MenuBarMatrixProcessor::releaseResources()
{
	if (m_inputDataAnalyzer)
		m_inputDataAnalyzer->clearParameters();
	if (m_outputDataAnalyzer)
		m_outputDataAnalyzer->clearParameters();
}

void MenuBarMatrixProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ignoreUnused(midiMessages);

	// the lock is currently gloablly taken in audioDeviceIOCallback which is calling this method
	//const ScopedLock sl(m_readLock);

	auto reinitRequired = false;

	jassert(s_minInputsCount <= m_inputChannelCount);
	jassert(s_minOutputsCount <= m_outputChannelCount);

	if (m_inputChannelCount > m_inputMuteStates.size())
		reinitRequired = true;

	for (auto input = 1; input <= m_inputChannelCount; input++)
	{
		if (m_inputMuteStates[input])
		{
			auto channelIdx = input - 1;
			buffer.clear(channelIdx, 0, buffer.getNumSamples());
		}
	}

	postMessage(new AudioInputBufferMessage(buffer));

	// process data in buffer to be what shall be used as output
	juce::AudioBuffer<float> processedBuffer;
	processedBuffer.setSize(m_outputChannelCount, buffer.getNumSamples(), false, true, true);
	for (auto inputIdx = 0; inputIdx < m_inputChannelCount; inputIdx++)
	{
		for (auto outputIdx = 0; outputIdx < m_outputChannelCount; outputIdx++)
		{
            auto gain = m_matrixCrosspointEnabledValues[inputIdx + 1][outputIdx + 1] ? 1.0f : 0.0f;
			processedBuffer.addFrom(outputIdx, 0, buffer.getReadPointer(inputIdx), buffer.getNumSamples(), gain);
		}
	}
	buffer.makeCopyOf(processedBuffer, true);

	if (m_outputChannelCount > m_outputMuteStates.size())
		reinitRequired = true;

	for (auto output = 1; output <= m_outputChannelCount; output++)
	{
		if (m_outputMuteStates[output])
		{
			auto channelIdx = output - 1;
			buffer.clear(channelIdx, 0, buffer.getNumSamples());
		}
	}

	postMessage(new AudioOutputBufferMessage(buffer));

	if (reinitRequired)
		postMessage(new ReinitIOCountMessage(m_inputChannelCount, m_outputChannelCount));
}

void MenuBarMatrixProcessor::handleMessage(const Message& message)
{
	if (auto const iom = dynamic_cast<const ReinitIOCountMessage*> (&message))
	{
		auto inputCount = iom->getInputCount();
		jassert(inputCount > 0);
		auto outputCount = iom->getOutputCount();
		jassert(outputCount > 0);

		for (auto const& inputCommander : m_inputCommanders)
			inputCommander->setChannelCount(inputCount);
		
		for (auto const& outputCommander : m_outputCommanders)
			outputCommander->setChannelCount(outputCount);

		for (auto const& crosspointCommander : m_crosspointCommanders)
			crosspointCommander->setIOCount(inputCount, outputCount);

		initializeCtrlValues(iom->getInputCount(), iom->getOutputCount());

		if (m_networkServer && m_networkServer->hasActiveConnection())
			m_networkServer->getActiveConnection()->sendMessage(iom->getSerializedMessage());
	}
	else if (auto m = dynamic_cast<const AudioBufferMessage*> (&message))
	{
		if (m->getFlowDirection() == AudioBufferMessage::FlowDirection::Input && m_inputDataAnalyzer)
		{
			m_inputDataAnalyzer->analyzeData(m->getAudioBuffer());
		}
		else if (m->getFlowDirection() == AudioBufferMessage::FlowDirection::Output && m_outputDataAnalyzer)
		{
			m_outputDataAnalyzer->analyzeData(m->getAudioBuffer());
		}

		if (m_networkServer && m_networkServer->hasActiveConnection())
			m_networkServer->getActiveConnection()->sendMessage(m->getSerializedMessage());
	}
}

double MenuBarMatrixProcessor::getTailLengthSeconds() const
{
	/*dbg*/return 0.0;
}

bool MenuBarMatrixProcessor::acceptsMidi() const
{
	return false;
}

bool MenuBarMatrixProcessor::producesMidi() const
{
	return false;
}

AudioProcessorEditor* MenuBarMatrixProcessor::createEditor()
{
	if (!m_processorEditor)
		m_processorEditor = std::make_unique<MenuBarMatrixEditor>(this);

	return m_processorEditor.get();
}

bool MenuBarMatrixProcessor::hasEditor() const
{
	return !!m_processorEditor;
}

int MenuBarMatrixProcessor::getNumPrograms()
{
	/*dbg*/return 0;
}

int MenuBarMatrixProcessor::getCurrentProgram()
{
	/*dbg*/return 0;
}

void MenuBarMatrixProcessor::setCurrentProgram(int index)
{
	/*dbg*/ignoreUnused(index);
}

const String MenuBarMatrixProcessor::getProgramName(int index)
{
	/*dbg*/ignoreUnused(index);
	/*dbg*/return String();
}

void MenuBarMatrixProcessor::changeProgramName(int index, const String& newName)
{
	/*dbg*/ignoreUnused(index);
	/*dbg*/ignoreUnused(newName);
}

void MenuBarMatrixProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	/*dbg*/ignoreUnused(destData);
}

void MenuBarMatrixProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	/*dbg*/ignoreUnused(data);
	/*dbg*/ignoreUnused(sizeInBytes);
}

void MenuBarMatrixProcessor::audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
	float* const* outputChannelData, int numOutputChannels, int numSamples, const AudioIODeviceCallbackContext& context)
{
    ignoreUnused(context);
    
	const juce::ScopedLock sl(m_readLock);

    jassert(m_inputChannelCount == numInputChannels);
    jassert(m_outputChannelCount == numOutputChannels);

	auto maxActiveChannels = std::max(numInputChannels, numOutputChannels);

	if (s_maxChannelCount < maxActiveChannels)
	{
		jassertfalse;
		return;
	}

	// copy incoming data to processing data buffer
	for (auto i = 0; i < numInputChannels && i < maxActiveChannels; i++)
	{
		memcpy(m_processorChannels[i], inputChannelData[i], (size_t)numSamples * sizeof(float));
	}

	// from juce doxygen: buffer must be the size of max(inCh, outCh) and feeds the input data into the method and is returned with output data
	juce::AudioBuffer<float> audioBufferToProcess(m_processorChannels, maxActiveChannels, numSamples);
    juce::MidiBuffer midiBufferToProcess;
	processBlock(audioBufferToProcess, midiBufferToProcess);

	// copy the processed data buffer data to outgoing data
	auto processedChannelCount = audioBufferToProcess.getNumChannels();
	auto processedSampleCount = audioBufferToProcess.getNumSamples();
	auto processedData = audioBufferToProcess.getArrayOfReadPointers();
	jassert(processedSampleCount == numSamples);
	for (auto i = 0; i < numOutputChannels && i < processedChannelCount; i++)
	{
		memcpy(outputChannelData[i], processedData[i], (size_t)processedSampleCount * sizeof(float));
	}

}

void MenuBarMatrixProcessor::audioDeviceAboutToStart(AudioIODevice* device)
{
	if (device)
    {
        auto activeInputs = device->getActiveInputChannels();
        auto inputChannelCnt  = activeInputs.getHighestBit() + 1; // from JUCE documentation
        auto activeOutputs = device->getActiveOutputChannels();
        auto outputChannelCnt = activeOutputs.getHighestBit() + 1; // from JUCE documentation
        auto currentSampleRate = device->getCurrentSampleRate();
        auto currentBufferSize = device->getCurrentBufferSizeSamples();
        //auto currentBitDepth = device->getCurrentBitDepth();
        
        //DBG(juce::String(__FUNCTION__) << " " << device->getName() << " i:" << device->getInputChannelNames().joinIntoString(",") << "(" << inputChannelCnt << ") o:" << device->getOutputChannelNames().joinIntoString(",") << "(" << outputChannelCnt << ")");
        
        setChannelCounts(inputChannelCnt, outputChannelCnt);
        prepareToPlay(currentSampleRate, currentBufferSize);
    }
}

void MenuBarMatrixProcessor::audioDeviceStopped()
{
	releaseResources();
}

void MenuBarMatrixProcessor::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == m_deviceManager.get())
	{
		auto config = JUCEAppBasics::AppConfigurationBase::getInstance();
		if (config != nullptr)
			config->triggerConfigurationDump(false);
	}
}

void MenuBarMatrixProcessor::initializeCtrlValues(int inputCount, int outputCount)
{
	auto inputChannelCount = (inputCount > s_minInputsCount) ? inputCount : s_minInputsCount;
	for (auto channel = 1; channel <= inputChannelCount; channel++)
		setInputMuteState(channel, false);
    
    auto outputChannelCount = (outputCount > s_minOutputsCount) ? outputCount : s_minOutputsCount;
    for (auto channel = 1; channel <= outputChannelCount; channel++)
        setOutputMuteState(channel, false);

	DBG(juce::String(__FUNCTION__) << " " << inputChannelCount << " " << outputChannelCount);

    for (auto in = 1; in <= inputChannelCount; in++)
        for (auto out = 1; out <= outputChannelCount; out++)
            setMatrixCrosspointEnabledValue(in, out, in == out);
}


} // namespace MenuBarMatrix
