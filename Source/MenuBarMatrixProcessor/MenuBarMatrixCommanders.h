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


class MenuBarMatrixChannelCommander
{
public:
    MenuBarMatrixChannelCommander();
    virtual ~MenuBarMatrixChannelCommander();

    virtual void setChannelCount(int channelCount) = 0;

protected:

private:
};

class MenuBarMatrixInputCommander : public MenuBarMatrixChannelCommander
{
public:
    MenuBarMatrixInputCommander();
    virtual ~MenuBarMatrixInputCommander() override;

    void setInputMuteChangeCallback(const std::function<void(MenuBarMatrixInputCommander* sender, int, bool)>& callback);
    void setInputLevelChangeCallback(const std::function<void(MenuBarMatrixInputCommander* sender, int, float)>& callback);
    void setInputMutePollCallback(const std::function<void(MenuBarMatrixInputCommander* sender, int)>& callback);
    void setInputLevelPollCallback(const std::function<void(MenuBarMatrixInputCommander* sender, int)>& callback);

    virtual void setInputMute(unsigned int channel, bool muteState) = 0;
    virtual void setInputLevel(unsigned int channel, float levelValue) { ignoreUnused(channel); ignoreUnused(levelValue); };

protected:
    void inputMuteChange(int channel, bool muteState);
    void inputLevelChange(int channel, float levelValue);
        
    void inputMutePoll(int channel);
    void inputLevelPoll(int channel);

private:
    std::function<void(MenuBarMatrixInputCommander* sender, int, float)> m_inputLevelChangeCallback{ nullptr };
    std::function<void(MenuBarMatrixInputCommander* sender, int)>        m_inputLevelPollCallback{ nullptr };
    std::function<void(MenuBarMatrixInputCommander* sender, int, bool)>  m_inputMuteChangeCallback{ nullptr };
    std::function<void(MenuBarMatrixInputCommander* sender, int)>        m_inputMutePollCallback{ nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarMatrixInputCommander)
};

class MenuBarMatrixOutputCommander : public MenuBarMatrixChannelCommander
{
public:
    MenuBarMatrixOutputCommander();
    virtual ~MenuBarMatrixOutputCommander() override;

    void setOutputMuteChangeCallback(const std::function<void(MenuBarMatrixOutputCommander* sender, int, bool)>& callback);
    void setOutputLevelChangeCallback(const std::function<void(MenuBarMatrixOutputCommander* sender, int, float)>& callback);
    void setOutputMutePollCallback(const std::function<void(MenuBarMatrixOutputCommander* sender, int)>& callback);
    void setOutputLevelPollCallback(const std::function<void(MenuBarMatrixOutputCommander* sender, int)>& callback);

    virtual void setOutputMute(unsigned int channel, bool muteState) = 0;
    virtual void setOutputLevel(unsigned int channel, float levelValue) { ignoreUnused(channel); ignoreUnused(levelValue); };

protected:
    void outputMuteChange(int channel, bool muteState);
    void outputLevelChange(int channel, float levelValue);
        
    void outputMutePoll(int channel);
    void outputLevelPoll(int channel);

private:
    std::function<void(MenuBarMatrixOutputCommander* sender, int, float)>    m_outputLevelChangeCallback{ nullptr };
    std::function<void(MenuBarMatrixOutputCommander* sender, int)>           m_outputLevelPollCallback{ nullptr };
    std::function<void(MenuBarMatrixOutputCommander* sender, int, bool)>     m_outputMuteChangeCallback{ nullptr };
    std::function<void(MenuBarMatrixOutputCommander* sender, int)>           m_outputMutePollCallback{ nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarMatrixOutputCommander)
};
    
class MenuBarMatrixCrosspointCommander : public MenuBarMatrixChannelCommander
{
public:
    MenuBarMatrixCrosspointCommander();
    virtual ~MenuBarMatrixCrosspointCommander() override;

    void setCrosspointEnabledChangeCallback(const std::function<void(MenuBarMatrixCrosspointCommander* sender, int, int, bool)>& callback);
    void setCrosspointEnabledPollCallback(const std::function<void(MenuBarMatrixCrosspointCommander* sender, int, int)>& callback);

    virtual void setCrosspointEnabledValue(int input, int output, bool enabledState) = 0;

    virtual void setIOCount(int inputCount, int outputCount) = 0;

protected:
    void crosspointEnabledChange(int input, int output, bool enabledState);
    void crosspointEnabledPoll(int input, int output);

private:
    void setChannelCount(int channelCount) override { ignoreUnused(channelCount); };

    std::function<void(MenuBarMatrixCrosspointCommander* sender, int, int, bool)> m_crosspointEnabledChangeCallback{ nullptr };
    std::function<void(MenuBarMatrixCrosspointCommander* sender, int, int)>       m_crosspointEnabledPollCallback{ nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuBarMatrixCrosspointCommander)
};


} // namespace MenuBarMatrix
