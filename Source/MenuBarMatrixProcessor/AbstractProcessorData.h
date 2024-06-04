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

class AbstractProcessorData
{
public:
    enum Type
    {
        Invalid,
		AudioSignal,
        Level,
        Spectrum,
    };
    
    AbstractProcessorData();
    virtual ~AbstractProcessorData();
    
    Type GetDataType();
    
    virtual void SetChannelCount(unsigned long count) = 0;
    virtual unsigned long GetChannelCount() = 0;
    
protected:
    Type    m_type;
};

}