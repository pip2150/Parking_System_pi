/*
 * lcd.hpp
 *
 * Created on: 2017. 11. 15.
 *      Author: dhrco   
 */

#ifndef LCD_HPP_
#define LCD_HPP_

#define MAXTEXTSIZE 10

struct RGB{
    int r;
    int g;
    int b;
};

class LCD{
    int disp;

    public :

    LCD();

    ~LCD();

    void display(char topText[], char bottomText[], RGB rgb);

};

#endif /* LCD_HPP_ */
