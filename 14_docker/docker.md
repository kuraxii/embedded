# docker

## 基本操作

### 下载镜像
```shell
docker pull <image_name>:<tag>
```
- \<image_name\>：镜像的名称（例如 nginx, python）。
- \<tag\>：镜像的标签，通常是版本号（例如 3.9，latest）。如果不指定，默认是 latest。

示例
```shell
docker pull python:3.9
```
结果查看：
执行后，会看到镜像的下载进度，包括分层下载情况。
你可以通过以下命令查看本地已下载的镜像：
```shell
docker images
```

### 创建并启动容器
```shell
docker run [OPTIONS] <image_name> [COMMAND]
# 如果镜像在本地不存在，它会先 pull 镜像，然后再启动容器。docker run 也可以用于创建并启动容器。
# <image_name>：要运行的镜像名称（例如 nginx, python）。
# [COMMAND]：可选的命令，指定运行容器时要执行的命令（例如 bash, python）。

# 常用选项：
# -d：后台运行容器（分离模式）。
# -it：以交互模式运行容器，并分配一个伪终端。
# --name：为容器命名。
# -p：端口映射，将容器内部端口映射到主机端口（例如 -p 8080:80）。

```

启动已经存在的、被停止的容器
```shell
docker start <container_name_or_id>
# <container_name_or_id>：你想要启动的容器的名称或 ID。
```
- 使用 docker ps 来查看容器是否成功启动。如果容器正在运行，它将显示在输出列表中。
    ```shell
    docker ps
    ```
- 如果你需要进入容器的命令行，可以使用 docker exec 进入：
    ```shell
    docker exec -it <container_name_or_id> /bin/bash
    ```