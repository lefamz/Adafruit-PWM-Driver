pwm-toradex: pwm-toradex.o
	g++ pwm-toradex.o -g -o pwm-toradex
pwm-toradex.o: pwm-toradex.cpp
	g++ pwm-toradex.cpp -c -g -Wall -pedantic
clean:
	rm *o pwm-toradex

