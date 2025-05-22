/**
 ******************************************************************************
 * File Name          : FastLogManager.cpp
 * Description        : FastLogManager manages the fast log state machine
 ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "FastLogManager.hpp"
#include "PBBProtocolTask.hpp"
#include "main_avionics.hpp"
#include "Data.h"
#include "DebugTask.hpp"
#include "Task.hpp"
#include "main.h"
#include "Flash.hpp"

/* Constructor ------------------------------------------------------------------*/
FastLogManager::FastLogManager()
{
    state = IDLE;
    msTick = 0;
    msStartTick = 0;
    sendIdx = 0;
}

/* External Functions ------------------------------------------------------------------*/
void FastLogManager::TransitionPend()
{
    if(state == IDLE) {
        state = PEND;
    }
}

void FastLogManager::TransitionStart()
{
    if(state == PEND) {
        msStartTick = 0;
        state = START;
    }
}

void FastLogManager::TransitionReset()
{
    pressureLogBuffer.clear();
    state = IDLE;
}

/* Internal Functions ------------------------------------------------------------------*/  
void FastLogManager::TransitionSend()
{
    sendIdx = 0;
    state = SEND;
}

void FastLogManager::TransmitLogData(PressureLog& pl, uint32_t timestamp) {
    Proto::TelemetryMessage msg;
	msg.set_source(Proto::Node::NODE_PBB);
	msg.set_target(Proto::Node::NODE_DMB);
	Proto::PressureLog plg;
	plg.set_time(timestamp);
	plg.set_ib_pressure(pl.pt18Pressure);
    plg.set_pv_pressure(pl.pt19Pressure);
    msg.set_pressureLog(plg);
	EmbeddedProto::WriteBufferFixedSize<DEFAULT_PROTOCOL_WRITE_BUFFER_SIZE> writeBuffer;
	msg.serialize(writeBuffer);

    // Send the data
    PBBProtocolTask::SendProtobufMessage(writeBuffer, Proto::MessageID::MSG_TELEMETRY);
}

/* Functions ------------------------------------------------------------------*/

void FastLogManager::Run() {
    xLastWakeTime = xTaskGetTickCount();
    switch (state) {
        case IDLE:
            SamplePressureTransducer();
            TransmitProtocolPressureData();
            vTaskDelayUntil(&xLastWakeTime, AUTO_PT_SLOW_PERIOD);
            break;
        case PEND:
            SamplePressureTransducer();

            // If the tick is greater than the AUTO_PT_SLOW_PERIOD, transmit the data
            if (++msTick > AUTO_PT_SLOW_PERIOD) {
                TransmitProtocolPressureData();
                msTick = 0;
            }
            
            // Add to buffer
            pressureLogBuffer.push(data);

            // Delay
            vTaskDelayUntil(&xLastWakeTime, 1);
            break;
        case START:
            SamplePressureTransducer();

//            Flash_Write(next_addr_pt18, PT18_END_ADDR, data.pt18Pressure);
//            Flash_Write(next_addr_pt19, PT19_END_ADDR, data.pt19Pressure);

            // If the tick is greater than the AUTO_PT_SLOW_PERIOD, transmit the data
            if (++msTick > AUTO_PT_SLOW_PERIOD) {
                TransmitProtocolPressureData();
                msTick = 0;
            }

            // Add to buffer
            pressureLogBuffer.push(data);

            // If the start tick is greater than the log buffer size - 1000, stop filling the buffer and go to send
            if (++msStartTick > (LOG_BUFFER_SIZE - 1000)) {
                state = SEND;
            }

            // Delay
            vTaskDelayUntil(&xLastWakeTime, 1);
            break;
        case SEND:
            // If the tick is past the slow period
            if ((++msTick * AUTO_PT_SEND_PERIOD) > AUTO_PT_SLOW_PERIOD) {
                // Sample and send
                SamplePressureTransducer();
                TransmitProtocolPressureData();
                msTick = 0;
            }

            // Manage sending the log data
            if(sendIdx < LOG_BUFFER_SIZE) {
                PressureLog data = pressureLogBuffer[sendIdx];
                TransmitLogData(data, sendIdx);
                sendIdx += 1;
            } else {
                sendIdx = 0;
            }
            
            // Delay
            vTaskDelayUntil(&xLastWakeTime, AUTO_PT_SEND_PERIOD);
            break;
    }
}


/* Pressure Transducer Readers ------------------------------------------------------------------*/

void ADC_Select_CH2 (void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	  */
	  sConfig.Channel = ADC_CHANNEL_2;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void ADC_Select_CH15 (void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	  */
	  sConfig.Channel = ADC_CHANNEL_15;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void FastLogManager::SamplePressureTransducer()
{
    static const int PT_VOLTAGE_ADC_POLL_TIMEOUT = 50;
    static const float ADC_TO_PRESSURE_SCALE = 0.4871f; // psi per ADC count
    static const float PRESSURE_OFFSET = -362.5f;       // psi offset

    uint32_t adcVal[2] = {0};

    ADC_Select_CH2();
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, PT_VOLTAGE_ADC_POLL_TIMEOUT) == HAL_OK) {
        adcVal[0] = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);

    ADC_Select_CH15();
    HAL_ADC_Start(&hadc2);
    if (HAL_ADC_PollForConversion(&hadc2, PT_VOLTAGE_ADC_POLL_TIMEOUT) == HAL_OK) {
        adcVal[1] = HAL_ADC_GetValue(&hadc2);
    }
    HAL_ADC_Stop(&hadc2);

    /* Calculate pressures */
    float pressure_psi_19 = adcVal[0] * ADC_TO_PRESSURE_SCALE + PRESSURE_OFFSET;
    float pressure_psi_18 = adcVal[1] * ADC_TO_PRESSURE_SCALE + PRESSURE_OFFSET;

    /* Store as PSI * 10 (for 1 decimal place) */
    data.pt18Pressure = (int16_t)((pressure_psi_18 * 10.0f) + 0.5f);
    data.pt19Pressure = (int16_t)((pressure_psi_19 * 10.0f) + 0.5f);
}

/**
 * @brief Transmits a protocol barometer data sample
 */
void FastLogManager::TransmitProtocolPressureData()
{
    Proto::TelemetryMessage msg;
	msg.set_source(Proto::Node::NODE_PBB);
	msg.set_target(Proto::Node::NODE_DMB);
	Proto::PbbPressure pressData;
	pressData.set_ib_pressure(data.pt18Pressure);
	pressData.set_lower_pv_pressure(data.pt19Pressure);
	msg.set_pbbPressure(pressData);

	EmbeddedProto::WriteBufferFixedSize<DEFAULT_PROTOCOL_WRITE_BUFFER_SIZE> writeBuffer;
	msg.serialize(writeBuffer);

    // Send the barometer data
    PBBProtocolTask::SendProtobufMessage(writeBuffer, Proto::MessageID::MSG_TELEMETRY);
}
