#!/bin/bash
for semid in $(ipcs -m | awk -v user=$(whoami) '$3 == user {print $2}'); do
    ipcrm -m $semid
done
