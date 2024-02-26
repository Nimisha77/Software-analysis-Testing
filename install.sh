#! /bin/bash

docker pull mdavis438/svfproject:latest
pull_status=$?

if [ $pull_status -eq 0 ]; then
    echo "##### Image mdavis438/svfproject:latest pulled successfully"
else
    echo "##### Failed to pull image mdavis438/svfproject:latest. Exit status: $pull_status"
    exit 1
fi
