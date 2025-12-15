git clone <repo>
cd <repo>

xhost +local:docker

docker compose down -v   # первый раз (или если нужно сбросить БД)
docker compose up --build --scale app=2
