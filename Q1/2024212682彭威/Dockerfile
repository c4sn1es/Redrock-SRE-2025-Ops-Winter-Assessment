# 使用基础镜像
FROM gcc:latest

# 设置工作目录
WORKDIR /app

# 复制项目文件到工作目录
COPY src/ .

# 安装依赖
RUN apt-get update && apt-get install -y \
    libyaml-dev \
    libcurl4-openssl-dev

# 编译代码
RUN gcc -o compose_tool main.c -lyaml -lcurl

# 定义入口命令
CMD ["./compose_tool"]