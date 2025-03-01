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
