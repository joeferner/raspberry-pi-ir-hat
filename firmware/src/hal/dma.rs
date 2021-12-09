use stm32g0::stm32g031;

use super::rcc::RCC;

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
    pub fn new(dma: stm32g031::DMA, rcc: &mut RCC) -> Self {
        rcc.enable_dma1();
        return Self {
            register_block: stm32g031::DMA::ptr(),
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
                .c5cr
                .modify(|_, w| unsafe { w.dmareq_id().bits(request.val()) }),
        }
    }
}

pub fn split(dma: stm32g031::DMAMUX) -> DmaChannelParts {
    return DmaChannelParts {
        ch5: DmaChannel {
            register_block: stm32g031::DMAMUX::ptr(),
            channel: DmaChannelNumber::Channel5,
        },
    };
}
