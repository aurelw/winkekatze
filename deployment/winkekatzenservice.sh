#!/bin/bash

### BEGIN INIT INFO
# Provides:          craftui
# Required-Start:    $local_fs $network
# Required-Stop:     $local_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: start script for devlock locksyste.
# Description:       start script for devlock locksyste.
### END INIT INFO

USER="pi"
WINKEKATZE_ROOT="/home/$USER/packages/winkekatze/"

MQTT_BROKER="test.mosquitto.org"

case "$1" in

    start)

        # start craftui and clients
        su $USER <<EOF
        # start winkekatze
        cd $WINKEKATZE_ROOT
        echo "Starting winkekatze...."
        screen -S winkekatze -d -m bash -c "./winkekatze_mqtt.py --host $MQTT_BROKER"
EOF

    ;;

    stop)
        if [ "`pgrep winkekatze`" ] 
        then
            echo "Stop winkekatze"
            kill -9 `pgrep winkekatze`
        fi

    ;;

esac

exit 0

