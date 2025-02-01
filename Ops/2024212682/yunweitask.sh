#!/bin/bash
LOG_FILE="/var/log/network_configuration.log"
log_message() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOG_FILE"
}
log_message "正在配置网络接口..."

cat << EOF > /etc/network/interfaces
auto eth0
iface eth0 inet static
    address 172.22.146.150
    netmask 255.255.255.0
    gateway 172.22.146.1

auto eth1
iface eth1 inet dhcp
EOF
log_message "网络接口配置成功。"
log_message "正在激活 eth1 的公网访问权限..."
get_ip=$(ip addr show eth1 | grep -oP '(?<=inet\s)\d+(\.\d+){3}')
if [ -z "$get_ip" ]; then
    log_message "无法获取 eth1 的 IP 地址。"
    exit 1
fi
url="http://192.168.202.2/?ip=$get_ip"
response=$(curl -s -o /dev/null -w "%{http_code}" -X GET "$url")
if [ "$response" != "200" ]; then
    log_message "无法激活公网访问权限，IP 地址：$get_ip。"
    exit 1
fi
log_message "公网访问权限已激活，IP 地址：$get_ip。"
log_message "正在配置路由规则..."
route add -net 10.16.0.0 netmask 255.0.0.0 gw 192.168.202.2
route add default gw 172.22.146.1
if ! ip route show | grep -q "default via 172.22.146.1 dev eth0"; then
    log_message "路由规则设置不正确。"
    exit 1
fi
log_message "路由规则配置成功。"
log_message "开始进行网络可用性检查和自动切换..."
while true; do
    if ping -c 1 -W 1 8.8.8.8 > /dev/null; then
        log_message "eth0 正常，正在使用 eth0 访问互联网。"
    else
        log_message "eth0 不可用，尝试切换到 eth1 访问互联网。"
        # 删除默认网关到 eth0 的路由
        route del default
        # 添加默认网关到 eth1 的路由
        route add default gw 192.168.202.2
        if [ $? -ne 0 ]; then
            log_message "切换到 eth1 访问互联网失败。"
            exit 1
        fi
        log_message "已切换到 eth1 访问互联网。"
    fi
    sleep 60
done
