简化版的 Docker Compose 工具
一、项目概述
本项目是一个使用 C 语言实现的简化版 Docker Compose 工具，旨在提高容器管理的效率和自动化程度。它具备启动和停止容器的基础功能，同时支持镜像的自动更新以及与自建镜像仓库的集成。由于我本地环境一直配置不好，代码可能存在我自己都不知道的问题，甚至无法运行，目前应该实现了level 0和level 1，level 2暂时无法完成了。
二、程序功能
1. Level 0：简化版 Docker Compose 工具
能够读取 Docker-Compose 文件，识别其中的 volumes、ports、environment、container_name 和 image 字段。
根据读取的配置信息启动和停止容器。
2. Level 1：镜像自动更新
在 Docker-Compose 文件中增加 auto_update 字段，支持布尔类型。
对于 auto_update 为 True 的容器，每 12 小时检测一次所使用的镜像是否有新版本。
若发现新版本，更新对应的 Compose 文件的 image 字段，使容器镜像的 Tag 与最新版本一致。

以下是添加了代码解释的 Windows 版本 README 文件：

---

# Docker 容器自动化管理工具（Windows 版本）

## 项目描述
本工具使用 C 语言开发，通过解析 `docker-compose.yml` 配置文件实现 Docker 容器的自动化管理，支持容器启动、停止、镜像更新检测及自动更新功能。

---

## 代码功能解释

### 1. 核心结构体定义
```c
typedef struct {
    char* image;          // 镜像名称
    char* container_name; // 容器名称
    char** volumes;       // 卷挂载配置
    int volumes_count;    // 卷数量
    char** ports;         // 端口映射配置
    int ports_count;      // 端口数量
    char** environment;   // 环境变量配置
    int environment_count;// 环境变量数量
    int auto_update;      // 自动更新开关（1为开启）
} ContainerConfig;
```
- **作用**：存储从 YAML 配置文件中解析的容器信息。
- **关键字段**：
  - `image`：指定容器使用的镜像。
  - `auto_update`：控制是否启用自动更新功能。

---

### 2. 核心函数说明

#### （1）`parse_yaml` 函数
```c
void parse_yaml(const char* filename, ContainerConfig* config);
```
- **功能**：解析 `docker-compose.yml` 文件，填充 `ContainerConfig` 结构体。
- **实现逻辑**：
  1. 使用 YAML 解析库读取文件内容。
  2. 遍历 YAML 键值对，将配置项映射到结构体字段。
  3. 处理数组类型配置（volumes/ports/environment），动态分配内存存储。

#### （2）`start_container` 函数
```c
void start_container(ContainerConfig* config);
```
- **功能**：根据配置生成并执行 `docker run` 命令。
- **实现逻辑**：
  1. 初始化命令字符串（`docker run -d`）。
  2. 依次添加容器名称、卷挂载、端口映射、环境变量等参数。
  3. 调用 `system` 执行命令。

#### （3）`check_image_update` 函数
```c
int check_image_update(const char* image);
```
- **功能**：检查 Docker Hub 上的镜像是否有新版本。
- **实现逻辑**：
  1. 解析镜像名称（`namespace/repository:tag`）。
  2. 调用 Docker Hub API 获取标签列表。
  3. 检查响应中是否包含 `latest` 标签（当前实现）。

#### （4）`schedule_image_update` 函数
```c
void schedule_image_update(ContainerConfig* config, const char* filename);
```
- **功能**：定时检查镜像更新，更新配置文件。
- **实现逻辑**：
  1. 每 12 小时检查一次（`Sleep(60*1000)` 实现循环）。
  2. 若发现更新，修改配置文件中的镜像标签为 `latest`。
  3. 覆盖原配置文件并更新结构体数据。

---

## 功能模块
1. **YAML 配置解析**  
   - 读取 `docker-compose.yml` 中的容器配置信息（镜像、卷挂载、端口映射、环境变量等）。
   - 支持自动更新开关（`auto_update`）。

2. **容器生命周期管理**  
   - 启动容器：根据配置生成并执行 `docker run` 命令。
   - 停止容器：通过 `docker stop` 终止指定容器。

3. **镜像更新检测**  
   - 定期调用 Docker Hub API 检查镜像是否有新版本。
   - 支持自动更新配置文件中的镜像标签。

---

## 依赖环境
1. **Docker Desktop**  
   - 下载地址：[Docker Desktop for Windows](https://www.docker.com/products/docker-desktop)
   - 安装后需确保 Docker 服务已启动。

2. **编译工具与库**  
   - **MinGW-w64**（用于编译 C 代码）：
     - 下载地址：[MinGW-w64](https://sourceforge.net/projects/mingw-w64/)
     - 安装时选择 `posix` 线程模型和 `seh` 异常处理。
   - **依赖库**：
     ```bash
     mingw32-make install libyaml libcurl
     ```

---

## 使用方法
### 1. 配置文件准备
在项目根目录创建 `docker-compose.yml`，示例内容：
```yaml
image: username/repo:tag
container_name: my_container
volumes:
  - ./data:/app/data
ports:
  - "8080:80"
environment:
  - NODE_ENV=production
auto_update: true
```

### 2. 编译与运行
1. **打开 MinGW 终端**  
   通过开始菜单搜索并打开 `MinGW-w64 Win64 Shell`。

2. **编译代码**  
   ```bash
   gcc -o docker_manager main.c -lyaml -lcurl
   ```

3. **运行程序**  
   ```bash
   ./docker_manager.exe
   ```

---

## Windows 系统特殊注意事项
1. **路径分隔符**  
   - 在 YAML 配置文件中，卷挂载路径建议使用正斜杠 `/`（如 `C:/data:/app/data`），Docker 会自动处理。

2. **镜像更新检测**  
   - 若镜像仓库非 Docker Hub，需修改 `check_image_update` 函数中的 API URL。

3. **依赖库安装**  
   - 若编译时提示找不到库，需检查 MinGW 安装路径是否包含在系统环境变量中。

---

## 常见问题
1. **编译失败**  
   - 确保已正确安装 MinGW-w64 和依赖库。
   - 检查 `libyaml` 和 `libcurl` 的开发文件是否存在。

2. **Docker 命令无法执行**  
   - 确认 Docker Desktop 已启动，并在终端中运行 `docker version` 验证。

3. **镜像更新不生效**  
   - 检查 `auto_update` 是否为 `true`。
   - 确保网络连接正常，Docker Hub API 可访问。
