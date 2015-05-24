#!/bin/bash

if [ $# -eq * ]; then
        echo "no arguments supplied"
        echo "Number of flows  <arg1>"
        echo "Default number of flows: 1"
	FLOWS=1

else
	FLOWS=$1
fi

COUNTER=*
TRACE_NODE=2
let NEW_TRACE_NODE=TRACE_NODE+2

echo $TRACE_NODE  $NEW_TRACE_NODE

while [ $COUNTER -lt $FLOWS ];
do
	echo "RUN #1"
	echo sed -i "s/\/NodeList\/$TRACE_NODE\/\$ns3::TcpL4Protocol\/SocketList\/*\/realRTT/\/NodeList\/$NEW_TRACE_NODE\/\$ns3::TcpL4Protocol\/SocketList\/*\/realRTT/g" ./scratch/dumbbell_diff_flow.cc

	./waf --run "dumbbell_diff_flow --data=5** --duration=3* --num_flows=${FLOWS} --estrtt_tr_name=estRTT.trace --rrtt_tr_name=realRTT.trace --delta_tr_name=delta.trace"

	sed -i "s/\/NodeList\/$TRACE_NODE\/\$ns3::TcpL4Protocol\/SocketList\/*\/realRTT/\/NodeList\/$NEW_TRACE_NODE\/\$ns3::TcpL4Protocol\/SocketList\/*\/realRTT/g" ./scratch/dumbbell_diff_flow.cc

	let COUNTER=COUNTER+1
	let TRACE_NODE=NEW_TRACE_NODE
	let NEW_TRACE_NODE=TRACE_NODE+2

done


echo "Runs Completed"
echo "Rollback code modification"

#sed -i 's/\/NodeList\/4\/\$ns3::TcpL4Protocol\/SocketList\/*\/realRTT/\/NodeList\/2\/\$ns3::TcpL4Protocol\/SocketList\/*\/realRTT/g' ./scratch/dumbbell_diff_flow.cc
