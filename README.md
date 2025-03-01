# 2025 年红岩网校工作站运维安全部运维方向寒假考核

## 考核说明

### 截止时间

本次寒假考核分为运维和开发两个部分，各部分的截止时间如下：

- 运维部分： 2025 年 2 月 3 日 23:59
- 开发部分： 2025 年 2 月 26 日 23:59

完成运维部分后即可继续进行开发部分，无需等待运维部分截止。

### 提交方式

1. 在 Github 上 Fork 本仓库。
2. 在对应题目的文件夹下放入你的项目文件夹，项目文件夹以你的学号命名。运维部分提交到 Ops 目录下。
3. 建议在你 Fork 仓库的项目文件夹中完成开发。如果感觉不便，可以另外新建仓库进行开发，但请在你 Fork 仓库的项目文件夹中给出你新建仓库的链接。
4. 在开发部分和运维部分完成后分别向上游仓库（即本仓库）发起 Pull Request。因为本次考核并非实际的协作项目，因此在最后合并提交为一个 PR 即可。

## 运维部分

### 出题人

@张方瑞

### 背景

WuXuan 的 Linux 服务器现在物理连接到两个网络。

第一个网络：

- 对应接口 `eth0`。
- 需要手动设置静态 IP 为 `172.22.146.150`，网关为 `172.22.146.1` 后才能上网。


- 可以访问公网，但不可以访问内网网段（10.16.0.0/16）。

- 公网上行带宽为 100 Mbps，下行带宽为 1000 Mbps。

- 这个网络不稳定，随时都有可能被非物理阻断（即接口仍然为连接状态，但任何经过此网络的数据包都会被丢弃）。

第二个网络：

- 对应接口 `eth1`。

- 通过 DHCP 获取 IP 地址，这里假设我们获得到的 IP 地址为 10.16.32.254/17，网关为 10.16.0.1。

- 可以访问内网网段，但需要向 192.168.202.2 发送一条 Get 请求激活公网访问权限后才能访问公网。
- 公网上行带宽和下行带宽均为 50 Mbps。
- 这个网络非常稳定，我们这里假定它永远不会断连。

Get 请求的格式如下：

```
http://192.168.202.2/?ip={接口 eth1 当前的 IP}
```

例如：

```
http://192.168.202.2/?ip=10.20.13.112
```

### 任务

WuXuan 希望能够通过服务器连接的两条宽带实现同时访问公网和内网，并且在保证网络连接可靠性的前提下实现尽可能高的公网访问速率。请你编写一个或多个 Linux 下的 Bash 脚本完成以下要求：

1. **配置网络接口**：

	配置 `/etc/network/interfaces` 文件，为 `eth0` 设置静态 IP 为 `172.22.146.150`，网关为 `172.22.146.1`；`eth1` 通过 DHCP 获取 IP 地址。

2. **发送激活请求**：

	自动获取 `eth1` 接口当前的 IP 地址，构造并发送 Get 请求以激活公网访问权限。

3. **配置路由规则**：

	配置路由规则，通过 `eth0` 访问公网，通过 `eth1` 访问内网网段（10.16.0.0/16）。

4. **可用性检测与自动切换**：
	每分钟检测一次 `eth0` 是否能够正常访问公网（检测方法不限，例如 `ping 8.8.8.8`）。如果 `eth0` 无法访问公网，则自动将默认路由切换到 `eth1`，并在 `eth0` 恢复时自动切换回 `eth0`。

### P.S.

在脚本编写的过程中，合理添加日志记录是非常重要的，它能够有效地帮助我们排查错误和定位问题。

因此，请在你的脚本中加入适当的日志记录机制。

## 开发部分

### 开发部分说明

- 以下三个题目**任选其一完成**即可。

- 如对题目有疑问，请联系出题人。

- 更多信息参见最后的备注。

### 开发部分要求

1. 你的项目**必须要有 Markdown 文档**，其中应当包含对你的程序能够做哪些工作、如何使用你的程序、哪些功能还没有实现、有哪些 bug 等的说明。对于项目文档的排版不做强制要求，但应当符合 Markdown 语法要求，同时推荐遵循 [《中文文案排版指北》](https://github.com/sparanoid/chinese-copywriting-guidelines)。
2. 你的项目**需要使用 Git 进行版本控制**，即在你 Fork 的本次寒假考核仓库中进行开发。如果感觉开发不便，也可以单独建立仓库进行开发，但需要在你 Fork 的本仓库的相应位置给出仓库链接。对于 Git Commit 消息格式不做强制要求，但推荐遵循 [约定式提交规范](https://www.conventionalcommits.org/zh-hans/v1.0.0/) 或 [Gitmoji 提交规范](https://gitmoji.dev/)。
3. **开发部分的第2、3题需要使用 Docker 进行容器化并给出 Dockerfile 文件**，通过配置文件或环境变量传入必要的配置信息即可直接使用。同时强烈建议将生成的容器镜像上传至任意公共容器镜像仓库（例如 DockerHub、Github Container Registry、Gitlab Container Registry 等）或可访问的自建容器镜像仓库（如 Harbor、Gitea Container Registry 等）并提供相应链接。如果因为网络等问题确实无法将容器镜像上传至容器镜像仓库，则至少需要提供关于容器构建和使用过程的图文说明。

### 1. Compose 增强计划

#### 出题人

@张方瑞

#### 背景

随着微服务架构的流行，使用 Docker 和 Docker Compose 来管理和编排多个容器变得越来越普遍。然而，手动管理这些容器及其配置（如启动、停止、更新等）既耗时又容易出错。为了提高效率和自动化程度，本题目要求实现一个简化版的 Docker Compose 工具，并在此基础上增加自动更新镜像以及自建镜像仓库的功能。

#### 任务

##### Level 0

**实现简化版的 Docker Compose 工具**

使用任意语言（推荐 Python 和 Go）实现 Docker Compose 的基础功能，即根据读取的 YAML 配置文件启动容器。

要求：至少要能识别 Docker-Compose 文件中的 `volumes`，`ports`，`environment`，`container_name` 和 `image` 字段，并能成功启动和停止容器。

##### Level 1

**对镜像进行自动更新**

在 Docker-Compose 文件原有的基础结构上，增加 Bool 类型的 `auto_update` 字段。

对于 `auto_update` 字段为 `True` 的容器，每 12 个小时检测一次所使用的镜像是否有新版本。如果发现新版本镜像，就修改对应的 Compose 文件的 `image` 字段，使容器镜像的 Tag 与最新版本一致。

为了保证服务的正常运行，这里无需停止旧容器，只需要更新对应 Compose 文件的 `image` 字段即可。

这里假设使用的容器镜像均来自 DockerHub。可以通过以下链接查询托管在 DockerHub 上的容器镜像的 tag：

```
https://registry.hub.docker.com/v2/repositories/{namespace}/{repository}/tags
```

- namespace: 镜像的所有者或组织名。对于官方镜像（如 `alpine`），这部分是 `library`。
- repository: 镜像的名称。

##### Level 2

**代理？不需要啦**

经常拉取镜像的同学都知道，拉取镜像是真的烧代理流量，尤其是有多台服务器的情况下，这时候自建的镜像仓库就很香了。

首先，你需要自己搭建一个容器镜像仓库（例如 Harbor 和 Gitea Container Registry）。

在一个新的 Compose 项目启动后，先从 DockerHub 上拉取所需的容器镜像（假设使用的镜像为 DockerHub 上的镜像），再修改镜像名称并将镜像推送到自己搭建的容器镜像仓库中，然后更新原始 Compose 文件中的 `image` 字段，将镜像源改为自建的容器镜像仓库。最后再启动容器。这样一来，即使之后代理出现了问题，也不会影响到我们服务的启动了。

#### 加分项

1. 在 Level 1 部分实现尽可能多的功能，如 `networks`，`logging`，`healthcheck`，`depends_on` 等等。

2. 日志很重要！日志很重要！！日志很重要！！！重要的事情说三遍。

#### 要求

**至少完成 Level 0 和 Level 1**。

#### 参考文档

- [Develop with Docker Engine SDKs](https://docs.docker.com/reference/api/engine/sdk/)

- [PyYAML Documentation](https://pyyaml.org/wiki/PyYAMLDocumentation)

- [Harbor Installation and Configuration](https://goharbor.io/docs/2.12.0/install-config/)

### 2. 你服务器的出口带宽怎么尖尖的

#### 出题人

@程远硕

#### 背景

众所周知，对于一个公开服务而言，一旦上线，就随时存在着被攻击的风险。

现在，几乎所有公有云厂商都提供了针对云服务器实例的网络攻击检测和通知服务。然而，从灵活性和可控性等方面考虑，自己实现一个网络攻击检测、通知和处理工具仍然是有必要的。

而自动化，一直都是运维工作中的重中之重。

Cysnies 希望你能够运用所学的相关知识，编写一个能够检测服务器异常流量并及时通知和自动处理的工具，以提高运维效率和服务可靠性。

#### 任务

##### Level 0

编写一个工具，能够根据可配置的指标（例如恶意 IP、请求次数、单个 IP 在一定时间内的流量等）和阈值持续检测和记录异常流量信息，同时在检测到异常流量后通过 iptables 或其他防火墙工具在一定时间内封禁来自恶意 IP 的请求，从而缓解攻击。

##### Level 1

通过 SMTP 等方式，在检测到异常流量后自动通过邮件通知特定人员。通知邮件中应当包含必要的信息，包括但不限于时间、恶意 IP、请求次数、一定时间段内的流量、处理措施等观测指标。

##### Level 2

以 ChatBot（聊天机器人）的形式在任意即时通讯平台（包括但不限于 QQ、微信、飞书、钉钉、Telegram、Discord、Matrix）实现通知功能，通知内容与 Level 1 相同。你可以使用飞书、钉钉等平台提供的 Webhook 通知推送能力，也可以使用开源的聊天机器人框架（例如 Nonebot 和 Koishi.js）进行开发以实现更复杂的逻辑。

##### Level 3

在 Level 2 的基础上，让你的机器人能够响应来自即时通讯平台用户的操作（例如通过在群聊中@机器人发送特定的指令来取消对某个 IP 的封禁状态），而不只是单方面推送信息。具体实现的功能不限，但必须与本次开发的主题相关。最终实现的功能越丰富越好。

##### Level 4

结合 Uptime Kuma 等服务可用性检测工具，在检测和处理恶意流量的同时持续观测服务可用性，并在服务不可用时通过邮件或即时通讯平台进行通知告警。

#### 加分项

1. 在 Level 3 部分实现丰富多样的功能，但实现的功能必须与本次开发的主题相关。
2. 以多样的形式（例如 HTML 邮件、各个观测指标的统计图表等）展示 IP、请求数、流量等指标。此外，也可以利用即时通讯平台的原生能力（例如 Matrix 的 Markdown 消息和飞书的卡片消息），让你的通知告警更加优雅美观。
3. 可以尝试对接 LLM 厂商（例如通义大模型、豆包大模型、DeepSeek 等）提供的 API 接口，让你的 Bot 更加智能化。

#### 要求

**至少完成 Level 0**。

#### 参考项目和文档

[The netfilter.org project](https://www.iptables.org/)

[MailDev](https://github.com/maildev/maildev)

[Mailpit](https://github.com/axllent/mailpit)

[NoneBot](https://nonebot.dev/)

[Koishi.js](https://koishi.chat/zh-CN/)

[飞书开发文档](https://open.feishu.cn/document/home/index)

[钉钉开发文档](https://open.dingtalk.com/document/)

[Uptime Kuma](https://github.com/louislam/uptime-kuma)

[火山方舟大模型服务平台文档](https://www.volcengine.com/docs/82379/1399009)

[DeepSeek API Docs](https://api-docs.deepseek.com/zh-cn/)

[通义千问 API 参考](https://help.aliyun.com/zh/model-studio/developer-reference/use-qwen-by-calling-api?spm=a2c4g.11186623.0.i1)

### 3. Cysnies 的造轮子计划

#### 出题人

@程远硕

#### 背景

在本学期的课程中，我们接触了 Linux 系统，并简单了解了一些常用的命令行工具。但对于一名合格的 Redrock Ops 而言，造轮子（自己开发底层工具和库）的能力是必不可少的。为了大家能够深入了解操作系统并提高开发能力，Cysnies 想让你帮他写点东西。

#### 任务

##### Level 0

编写一个 Linux 系统下的类 top 工具，实现对进程和系统资源的实时监控。你的程序应当能够**展示当前的 CPU 占用率、内存占用以及 Swap 交换分区占用等基本信息，同时能够展示当前系统中的进程列表、进程树以及每一个进程的详细信息（PID、USER、PRI、NI、TIME、Command 以及进程的 CPU 和内存占用情况），并支持通过工具直接进行进程操作（如杀死进程和改变进程优先级等）**。

不限编程语言，但要求至少实现上述列举的命令和功能。

##### Level 1

使用 POSIX API，编写一个 Linux 下的 Shell 程序（注意不是 Shell 脚本）。你的 Shell 应当实现 Shell 的基本功能，包括但不限于**读取用户输入、运行程序、使用内建基本命令（cd、pwd、echo、mkdir 和 exit 等）以及输入/输出重定向（ >, < 和 >> 等）和管道（|）**。

不限编程语言（但推荐使用 C、C++ 或 Rust），也可以使用二次封装的 POSIX API，但要求至少实现上述列举的命令和功能。

#### 加分项

对于 Level 0，在完成题目基本要求的基础上，你还可以尝试增加进程自定义排序、进程过滤、网络和磁盘 I/O 监控、颜色编码和主题支持、图表展示等功能。

对于 Level 1，在完成题目基本要求的基础上，你还可以为你的 Shell 添加诸如彩色高亮、查看和切换命令历史以及命令补全等功能。

#### 要求

**至少完成两个 Level 中的任意一个**。

#### 参考项目和文档

[htop](https://github.com/htop-dev/htop)

[btop](https://github.com/aristocratos/btop)

[CSAPP Shell Lab README](http://csapp.cs.cmu.edu/3e/README-shlab)

[CSAPP Shell Lab Writeup](http://csapp.cs.cmu.edu/3e/shlab.pdf)

[Oh My Zsh](https://github.com/ohmyzsh/ohmyzsh)

[fish](https://github.com/fish-shell/fish-shell)

## 备注

1. 开发部分使用的编程语言不限，鼓励大家利用寒假自学编程语言。
2. 题目中给出的参考项目和参考文档仅供参考，请不要照抄参考实现。
3. 请尽量完成每道题的基础要求。截止时即使没有完成，也请提交到你 Fork 的仓库并发起 Pull Request，做到多少算多少。

4. 不要照抄代码。大家都会使用 LLM Tools。你可以用来查资料、找思路、学习参考实现或辅助 Debug，但务必不要照搬 LLM 生成的代码，这对你的成长没有任何帮助。但可以在理解 LLM 生成的代码后改进或重写，就是学习了。

5. 不要拖，当鸽子要有能当的了鸽子的觉悟。
6. 尽最大努力去做，实在不会的知识可以在线上问我们，我们都很乐意解答问题。
7. 不要照抄他人和其他已有项目的代码！！！一经发现，后果自负。
