# Use an appropriate base image (e.g., Ubuntu, Debian, etc.)
FROM ubuntu:latest

# Instala las dependencias necesarias (curl y bash)
RUN apt-get update && apt-get install -y curl bash libreadline-dev file

# Descarga el instalador desde GitHub
RUN curl -fsSL -o /install.sh https://github.com/kingcreek/42-Malloc_tester/raw/main/install.sh

RUN chmod +x install.sh
# Establece el directorio de trabajo
WORKDIR /

# Ejecuta el script install.sh al iniciar el contenedor
CMD ["bash", "/install.sh"]