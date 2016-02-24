#pragma once

#define F_BUTTON_DDR DDRB
#define L_BUTTON_DDR DDRB
#define B_BUTTON_DDR DDRB
#define R_BUTTON_DDR DDRD
#define U_BUTTON_DDR DDRD
#define D_BUTTON_DDR DDRD

#define F_BUTTON_PORT PORTB
#define L_BUTTON_PORT PORTB
#define B_BUTTON_PORT PORTB
#define R_BUTTON_PORT PORTD
#define U_BUTTON_PORT PORTD
#define D_BUTTON_PORT PORTD

#define DEBOUNCE_TIME 1000

typedef struct 
{
    char characterId;
    int buttonPin;
    int pressed;
} ButtonMap;

typedef struct
{
    ButtonMap forward, left, back, right, up, down;
    int f_id, l_id, b_id, r_id, u_id, d_id;
} ButtonController;

void initButtonController(ButtonController *buttonController);
int buttonPressed(ButtonController *buttonController, char buttonSel);
uint8_t getButtonStates(ButtonController *buttonController);
int buttonPressedDebounce(ButtonController *buttonController, char buttonSel);
