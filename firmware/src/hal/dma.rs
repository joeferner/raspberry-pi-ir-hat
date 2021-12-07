use stm32g0::stm32g031;

pub struct Parts {
    pub ch5: DmaChannel,
}

pub struct DmaChannel {
    register_block: *const stm32g031::dmamux::RegisterBlock,
    channel: u8,
}

pub fn split(dma: stm32g031::DMAMUX) -> Parts {
    return Parts {
        ch5: DmaChannel {
            register_block: stm32g031::DMAMUX::ptr(),
            channel: 5,
        },
    };
}
