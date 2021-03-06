
#include <xpcc/architecture.hpp>
#include <xpcc/workflow.hpp>
#include <xpcc/debug.hpp>

#include "loa/damballa.hpp"
#include "pins.hpp"

#include "uplink.hpp"
#include "fpga.hpp"
#include "led.hpp"
#include "ui.hpp"


xpcc::stm32::Uart5 uart5(115200);
xpcc::IODeviceWrapper<xpcc::stm32::Uart5> loggerDevice(uart5);

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

using namespace xpcc::stm32;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	uart5.configurePins(uart5.REMAP_PC12_PD2);
	
	// Initialize predefined IO-Pins and load the FPGA configuration
	loa::Damballa::initialize();
	
	xpcc::stm32::SysTickTimer::enable();
	
	Uplink::initialize();
	Led::initialize();
	Ui::initialize();
	Fpga::initialize();
	
	XPCC_LOG_INFO << "Motortestboard ready ..." << xpcc::endl;
	
	ColorHsv color = { 0, 255, 100 };
	ColorHsv color2 = { 10, 255, 100 };
	xpcc::PeriodicTimer<> timer(20);
	xpcc::PeriodicTimer<> timer2(500);
	while (1)
	{
		if (timer.isExpired())
		{
			color.hue++;
			color2.hue++;
			
			Led::setHsv(Led::LED1, color);
			Led::setHsv(Led::LED2, color2);
			
			//servo1 = 34150 + (11 - encoder6) * 2845;*/
			//Fpga::setServo(Fpga::SERVO1, servo1);
		}
		
		if (timer2.isExpired())
		{
			loa::Led1::toggle();
			
			uint16_t buttons = Fpga::getButtons() & 0x000f;
			XPCC_LOG_DEBUG << "but=" << buttons << xpcc::endl;
			
			//XPCC_LOG_DEBUG << "l=" << static_cast<int16_t>(Ui::getEncoder(Ui::ENCODER_6) - 12) << xpcc::endl;
			//XPCC_LOG_DEBUG << "r=" << static_cast<int16_t>(Ui::getEncoder(Ui::ENCODER_7) - 12) << xpcc::endl;
			
			//int16_t indexLeft = static_cast<int16_t>(Ui::getEncoder(Ui::ENCODER_6) - 12);
			//int16_t servo1 = indexLeft * 2730;
			
			//XPCC_LOG_DEBUG.printf("servo  =%d\n", servo1);
			//XPCC_LOG_DEBUG.printf("buttons=%04x\n", Ui::button.isPressed(0xffff));

			XPCC_LOG_INFO << "CH0: " << Fpga::getAdc(Fpga::ADC_0);
			XPCC_LOG_INFO << " CH1: " << Fpga::getAdc(Fpga::ADC_1);
			XPCC_LOG_INFO << " CH2: " << Fpga::getAdc(Fpga::ADC_2);
			XPCC_LOG_INFO << " CH3: " << Fpga::getAdc(Fpga::ADC_3);
			XPCC_LOG_INFO << " CH4: " << Fpga::getAdc(Fpga::ADC_4);
			XPCC_LOG_INFO << " CH5: " << Fpga::getAdc(Fpga::ADC_5);
			XPCC_LOG_INFO << " CH6: " << Fpga::getAdc(Fpga::ADC_6);
			XPCC_LOG_INFO << " CH7: " << Fpga::getAdc(Fpga::ADC_7) << xpcc::endl;


			if (Ui::button.isPressed(Ui::BUTTON1))
			{
			}
			
			// set PWM for BLDC2
			//loa::Damballa::writeWord(0x0020, 512 + speed);
		}
		
		Uplink::update();
		Ui::update();
	}
}
