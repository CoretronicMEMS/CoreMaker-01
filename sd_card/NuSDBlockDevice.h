/* mbed Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
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
#ifndef __NU_SD_BLOCK_DEVICE_H__
#define __NU_SD_BLOCK_DEVICE_H__

#if TARGET_NUVOTON

#include "BlockDevice.h"
#include "platform/PlatformMutex.h"
#include "mbed.h"

struct nu_modinit_s;

class NuSDBlockDevice : public BlockDevice {
public:
    /** Lifetime of an SD card
     */
    NuSDBlockDevice();
    NuSDBlockDevice(PinName sd_dat0, PinName sd_dat1, PinName sd_dat2, PinName sd_dat3,
        PinName sd_cmd, PinName sd_clk, PinName sd_cdn);
    virtual ~NuSDBlockDevice();

    /** Initialize a block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int init();

    /** Deinitialize a block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int deinit();

    /** Read blocks from a block device
     *
     *  @param buffer   Buffer to write blocks to
     *  @param addr     Address of block to begin reading from
     *  @param size     Size to read in bytes, must be a multiple of read block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int read(void *buffer, bd_addr_t addr, bd_size_t size);

    /** Program blocks to a block device
     *
     *  The blocks must have been erased prior to being programmed
     *
     *  @param buffer   Buffer of data to write to blocks
     *  @param addr     Address of block to begin writing to
     *  @param size     Size to write in bytes, must be a multiple of program block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int program(const void *buffer, bd_addr_t addr, bd_size_t size);

    /** Erase blocks on a block device
     *
     *  The state of an erased block is undefined until it has been programmed
     *
     *  @param addr     Address of block to begin erasing
     *  @param size     Size to erase in bytes, must be a multiple of erase block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int erase(bd_addr_t addr, bd_size_t size);

    /** Get the size of a readable block
     *
     *  @return         Size of a readable block in bytes
     */
    virtual bd_size_t get_read_size() const;

    /** Get the size of a programable block
     *
     *  @return         Size of a programable block in bytes
     *  @note Must be a multiple of the read size
     */
    virtual bd_size_t get_program_size() const;

    /** Get the size of an erasable block
     *
     *  @return         Size of an erasable block in bytes
     *  @note Must be a multiple of the program size
     */
    virtual bd_size_t get_erase_size() const;

    /** Get the size of an erasable block given address
     *
     *  @param addr     Address within the erasable block
     *  @return         Size of an erasable block in bytes
     *  @note Must be a multiple of the program size
     */
    virtual bd_size_t get_erase_size(bd_addr_t addr) const;

    /** Get the total size of the underlying device
     *
     *  @return         Size of the underlying device in bytes
     */
    virtual bd_size_t size() const;

    /** Enable or disable debugging
     *
     *  @param          State of debugging
     */
    virtual void debug(bool dbg);
    
    /** Get the BlockDevice class type.
     *
     *  @return         A string representation of the BlockDevice class type.
     */
    virtual const char *get_type() const;

private:
    int _init_sdh();
    uint32_t _sd_sectors();
    void _sdh_irq();

    uint32_t _sectors;
    bool _dbg;
    PlatformMutex _lock;
    
    const struct nu_modinit_s * _sdh_modinit;
    SDName      _sdh;
    SDH_T *     _sdh_base;
#if TARGET_NUMAKER_PFM_NUC472
    uint32_t    _sdh_port;
#endif

    CThunk<NuSDBlockDevice>     _sdh_irq_thunk;

    PinName _sd_dat0;
    PinName _sd_dat1;
    PinName _sd_dat2;
    PinName _sd_dat3;
    PinName _sd_cmd;
    PinName _sd_clk;
    PinName _sd_cdn;

    bool                        _is_initialized;
    uint32_t                    _init_ref_count;
};

#endif  /* TARGET_NUVOTON */
#endif  /* __NU_SD_BLOCK_DEVICE_H__ */
