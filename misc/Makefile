%: %.c
	gcc -Wall -O2 -o $@ $<

%.c: %.rl
	ragel -G2 $<

