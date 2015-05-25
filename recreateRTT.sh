#!/bin/bash
##
# BY ALAN LIN
#INITIAL TEST SCRIPT TO DIFFERENTIATE CAPTURES FROM DIFFERENT FLOWS
#
#CURRENT SCRIPT ONLY EXTRACTS TWO FLOWS OUT OF AGGREGATE TRACE
#CAN BE EASILY MODIFIED TO EXTRACT MORE
#
#FIRST PORTION SHIRTS THE NODEID UP BY ONE TO FIX OFF BY ONE ISSUE FROM OUTPUT
#
#######################################################

#REMOTE FILE SHARING NAME TO THE OUTPUT FILES OF THIS SCRIPT
rm rtt.trace rtt_a.trace rtt_b.trace


#PART IN DELTA AND ESTIMATED RTT OUTPUT FROM SIMULATION RUN
A=delta.trace
B=estRtt.trace


#CORRECTION TO OUTPUT FILE
#SHIFT NODE COLUMN UP BY 1 to correct off-by-1 issue with estRTT output
paste $B | awk 'NR>1{print $1}' >> node.trace
#GENERATE A LIST OF JUST NODEID (REMOVING THE FIRST ENTRY) 
C=node.trace
#CREATE TEMP EST RTT TRACE WITH NEW NODEID LINEUP
paste $C $B | awk '{print $1, $3, $4}' >> estRtt_temp.trace
#CREATE NEW TEMP DELTA (REMOVED FIRST TWO ENTRY - DOESN'T HAVE CORRESPONDING RTT ENTRY) -- THIS STEP MAY NOT BE NECESSARY
paste $A | awk 'NR>2{print $1, $2, $3}' >> delta_temp.trace


D=estRtt_temp.trace
E=delta_temp.trace


##
##
# Simular to sql join query -- key used: NodeID and Time Stamp
#
#join on send field of first file, and 2nd of second file (TIME STAMP) and NODEID ()
join -1 2 -2 2 $D $E  | awk '{ if ($2==$4) printf("%s %s %s %s %s\n", $1, $2, $3, $4, $5) }'>> rtt.trace

#EXTRACT INFORMATION BASED ON NODE ID
F=rtt.trace
paste $F |awk '{ if ($2==2) printf("%s %s %s %s\n", $2, $1, $3, $5) }'>> rtt_a.trace
paste $F |awk '{ if ($2==3) printf("%s %s %s %s\n", $2, $1, $3, $5) }'>> rtt_b.trace

#USED EST RTT ANB DELTA TO REGENERATE ACTUAL RTT
#paste $A $B | awk '{print $1, $2, ($1 +$2)}'
#paste $A $B | awk '{print ($1 +$2)}' >> rtt.trace

#DELETE TEMP FILES
rm node.trace estRtt_temp.trace delta_temp.trace




