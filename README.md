cd ~/Загрузки/kursach-realese
docker compose down -v
docker compose up -d --build --scale app=1

docker compose up -d --scale app=2
docker compose ps
