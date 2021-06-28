#!/bin/bash
echo 1 = "$1"
if [ "$1" == "0" ]; then
        echo "set to 0 = text mode\n"
        cp /etc/default/grub_text /etc/default/grub && sudo systemctl set-default multi-user
        echo "set ok\n"
elif [ "$1" == "1" ]; then
        echo "set to 1 = graphical mode\n"
        cp /etc/default/grub_graphical /etc/default/grub && sudo systemctl set-default graphical
        echo "set ok\n"
else
        echo "please input mode 0 = text, 1 = graphical\n"
        echo "set faild\n"
fi
