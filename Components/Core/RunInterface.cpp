/*
 *  RunInterface.cpp
 *
 *  Created on: Apr 3, 2023
 *      Author: Chris (cjchanx)
 */

#include "main_avionics.hpp"
#include "UARTDriver.hpp"

extern "C" {
    void run_interface()
    {
        run_main();
    }

    void cpp_USART1_IRQHandler()
    {
        Driver::uart1.HandleIRQ_UART();
    }

    void cpp_USART3_IRQHandler()
    {
        Driver::uart3.HandleIRQ_UART();
    }
}
