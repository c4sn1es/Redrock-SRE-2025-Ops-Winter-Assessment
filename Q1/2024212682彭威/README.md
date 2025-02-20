Compose 增强计划项目文档
一、项目概述
本项目是一个使用 C 语言实现的简化版 Docker Compose 工具，旨在提高容器管理的效率和自动化程度。它具备启动和停止容器的基础功能，同时支持镜像的自动更新以及与自建镜像仓库的集成。
二、程序功能
1. Level 0：简化版 Docker Compose 工具
能够读取 Docker-Compose 文件，识别其中的 volumes、ports、environment、container_name 和 image 字段。
根据读取的配置信息启动和停止容器。
2. Level 1：镜像自动更新
在 Docker-Compose 文件中增加 auto_update 字段，支持布尔类型。
对于 auto_update 为 True 的容器，每 12 小时检测一次所使用的镜像是否有新版本。
若发现新版本，更新对应的 Compose 文件的 image 字段，使容器镜像的 Tag 与最新版本一致。
3. Level 2：自建镜像仓库支持
可以从 DockerHub 拉取所需的容器镜像。
修改镜像名称并将镜像推送到自建的容器镜像仓库中。
更新原始 Compose 文件中的 image 字段，将镜像源改为自建的容器镜像仓库。
最后启动容器，确保即使代理出现问题也不影响服务启动。