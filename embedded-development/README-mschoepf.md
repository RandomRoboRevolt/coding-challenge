# Task 1
### Rationale
The simplest solution would probably be to implement a state machine with switch case. This has some advantages but also some shortcomings especially if the state machine gets larger and more complex. The next idea I had was using std::variant. I have seen some implementations on the internet, but it soon got quite complex. Therefore, I turned to something I have been using in the past: get a well tested library that lets you fill out a transition table. In my experience, later debugging and adding states etc. has been much easier then with switch cases that span over screens. I have been using boost::msm, now I switched to boost::sml, which is more modern and does not have these long compile cycles.
### Details of the Implementation

I have tried to figure out the transitions given in the "datasheet", it was not really precise in some points, so I hope I got the right interpretation.

#### statemachine.h

I started out with some helper functions to generate the right uint32_t for the data transfer. Most of them are static, at compile time known uint32_t, so I tried to compute them via a constexpr. The checksum should be implemented here, but I would probably make a function for it.

The statemachine uses one "event", which is the reading of the Status Word. I would guess, that in a real application, the status word of the motor needs to be checked cyclically. So this is what happens (but only is used at the moment in the first transition). Then we have a number of guards and actions.

The state machine itself is simple and basically can be read as a transition table. For more Information, see [the official documentation](https://boost-ext.github.io/sml/index.html).

I added an error handler with throws on an unexpected event. I prefer my code to throw early on any unexpected things. A crash can be better then an undiscovered error that gives you headaches much later.

#### unit tests

I basically developed the state machine employing Test Driven Design. I used Catch2 v3 and trompeloeil. That is also why I had to template the state machine, so I could inject my mock in there. It has only compile time costs. A Class hierarchy would have had runtime costs.

The test cases are ugly but they serve the purpose.

Integrating the state machine into master was then quite easy.

### Building and testing

The cmake should download the required packages *if they are not installed*. So sml, Catch2 and trompeloeil will be downloaded. The later two are no issue, since they are only used for testing. The boost::sml is covered by the Boost Licence. For a proper build (i.e. embedded linux) one should not do it like this.


