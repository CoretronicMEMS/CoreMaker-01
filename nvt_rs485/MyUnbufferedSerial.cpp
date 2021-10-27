/* mbed Microcontroller Library
 * Copyright (c) 2006-2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "MyUnbufferedSerial.h"
#if (MBED_MAJOR_VERSION >= 6)
#if DEVICE_SERIAL

#include "platform/mbed_critical.h"

namespace mbed {

MyUnbufferedSerial::MyUnbufferedSerial(
    PinName tx,
    PinName rx,
    int baud
) : SerialBase(tx, rx, baud)
{
    // No lock needed in the constructor
}

MyUnbufferedSerial::MyUnbufferedSerial(
    const serial_pinmap_t &static_pinmap, int baud
) : SerialBase(static_pinmap, baud)
{
    // No lock needed in the constructor
}

ssize_t MyUnbufferedSerial::write(const void *buffer, size_t size)
{
    const unsigned char *buf = static_cast<const unsigned char *>(buffer);

    if (size == 0) {
        return 0;
    }

    bool lock_api = !core_util_in_critical_section();

    if (lock_api) {
        lock();
    }

    for (size_t i = 0; i < size; i++) {
        _base_putc(buf[i]);
    }

    if (lock_api) {
        unlock();
    }

    return size;
}

ssize_t MyUnbufferedSerial::read(void *buffer, size_t size)
{
    unsigned char *buf = static_cast<unsigned char *>(buffer);

    if (size == 0) {
        return 0;
    }

    lock();

    buf[0] = _base_getc();

    unlock();

    return 1;
}

short MyUnbufferedSerial::poll(short events) const
{
    short revents = 0;
    if (
        (events & POLLIN)
        && (const_cast <MyUnbufferedSerial *>(this))->SerialBase::readable()
    ) {
        revents |= POLLIN;
    }
    if (
        (events & POLLOUT)
        && (const_cast <MyUnbufferedSerial *>(this))->SerialBase::writeable()
    ) {
        revents |= POLLOUT;
    }
    return revents;
}

int MyUnbufferedSerial::enable_input(bool enabled)
{
    SerialBase::enable_input(enabled);

    return 0;
}

int MyUnbufferedSerial::enable_output(bool enabled)
{
    SerialBase::enable_output(enabled);

    return 0;
}

#if DEVICE_SERIAL_FC
void MyUnbufferedSerial::set_flow_control(Flow type, PinName flow1, PinName flow2)
{
    lock();
    SerialBase::set_flow_control(type, flow1, flow2);
    unlock();
}
#endif // DEVICE_SERIAL_FC

} // namespace mbed

#endif // #if DEVICE_SERIAL
#endif // (MBED_MAJOR_VERSION >= 6 )