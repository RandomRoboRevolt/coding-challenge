#include <catch2/catch_test_macros.hpp>
#include <trompeloeil.hpp>
#include "statemachine.h"

/* We source the header from motordriver.h, but we implement
 * the class here in the test case to be able to Mock it
 * Also that is why we needed to template the state machine to be
 * able to Mock the MotorDriver without making it virtual */
class MotorDriverMock {
public:
    MAKE_MOCK1 (transferData, uint32_t (uint32_t));
    MAKE_MOCK0 (update, void());
};

/* Helper function */
uint32_t generateStatusMessageHelper(uint16_t data) {
    constexpr uint8_t lowByte = (STATUSWORD << 1);
    const auto *dataP = reinterpret_cast<uint8_t *>(&data);
    return lowByte | (data << 8) | ((lowByte ^ dataP[0] ^ dataP[1]) << 24);
}

SCENARIO("State Machine")
{
    using namespace sml;
    GIVEN("A MotorDriver and the MotorDriver State Machine") {
        auto MotorDriverMockP = std::make_shared<MotorDriverMock>();
        boost::sml::sm<MotorStateMachine<MotorDriverMock>> sm{MotorDriverMockP};
        WHEN("I do nothing") {
            THEN("It gets destroyed with not issues") {}
        }WHEN("I process a random unknown event") {
            THEN("The state machine will issue an error") {
                CHECK_THROWS_AS(sm.process_event(int{}), std::logic_error);
                REQUIRE(sm.is(sml::X));
            }
        }WHEN("I am in Boot state and I do *not* read PREOP ") {
            THEN("I will remain in boot state") {
                REQUIRE_CALL(*MotorDriverMockP, transferData(trompeloeil::_)).RETURN(0);
                sm.process_event(StatusWordRead(MotorDriverMockP));
                REQUIRE(true == sm.is("boot"_s));
            }
        }WHEN("I am in Boot state and Status Word read *is* PREOP") {
            trompeloeil::sequence seq1;
            REQUIRE_CALL(*MotorDriverMockP, transferData(generateStatusRegisterRead())).RETURN(
                    generateStatusMessageHelper(STATE_PREOP)).IN_SEQUENCE(seq1);
            REQUIRE_CALL(*MotorDriverMockP, transferData(generateWriteZeroToEncoder()))
            .RETURN(generateWriteZeroToEncoder()).IN_SEQUENCE(seq1);
            THEN(
                    "Transition to SafeOP, write Zero to Encoder Value") {
                sm.process_event(StatusWordRead(MotorDriverMockP));
                REQUIRE(sm.is("safeop"_s));
                AND_WHEN("Fault Register is not set") {
                    REQUIRE(sm.is("safeop"_s));
                    trompeloeil::sequence seq2;
                    REQUIRE_CALL(*MotorDriverMockP, transferData(generateStatusRegisterRead())).RETURN(
                            generateStatusMessageHelper(STATE_PREOP)).IN_SEQUENCE(seq2);

                    REQUIRE_CALL(*MotorDriverMockP, transferData(generateFaultRegisterRead()))
                    .RETURN(generateFaultRegisterRead()).IN_SEQUENCE(seq2);

                    REQUIRE_CALL(*MotorDriverMockP, transferData(generateReadEncoder()))
                    .RETURN(generateReadEncoder()).IN_SEQUENCE(seq2);
                    REQUIRE_CALL(*MotorDriverMockP, transferData(generateClearFaultFlag())).RETURN(generateClearFaultFlag()).IN_SEQUENCE(seq2);
                    REQUIRE_CALL(*MotorDriverMockP, transferData(generateEnableOutput())).RETURN(0).IN_SEQUENCE(seq2);
                    AND_THEN("Transition to OP") {
                        sm.process_event(StatusWordRead(MotorDriverMockP));
                        REQUIRE(sm.is("op"_s));
                        AND_WHEN("We still have no fault flag") {
                            trompeloeil::sequence seq3;
                            REQUIRE_CALL(*MotorDriverMockP, transferData(generateStatusRegisterRead())).RETURN(
                            generateStatusMessageHelper(STATE_PREOP)).IN_SEQUENCE(seq3);
                            REQUIRE_CALL(*MotorDriverMockP, transferData(generateFaultRegisterRead()))
                    .RETURN(generateFaultRegisterRead()).IN_SEQUENCE(seq3);
                            REQUIRE_CALL(*MotorDriverMockP,transferData(generateReadEncoder())).RETURN(generateReadEncoder()).IN_SEQUENCE(seq3);
                            REQUIRE_CALL(*MotorDriverMockP,transferData(generateWriteMotorCommand(1234))).RETURN(0).IN_SEQUENCE(seq3);
                            AND_THEN("We remain in op state") {
                                REQUIRE(sm.is("op"_s));
                                sm.process_event(StatusWordRead(MotorDriverMockP));
                                REQUIRE(sm.is("op"_s));
                            }
                        }
                        AND_WHEN("We get a fault flag") {
                            trompeloeil::sequence seq4;
                            REQUIRE_CALL(*MotorDriverMockP, transferData(generateStatusRegisterRead())).RETURN(
                            generateStatusMessageHelper(STATE_PREOP)).IN_SEQUENCE(seq4);
                            REQUIRE_CALL(*MotorDriverMockP, transferData(generateFaultRegisterRead())).RETURN(0xffffffff).IN_SEQUENCE(seq4);
                            REQUIRE_CALL(*MotorDriverMockP, transferData(generateFaultRegisterRead())).RETURN(0xffffffff).IN_SEQUENCE(seq4);
                            REQUIRE_CALL(*MotorDriverMockP,transferData(generateDisableOutput())).RETURN(0).IN_SEQUENCE(seq4);
                            REQUIRE_CALL(*MotorDriverMockP,transferData(generateWriteZeroToEncoder())).RETURN(generateWriteZeroToEncoder()).IN_SEQUENCE(seq4);
                            AND_THEN("we drop to safeop state") {
                                REQUIRE(sm.is("op"_s));
                                sm.process_event(StatusWordRead(MotorDriverMockP));
                                REQUIRE(sm.is("safeop"_s));
                            }
                        }
                    }
                }
            }
        }
    }
}