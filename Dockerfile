FROM ubuntu:focal

RUN apt-get update && apt-get install -y curl bash libreadline-dev file binutils gcc git

RUN curl -fsSL -o /install.sh https://github.com/kingcreek/42-Malloc_tester/raw/main/install.sh

RUN chmod +x install.sh

WORKDIR /

CMD ["bash", "/install.sh"]
