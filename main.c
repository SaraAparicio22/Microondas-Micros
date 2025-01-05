#include "main.h"

// Variables globales
volatile uint32_t tiempo_restante = 0; // Tiempo restante en segundos
volatile uint8_t microondas_activo = 0; // Estado del microondas
uint16_t potencia_adc = 0; // Valor de potencia leído por el ADC

// Prototipos de funciones
void iniciar_microondas(void);
void detener_microondas(void);

// Interrupción de temporizador (1 Hz)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM3) { // Temporizador 3
        if (microondas_activo && tiempo_restante > 0) {
            tiempo_restante--;
            if (tiempo_restante == 0) {
                detener_microondas();
            }
        }
    }
}

// Interrupción por pulsador
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) { // Botón de inicio/parada
        if (microondas_activo) {
            detener_microondas();
        } else {
            iniciar_microondas();
        }
    } else if (GPIO_Pin == GPIO_PIN_1) { // Botón de incremento de tiempo
        if (!microondas_activo) {
            tiempo_restante += 30; // Añadir 30 segundos
        }
    }
}

// Función para iniciar el microondas
void iniciar_microondas(void) {
    microondas_activo = 1;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // Encender LED
}

// Función para detener el microondas
void detener_microondas(void) {
    microondas_activo = 0;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // Apagar LED
}

// Lectura del ADC (potencia)
void leer_potencia_adc(void) {
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
        potencia_adc = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM3_Init();
    MX_ADC1_Init();

    // Iniciar temporizador
    HAL_TIM_Base_Start_IT(&htim3);

    while (1) {
        leer_potencia_adc(); // Leer la potencia del ADC
        // Aquí se actualizaría el display con el tiempo restante y la potencia
    }
}
