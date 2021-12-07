use stm32g0::stm32g031;

pub struct Dma {}

impl Dma {
    pub fn new(dma: stm32g031::DMA) -> Self {
        return Self {};
    }
}

pub struct DmaChannelParts {
    pub ch5: DmaChannel,
}

pub struct DmaChannel {
    register_block: *const stm32g031::dmamux::RegisterBlock,
    channel: u8,
}

pub fn split(dma: stm32g031::DMAMUX) -> DmaChannelParts {
    return DmaChannelParts {
        ch5: DmaChannel {
            register_block: stm32g031::DMAMUX::ptr(),
            channel: 5,
        },
    };
}
