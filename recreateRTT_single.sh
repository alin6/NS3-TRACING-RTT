#!/bin/bash

A=wireless.rtt
B=wireless.delta

paste $A $B | awk '{print $2, $4, ($2 +$4)}'


paste $A $B | awk '{print ($2)}' >> est_rtt.trace
paste $A $B | awk '{print ($4)}' >> delta.trace
paste $A $B | awk '{print ($2 +$4)}' >> rtt.trace

