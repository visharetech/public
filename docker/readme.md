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
```

### Docker info
user name: vishare<br >
password: vishare

The docker included below packages:
* riscv32-unknown-elf cross-compile toolchain
* asim
* asim helloworld example

