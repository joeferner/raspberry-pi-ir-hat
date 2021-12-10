use crate::hal::{
    dma::{
        self, Dma, DmaChannelNumber, DmaCircularMode, DmaMemoryIncrementMode, DmaMemorySize,
        DmaMuxRequest, DmaPeripheralIncrementMode, DmaPeripheralSize, DmaPriorityLevel,
        DmaTransferDirection,
    },
    gpio::{AlternateFunctionMode, Pin, PortName},
    hertz::Hertz,
    nvic::NVIC,
    rcc::RCC,
    timer::{
        Timer, TimerActiveInput, TimerCenterAlignMode, TimerChannel, TimerClockDivision,
        TimerCounterDirection, TimerFilter, TimerPolarity, TimerPrescaler, TimerTriggerOutput,
    },
};
use core::{cell::RefCell, ops::DerefMut};
use cortex_m::interrupt::Mutex;

const IR_RX_BUFFER_SAMPLES: usize = 128;
static IR_RX_BUFFER: [u16; IR_RX_BUFFER_SAMPLES] = [0; IR_RX_BUFFER_SAMPLES];

pub struct IrRx {
    dma_ch: dma::DmaChannel,
    read_index: usize,
}

const DMA_BUFFER_SIZE: usize = 128;
static dma_buffer: [u16; DMA_BUFFER_SIZE] = [0; DMA_BUFFER_SIZE];

impl IrRx {
    pub fn new(
        mut input_pin: Pin,
        mut timer: Timer,
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
        dma.disable_channel(dma_ch.get_channel());
        dma.clear_global_interrupt_flag(dma_ch.get_channel());
        dma.set_data_length(dma_ch.get_channel(), dma_buffer.len());
        dma.set_peripheral_address(
            dma_ch.get_channel(),
            timer.get_capture_compare_register_address(),
        );
        dma.set_memory_address(dma_ch.get_channel(), dma_buffer.as_ptr() as u32);

        timer.set_counter_direction(TimerCounterDirection::Up);
        timer.set_center_align_mode(TimerCenterAlignMode::Edge);
        timer.set_clock_division(TimerClockDivision::Div1);
        timer.set_auto_reload(65535);
        timer.set_prescaler(0);
        timer.generate_event_update();

        timer.disable_auto_reload_preload();
        timer.set_trigger_output(TimerTriggerOutput::Reset);
        timer.disable_master_slave_mode();
        timer.ic_set_active_input(TimerChannel::Channel1, TimerActiveInput::DirectTI);
        timer.ic_set_prescaler(TimerChannel::Channel1, TimerPrescaler::Div1);
        timer.ic_set_filter(TimerChannel::Channel1, TimerFilter::Div1);
        timer.ic_set_polarity(TimerChannel::Channel1, TimerPolarity::BothEdges);

        timer.set_prescaler_hertz(Hertz::megahertz(1), &rcc);

        dma.enable_interrupt_transfer_complete(dma_ch.get_channel());
        dma.enable_interrupt_transfer_error(dma_ch.get_channel());

        // start dma
        // LL_DMA_EnableChannel(IR_RX_DMA, IR_RX_DMA_CH);

        // memset(ir_rx_dma_data, 0, sizeof(ir_rx_dma_data));
        // ir_rx_dma_data_read_index = ir_rx_get_pos();

        // IR_RX_LL_TIM_EnableIT_CC();
        // IR_RX_LL_TIM_EnableDMAReq_CC();
        // LL_TIM_CC_EnableChannel(IR_RX_TIMER, IR_RX_TIMER_CH);
        // LL_TIM_EnableCounter(IR_RX_TIMER);

        nvic.enable_interrupt_timer3();

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
