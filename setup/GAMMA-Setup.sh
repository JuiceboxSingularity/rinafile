ip link add link tap0 name tap0.101 type vlan id 101
ip link set dev tap0 up
ip link set dev tap0.101 up


modprobe shim-eth-vlan
modprobe rina-default-plugin
modprobe normal-ipcp
