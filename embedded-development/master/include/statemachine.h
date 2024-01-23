#ifndef EMBEDDEDDEVELOPMENT_STATEMACHINE_H
#define EMBEDDEDDEVELOPMENT_STATEMACHINE_H

#include "motordriver.h"

#include <boost/sml.hpp>

#include <cstdint>
#include <array>
#include <memory>
#include <stdexcept>

namespace sml = boost::sml;

/* Helper Functions */

constexpr uint32_t generateFaultRegisterRead() {
    return (FAULT << 1) | ((FAULT << 1) << 24); //write flag is 0, Value is 0 -> chksum is equal to byte 0
}

constexpr uint32_t generateStatusRegisterRead() {
    return (STATUSWORD << 1)| ((STATUSWORD << 1) << 24); //write flag is 0, Value is 0 -> chksum is equal to byte 0
}

constexpr uint32_t generateReadEncoder() {
    return (ENCODER_VALUE << 1) | ((ENCODER_VALUE << 1) << 24); //write flag is 0, Value is 0 -> chksum is equal to byte 0
}

constexpr uint32_t generateClearFaultFlag() {
    return 0x1 /*Write */ | (FAULT << 1) | ((0x1 /*Write */ | (FAULT << 1)) << 24);
}

uint32_t generateWriteMotorCommand(uint16_t data) {
    const auto* dataP = reinterpret_cast<uint8_t*>(&data);
    constexpr uint8_t command = 0x1 /* Write */ | (MOTOR_VELOCITY_COMMAND << 1);
    return ((command << 24) | (data << 8) | ((command ^ dataP[0] ^ dataP[1]) << 24));
}

constexpr uint32_t generateWriteZeroToEncoder() {
    return (0x1 /*Write*/ | ENCODER_VALUE << 1) | ((0x1 /*Write*/ | ENCODER_VALUE << 1) << 24);
}

constexpr uint32_t generateEnableOutput() {
    return (0x1 /* Write */ | (OUTPUT_ENABLE << 1) | 0x1 << 8 | ((OUTPUT_ENABLE << 1) << 24));
}

constexpr uint32_t generateDisableOutput() {
    return (0x1 /* Write */ | (OUTPUT_ENABLE << 1) | (0x1 | (OUTPUT_ENABLE << 1)) << 24);
}

/* events */
template <typename T>
struct StatusWordRead
{
    explicit StatusWordRead(const std::shared_ptr<T>& motorDriverP) {
        responseFromMotorDriver = motorDriverP->transferData(generateStatusRegisterRead());
    }

    uint32_t responseFromMotorDriver{};
};


/* guards */
template <typename T>
auto modePreOpSet = [](StatusWordRead<T> s) {
    const uint16_t data = (s.responseFromMotorDriver >> 8) & 0xFFFF;
    return STATE_PREOP == data;
};

template <typename T>
auto faultRegisterSet = [](const std::shared_ptr<T>& motorDriverP) {
    auto responseFromMotorDriver = motorDriverP->transferData(generateFaultRegisterRead());
    //Mask Value Bits to check if set.
    // ToDo CHKSUM
    return (0x0 != (responseFromMotorDriver&0x00ffff00));
};

template <typename T>
auto readEncoder = [](const std::shared_ptr<T>& motorDriverP) {
    auto responseFromMotorDriver = motorDriverP->transferData(generateReadEncoder());
    //ToDo Now do something with it, save it or pass it to some control...
};

template <typename T>
auto clearFaultFlag = [](const std::shared_ptr<T>& motorDriverP) {
    (void) motorDriverP->transferData(generateClearFaultFlag());
};

template <typename T>
auto writeMotorCommand = [](const std::shared_ptr<T>& motorDriverP) {
    (void) motorDriverP->transferData(generateWriteMotorCommand(1234)); //ToDo Write data that makes sense here
};

template <typename T>
auto setEncoderValueToZero = [](const std::shared_ptr<T>& motorDriverP){
    (void) motorDriverP->transferData(generateWriteZeroToEncoder());
};

template <typename T>
auto enableOutput = [](const std::shared_ptr<T>& motorDriverP){
    (void) motorDriverP->transferData(generateEnableOutput());
};

template <typename T>
auto disableOutput = [](const std::shared_ptr<T>& motorDriverP){
    (void) motorDriverP->transferData(generateDisableOutput());
};


template <typename T>
struct MotorStateMachine
{

  auto operator()() const noexcept
  {
    using namespace sml;
    return make_transition_table(
      //*"src_state"_s + event<e1> [ guard1 && guard2 ] / (action1, action2) = dst_state
      // clang-format off
      *"boot"_s + sml::event<StatusWordRead<T>> [ modePreOpSet<T> ] = "preop"_s,
       "preop"_s = "safeop"_s, //immediate transition to safeop
       "safeop"_s + sml::on_entry<_> / setEncoderValueToZero<T>,
       "safeop"_s + sml::event<StatusWordRead<T>> [ !faultRegisterSet<T> ] / (readEncoder<T>,clearFaultFlag<T>) = "op"_s,
       "op"_s + sml::on_entry<_> / enableOutput<T>,
       "op"_s + sml::on_exit<_> / disableOutput<T>,
       "op"_s + sml::event<StatusWordRead<T>> [ ! faultRegisterSet<T> ] / (readEncoder<T>, writeMotorCommand<T>),
       "op"_s + sml::event<StatusWordRead<T>> [faultRegisterSet<T>]  = "safeop"_s,
       //Orthogonal Region to catch faulty transitions / events
       *"error_handler"_s + unexpected_event<_> / []() {throw std::logic_error("Unexcpected Event");} = X
    );
    //clang-format on
  }
};
#endif //EMBEDDEDDEVELOPMENT_STATEMACHINE_H
