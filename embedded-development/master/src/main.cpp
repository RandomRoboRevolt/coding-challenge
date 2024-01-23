#include "statemachine.h"
#include "motordriver.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <memory>




/**
 * @brief main This is a minimal main function.
 * @return
 */

int main()
{
    std::cout << "Starting simulated motor driver" << std::endl;


    auto motorDriverP = std::make_shared<MotorDriver>();
    boost::sml::sm<MotorStateMachine<MotorDriver>> stateMachine{motorDriverP};

    while(1)
    {
        stateMachine.process_event(StatusWordRead(motorDriverP));

        stateMachine.visit_current_states([](auto state) { std::cout << "I am in state: " << state.c_str() << std::endl; });

        // Call update to simulate motor driver doing work
        motorDriverP->update();

        // Sleep for 100ms to simulate cyclic control
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
    }
}
