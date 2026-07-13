A compilation of tests that I have written to prove that the emulator works as expected.

May be done in a similar fashion to `Octo` from `JohnEarnest` (GitHub): https://github.com/JohnEarnest/Octo/tree/gh-pages/examples/tests

- `test_opcode.ch8`:: SUCCESS
    ![SUCCESS!](image.png)
- `random_number_test.ch8`:: SUCCESS
  - A simple CHIP-8 program used to demonstrate the functionality of the random number generator. When any key is pressed, a random number between 0 and 255 is generated and shown on screen.
- `delay_timer_test.ch8`:: SUCCESS
  - Tests the `Fx0A` (wait for keypress), `Fx15` (set delay timer), and `Fx07` (read delay timer) instructions, along with the automatic 60Hz decrement of the delay timer.
  - Pressing `2` increments it, pressing `8` decrements, and `5` causes a countdown.