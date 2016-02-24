#include <stdlib.h>
#include <avr/io.h>
#include "ButtonController.h"
#include <util/delay.h>


void initButtonController(ButtonController *buttonController)
{
    //Set Variables
    buttonController->forward.buttonPin = PB6;
    buttonController->left.buttonPin = PB0;
    buttonController->back.buttonPin = PB7;
    buttonController->right.buttonPin = PD7;

    buttonController->up.buttonPin = PD5;
    buttonController->down.buttonPin = PD6;
       
    //Set Character Ids
    buttonController->forward.characterId = 'F';
    buttonController->left.characterId = 'L';
    buttonController->back.characterId = 'B';
    buttonController->right.characterId = 'R';

    buttonController->up.characterId = 'U';
    buttonController->down.characterId = 'D';

    //Set initial button states
    buttonController->forward.pressed = 0;
    buttonController->left.pressed = 0;
    buttonController->back.pressed = 0;
    buttonController->right.pressed = 0;

    buttonController->up.pressed = 0;
    buttonController->down.pressed = 0;


    buttonController->f_id = 0;
    buttonController->l_id = 1;
    buttonController->b_id = 2;
    buttonController->r_id = 3;
    buttonController->u_id = 4;
    buttonController->d_id = 5;

    //Set up DDRs
    F_BUTTON_DDR &= ~(1 << buttonController->forward.buttonPin);
    L_BUTTON_DDR &= ~(1 << buttonController->left.buttonPin);
    B_BUTTON_DDR &= ~(1 << buttonController->back.buttonPin);
    R_BUTTON_DDR &= ~(1 << buttonController->right.buttonPin);
    U_BUTTON_DDR &= ~(1 << buttonController->up.buttonPin);
    D_BUTTON_DDR &= ~(1 << buttonController->down.buttonPin);

    //Set up pull up resistors
    F_BUTTON_PORT |= (1 << buttonController->forward.buttonPin);
    L_BUTTON_PORT |= (1 << buttonController->left.buttonPin);
    B_BUTTON_PORT |= (1 << buttonController->back.buttonPin);
    R_BUTTON_PORT |= (1 << buttonController->right.buttonPin);
    U_BUTTON_PORT |= (1 << buttonController->up.buttonPin);
    D_BUTTON_PORT |= (1 << buttonController->down.buttonPin);

}
int buttonPressed(ButtonController *buttonController, char buttonSel)
{
    switch (buttonSel)
    {
        case 'F':
            return !(PINB & (1 << buttonController->forward.buttonPin));
        case 'L':
            return !(PINB & (1 << buttonController->left.buttonPin));
        case 'B':
            return !(PINB & (1 << buttonController->back.buttonPin));
        case 'R':
            return !(PIND & (1 << buttonController->right.buttonPin));
        case 'U':
            return !(PIND & (1 << buttonController->up.buttonPin));
        case 'D':
            return !(PIND & (1 << buttonController->down.buttonPin));
        default:
            return 0;
    }
}
uint8_t getButtonStates(ButtonController *buttonController)
{
    uint8_t out = 0x00;
                     
    if (buttonPressed(buttonController, 'F'))
        out |= (1 << buttonController->f_id);
    if (buttonPressed(buttonController, 'L'))
        out |= (1 << buttonController->l_id);
    if (buttonPressed(buttonController, 'B'))
        out |= (1 << buttonController->b_id);
    if (buttonPressed(buttonController, 'R'))
        out |= (1 << buttonController->r_id);

    if (buttonPressed(buttonController, 'U'))
        out |= (1 << buttonController->u_id);
    if (buttonPressed(buttonController, 'D'))
        out |= (1 << buttonController->d_id);

    return out;
}
int buttonPressedDebounce(ButtonController *buttonController, char buttonSel)
{
    switch (buttonSel)
    {
        case 'F':
        {
            if (!(PINB & (1 << buttonController->forward.buttonPin)))
            {
                _delay_us(DEBOUNCE_TIME);
                return !(PINB & (1 << buttonController->forward.buttonPin));
            }
            else
                return 0;
        }
        case 'L':
        {
            if (!(PINB & (1 << buttonController->left.buttonPin)))
            {
                _delay_us(DEBOUNCE_TIME);
                return !(PINB & (1 << buttonController->left.buttonPin));
            }
            else
                return 0;
        }
        case 'B':
        {
            if (!(PINB & (1 << buttonController->back.buttonPin)))
            {
                _delay_us(DEBOUNCE_TIME);
                return !(PINB & (1 << buttonController->back.buttonPin));
            }
            else
                return 0;
        }
        case 'R':
        {
            if (!(PIND & (1 << buttonController->right.buttonPin)))
            {
                _delay_us(DEBOUNCE_TIME);
                return !(PIND & (1 << buttonController->right.buttonPin));
            }
            else
                return 0;
        }
        case 'U':
        {
            if (!(PIND & (1 << buttonController->up.buttonPin)))
            {
                _delay_us(DEBOUNCE_TIME);
                return !(PIND & (1 << buttonController->up.buttonPin));
            }
            else
                return 0;
        }
        case 'D':
        {
            if (!(PIND & (1 << buttonController->down.buttonPin)))
            {
                _delay_us(DEBOUNCE_TIME);
                return !(PIND & (1 << buttonController->down.buttonPin));
            }
            else
                return 0;
        }
        default:
            return 0;
    }
}

