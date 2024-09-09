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

#include "MenuBarMatrixCommanders.h"


namespace MenuBarMatrix
{


MenuBarMatrixChannelCommander::MenuBarMatrixChannelCommander()
{
}

MenuBarMatrixChannelCommander::~MenuBarMatrixChannelCommander()
{
}

MenuBarMatrixInputCommander::MenuBarMatrixInputCommander()
{
}

MenuBarMatrixInputCommander::~MenuBarMatrixInputCommander()
{
}

void MenuBarMatrixInputCommander::setInputMuteChangeCallback(const std::function<void(MenuBarMatrixInputCommander* sender, int, bool)>& callback)
{
	m_inputMuteChangeCallback = callback;
}


void MenuBarMatrixInputCommander::setInputLevelChangeCallback(const std::function<void(MenuBarMatrixInputCommander* sender, int, float)>& callback)
{
	m_inputLevelChangeCallback = callback;
}
void MenuBarMatrixInputCommander::setInputMutePollCallback(const std::function<void(MenuBarMatrixInputCommander* sender, int)>& callback)
{
	m_inputMutePollCallback = callback;
}


void MenuBarMatrixInputCommander::setInputLevelPollCallback(const std::function<void(MenuBarMatrixInputCommander* sender, int)>& callback)
{
	m_inputLevelPollCallback = callback;
}
void MenuBarMatrixInputCommander::inputMuteChange(int channel, bool muteState)
{
	if (m_inputMuteChangeCallback)
		m_inputMuteChangeCallback(this, channel, muteState);
}


void MenuBarMatrixInputCommander::inputLevelChange(int channel, float levelValue)
{
	if (m_inputLevelChangeCallback)
		m_inputLevelChangeCallback(this, channel, levelValue);
}
void MenuBarMatrixInputCommander::inputMutePoll(int channel)
{
	if (m_inputMutePollCallback)
		m_inputMutePollCallback(this, channel);
}


void MenuBarMatrixInputCommander::inputLevelPoll(int channel)
{
	if (m_inputLevelPollCallback)
		m_inputLevelPollCallback(this, channel);
}


MenuBarMatrixOutputCommander::MenuBarMatrixOutputCommander()
{
}

MenuBarMatrixOutputCommander::~MenuBarMatrixOutputCommander()
{
}

void MenuBarMatrixOutputCommander::setOutputMuteChangeCallback(const std::function<void(MenuBarMatrixOutputCommander* sender, int, bool)>& callback)
{
	m_outputMuteChangeCallback = callback;
}

void MenuBarMatrixOutputCommander::setOutputLevelChangeCallback(const std::function<void(MenuBarMatrixOutputCommander* sender, int, float)>& callback)
{
	m_outputLevelChangeCallback = callback;
}
void MenuBarMatrixOutputCommander::setOutputMutePollCallback(const std::function<void(MenuBarMatrixOutputCommander* sender, int)>& callback)
{
	m_outputMutePollCallback = callback;
}

void MenuBarMatrixOutputCommander::setOutputLevelPollCallback(const std::function<void(MenuBarMatrixOutputCommander* sender, int)>& callback)
{
	m_outputLevelPollCallback = callback;
}

void MenuBarMatrixOutputCommander::outputMuteChange(int channel, bool muteState)
{
	if (m_outputMuteChangeCallback)
		m_outputMuteChangeCallback(nullptr, channel, muteState);
}

void MenuBarMatrixOutputCommander::outputLevelChange(int channel, float levelValue)
{
	if (m_outputLevelChangeCallback)
		m_outputLevelChangeCallback(nullptr, channel, levelValue);
}

void MenuBarMatrixOutputCommander::outputMutePoll(int channel)
{
	if (m_outputMutePollCallback)
		m_outputMutePollCallback(nullptr, channel);
}

void MenuBarMatrixOutputCommander::outputLevelPoll(int channel)
{
	if (m_outputLevelPollCallback)
		m_outputLevelPollCallback(nullptr, channel);
}


MenuBarMatrixCrosspointCommander::MenuBarMatrixCrosspointCommander()
{
}

MenuBarMatrixCrosspointCommander::~MenuBarMatrixCrosspointCommander()
{
}

void MenuBarMatrixCrosspointCommander::setCrosspointEnabledChangeCallback(const std::function<void(MenuBarMatrixCrosspointCommander* sender, int, int, bool)>& callback)
{
	m_crosspointEnabledChangeCallback = callback;
}

void MenuBarMatrixCrosspointCommander::setCrosspointEnabledPollCallback(const std::function<void(MenuBarMatrixCrosspointCommander* sender, int, int)>& callback)
{
	m_crosspointEnabledPollCallback = callback;
}

void MenuBarMatrixCrosspointCommander::crosspointEnabledChange(int input, int output, bool enabledState)
{
	if (m_crosspointEnabledChangeCallback)
		m_crosspointEnabledChangeCallback(nullptr, input, output, enabledState);
}

void MenuBarMatrixCrosspointCommander::crosspointEnabledPoll(int input, int output)
{
	if (m_crosspointEnabledPollCallback)
		m_crosspointEnabledPollCallback(nullptr, input, output);
}


} // namespace MenuBarMatrix
