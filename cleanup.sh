#! /bin/bash

echo "##### Stopping existing [svfproject] container #####\n"

docker stop svfproject
stop_status=$?

if [ $stop_status -eq 0 ]; then
    echo "\n##### [svfproject] container stopped successfully"
else
    echo "\n##### Failed to stop [svfproject] container. Exit status: $stop_status"
    exit 1
fi

echo "\n##### Deleting existing [svfproject] container #####"

docker rm svfproject
rm_status=$?

if [ $rm_status -eq 0 ]; then
    echo "##### [svfproject] container deleted successfully"
else
    echo "##### Failed to delete [svfproject] container. Exit status: $rm_status"
    exit 1
fi
