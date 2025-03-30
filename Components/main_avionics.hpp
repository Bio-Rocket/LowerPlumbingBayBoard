/**
 ******************************************************************************
 * File Name          : Main.hpp
 * Description        : Header file for Main.cpp, acts as an interface between
 *  STM32CubeIDE and our application.
 ******************************************************************************
*/
#ifndef AVIONICS_INCLUDE_SOAR_MAIN_H
#define AVIONICS_INCLUDE_SOAR_MAIN_H
#include "Mutex.hpp"
#include "stm32f4xx_hal.h"

/* Interface Functions ------------------------------------------------------------------*/
/* These functions act as our program's 'main' and any functions inside CubeIDE's main --*/
void run_main();
void run_StartDefaultTask();

/* Global Functions ------------------------------------------------------------------*/
void print(const char* format, ...);
void soar_assert_debug(bool condition, const char* file, uint16_t line, const char* str = nullptr, ...);

/* Global Variable Interfaces ------------------------------------------------------------------*/
/* All must be extern from main_avionics.cpp -------------------------------------------------*/
namespace Global
{
    extern Mutex vaListMutex;
}


/* System Handles ------------------------------------------------------------------*/
/* This should be the only place externs are allowed -------------------------------*/

//ADC Handles
extern ADC_HandleTypeDef hadc1;      // ADC1 - Pressure Transducer 19 ADC
extern ADC_HandleTypeDef hadc2;      // ADC2 - Pressure Transducer 18 ADC

//SPI Handles
extern SPI_HandleTypeDef hspi1;		 // SPI1 - Thermocouple 12 MISO/CLK
extern SPI_HandleTypeDef hspi2;		 // SPI2 - Thermocouple 11 MISO/CLK

//CRC Handles
extern CRC_HandleTypeDef hcrc;       // CRC - Hardware CRC System Handle

//Timer Handles
extern TIM_HandleTypeDef htim8; // TIM 8


namespace SystemHandles {
    // Aliases

	constexpr ADC_HandleTypeDef* ADC_Pressure_Transducer_19 = &hadc1;
	constexpr ADC_HandleTypeDef* ADC_Pressure_Transducer_28 = &hadc2;

	constexpr SPI_HandleTypeDef* SPI_Thermocouple12 = &hspi1;
	constexpr SPI_HandleTypeDef* SPI_Thermocouple11 = &hspi2;

    constexpr CRC_HandleTypeDef* CRC_Handle = &hcrc;
}

#endif /* AVIONICS_INCLUDE_SOAR_MAIN_H */
