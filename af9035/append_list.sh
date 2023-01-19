#!/bin/sh

FILE="$1"
HEADER="${FILE%.c}.h"

declare -a PACKETS
readarray -t PACKETS < <(sed -n 's@^.*\(pkt[0-9]*\).*$@\1@ p' "$FILE")

PACKETS_VAR='const unsigned char *packets[]'
PACKETS_NR_VAR='const unsigned int packets_nr'

cat >"$HEADER" <<EOF
#ifndef PACKET_HEADER
#define PACKET_HEADER

extern $PACKETS_VAR;
extern $PACKETS_NR_VAR;

#endif
EOF

exec >> $FILE

cat <<EOF
#include "$HEADER"

$PACKETS_VAR = {
EOF

for I in "${PACKETS[@]}"; do
	echo -e "\t$I,"
done

PACKETS_NR="${#PACKETS[@]}"

cat <<EOF
};

$PACKETS_NR_VAR = $PACKETS_NR;
EOF
