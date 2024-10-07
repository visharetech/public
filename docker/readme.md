## Docker File

### Build the Docker Image
```console
# Download the Dockerfile and place into new directory
# Build the docker image
docker build -t vishare .
```

### Run the Container
```
docker run -it vishare

# The -v option in Docker is used to create a volume mount, which allows you to share files between your host machine and a Docker container.
# Below example demonstrates how to mount the current example directory in Windows host to the /mnt/example
docker run -v .\example:/mnt/example -it vishare
```

### Docker info
user name: vishare<br >
password: vishare

The docker included below packages:
* riscv32-unknown-elf cross-compile toolchain
* asim
* asim helloworld example

