cd ~/Загрузки/kursach-realese

# 0) стоп + снести БД-том (чистый старт)
docker compose down -v    # удалит volume с Postgres данными [web:4315]

# 1) поднять БД и ОДНО приложение (оно создаст таблицы/индексы)
docker compose up -d --build --scale app=1  # scale через compose up [web:4287]

# 2) дождаться, что app-1 не упал (проверь статус)
docker compose ps

# 3) поднять второе приложение
docker compose up -d --scale app=2
docker compose ps
