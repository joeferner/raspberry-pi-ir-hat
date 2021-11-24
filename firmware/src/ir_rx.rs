use hal::dma;
use hal::dma::Channel;
use hal::{
    rcc::Rcc,
    stm32::{self},
    time::Hertz,
};

const IR_RX_BUFFER_SAMPLES: usize = 128;
static IR_RX_BUFFER: [u16; IR_RX_BUFFER_SAMPLES] = [0; IR_RX_BUFFER_SAMPLES];

pub struct IrRx {
    dma_ch: dma::C5,
    read_index: usize,
}

impl IrRx {
    pub fn new(tim3: stm32::TIM3, dma_ch: dma::C5, dma: &mut stm32::DMA, rcc: &mut Rcc) -> IrRx {
        let sample_rate = Hertz(1000000);
        let clk = rcc.clocks.apb_clk;
        let prescaler = if clk >= sample_rate {
            (clk / sample_rate) - 1
        } else {
            0
        };
        tim3.psc.write(|w| unsafe { w.bits(prescaler) });

        // configure dma
        dma_ch.disable();
        // TODO do we need to listen here?
        // dma_ch.listen(dma::Event::TransferComplete);
        // dma_ch.listen(dma::Event::TransferError);
        dma_ch.clear_event(dma::Event::Any);
        dma_ch.set_transfer_length(IR_RX_BUFFER_SAMPLES as u16);
        dma_ch.set_direction(dma::Direction::FromPeripheral);
        dma_ch.set_word_size(dma::WordSize::BITS16);

        let tim3_ptr = unsafe { &(*stm32::TIM3::ptr()) };
        let tim3_cr1_addr = &tim3_ptr.cr1 as *const _ as u32;
        dma_ch.set_peripheral_address(tim3_cr1_addr, false);

        let buf_addr: u32 = IR_RX_BUFFER.as_ptr() as u32;
        dma_ch.set_memory_address(buf_addr, true);

        dma_ch.enable();

        // enable timer capture/compare interrupt and capture/compare dma request
        tim3.dier
            .modify(|_, w| w.cc1ie().set_bit().cc1de().set_bit());

        // enable timer capture/compare channel
        tim3.ccer.modify(|_, w| w.cc1e().set_bit());

        // enable counter
        tim3.cr1.modify(|_, w| w.cen().set_bit());

        return IrRx {
            dma_ch,
            read_index: IR_RX_BUFFER_SAMPLES as usize,
        };
    }

    fn get_dma_rx_pos(&self) -> usize {
        return IR_RX_BUFFER_SAMPLES as usize - self.dma_ch.get_transfer_length() as usize;
    }
}
