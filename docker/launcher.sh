#!/bin/bash

_term() {
  echo "Caught SIGTERM signal!"
  kill -TERM "$child"
}

trap _term SIGTERM

env

./dns_replace.sh
cat /srsLTE/build/srsenb/src/enb.conf
./srsLTE/build/srsenb/src/srsenb /srsLTE/build/srsenb/src/enb.conf &

child=$!

wait "$child"
