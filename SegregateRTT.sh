#########################################################################
# AUTHOR: ALAN LIN
#########################################################################
# DESCRIPTION: SIMPLE ONE LINER CALLED ON AND MODIFIED BY recreateRTT.sh
#              TO EXTRACT TRACE INFORMATION CORRESPONDING TO TRACE ID
#
########################################################################

#!/bin/bash
E=agg_temp.trace

#recreateRTT.sh WILL ADD AND ITEREATE ID NUMBER OF NODE
#IN THE IF CONDITION SUCH THAT IT WILL LOOK LIKE "2==NODE_ID"
paste $E | awk '{if ($2==)print $1, $2, $3, $4}'

