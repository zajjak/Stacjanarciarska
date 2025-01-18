#!/bin/bash
for semid in $(ipcs -q | awk -v user=$(whoami) '$3 == user {print $2}'); do
    ipcrm -q $semid
done
