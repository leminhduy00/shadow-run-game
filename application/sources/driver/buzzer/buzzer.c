#include <stdbool.h>
#include <buzzer.h>
#include "tim.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_rcc.h"

volatile       uint32_t          _beep_duration;
volatile       bool              _tones_playing;
volatile const Tone_TypeDef     *_tones;
volatile       bool              _buzzer_silent = BUZZER_SILENT_OFF;

typedef struct {
	buzzer_sound_t sound;
	const Tone_TypeDef* tones;
} buzzer_music_t;

static const buzzer_music_t buzzer_music_table[] = {
	{BUZZER_SOUND_CLICK,			tones_click},
	{BUZZER_SOUND_BANG,				tones_bang},
	{BUZZER_SOUND_USB_CONNECTED,	tones_usb_connected},
	{BUZZER_SOUND_USB_DISCONNECTED,	tones_usb_disconnected},
	{BUZZER_SOUND_LETS_GO,			tones_lets_go},
	{BUZZER_SOUND_STARTUP,			tones_startup},
	{BUZZER_SOUND_3BEEP,			tones_3beep},
	{BUZZER_SOUND_WELCOME,			tones_welcome},
	{BUZZER_SOUND_GOODBYE,			tones_goodbye},
	{BUZZER_SOUND_HIGHSCORE,		tones_highscore},
	{BUZZER_SOUND_LOWSCORE,			tones_lowscore},
	{BUZZER_SOUND_SUPER_MARIO,		tones_supper_mario_bros},
	{BUZZER_SOUND_MERRY_CHRISTMAS,	tones_merry_christmas},
	{BUZZER_SOUND_TONE_1,           sTone1},
	{BUZZER_SOUND_TONE_2,           sTone2},
	{BUZZER_SOUND_TONE_3,           sTone3},
	{BUZZER_SOUND_TONE_4,           sTone4},
	{BUZZER_SOUND_TONE_5,           sTone5},
	{BUZZER_SOUND_TONE_6,           sTone6},
	{BUZZER_SOUND_TONE_7,           sTone7},
	{BUZZER_SOUND_MAX,				(const Tone_TypeDef*)0}
};

static const Tone_TypeDef* buzzer_get_music(buzzer_sound_t sound) {
	uint32_t index = 0;

	while (buzzer_music_table[index].tones != (const Tone_TypeDef*)0) {
		if (buzzer_music_table[index].sound == sound) {
			return buzzer_music_table[index].tones;
		}
		index++;
	}

	return (const Tone_TypeDef*)0;
}

void buzzer_irq( void ) {
	if (LL_TIM_IsActiveFlag_UPDATE(BUZZER_TIM) && LL_TIM_IsEnabledIT_UPDATE(BUZZER_TIM))
	{
		LL_TIM_ClearFlag_UPDATE(BUZZER_TIM); // Clear the TIMx's interrupt pending bit// Clear the TIMx's interrupt pending bit

		_beep_duration--;
		if (_beep_duration == 0) {
			if (_tones_playing) {
				// Currently playing tones, take next tone
				_tones++;
				if (_tones->frequency == 0 && _tones->duration == 0) {
					// Last tone in sequence
					BUZZER_Disable();
					_tones_playing = false;
					_tones = NULL;
				} else {
					if (_tones->frequency == 0) {
						// Silence period
						LL_TIM_SetAutoReload(BUZZER_TIM, HAL_RCC_GetPCLK1Freq() / (100 * (LL_TIM_GetPrescaler(BUZZER_TIM) + 1)) - 1);
						LL_TIM_OC_SetCompareCH2(BUZZER_TIM, 0); // 0% duty cycle
						LL_TIM_GenerateEvent_UPDATE(BUZZER_TIM);
						_beep_duration = _tones->duration + 1;
					} else {
						// Play next tone in sequence
						BUZZER_Enable(_tones->frequency,_tones->duration);
					}
				}
			} else {
				BUZZER_Disable();
			}
		}
	}
}

// Initialize buzzer output
void BUZZER_Init(void) {
	MX_TIM3_Init();
}

// Turn on buzzer with specified frequency
// input:
//   freq - PWM frequency for buzzer (Hz)
//   duration - duration of buzzer work (tens ms: 1 -> 10ms sound duration)
void BUZZER_Enable(uint16_t freq, uint32_t duration) {
	if (freq < 100 || freq > 8000 || duration == 0) {
		BUZZER_Disable();
	} else {
		_beep_duration = (freq / 100) * duration + 1;

		// Configure buzzer pin
		LL_GPIO_SetPinMode(BUZZER_IO_PORT, BUZZER_IO_PIN, LL_GPIO_MODE_ALTERNATE);

		// Configure and enable PWM timer
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3); // Enable TIMx peripheral
		LL_TIM_SetAutoReload(BUZZER_TIM, HAL_RCC_GetPCLK1Freq() / (freq * (LL_TIM_GetPrescaler(BUZZER_TIM) + 1)) - 1);
		LL_TIM_OC_SetCompareCH2(BUZZER_TIM, LL_TIM_GetAutoReload(BUZZER_TIM) >> 1); // 50% duty cycle
		LL_TIM_ClearFlag_UPDATE(BUZZER_TIM);
		LL_TIM_GenerateEvent_UPDATE(BUZZER_TIM);
		LL_TIM_EnableIT_UPDATE(BUZZER_TIM);
		LL_TIM_CC_EnableChannel(BUZZER_TIM, LL_TIM_CHANNEL_CH2);
		LL_TIM_EnableCounter(BUZZER_TIM); // Counter enable
	}
}

// Turn off buzzer
void BUZZER_Disable(void) {
	// Counter disable
	LL_TIM_DisableIT_UPDATE(BUZZER_TIM);
	LL_TIM_CC_DisableChannel(BUZZER_TIM, LL_TIM_CHANNEL_CH2);
	LL_TIM_DisableCounter(BUZZER_TIM);
	// Disable TIMx peripheral to conserve power
	LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM3);
	// Configure buzzer pin as analog input without pullup to conserve power
	LL_GPIO_SetPinMode(BUZZER_IO_PORT, BUZZER_IO_PIN, LL_GPIO_MODE_ANALOG);
}

// Start playing tones sequence
// input:
//   tones - pointer to tones array
static void BUZZER_PlayTones(const Tone_TypeDef * tones) {
	if (_buzzer_silent != BUZZER_SILENT_ON) {
		_tones = tones;
		_tones_playing = true;
		BUZZER_Enable(_tones->frequency,_tones->duration);
	}
}

void BUZZER_PlaySound(buzzer_sound_t sound) {
	const Tone_TypeDef* tones = buzzer_get_music(sound);
	if (tones != NULL) {
		BUZZER_PlayTones(tones);
	}
}

void BUZZER_Silent(bool isSilent) {
	_buzzer_silent = isSilent;
}
