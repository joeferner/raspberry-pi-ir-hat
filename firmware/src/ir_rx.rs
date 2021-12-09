use crate::hal::{
    dma::{
        self, Dma, DmaChannelNumber, DmaCircularMode, DmaMemoryIncrementMode, DmaMemorySize,
        DmaMuxRequest, DmaPeripheralIncrementMode, DmaPeripheralSize, DmaPriorityLevel,
        DmaTransferDirection,
    },
    gpio::{AlternateFunctionMode, Pin, PortName},
    nvic::NVIC,
    rcc::RCC,
    timer::Timer,
};

const IR_RX_BUFFER_SAMPLES: usize = 128;
static IR_RX_BUFFER: [u16; IR_RX_BUFFER_SAMPLES] = [0; IR_RX_BUFFER_SAMPLES];

pub struct IrRx {
    dma_ch: dma::DmaChannel,
    read_index: usize,
}

impl IrRx {
    pub fn new(
        mut input_pin: Pin,
        timer: Timer,
        mut dma_ch: dma::DmaChannel,
        dma: &mut Dma,
        rcc: &mut RCC,
        nvic: &mut NVIC,
    ) -> IrRx {
        assert_eq!(6, input_pin.get_pin());
        assert_eq!(PortName::A, input_pin.get_port_name());
        assert_eq!(3, timer.get_timer_number());
        assert_eq!(DmaChannelNumber::Channel5, dma_ch.get_channel());

        input_pin.set_as_input();
        input_pin.set_as_alternate_function(AlternateFunctionMode::AF1);

        dma_ch.set_peripheral_request(DmaMuxRequest::Timer3Channel1);
        dma.set_data_transfer_direction(
            dma_ch.get_channel(),
            DmaTransferDirection::PeripheralToMemory,
        );
        dma.set_channel_priority_level(dma_ch.get_channel(), DmaPriorityLevel::Low);
        dma.set_channel_circular_mode(dma_ch.get_channel(), DmaCircularMode::Circular);
        dma.set_peripheral_increment_mode(
            dma_ch.get_channel(),
            DmaPeripheralIncrementMode::Disabled,
        );
        dma.set_memory_increment_mode(dma_ch.get_channel(), DmaMemoryIncrementMode::Increment);
        dma.set_peripheral_size(dma_ch.get_channel(), DmaPeripheralSize::U16);
        dma.set_memory_size(dma_ch.get_channel(), DmaMemorySize::U16);
        
        // LL_TIM_InitTypeDef TIM_InitStruct = {0};
        // TIM_InitStruct.Prescaler = 0;
        // TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
        // TIM_InitStruct.Autoreload = 65535;
        // TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
        // LL_TIM_Init(TIM3, &TIM_InitStruct);
        // LL_TIM_DisableARRPreload(TIM3);
        // LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
        // LL_TIM_DisableMasterSlaveMode(TIM3);
        // LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
        // LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
        // LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
        // LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_BOTHEDGE);

        nvic.enable_interrupt_timer3();

        // TODO
        // let sample_rate = Hertz(1000000);
        // let clk = rcc.clocks.apb_clk;
        // let prescaler = if clk >= sample_rate {
        //     (clk / sample_rate) - 1
        // } else {
        //     0
        // };
        // timer.psc.write(|w| unsafe { w.bits(prescaler) });

        // // configure dma
        // dma_ch.disable();
        // // TODO do we need to listen here?
        // // dma_ch.listen(dma::Event::TransferComplete);
        // // dma_ch.listen(dma::Event::TransferError);
        // dma_ch.clear_event(dma::Event::Any);
        // dma_ch.set_transfer_length(IR_RX_BUFFER_SAMPLES as u16);
        // dma_ch.set_direction(dma::Direction::FromPeripheral);
        // dma_ch.set_word_size(dma::WordSize::BITS16);

        // let tim3_ptr = unsafe { &(*stm32::TIM3::ptr()) };
        // let tim3_cr1_addr = &tim3_ptr.cr1 as *const _ as u32;
        // dma_ch.set_peripheral_address(tim3_cr1_addr, false);

        // let buf_addr: u32 = IR_RX_BUFFER.as_ptr() as u32;
        // dma_ch.set_memory_address(buf_addr, true);

        // dma_ch.enable();

        // // enable timer capture/compare interrupt and capture/compare dma request
        // timer
        //     .dier
        //     .modify(|_, w| w.cc1ie().set_bit().cc1de().set_bit());

        // // enable timer capture/compare channel
        // timer.ccer.modify(|_, w| w.cc1e().set_bit());

        // // enable counter
        // timer.cr1.modify(|_, w| w.cen().set_bit());

        return IrRx {
            dma_ch,
            read_index: IR_RX_BUFFER_SAMPLES as usize,
        };
    }

    fn get_dma_rx_pos(&self) -> usize {
        return 0;
        // return IR_RX_BUFFER_SAMPLES as usize - self.dma_ch.get_transfer_length() as usize;
    }
}
