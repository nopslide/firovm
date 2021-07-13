# Script to generate a new block every 15 seconds
# Put this script at the root of your unpacked folder
#!/bin/bash

echo "Generating a block every 15 seconds. Press [CTRL+C] to stop.."

address=`firovm-cli getnewaddress`

while :
do
        echo "Generate a new block `date '+%d/%m/%Y %H:%M:%S'`"
        firovm-cli generatetoaddress 1 $address
        sleep 15
done