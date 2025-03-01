
本脚本用于自动化配置服务器网络接口、激活公网访问权限、设置路由规则，并实现网络故障自动切换功能。主要适用于需要双网络接口（主/备）的服务器环境，确保网络连接的高可用性。

---

## 二、核心功能
1. **网络接口配置**
   - 静态配置 `eth0` 为内网接口（IP: `172.22.146.150`）
   - 动态配置 `eth1` 为外网接口（通过 DHCP 获取 IP）

2. **公网访问激活**
   - 自动获取 `eth1` 的 IP 地址
   - 通过 HTTP 请求激活公网访问权限

3. **路由规则管理**
   - 配置 `10.16.0.0/8` 私有网络路由
   - 设置默认路由并验证配置正确性

4. **网络监控与切换**
   - 实时监测 `eth0` 的网络连通性
   - 当 `eth0` 不可用时自动切换至 `eth1`
   - 支持网络恢复后自动切回主接口

---

## 三、使用方法
### 1. 执行权限
```bash
sudo chmod +x network_config.sh
sudo ./yunweitask.sh
```

### 2. 日志查看
```bash
tail -f /var/log/network_configuration.log
```

---

## 四、配置说明
### 1. 接口配置参数
```bash
# /etc/network/interfaces 配置
auto eth0
iface eth0 inet static
    address 172.22.146.150   # 内网 IP
    netmask 255.255.255.0    # 子网掩码
    gateway 172.22.146.1     # 内网网关

auto eth1
iface eth1 inet dhcp       # 动态获取外网 IP
```

### 2. 公网激活服务
```bash
# 需要修改的公网激活 URL
url="http://192.168.202.2/?ip=$get_ip"
```

### 3. 路由规则
```bash
# 私有网络路由
ip route add 10.16.0.0/8 via 192.168.202.2

# 默认路由（主接口）
ip route add default via 172.22.146.1 dev eth0
```
