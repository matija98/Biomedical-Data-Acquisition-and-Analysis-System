#include <main.h>

RCC_ClocksTypeDef RCC_Clocks;

static __IO uint32_t TimingDelay;
void print_on_screen(char *text, uint32_t lineStart, uint32_t flag);

#define TEST_LENGTH_SAMPLES		12000
#define BLOCK_SIZE						100
#define NUM_TAPS							21
#define FFT_SIZE							2048

//Filter coefficients
const float32_t firCoeffsLowECG[NUM_TAPS] = {
0.00659214f, 0.00866379f, 0.01450731f, 0.02376434f, 0.03566341f, 0.04909693f, 0.06274115f, 0.07520499f, 0.08518983f, 0.09164080f, 0.09387062f, 0.09164080f, 0.08518983f, 0.07520499f, 0.06274115f, 0.04909693f, 0.03566341f, 0.02376434f, 0.01450731f, 0.00866379f, 0.00659214f
};
const float32_t firCoeffsHighECG[NUM_TAPS] = {
-0.00008888f, -0.00011390f, -0.00018649f, -0.00029957f, -0.00044206f, -0.00060002f, -0.00075798f, -0.00090049f, -0.00101358f, -0.00108619f, 0.99897767f, -0.00108619f, -0.00101358f, -0.00090049f, -0.00075798f, -0.00060002f, -0.00044206f, -0.00029957f, -0.00018649f, -0.00011390f, -0.00008888f
};
const float32_t firCoeffsLowRESP[NUM_TAPS] = {
0.00709687f, 0.00916830f, 0.01512099f, 0.02444405f, 0.03626932f, 0.04945767f, 0.06271479f, 0.07472463f, 0.08428592f, 0.09043764f, 0.09255962f, 0.09043764f, 0.08428592f, 0.07472463f, 0.06271479f, 0.04945767f, 0.03626932f, 0.02444405f, 0.01512099f, 0.00916830f, 0.00709687f
};
const float32_t firCoeffsHighRESP[NUM_TAPS] = {
-0.00003200f, -0.00004101f, -0.00006714f, -0.00010785f, -0.00015914f, -0.00021601f, -0.00027287f, -0.00032416f, -0.00036487f, -0.00039101f, 0.99963199f, -0.00039101f, -0.00036487f, -0.00032416f, -0.00027287f, -0.00021601f, -0.00015914f, -0.00010785f, -0.00006714f, -0.00004101f, -0.00003200f
};
const float32_t firCoeffsLowEMG[NUM_TAPS] = {
-0.00255270f, 0.00358980f, -0.00636727f, 0.01095073f, -0.01711741f, 0.02436794f, -0.03198506f, 0.03913135f, -0.04497049f, 0.04879249f, 0.95232124f, 0.04879249f, -0.04497049f, 0.03913135f, -0.03198506f, 0.02436794f, -0.01711741f, 0.01095073f, -0.00636727f, 0.00358980f, -0.00255270f
};
const float32_t firCoeffsHighEMG[NUM_TAPS] = {
-0.00180385f, -0.00235891f, -0.00393261f, -0.00641746f, -0.00959937f, -0.01317920f, -0.01680454f, -0.02010847f, -0.02275062f, -0.02445567f, 0.97674260f, -0.02445567f, -0.02275062f, -0.02010847f, -0.01680454f, -0.01317920f, -0.00959937f, -0.00641746f, -0.00393261f, -0.00235891f, -0.00180385f
};

// Initialize arrays
static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];
static float32_t signal[TEST_LENGTH_SAMPLES] = {0};
static float32_t filterLow[TEST_LENGTH_SAMPLES];
static float32_t filterHigh[TEST_LENGTH_SAMPLES];
static float32_t outputFFT[FFT_SIZE];

uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = TEST_LENGTH_SAMPLES / BLOCK_SIZE;

int main(void){

	char serialInByte;
	char stringBuffer[10] = {0};
	char stringOutput[8];
	uint32_t indicator = 0;
	uint32_t signalIndex = 0;
	uint32_t i, j;
	uint32_t firstInput = 1;
	float32_t signalElement;
	float32_t frequency;
	float32_t sampleFreq;

	arm_fir_instance_f32 SL;
	arm_fir_instance_f32 SH;
	arm_rfft_fast_instance_f32 SF;
	uint32_t fftSize = FFT_SIZE; 
	uint32_t indexMaxValue = 0;
	float32_t maxValue;
  float32_t  *inputF32, *outputfirL, *outputfirH;
	inputF32 = &signal[0];
  outputfirL = &filterLow[0];
	outputfirH = &filterHigh[0];
		
	RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	
	// Initialize LCD
  LCD_Init();
	LCD_LayerInit();
  LTDC_Cmd(ENABLE);
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
  LCD_Clear(LCD_COLOR_WHITE);

	// Initialize RFFT
	arm_rfft_fast_init_f32(&SF, fftSize);
		
	// Initialize USARTx	
	USART1_Init();
	USART6_Init();
	
	print_on_screen("Initialization", 100, 0);
	print_on_screen("completed", 140, 0);
	
	while(1){
		
		if (USART1_Dequeue(&serialInByte) != 0){
			// ECG signal
			if(serialInByte == 'C'){
				print_on_screen("Received", 100, 1);
				print_on_screen("ECG signal", 140, 0);
				// Initialize FIR filters
				arm_fir_init_f32(&SL, NUM_TAPS, (float32_t *)&firCoeffsLowECG[0], &firStateF32[0], blockSize);
				arm_fir_init_f32(&SH, NUM_TAPS, (float32_t *)&firCoeffsHighECG[0], &firStateF32[0], blockSize);
				signalIndex = 0;
				firstInput = 1;
				// Filtering
				for(i=0; i < numBlocks; i++){
					arm_fir_f32(&SL, inputF32 + (i * blockSize), outputfirL + (i * blockSize), blockSize);
					arm_fir_f32(&SH, outputfirL + (i * blockSize), outputfirH + (i * blockSize), blockSize);
				}
				print_on_screen("Filtering", 100, 1);
				print_on_screen("completed", 140, 0);
				// RFFT, cmplx magnitude calculation, finding max frequency
				arm_rfft_fast_f32(&SF, outputfirH, &outputFFT[0], 0); //promjena iz outputfirh u inputf32
				arm_cmplx_mag_f32(&outputFFT[0], &outputFFT[0], fftSize);
				outputFFT[0] = 0;
				arm_max_f32(&outputFFT[0], 12, &maxValue, &indexMaxValue);
				frequency = (float32_t)sampleFreq / fftSize * indexMaxValue;
				print_on_screen("Frequency", 100, 1);
				print_on_screen("calculated", 140, 0);
				// Serial output of sample frequency, filtered signal and max frequency
				sprintf(stringOutput, "%.2f", sampleFreq);
				for(i=0; i<7; i++){
					USART6_SendChar(stringOutput[i]);
				}
				for(i=0; i < TEST_LENGTH_SAMPLES; i++){
					sprintf(stringOutput, "%.4f", filterHigh[i]);
					for(j=0; j<7; j++){
						USART6_SendChar(stringOutput[j]);
					}
				}
				sprintf(stringOutput, "%.4f", frequency * 60);
				for(i=0; i<7; i++){
					USART6_SendChar(stringOutput[i]);
				}
				print_on_screen("Data sent,", 100, 1);
				print_on_screen("ready for", 140, 0);
				print_on_screen("next signal", 180, 0);
				continue;
			}
			// Respiratory signal
			if(serialInByte == 'R'){
				print_on_screen("Received", 100, 1);
				print_on_screen("respiratory", 140, 0);
				print_on_screen("signal", 180, 0);
				arm_fir_init_f32(&SL, NUM_TAPS, (float32_t *)&firCoeffsLowRESP[0], &firStateF32[0], blockSize);
				arm_fir_init_f32(&SH, NUM_TAPS, (float32_t *)&firCoeffsHighRESP[0], &firStateF32[0], blockSize);
				signalIndex = 0;
				firstInput = 1;
				// Filtering
				for(i=0; i < numBlocks; i++){
					arm_fir_f32(&SL, inputF32 + (i * blockSize), outputfirL + (i * blockSize), blockSize);
					arm_fir_f32(&SH, outputfirL + (i * blockSize), outputfirH + (i * blockSize), blockSize);
				}
				print_on_screen("Filtering", 100, 1);
				print_on_screen("completed", 140, 0);
				// RFFT, cmplx magnitude calculation, finding max frequency
				arm_rfft_fast_f32(&SF, outputfirH, &outputFFT[0], 0); //promjena iz outputfirh u inputf32
				arm_cmplx_mag_f32(&outputFFT[0], &outputFFT[0], fftSize);
				outputFFT[0] = 0;
				arm_max_f32(&outputFFT[0], 12, &maxValue, &indexMaxValue);
				frequency = (float32_t)sampleFreq / fftSize * indexMaxValue;
				print_on_screen("Frequency", 100, 1);
				print_on_screen("calculated", 140, 0);
				// Serial output of sample frequency, filtered signal and max frequency
				sprintf(stringOutput, "%.2f", sampleFreq);
				for(i=0; i<7; i++){
					USART6_SendChar(stringOutput[i]);
				}
				for(i=0; i < TEST_LENGTH_SAMPLES; i++){
					sprintf(stringOutput, "%.4f", filterHigh[i]);
					for(j=0; j<7; j++){
						USART6_SendChar(stringOutput[j]);
					}
				}
				sprintf(stringOutput, "%.4f", frequency * 60);
				for(i=0; i<7; i++){
					USART6_SendChar(stringOutput[i]);
				}
				print_on_screen("Data sent,", 100, 1);
				print_on_screen("ready for", 140, 0);
				print_on_screen("next signal", 180, 0);
				continue;
			}
			// EMG signal
			if(serialInByte == 'M'){
				print_on_screen("Received", 100, 1);
				print_on_screen("EMG signal", 140, 0);
				arm_fir_init_f32(&SL, NUM_TAPS, (float32_t *)&firCoeffsLowEMG[0], &firStateF32[0], blockSize);
				arm_fir_init_f32(&SH, NUM_TAPS, (float32_t *)&firCoeffsHighEMG[0], &firStateF32[0], blockSize);
				signalIndex = 0;
				firstInput = 1;
				// Filtering
				for(i=0; i < numBlocks/2; i++){
					arm_fir_f32(&SL, inputF32 + (i * blockSize), outputfirL + (i * blockSize), blockSize);
					arm_fir_f32(&SH, outputfirL + (i * blockSize), outputfirH + (i * blockSize), blockSize);
				}
				print_on_screen("Filtering", 100, 1);
				print_on_screen("completed", 140, 0);
				// RFFT, cmplx magnitude calculation, finding max frequency
				arm_rfft_fast_f32(&SF, outputfirH, &outputFFT[0], 0);
				arm_cmplx_mag_f32(&outputFFT[0], &outputFFT[0], fftSize);
				outputFFT[0] = 0;
				arm_max_f32(&outputFFT[0], fftSize/2, &maxValue, &indexMaxValue);
				frequency = (float32_t)sampleFreq / fftSize * indexMaxValue;
				print_on_screen("Frequency", 100, 1);
				print_on_screen("calculated", 140, 0);
				// Serial output of sample frequency, signal after FFT and max frequency
				sprintf(stringOutput, "%.2f", sampleFreq);
				for(i=0; i<7; i++){
					USART6_SendChar(stringOutput[i]);
				}
				for(i=0; i < fftSize/2; i++){
					sprintf(stringOutput, "%.4f", outputFFT[i]);
					for(j=0; j<7; j++){
						USART6_SendChar(stringOutput[j]);
					}
				}
				sprintf(stringOutput, "%.4f", frequency);
				for(i=0; i<7; i++){
					USART6_SendChar(stringOutput[i]);
				}
				print_on_screen("Data sent,", 100, 1);
				print_on_screen("ready for", 140, 0);
				print_on_screen("next signal", 180, 0);
				continue;
			}
			// Load signal element byte by byte until 'X'
			if(serialInByte != 'X'){
				stringBuffer[indicator] = serialInByte;
				indicator++;
				continue;
			}
			indicator=0;
			signalElement = atof(stringBuffer);
			// The first input element is the signal sampling frequency
			if(firstInput == 1){
				sampleFreq = signalElement;
				firstInput = 0;
				continue;
			}
			// Store signal element in array
			signal[signalIndex] = signalElement;
			signalIndex++;
		}
	}
}

//Print on LCD function
void print_on_screen(char *text, uint32_t lineStart, uint32_t flag){
	if(flag == 1){
		Delay(100);
		LCD_Clear(LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_BLACK);
	}
	LCD_DisplayStringLine(lineStart, text);
}

//void print_on_screen(float32_t pulse_value){
//	
//	char characterBuffer[15] = {0};
//	
//	sprintf(characterBuffer, "%.5f", pulse_value); // Stavi float u string
//	LCD_Clear(LCD_COLOR_WHITE);
//  LCD_SetTextColor(LCD_COLOR_BLACK);
//	LCD_DisplayStringLine(140, characterBuffer);
//	Delay(150);
//}

// Delay function, nTime = 1 => delay = 10 ms
void Delay(__IO uint32_t nTime){
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

//  Decrement TimingDelay variable
void TimingDelay_Decrement(void){
  if (TimingDelay != 0x00){ 
    TimingDelay--;
  }
}
