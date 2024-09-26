#!/bin/bash
cd /home
chmod +x simple_client
while [ true ]
do
  nohup ./simple_client &
  #sleep $(( RANDOM % 15 + 1 ))
  sleep 300
  echo " client side "
done
