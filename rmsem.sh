#!/bin/bash
for semid in $(ipcs -s | awk -v user=$(whoami) '$3 == user {print $2}'); do
    ipcrm -s $semid
done

