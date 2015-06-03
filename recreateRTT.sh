#!/bin/bash
##########################################################################################################################
# AUTHOR: ALAN LIN
# MOTIVATIONl: L4 TRACE OUTPUTS OF OF CURRENT NS3 IMPLEMENTATION PIPES ALL TRACES
#              INTO A SINGLE FILE REGARDLESS OF THE NUMBER OF DIFFERENCE TRACE SOURCES
#
#
# DESCRIPTION: THIS FILE RELIES ON THE MODIFIED NS3 FILES IN THIS PROJECT TO EXTRACT
#              INFORMATION SUCH AS THE DELTA AND NODE_ID CORRESPONDING TO RTT TRACES
#              USING THE EXTRACTING ID AND TIMESTAMPS, WE WILL SPLIT UP THE TRACES
#              SUCH THAT EACH NODE WILL HAVE THEIR INDIVIDIAL TRACE FILES
#
# OUTPUT: node<_ID>.estRTT     node<_ID>.realRTT node<_ID>.delta
#
# ARGUMENTS: <number of flows> <estimated_rtt_filename> <rtt_delta_filename> <First Source Node's ID number>
# USAGE: ./recreateRTT.sh 3 dummy.rtt dummy.delta 2
#
# NOTE: MUST BE IN SAME FOLDER AS "SegregateRTT.SH" WHICH CONTAINS A ONE LINER SCRIPT THAT THIS FILE
#       MODIFY TO ITERATE THROUGH NODE ID AND EXTRACT CORRESPONDING DATA
#
#  CONTENT IN "SegregateRTT.SH" :
#     > #!/bin/bash
#     > E=agg_temp.trace
#     > paste $E | awk '{if ($2==)print $1, $2, $3, $4}'
#
#########################################################################################################################

#CHECK FOR THE RIGHT NUMBER OF ARGUMENTS
if [ $# -lt 4 ]; then
        echo no arguments supplied
        echo "arugments: <number of flows> <estimated_rtt_filename> <rtt_delta_filename> <First Source Node's ID number> "

else
  echo "Number of Flows: $1"
  echo "Estimate RTT input file: $2"
  echo "Delta input file: $3"
  echo "ID of First Source Node: $4"
  echo "============================================================="
  FLOWS=$1
  A=$2
  B=$3


  ########################################################
  # SPLIT AND COMPUTER REAL RTT, ESTRTT, DELTA FOR
  # TO CORRECT FORMAT FOR SENSE - STANDALONE PROGRAM
  ########################################################
  echo "SINGLE FLOW"
  echo "Fix the 1-off node ID in wireless RTT file "
  # NODE ID IN OUTPUT OF RTT ESTIMATE ARE 1-OFF, INVESTEGATED AND VALIDATED
  # WITH OUTPUT OF dumbbell_diff_flows.cc.
  ################################################################
  # EXTRA NODE ID REMOVING FIRST ENTRY
  paste $A | awk 'NR>1{print $1}' >> node_ID.trace
  C=node_ID.trace

  ################################################################
  # CREATE TEMP EST RTT TRACE WITH NEW NODE ID LINEUP
  # MERGE NODE ID BACK WITH (EST)RTT OUTPUT
  # FIRST ENTRY OF ALL (EST)RTT OUTPUT DO NO HAVE CORRESPONDING DELTA
  # REMOVE FIRST ENTRY TO LINE UP FOR CALCULATION
  paste $C $A | awk 'NR>1{print $1, $3, $4}' >> estRtt_temp.trace
  D=estRtt_temp.trace


  if [ "$1" -lt 2 ]; then

    # INITIAL TRACES COMES WITH TIMESTAMPS THAT ARE NOT SUPPORT BY
    # THE SENSE STANDALONE PROGRAM
    # EXTRACT EST RTT
    paste $D $B| awk '{print ($3)}' >> node.estRTT
    # EXTRACT DELTA
    paste $D $B | awk '{print ($5)}' >> node.delta
    # EXTRACT REAL RTT
    paste $D $B | awk '{print ($3 +$5)}' >> node.realRTT

  else
    echo "Multiflows"
    ############################################################################
    # SOME ENTRIES FROM TWO DIFFERENT NODES SHARE SAME TIME STAMPS THUS THE TROUBLE
    # OF ADDING NODE ID's NODE
    #########################
    # JOIN NEW (SHIFTED) EST RTT AND THE DELTA INPUT FILE USING TIMESTAMPS
    # REMOVE ANY POTENTIAL UNWANTED DUPLICATES AS RESULT OF JOIN USING
    # NODE ID AND TIME STAMP AS KEY
    join -1 2 -2 1 $D $B |  awk '!x[$1,$2]++' >> agg_temp.trace

    E=agg_temp.trace

    ############################################################################
    # SINCE ALL THE TRACES ARE STILL IN A SINGLE FILE AT THIS POINT
    # WE WILL NEED TO SEPARATE THEM INTO DIFFERENT FILES FOR EACH FLOW AND
    # EACH OF THE ESTRTT, REALRTT, DELTA
    ############################################################################
    # A seperate script ./SegregateRTT.sh is used to allow for this script
    # to modify a field in the awk one liner such that we can iterate through
    # the node output by incrementing and replacing the node ID
    ############################################################################
    COUNTER=0
    NODE_ID=$4
    PREV_NODE_ID=0
    # LOOP FOR ITERATING THROUHG NUMBER FOR FLOWS
    while [ $COUNTER -lt $FLOWS ];
    do
      ###################################################################################
	    #SED IS USED TO MODIFY SCRIPT FOR AWK TO OUTPUT NEW FILE FOR DIFFERENT NODES
      #IT CHANGES THE NODE ID IN THE CONDITION FOR AWK SO WE CAN EXTRACK THE
      # DIFFERENT TRACES
      ###################
      if [ "$COUNTER" -lt 1 ]; then
        #NO NODE ID IS SPECIFIED IN THE SCRIPT AT THE BEGINNING
        #WE MUST SPECIFY IT AT THE FIRST ITERACTION OF LOOP
        sed -i "s/2==/2==$NODE_ID/g" './SegregateRTT.sh'
      else
        #REPLACE NODE ID WITH THE NEW ID TO BE PROCESSED
        sed -i "s/2==$PREV_NODE_ID/2==$NODE_ID/g" './SegregateRTT.sh'
      fi
      #RUN SCRITP TO GENERATE PER_NODE TRACES
      echo "Running SegregateRTT.sh to extract traces for node $NODE_ID"
      ./SegregateRTT.sh >> node_$NODE_ID.temp_info
      # EXTRACT EST RTT
      paste node_$NODE_ID.temp_info | awk '{print $3}' >> node_$NODE_ID.estRTT
      # EXTRACT DELTA
      paste node_$NODE_ID.temp_info | awk '{print $4}' >> node_$NODE_ID.delta
      # EXTRACT REAL RTT
      paste node_$NODE_ID.temp_info | awk '{print ($3 + $4)}' >> node_$NODE_ID.realRTT

      let PREV_NODE_ID=NODE_ID
      let NODE_ID=NODE_ID+1
      let COUNTER=COUNTER+1
    done
    # UNDO CHANGES TO THE AWK SCRIPT TO PREP FOR FUTURE USAGE
    echo "REVERT SCRIPT BACK TO ORIGINAL FORM"
    echo sed -i "s/2==$PREV_NODE_ID/2==/g" './SegregateRTT.sh'
    sed -i "s/2==$PREV_NODE_ID/2==/g" './SegregateRTT.sh'
  fi

  #CLEAN UP ALL THE TEMP FILES THAT WERE CREATED IN THIS PROCESS
  echo rm *.temp_info estRtt_temp.trace agg_temp.trace node_ID.trace
  rm *.temp_info estRtt_temp.trace agg_temp.trace node_ID.trace

fi
