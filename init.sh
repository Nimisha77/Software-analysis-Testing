#! /bin/bash

docker run -d --privileged -v $(pwd)/src:/home/project/src --name=svfproject mdavis438/svfproject:latest tail -f /dev/null
run_status=$?

if [ $run_status -eq 0 ]; then
    echo "##### [svfproject] container started successfully"
else
    echo "##### Failed to start [svfproject] container. Exit status: $run_status"
    exit 1
fi

docker exec -it svfproject /bin/bash
exec_status=$?

if [ $exec_status -ne 0 ]; then
    echo "##### Failed to execute command in [svfproject] container. Exit status: $exec_status"
    exit 1
fi