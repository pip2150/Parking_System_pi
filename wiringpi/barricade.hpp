/*
 * barricade.hpp
 *
 * Created on: 2017. 11. 15.
 *      Author: dhrco   
 */
#ifndef BARRICADE_HPP_
#define BARRICADE_HPP_

class Barricade{
    int servo;

    public :

    Barricade(int servo);

    void open();

    void close();

    void hold();

};

#endif /* BARRICADE_HPP_ */
