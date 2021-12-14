use stm32g0::stm32g031;

use super::rcc::RCC;

// see stm32g0x1 datasheet: Table 42. DMAMUX: assignment of multiplexer inputs to resources
pub enum DmaMuxRequest {
    Timer3Channel1 = 32,
}

impl DmaMuxRequest {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub enum DmaTransferDirection {
    PeripheralToMemory = 0,
    MemoryToPeripheral = 1,
}

impl DmaTransferDirection {
    pub fn val(self) -> bool {
        return (self as u8) == 1;
    }
}

pub enum DmaPriorityLevel {
    Low = 0b00,
    Medium = 0b01,
    High = 0b10,
    VeryHigh = 0b11,
}

impl DmaPriorityLevel {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub enum DmaCircularMode {
    Disabled = 0,
    Circular = 1,
}

impl DmaCircularMode {
    pub fn val(self) -> bool {
        return (self as u8) == 1;
    }
}

pub enum DmaPeripheralIncrementMode {
    Disabled = 0,
    Increment = 1,
}

impl DmaPeripheralIncrementMode {
    pub fn val(self) -> bool {
        return (self as u8) == 1;
    }
}

pub enum DmaMemoryIncrementMode {
    Disabled = 0,
    Increment = 1,
}

impl DmaMemoryIncrementMode {
    pub fn val(self) -> bool {
        return (self as u8) == 1;
    }
}

pub enum DmaPeripheralSize {
    U8 = 0b00,
    U16 = 0b01,
    U32 = 0b10,
}

impl DmaPeripheralSize {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

pub enum DmaMemorySize {
    U8 = 0b00,
    U16 = 0b01,
    U32 = 0b10,
}

impl DmaMemorySize {
    pub fn val(self) -> u8 {
        return self as u8;
    }
}

#[derive(Debug, PartialEq, Clone, Copy)]
#[repr(u8)]
pub enum DmaChannelNumber {
    Channel5 = 5,
}

pub struct Dma {
    register_block: *const stm32g031::dma::RegisterBlock,
}

impl Dma {
    pub fn new(_dma: stm32g031::DMA, rcc: &mut RCC) -> Self {
        rcc.enable_dma1();
        return Self {
            register_block: stm32g031::DMA::ptr(),
        };
    }

    pub fn is_transfer_complete_interrupt_flag_set(channel: DmaChannelNumber) -> bool {
        let reg = unsafe { &*stm32g031::DMA::ptr() };
        return match channel {
            DmaChannelNumber::Channel5 => reg.isr.read().tcif5().bit_is_set(),
        };
    }

    pub fn is_global_interrupt_flag_set(channel: DmaChannelNumber) -> bool {
        let reg = unsafe { &*stm32g031::DMA::ptr() };
        return match channel {
            DmaChannelNumber::Channel5 => reg.isr.read().gif5().bit_is_set(),
        };
    }

    pub fn set_data_transfer_direction(
        &mut self,
        channel: DmaChannelNumber,
        direction: DmaTransferDirection,
    ) {
        // LL_DMA_SetDataTransferDirection
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg.ch5.cr.modify(|_, w| w.dir().bit(direction.val())),
        }
    }

    pub fn set_channel_priority_level(
        &mut self,
        channel: DmaChannelNumber,
        priority_level: DmaPriorityLevel,
    ) {
        // LL_DMA_SetChannelPriorityLevel
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg
                .ch5
                .cr
                .modify(|_, w| unsafe { w.pl().bits(priority_level.val()) }),
        }
    }

    pub fn set_channel_circular_mode(&mut self, channel: DmaChannelNumber, mode: DmaCircularMode) {
        // LL_DMA_SetMode
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg.ch5.cr.modify(|_, w| w.circ().bit(mode.val())),
        }
    }

    pub fn set_peripheral_increment_mode(
        &mut self,
        channel: DmaChannelNumber,
        mode: DmaPeripheralIncrementMode,
    ) {
        // LL_DMA_SetPeriphIncMode
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg.ch5.cr.modify(|_, w| w.pinc().bit(mode.val())),
        }
    }

    pub fn set_memory_increment_mode(
        &mut self,
        channel: DmaChannelNumber,
        mode: DmaMemoryIncrementMode,
    ) {
        // LL_DMA_SetMemoryIncMode
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg.ch5.cr.modify(|_, w| w.minc().bit(mode.val())),
        }
    }

    pub fn set_peripheral_size(&mut self, channel: DmaChannelNumber, size: DmaPeripheralSize) {
        // LL_DMA_SetPeriphSize
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg
                .ch5
                .cr
                .modify(|_, w| unsafe { w.psize().bits(size.val()) }),
        }
    }

    pub fn set_memory_size(&mut self, channel: DmaChannelNumber, size: DmaMemorySize) {
        // LL_DMA_SetPeriphSize
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg
                .ch5
                .cr
                .modify(|_, w| unsafe { w.msize().bits(size.val()) }),
        }
    }

    pub fn enable_interrupt_transfer_complete(&mut self, channel: DmaChannelNumber) {
        // LL_DMA_EnableIT_TC
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg.ch5.cr.modify(|_, w| w.tcie().set_bit()),
        }
    }

    pub fn enable_interrupt_transfer_error(&mut self, channel: DmaChannelNumber) {
        // LL_DMA_EnableIT_TE
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg.ch5.cr.modify(|_, w| w.teie().set_bit()),
        }
    }

    pub fn disable_channel(&mut self, channel: DmaChannelNumber) {
        // LL_DMA_DisableChannel
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg.ch5.cr.modify(|_, w| w.en().clear_bit()),
        }
    }

    pub fn clear_global_interrupt_flag(&mut self, channel: DmaChannelNumber) {
        // LL_DMA_ClearFlag_GI
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg.ifcr.write(|w| w.cgif5().set_bit()),
        }
    }

    pub fn set_data_length(&mut self, channel: DmaChannelNumber, len: u16) {
        // LL_DMA_SetDataLength
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg.ch5.ndtr.modify(|_, w| unsafe { w.ndt().bits(len) }),
        }
    }

    pub fn set_peripheral_address(&mut self, channel: DmaChannelNumber, address: u32) {
        // LL_DMA_SetPeriphAddress
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => {
                reg.ch5.par.modify(|_, w| unsafe { w.pa().bits(address) })
            }
        }
    }

    pub fn set_memory_address(&mut self, channel: DmaChannelNumber, address: u32) {
        // LL_DMA_SetMemoryAddress
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => {
                reg.ch5.mar.modify(|_, w| unsafe { w.ma().bits(address) })
            }
        }
    }

    pub fn enable_channel(&mut self, channel: DmaChannelNumber) {
        // LL_DMA_EnableChannel
        let reg = unsafe { &*self.register_block };
        match channel {
            DmaChannelNumber::Channel5 => reg.ch5.cr.modify(|_, w| w.en().set_bit()),
        }
    }

    pub fn get_data_length(&self, channel: DmaChannelNumber) -> u16 {
        // LL_DMA_GetDataLength
        let reg = unsafe { &*self.register_block };
        return match channel {
            DmaChannelNumber::Channel5 => reg.ch5.ndtr.read().ndt().bits(),
        };
    }
}

pub struct DmaChannelParts {
    pub ch5: DmaChannel,
}

pub struct DmaChannel {
    register_block: *const stm32g031::dmamux::RegisterBlock,
    channel: DmaChannelNumber,
}

impl DmaChannel {
    pub fn get_channel(&self) -> DmaChannelNumber {
        return self.channel;
    }

    pub fn set_peripheral_request(&mut self, request: DmaMuxRequest) {
        // LL_DMA_SetPeriphRequest
        let reg = unsafe { &*self.register_block };
        match self.channel {
            DmaChannelNumber::Channel5 => reg
                .c4cr
                .modify(|_, w| unsafe { w.dmareq_id().bits(request.val()) }),
        }
    }
}

pub struct DmaMux {}

impl DmaMux {
    pub fn new(_dma_mux: stm32g031::DMAMUX) -> Self {
        return DmaMux {};
    }

    pub fn split(self) -> DmaChannelParts {
        return DmaChannelParts {
            ch5: DmaChannel {
                register_block: stm32g031::DMAMUX::ptr(),
                channel: DmaChannelNumber::Channel5,
            },
        };
    }
}
