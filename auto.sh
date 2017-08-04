#!/bin/bash

while true
	do
		./main
		if [ $? -ne 0 ]
		then
			break
		fi
		sleep 1
	done

