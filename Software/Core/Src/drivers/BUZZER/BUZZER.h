#ifndef SRC_DRIVERS_BUZZER_BUZZER_H_
#define SRC_DRIVERS_BUZZER_BUZZER_H_

//  Imports
#include <stm32f4xx_hal.h>

// --- Octave 5 (The "Low" Audible Range) ---
// Note: C5, C#5, D5 are omitted as their ARR > 800 (Inaudible)

#define NOTE_DS5  803   // D# / Eb (622 Hz) - START OF AUDIBLE RANGE
#define NOTE_E5   758   // E (659 Hz)
#define NOTE_F5   715   // F (698 Hz)
#define NOTE_FS5  675   // F# / Gb (740 Hz)
#define NOTE_G5   637   // G (784 Hz)
#define NOTE_GS5  601   // G# / Ab (831 Hz)
#define NOTE_A5   567   // A (880 Hz)
#define NOTE_AS5  535   // A# / Bb (932 Hz)
#define NOTE_B5   505   // B (988 Hz)

// --- Octave 6 (Mid Range - Clear Tones) ---
#define NOTE_C6   477   // C (1047 Hz)
#define NOTE_CS6  450   // C# / Db (1109 Hz)
#define NOTE_D6   425   // D (1175 Hz)
#define NOTE_DS6  401   // D# / Eb (1245 Hz)
#define NOTE_E6   378   // E (1319 Hz)
#define NOTE_F6   357   // F (1397 Hz)
#define NOTE_FS6  337   // F# / Gb (1480 Hz)
#define NOTE_G6   318   // G (1568 Hz)
#define NOTE_GS6  300   // G# / Ab (1661 Hz)
#define NOTE_A6   283   // A (1760 Hz)
#define NOTE_AS6  267   // A# / Bb (1865 Hz)
#define NOTE_B6   252   // B (1976 Hz)

// --- Octave 7 (High Range - RESONANT/LOUDEST) ---
// Your resonant peak (~185 ARR) is right around E7

#define NOTE_C7   238   // C (2093 Hz)
#define NOTE_CS7  224   // C# / Db (2217 Hz)
#define NOTE_D7   212   // D (2349 Hz)
#define NOTE_DS7  200   // D# / Eb (2489 Hz)
#define NOTE_E7   189   // E (2637 Hz) - *** SWEET SPOT ***
#define NOTE_F7   178   // F (2794 Hz)
#define NOTE_FS7  168   // F# / Gb (2960 Hz)
#define NOTE_G7   158   // G (3136 Hz)
#define NOTE_GS7  149   // G# / Ab (3322 Hz)
#define NOTE_A7   141   // A (3520 Hz)
#define NOTE_AS7  133   // A# / Bb (3729 Hz)
#define NOTE_B7   126   // B (3951 Hz)

#define NOTE_REST 0     // Silence

#define BEEP      185

typedef struct {
    TIM_HandleTypeDef *htim;    // pointer to Timer handle
    uint32_t TIM_CHANNEL;       // Channel of the timer to which output compare is configured
} Buzzer_Handle_t;

HAL_StatusTypeDef BuzzerInit(Buzzer_Handle_t *buzzer);
HAL_StatusTypeDef BuzzerBeep(Buzzer_Handle_t *buzzer);
HAL_StatusTypeDef BuzzerPlayNote(Buzzer_Handle_t *buzzer, uint16_t note);

#endif /* SRC_DRIVERS_BUZZER_BUZZER_H_ */
